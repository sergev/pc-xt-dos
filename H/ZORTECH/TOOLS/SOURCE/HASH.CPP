#include <string.h>
#include <assert.h>
#include <iostream.hpp>
#include <iomanip.hpp>
#include <hash.hpp>

zHashTable::zHashTable(MATCHING mf,
        int sz, HASHFUNC foo,
        int purge, VISIT v)
{
    table = new zSList *[sz];
    if (!table) {
        err = ENOMEM;
        return;
    }
    memset(table,0,sz*sizeof(void *));
    tabsize = sz; hashfunc = foo;
    matching = mf; purging = purge;
    visit = v;
    err = 0;
}

int zHashTable::insert(void *item)
{
    unsigned hv;
    if (!item) {
        err = HASH_BADARG;
        return err;
    }
    hv = hashfunc(item,tabsize);     // get hash number
    if (table[hv]) {
        zSListIterator li(*table[hv]);
        for (;;) {
            void *p = li();
            if (!p) break;
            if (!(*matching)(p,item)) {
                    // already there
                err = HASH_DUP;
                return err;
            }
        }
        if (table[hv]->append(item))
            goto errcon;    // apologies to purists
    } else {                            // no chain
        table[hv] = new zSList(item,purging);
                            // start a zSList
        if (!table[hv])
            goto errcon;
    }
    err = HASH_OK;
    return 0;
errcon:
    err = ENOMEM;
    return err;
}


void *zHashTable::remove(void *item)
{
    unsigned hv;

    if (!item) {
        err = HASH_BADARG;
        return 0;
    }
    hv = (*hashfunc)(item,tabsize);
    if (!table[hv]) {           // no such item
nosuch:
        err = HASH_NOSUCH;
        return 0;
    }
    zSList *sl = new zSList(purging);
    if (!sl) goto nomem;
    int n = 0;
    for (void *q = 0;;) {
        void *p = table[hv]->get();
        if (!p) break;
        if (!(*matching)(p,item))
            q = p;              // found it
        else {
            ++n;
            if (sl->append(p)) { // transfer to new list
                delete sl;
                goto nomem;
            }
        }
    }
    delete table[hv];           // purge old zSList
    if (n)
        table[hv] = sl;
    else {
        table[hv] = 0;
        delete sl;
    }
    if (!q) goto nosuch;
    err = HASH_OK;
    return q;
nomem:
    err = ENOMEM;
    return 0;
}

void* zHashTable::seek(void *item)
{
    unsigned hv;

    hv = (*hashfunc)(item,tabsize);
    if (table[hv]) {
        zSListIterator li(*table[hv]);
        for (;;) {
            void *p = li();
            if (!p) break;
            if (!(*matching)(p,item)) {
                err = HASH_OK;
                return p;
            }
        }
    }
nosuch:
    err = HASH_NOSUCH;
    return 0;
}

void zHashTable::inorder()
{
    for (int i = 0; i < tabsize; ++i) {
        if (!table[i]) continue;
        zSListIterator li(*table[i]);
        for (;;) {
            void *p = li();
            if (!p) break;
            (*visit)(p);
        }
    }
}

void zHashTable::cleanup()
{
    for (int i = 0; i < tabsize; ++i)
        if (table[i]) delete table[i];
    delete table;
}

void zHashTable_report(zHashTable &ht, const char * (*xlate)(void *))
{
    int n,i, freq[22];

    memset(freq,'\0',sizeof(freq));
    cout << setw(4) << stickywidth << spacing;
    for (i = 0; i < ht.tabsize; ++i) {
        if (!ht.table[i]) {
            freq[0]++;
        } else {
            zSListIterator li(*ht.table[i]);
            for (n = 0; li();++n) ;
            freq[n]++;
#if 0
// Include this for a more comprehensive report
            if (n) {
                cout << n;
                for (; n--;)
                    cout << (*xlate)(li()));
                cout << endl;
            }
#endif
        }
    }
    for (i = 0; i < 21; ++i)
        cout << i << freq[i] << endl;
}

static unsigned rtab[] = {
    7092,
    5393,
    1741,
    4217,
    8537,
    3043,
    5931,
    9660,
    5143,
    1161,
    3498,
    1563,
    4308,
    6730,
    2102,
    1535,
    7973,
    3797,
    6688,
    2813,
    9165,
    3116,
    9907,
    1224,
    5662,
    2614
};

// This hash function for alpha strings may be useful as a component
// of user hash functions.  It is the one used for the test program,
// and it assumes a table size which is a power of two.
unsigned zAlphaHash(const char *w, unsigned modulo)
{
    const char *p;
    unsigned x,y = 0;

    x = strlen(w)-1;
    for (p = w+x; p >= w; --p) {
        y += rtab[*p - 'a'];    // effect outcome as broadly as possible
        y += x*(*p);            // make it depend on order
        --x;
    }

// Use a table size which is a power of two
    return (y & (modulo-1));
}

#if 0
// This is a simpler alternative
unsigned default_hash(const char *w, int modulo)
{
    unsigned x,y = 0;

    while (*w) y = y << 1 ^ *w++;
    y = y & (modulo-1);
    return(y);
}
#endif
