#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream.hpp>
#include <iomanip.hpp>
#include <vma.hpp>

// For the test we'll create a large virtual array of doubles, about
// 50000 of them.  This will just about fit on a 360k floppy disk.
// Our initialization function just does the obvious thing, and zeroes
// the storage.
declare(zGVMA,double);

void zeroit(double *p)
{
    *p = 0.0;
}

main(int argc, char *argv[])
{
    time_t tt = time(0);
// This line is commented out for the benefit of the test suite!
//    srand((unsigned) tt);
    int create = (argc > 1 && !strcmp(argv[1],"-c"));

// Now we can create or open an instance depending on the command line.
// 40960 elements, 512 elements per block, keeping 10 blocks in
// memory, file called THING.DAT, using the zeroit function for
// initialization.
    zGVMA(double)
        v(40960,512,10,"vmathing.dat",
          create,zeroit);
    int rv;
    if ((rv = v.error()) != 0) {
        cout << "Constructor failed, error = " << rv << endl;
        return rv;
    }
    long i,j;

// If we are creating the file give a value to every 100th element
// using the [] operator.
    if (create) {
        for (i = 0; i < 40960; ++i) {
            if (!(i%100))
                v[i] = (double) i;
        }
    } else {
        for (i = 50; i--;) {
            j = rand();
            j = (j*j)%40960;
            v[j] = PI;
        }
    }

// Then access the array non-destructively using the Ý() operator.
    cout << setprecision(1) << fixed << showpoint << spacing;
    for (i = 50; i--;) {
        j = rand();
        j = (j*j)%40960;
        cout << setw(5) << j << setw(7) << v(j) << endl;
    }
    return EXIT_SUCCESS;
}
