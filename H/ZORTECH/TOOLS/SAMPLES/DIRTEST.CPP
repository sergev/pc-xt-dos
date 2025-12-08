#include <stdlib.h>
#include <iostream.hpp>
#include <dir.hpp>

main()
{
    zDirectory cd;
    FileInfo *p;

    for (p = cd.first(); p; p = cd.next())
        cout << p->name << endl;
    cout << "\n" << cd.count() << " Files in all" << endl;
    return EXIT_SUCCESS;
}
