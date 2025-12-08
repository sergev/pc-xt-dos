#include <dlist.hpp>

zDList::zDList(INT16 p)
{
    root = 0; current = 0;
    lpos = count = 0L; purge = p;
    err = 0;
}

zDList::zDList(void* a, INT16 p)
{
    root = current = 0; lpos = count = 0L;
    purge = p;
    if (linkin(a) == -1)
        err = ENOMEM;
    else err = 0;
}

int zDList::linkin(void* ent)
{
    zDLNode *t;
    if (!root) {                // nothing there yet
        root = new zDLNode;      // allocate a node
        if (!root)
            return ENOMEM;
        root->prev = root;      // a circular list of one item see fig 5.1
        root->next = root;      // prev and next point to itself
        root->body = ent;       // associate the data item
        current = root;         // current == only
        lpos = 1L;
    } else {
        t = new zDLNode;
        if(!t)
            return(ENOMEM);
#if LPTR
        if ((long) current == (long) root)
#else
        if (current == root)
#endif
            root = t;           // adjust root if inserting at tail
        current->next->prev = t;
        t->next = current->next;
        current->next = t;
        t->prev = current;      // link in new node after current
        t->body = ent;          // see fig 2.2
        ++lpos;
        current = t;            // adjust currency to new item
    }
    count++;
    return(0);
}

void* zDList::linkout()
{
    zDLNode *t;
    void *r;

    if (!root) {
        err = DLIST_EMPTY;
        return 0;
    }
#if LPTR
    if ((long) current == (long) root) {
#else
    if (current == root) {
#endif
        if (count == 1L) {              // only item
            r = root->body;
            delete root;
            root = current = 0;      // revert to initial state
            lpos = count = 0L;
        } else {                        // deleting tail
            current = root->prev;
            root->next->prev = current;
            current->next = root->next;
            r = root->body;
            delete root;
            root = current;             // root must be adjusted to new tail
            --count;
            --lpos;
        }
    } else {
        t = current->next;              // currency moves to next item
        current->prev->next = t;        // preserving head of list position
        t->prev = current->prev;        // if originally there
        r = current->body;
        delete current;
        current = t;
        --count;                        // lpos not affected
    }
    return r;
}
            
void zDList::cleanup()
{
    zDLNode *t;
    if (!root)
        return;
    for (; count--;) {
        t = root->next;
        if (purge)
            delete root->body;
        delete root;
        root = t;
    }
}

void *zDList::get()
{
    if (!root) {
        err = DLIST_EMPTY;
        return 0;
    }
    return current->body;
}

void *zDList::update(void *p)
{
    if (!root) {
        err = DLIST_EMPTY;
        return 0;
    }
    void *t = current->body;
    current->body = p;
    return t;
}

int zDList::fwd()
{
    if (lpos < count) {
        current = current->next;
        ++lpos;
        return 0;
    } return DLIST_ATEND;     // just a warning
}

int zDList::bkwd()
{
    if (lpos > 1) {
        current = current->prev;
        --lpos;
        return 0;
    } return DLIST_ATEND;     // just a warning
}

int zDList::start()
{
    if (count) {
        current = root->next;
        lpos = 1;
        return 0;
    } return DLIST_EMPTY;
}

int zDList::end()
{
    if (count) {
        current = root;
        lpos = count;
        return 0;
    } return DLIST_EMPTY;
}

zDLCursor::zDLCursor(zDList &dl, int pos)
    : thislist(dl)
{
    if (!pos) {
        current = thislist.root->next;
        lpos = 1;
    } else {
        current = thislist.current;
        lpos = thislist.lpos;
    }
}

int zDLCursor::fwd()
{
    if (lpos < thislist.count) {
        current = current->next;
        ++lpos;
        return 0;
    } return DLIST_ATEND;     // just a warning
}

int zDLCursor::bkwd()
{
    if (lpos > 1) {
        current = current->prev;
        --lpos;
        return 0;
    } return DLIST_ATEND;     // just a warning
}

int zDLCursor::start()
{
    if (thislist.count) {
        current = thislist.root->next;
        lpos = 1;
        return 0;
    } return DLIST_EMPTY;
}

int zDLCursor::end()
{
    if (thislist.count) {
        current = thislist.root;
        lpos = thislist.count;
        return 0;
    } return DLIST_EMPTY;
}

void *zDLCursor::update(void *p)
{
    if (!thislist.root) {
        thislist.err = DLIST_EMPTY;
        return 0;
    }
    void *t = current->body;
    current->body = p;
    return t;
}

void *zDLCursor::get()
{
    if (!thislist.root) {
        thislist.err = DLIST_EMPTY;
        return 0;
    }
    return current->body;
}

