#include <slist.hpp>

zSList::zSList(INT16 p)
{
    purging = p;
    last = 0;
}

zSList::zSList(void *a, INT16 p)
{
    if ((last = new zSLink(a)) == 0) {
        err = ENOMEM;
        return;
    }
    purging = p;
    err = 0;
    last->next = last;
}

int zSList::insert(void *a)
{
    zSLink* t = new zSLink(a);
    if (!t) return(-1); // insert failed
    if (last) {         // list already exists
        t->next = last->next;
        last->next = t; // a becomes new head of list
    } else {
        last = t;       // new first item
        last->next = last;
    }
    return(0);
}

int zSList::append(void *a)
{
    zSLink* t = new zSLink(a);
    if (!t) return(-1);
    if (last) {
        t->next = last->next; // put a at head of list
        last = last->next = t;// then move tail pointer round one!
    } else {
        last = t;       // new first item
        last->next = last;
    }
    return(0);
}

void *zSList::get()
{
    if (last == 0) {
        err = SLIST_EMPTY;
        return 0;
    }
    zSLink* f = last->next;
    void *r = f->e;
#if LPTR
    if ((long) f == (long) last)
#else
    if (f == last)
#endif
        last = 0;
    else
        last->next = f->next;
    delete f;
    return r;
}

void zSList::cleanup()
{
    zSLink* l = last;
    if (!l)
        return;
    do {
        zSLink* ll = l;
        l = l->next;
        if (purging) 
            delete ll->e;// attached object as well
        delete ll;
    } while (l != last);
    last = 0;            // put back in initial state
}

void *zSListIterator::operator()()
{
    if (!cs->last)
        return(0);
    if (!ce) {
        ce = cs->last->next;
        return ce->e;
    } else if
#if LPTR
            ((long) ce == (long) cs->last) {
#else
            (ce == cs->last) {
#endif
        ce = 0;
        return(0);
    } else {
        ce = ce->next;
        return ce->e;
    }
}
