#include <bios.h>
#include <iostream.hpp>
#include <gslist.hpp>

struct any {
    int len;
    char *cp;
};

declare(zGSList,any);
declare(zGSListIterator,any);

any xx[] = {
    5,"First",
    6,"Second",
    5,"Third",
    6,"Fourth",
    5,"Fifth"
};

#if __ZTC__ > 0x0214
void
#endif
main()
{
    zGSList(any) w;
    zGSListIterator(any) z = w;
    int i;
    any *p;

    cout << "Testing class zGSList" << endl;

    for (i = 0; i < 5; ++i)
        w.insert(&xx[i]);
    while ((p = z()) != 0)
        cout << p->len << " " << p->cp << endl;
    cout << endl;

    for (i = 0; i < 5; ++i)
        w.append(&xx[i]);
    while ((p = z()) != 0)
        cout << p->len << " " << p->cp << endl;
    cout << endl;

    while ((p = w.get()) != 0)
        cout << p->len << " " << p->cp << endl;
}

