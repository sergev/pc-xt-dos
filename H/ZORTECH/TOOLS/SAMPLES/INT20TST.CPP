#include <stdio.h>
#include <stdlib.h>
#include <bios.h>
#include <intnn.hpp>

//. The Ýprint function will be useful for the test, and will check
//. out the Ýformat function to some extent.
void print(zInt20 &a, char nl = ' ')
{
    char buf[31];
    a.format(buf,30);
    printf("%s%c",buf,nl);
}

const int nn = 0xffff;
const unsigned int uu = 0xffff;
const unsigned char uc = 0xff;
const char *MAXINT20 = "99999999999999999999";

main()
{
    unsigned char uc = 0xff;
    int nn
#ifdef DOS386
            = 0xffffffff;
#else
            = 0xffff;
#endif
    unsigned uu = 0xffff;

//. Declare a ÝzInt20 using the default constructor, then check out
//. each of the assignment operators.
    zInt20 a;            // constructor with no arguments
    print(a,'\n');
    a = 'A';            // assignment from char
    print(a,'\n');
    a = uc;             // from unsigned char
    print(a,'\n');
    a = nn;             // from int
    print(a,'\n');
    a = uu;             // from unsigned
    print(a,'\n');
    a = -2111111111;    // from long
    print(a,'\n');
    a = 4222222222;     // from unsigned long
    print(a,'\n');
    a = 123456789012.0; // from double
    print(a,'\n');
    a = "-0";           // from string
    print(a,'\n');
    a = "999999999999999999999";
                        // overflow from string
    printf("a.error() = %d\n",a.error());
    a = MAXINT20;
    print(a,'\n');

    puts("Press ENTER to continue");
    getchar();

//. Then try out the other constructors, slipping in assignment
//. from a ÝzInt20 along the way.
    zInt20 b = 'B';      // constructor from char
    a = b;
    zInt20 c = uc;       // from unsigned char
    zInt20 d = nn;       // from int
    zInt20 e = uu;       // from unsigned
    zInt20 f = -2111111111;
                        // from long;
    zInt20 g = 4222222222;
                        // from unsigned long
    zInt20 h = 123456789012.0;
                        // from double
    zInt20 m = MAXINT20;
    zInt20 n = "-0";
                        // from string
    print(a,'\n');
    print(b,'\n');
    print(c,'\n');
    print(d,'\n');
    print(e,'\n');
    print(f,'\n');
    print(g,'\n');
    print(h,'\n');
    print(m,'\n');
    puts("Press ENTER to continue");
    getchar();

//. Now force overflow using +, -, ++, --, +=, and -= operators
    a = "99999999999999999991";
    for (int i = 18; i--;) {
        ++a;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    a = "-99999999999999999991";
    for (i = 18; i--;) {
        --a;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    a = "99999999999999999991";
    for (i = 18; i--;) {
        a = a+1;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    a = "-99999999999999999991";
    for (i = 18; i--;) {
        a = a-1;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    a = "99999999999999999991";
    for (i = 18; i--;) {
        a += 1;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    a = "-99999999999999999991";
    for (i = 18; i--;) {
        a -= 1;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

//. Investigate zero crossing using the same set of operators.
    a = 5;
    for (i = 10; i--;) {
        --a;
        print(a,'\n');
    }
    for (i = 10; i--;) {
        ++a;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    for (i = 10; i--;) {
        a = a-1;
        print(a,'\n');
    }
    for (i = 10; i--;) {
        a = a+1;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    for (i = 10; i--;) {
        a -= 1;
        print(a,'\n');
    }
    for (i = 10; i--;) {
        a += 1;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    puts("add to overflow");
    getchar();
//. Add some large numbers until overflow then retrace the same steps.
    b = 0;
    a = "9876543210987654321";
    for (;;) {
        c = b;
        b += a;
        if (b.error()) break;
        print(b,'\n');
    }
    b = c;
    for (i = 1000; i--;) {
        b = b - a;
        if (b <= 0) break;
        print(b,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

//. Then multiply to overflow and divide back using the various operators
//. available.

    b = 1;
    for (;;) {
        c = b;
        b *= 13;
        if (b.error()) break;
        print(b,'\n');
    }
    b = c;
    for (;;) {
        b /= 13;
        if (b <= 13) break;
        print(b,'\n');
    }
    print(b,'\n');
    puts("Press ENTER to continue");
    getchar();

    a = 13;
    b = 1;
    for (;;) {
        c = b;
        b = b * a;
        if (b.error()) break;
        print(b,'\n');
    }
    b = c;
    for (;;) {
        b = b/a;
        if (b <= 13) break;
        print(b,'\n');
    }
    print(b,'\n');
    puts("Press ENTER to continue");
    getchar();

//. Test the behaviour of the shift operators
    b = "10000000000000000000";
    b -= 1;
    print(b,'\n');
    b >>= 6;
    print(b,'\n');
    b <<= 6;
    print(b,'\n');
    puts("Press ENTER to continue");
    getchar();

//. Check out modulo
    a = 31;
    for (i = 5; i > -6; --i)
        print(a%i,'\n');
    puts("Press ENTER to continue");
    getchar();

//. Then exersise the conversion functions
    long x;
    b = 2999999999;
    int rv = b.tolong(x);
    printf("%d %ld\n",rv,x);
    b = 2111111111;
    rv = b.tolong(x);
    printf("%d %ld\n",rv,x);
    double v = b;
    printf("v = %f\n",v);

//. then operator not and unary minus
    a = -b;
    print(a,'\n');
    b = 0;
    if (!b) puts("b is zero");
    puts("Press ENTER to continue");
    getchar();

// Finally test the set of boolean functions
    a = 1234567;
    b = 1234567;
    printf("a == b %d\n",a == b);
    printf("a != b %d\n",a != b);
    printf("a <= b %d\n",a <= b);
    printf("b <= a %d\n",b <= a);
    printf("a < b %d\n",a < b);
    printf("b < a %d\n",b < a);
    printf("a > b %d\n",a > b);
    printf("b > a %d\n",b > a);
    printf("a >= b %d\n",a >= b);
    printf("b >= a %d\n",b >= a);
    puts("Press ENTER to continue");
    getchar();

    a = 1234566;
    b = 1234567;
    printf("a == b %d\n",a == b);
    printf("a != b %d\n",a != b);
    printf("a <= b %d\n",a <= b);
    printf("b <= a %d\n",b <= a);
    printf("a < b %d\n",a < b);
    printf("b < a %d\n",b < a);
    printf("a > b %d\n",a > b);
    printf("b > a %d\n",b > a);
    printf("a >= b %d\n",a >= b);
    printf("b >= a %d\n",b >= a);
    puts("Press ENTER to continue");
    getchar();

    a = 1234568;
    b = 1234567;
    printf("a == b %d\n",a == b);
    printf("a != b %d\n",a != b);
    printf("a <= b %d\n",a <= b);
    printf("b <= a %d\n",b <= a);
    printf("a < b %d\n",a < b);
    printf("b < a %d\n",b < a);
    printf("a > b %d\n",a > b);
    printf("b > a %d\n",b > a);
    printf("a >= b %d\n",a >= b);
    printf("b >= a %d\n",b >= a);
    return EXIT_SUCCESS;
}
