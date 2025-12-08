#include <io.h>
#include <assert.h>
#include <vms.hpp>

zVMS::zVMS(long max, unsigned ri, unsigned iw,
        const char *fname, INT16 create,
        INITFUNC ei, unsigned elemsize)
{
    char filen[13];
    elem_init = ei;
// Allocate memory for index chain pointers and initialize to zero.
    table = new IndexItem *[iw];
    if (!table) {
allerr:
        err = ENOMEM;
        return;
    }
    memset(table,'\0',iw*sizeof(void*));

    tabsize = iw;
    es = elemsize;

// Allocate a copy of an initialized element
    empty = new char[es];
    if (!empty) goto allerr;
    elem_init(empty);

// Make array of pointers to resident blocks
    rp = new ResidencePtr[ri];
    if (!rp) goto allerr;

// Then set up each resident item.
    for (int i = 0; i < ri; ++i) {
        rp[i] = (Residence *) new
            char[sizeof(Residence)+elemsize-1];
        if (!rp[i])
            goto allerr;
        rp[i]->flags = 0;
    }

// Note how may free block there are
    freeblocks = ri-1;
    ritems = ri;

// Allocate store for file system name, and copy the name
    fn = new char[strlen(fname)+1];
    if (!fn) goto allerr;
    strcpy(fn,fname);

    nmax = max;

// Check if file is to be created
    if (create) {
        strcpy(filen,fn);
        strcat(filen,".dta");
        if ((fp = fopen(filen,"w+b")) == 0) {
err1:
            err = VMS_CREATE;
            return;
        }
// Initialize item count
        ccount = 0L; 

// Otherwise presume the filesystem exists - read the index
    } else {
        strcpy(filen,fn); 
        strcat(filen,".idx");
        if ((fp = fopen(filen,"rb")) == 0) {  // get index into memory
            err = VMS_OPEN_INDEX;
            return;
        }                           // open the index file
        ccount = 0L;
        for (int ni = 0; ni < iw; ++ni) {    // for each chain
            unsigned n,i;
            IndexItem *p;
            if (fread(&n,sizeof(unsigned),1,fp) != 1) {
                err = VMS_READ_INDEX;
                return;
            }
            ccount += n;            // count entries in file system
            for (i = n; i--;) {     // then read and fill in each of them
                p = new IndexItem;
                if (!p) goto allerr;
                if (fread(&p->logical,sizeof(long),2,fp) != 2) {
                    err = VMS_READ_INDEX;
                    return;
                }
                insert(p);
            }
        }
        fclose(fp);                 // done with index file
        strcpy(filen,fn);           // make the data file name
        strcat(filen,".dta");
        if ((fp = fopen(filen,"r+b")) == 0) {
            err = VMS_OPEN;
            return;
        }
    }
    err = 0;
}

// Insert puts an entry into the index.  An index chain is chosen simply by
// taking the modulus of the index number, and the entry is added to the
// corresponding list.
void zVMS::insert(IndexItem *p)
{
    unsigned hv;
    IndexItem *np;
    hv = p->logical%tabsize;        // select the required chain
    if (table[hv]) {                // anything in it?
        np = table[hv];
        for (;;) {
            if (p->logical == np->logical)
                return;             // it is already there
            if (!np->next)
                break;              // at end of chain
            np = np->next;          // otherwise step on to next
        }
        np->next = p;               // fill in the new entry
        p->next = 0;             // and mark as end of chain
    } else {
        table[hv] = p;              // first item in this chain
        p->next = 0;             // mark as end
    }
    return;
}

IndexItem *zVMS::lookup(long t)
{
    unsigned hv;
    IndexItem *np;    

    hv = t%tabsize;                 // select the required chain
    np = table[hv];
    for (; np; np = np->next) {
        if (np->logical == t)       // step along until found or end of chain
            return(np);             // found - return a pointer to it
    }
    return(0);                      // end of chain - item not present
}

void zVMS::cleanup()
{
    int i;
    IndexItem *p, *q;

    for (i = 0; i < tabsize; ++i) {
        p = table[i];
        for (p = table[i]; p; p = q) {
            q = p->next;            // eat up each chain
            delete p;
        }
    }
    delete table;                   // then delete the chain pointers
}

