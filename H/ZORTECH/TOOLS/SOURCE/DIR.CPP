#include <dos.h>
#include <dir.hpp>

zDirectory::zDirectory(char *spec, int attr) : d(1)
{
    struct FIND *p;
    FileInfo *q,r;
    int t,s;

    q = new FileInfo;
    err = 0;
    if (!q) {
allerr:
        err = ENOMEM;
        return;
    }
    *q->name = '\0';
    if (d.linkin(q))       // create a dummy entry
        goto allerr;
    _count = 0;
    p = findfirst(spec,attr);
    while (p) {
        q = new FileInfo;
        if (!q)
            goto allerr;
        q->attribute = p->attribute;
        q->time = p->time;              // DJB
        q->date = p->date;              // DJB
        strcpy(q->name,p->name);        // DJB
        q->size = p->size;              // DJB
        d.end();
        for (;;) {      // insertion sort - not highly efficient
                        // but we can do it as we go along
            if (strcmp(d()->name,p->name) <= 0) {
                if (d.linkin(q)) goto allerr;
                break;
            } else
                --d;
        }
        ++_count;
        p = findnext();
    }
    d.start();
    q = *d;
    delete q;
}
