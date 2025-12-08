#include <iostream.hpp>
#include <bios.h>
#include <dlist.hpp>
#include <stdlib.h>

//. First declare the customary arbitrary type for test purposes,
struct any {
    int len;
    char *cp;
};

//. then define the new types to deal with doubly linked lists of
//. Ýany objects, and define some objects to do tests with.
declare(zGDList,any);
declare(zGDLCursor,any);

struct any xx[] = {
    4,"head",
    6,"second",
    5,"third",
    6,"fourth",
    5,"fifth",
    5,"sixth"
};

any intruder = {8,"Intruder"};

main()
{
    any *p = xx;

//. Declare an instance of ÝzGDList(any)Þ, taking the default cleanup
//. behaviour (no purging of items on the list).
    zGDList(any) w(p++);
    int i;

//. Now link some items into the list, then go back to the start and
//. check that it's possible to scan through them using Ýw++Þ.
    for (i = 1; i < 6; ++i)
        w.linkin(p++);
    w.start();
    for (;;) {
        cout << w()->len << " " << w()->cp << endl;
        if (w++) break;
    }
    cout << "Press ENTER to continue" << endl;;
    getchar();

//. Go to some arbitrary point in the list, link out the object that
//. is there.
    w.start();
    w += 2;
    p = *w;
    cout << "\nExtracted item was ("
         << p->len << " "
         << p->cp << ")\n" << endl;

//. Then link a new object in further along the list, and scan through the
//. list again.
    w += 2;
    w.linkin(&intruder);
    w.start();
    w++;
    w.update(&intruder);
    w.start();
    cout << "Scanning from start" << endl;
    for (;;) {
        cout << w()->len << " " << w()->cp << endl;
        if (w++) break;
    }
    cout << "Press ENTER to continue" << endl;
    getchar();

//. The list currency is now at the end of the list.  Define two cursors,
//. one tied to the currency, and the other set to the start of the list,
//. then scan in both directions using the cursors.
    zGDLCursor(any) c1(w), c2(w,1);
    cout << "\nCursor 1 scanning forwards" << endl;
    for (;;) {
        cout << c1()->len << " " << c1()->cp << endl;
        if (c1++) break;
    }
    cout << "\nCursor 2 scanning backwards" << endl;
    for (;;) {
        cout << c2()->len << " " << c2()->cp << endl;
        if (c2--) break;
    }

//. Finally check out the Ýsize function.
    cout << "\nEnded up with " << w.size() << " items" << endl;
    return EXIT_SUCCESS;
}
