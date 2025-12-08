#include <iostream.hpp>
#include <dynarr.hpp>
#include <stdlib.h>

//. First declare the dynamic array type to work with doubles,
declare(zGDynamicArray,double);

main()
{
//. then declare an instance of this new type.
    zGDynamicArray(double) a;

//. The first access will size the array to 100 elements,
    for (int i = 100; i--;)
        a[i] = i;

    double *p = &a;
    unsigned u = a.size();
    cout << u << " elements" << endl;

//. then we can sweep through it using a pointer
    for (i = u; i--;)
        cout << *p++ << endl;

//. Test the error catching by trying to expand it beyond 
//. memory limitations
    char *waste, *last = 0;
    for (;;) {
// allocate away most of the memory
        waste = new char [100*sizeof(double)];
        if (waste) last = waste;
        else break;
    }
    delete last;
    a[300] = 23.456;
    cout << "a.error() = " << a.error();
    return EXIT_SUCCESS;
}