void zVMS::reindex()
{
    char filen[13];
    FILE *f;
    unsigned off;
    IndexItem *p;
    strcpy(filen,fn);           // make the index filename
    strcat(filen,".idx");
    if ((f = fopen(filen,"wb")) == 0) {
        err = VMS_OPEN_INDEX;
        return;
    }
// For each chain
    for (unsigned n, j, i = 0;
            i < tabsize; ++i) {
// Count the items
        for (n = 0, p = table[i]; p; p = p->next)
            ++n;
        if (fwrite(&n,sizeof(unsigned),1,f) != 1)
            goto err2;          // write the number of items
        for (p = table[i], j = n;
                j--; p = p->next)
                                // hop along the chain
            if (fwrite(&p->logical,
                sizeof(long),2,f) != 2)
                goto err2;      // writing each item
    }
    if (fclose(f)) {      // close index file
err2:
        err = VMS_WRITE_INDEX;
    }
}

char *zVMS::access(long idx, INT16 must_update)
{
    int free;
    if (idx < 0L || idx >= nmax) {
                    // requested index in range?
        err = VMS_OUTBOUNDS;
        return 0;
    }
    IndexItem *p;
    if ((p = lookup(idx)) != 0) {
                    // if found element != to initial value
        if (p->slot != -1) {
                    // if pslot > -1 it is in memory
            age(p->slot);
                    // age everything else
            if (must_update)
                    // mark dirty if it's a reference access
                rp[p->slot]->flags |= VM_DIRTY;
            return &rp[p->slot]->it;
                    // return a pointer to the data
        }
// Value only required, and this element is in its initial state
    } else if (!must_update)
        return empty;

// Otherwise the element has never been
// modified so one way or the other we now
// need a resident block to fill in from
// the data file or initialize
    if (freeblocks < 0) {
                    // there is no unused block
        for (int j = 0, i = 0;
                    i < ritems; ++i) 
            if (rp[i]->asu > j) {
                j = rp[i]->asu;
                    // find least recently used block
                free = i;
            }
        IndexItem *q;
// Anything which is in a memory slot should be in the index - but check
        if ((q = lookup(rp[free]->ind)) == 0) {
            err = VMS_INCONSISTENT;
            return 0;
        }
        q->slot = -1;
                    // mark it as no longer in memory
        if (rp[free]->flags & VM_DIRTY)
                    // update file if neccessary
            file_put(free,q);
                    // if dirty write it to file
    } else 
        free = freeblocks--;
                    // another free block used up
    if (!p) {
                    // the element has never been accessed
        (* elem_init)(&rp[free]->it);
                    // initialize the block
        rp[free]->ind = idx;
                    // and fill it in
        IndexItem *si;
        si = new
    IndexItem(idx,ccount*(es+sizeof(long)),free);
        if (!si) {
            err = ENOMEM;
            return 0;
        }
        ++ccount;
        insert(si);
    } else {
        file_get(free,p);
        p->slot = free;
    }
    rp[free]->asu = 0;
                    // Make it most recently used
    rp[free]->flags =
        VM_INUSE + must_update;
                    // fill in housekeeping info
    age(free);              // and age the rest
    return(&rp[free]->it);
}

void zVMS::file_get(int free, IndexItem *p)
{
    if (fseek(fp,p->actual,SEEK_SET))
        goto err3;
    if (fread(&rp[free]->ind,
        sizeof(long)+es,1,fp) != 1) {
err3:
        err = VMS_READ;
    }
}

void zVMS::file_put(int old, IndexItem *p)
{
    if (fseek(fp,p->actual,SEEK_SET))
        goto err4;
    if (fwrite(&rp[old]->ind,
        sizeof(long)+es,1,fp) != 1) 
err4:
        err = VMS_WRITE;
}

// Update the file and make it safe
void zVMS::flush()
{
    char filen[13];
    for (int i = 0; i < ritems; ++i)
        if (rp[i]->flags & VM_DIRTY) {
            IndexItem *p = lookup(rp[i]->ind);
            if (!p) {
                err = VMS_INCONSISTENT;
                return;
            }
            file_put(i,p);
                    // write dirty blocks
            rp[i]->flags = VM_INUSE;
                    // and mark as clean
        }
    if (fflush(fp)) {
        err = VMS_FLUSH;
        return;
    }
    reindex();
}

void zVMS::age(int n)
{
// Increment the age of everything except n
    for (int i = 0; i < ritems; ++i)
        if (i != n)
            ++rp[i]->asu;
}
