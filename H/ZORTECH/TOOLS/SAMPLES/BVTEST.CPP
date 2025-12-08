#include <stdlib.h>
#include <iostream.hpp>
#include <string.h>
#include <bios.h>
#include <bitvec.hpp>

//. This display function will check out polymorphic handling of
//. bit vectors. It does not know what sort of reference it is
//. going to get until run time!
void display(zBitVec &bv)
{
    int n = bv.capacity();
    for (int i = 0; i < n; ++i)
        cout << bv[i];
    cout << endl;
}

main()
{
    int n, i;
//. First initialize a 16 bit vector from a binary integer constant.
    zBitVec16 a = 0b1010101010101010;
    display(a);

//. Then check out setting and resetting.
    n = a.capacity();
    for (i = 0; i < n; ++i)
        a.reset(i);
    display(a);
    a = 0b1010101010101010;

//. Do similar tests on ÝzBitVec32 with b, but check assignment from
//. a ÝzBitVec16 as well.
    zBitVec32 b = 0b10000000100000001000000010000000;
    display(b);
    n = b.capacity();
    for (i = 0; i < n; ++i)
        b.set(i);
    display(b);
    b = a;
    display(b);

//. Now much the same with the ÝzBitVecX type, only here we do a truncating
//. assignment to b, then an assignment back to c which alters its size.
    zBitVecX c(60), f(60);
    n = c.capacity();
    for (i = 0; i < n; ++i)
        if (i & 1)
            c.set(i);
    display(c);
    f = c;
    display(f);
    b = c;
    display(b);
    n = c.capacity();
    for (i = 0; i < n; ++i)
        c.reset(i);
    display(c);
    c = b;
    display(c);

// Finally check out a truncating initialization
    zBitVec16 d = b;
    display(d);
    return EXIT_SUCCESS;
}
