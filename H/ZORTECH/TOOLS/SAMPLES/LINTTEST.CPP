#include <stdio.h>
#include <stdlib.h>
#include <bios.h>
#include "lint.hpp"

//. The Ýprint function will be used in the example, and will check
//. out the Ýformat function.
void print(zLint4 &a, char nl = ' ')
{
    char buf[22];
    a.format(buf);
    printf("%s%c",buf,nl);
}

const int nn = 0xffff;
const unsigned int uu = 0xffff;
const unsigned char uc = 0xff;
const char *MAXLINT = "0x7fffffffffffffff";

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

//. Use the default constructor to create a ÝzLint4 object, then
//. check out the various assignment operators.
    zLint4 a;           // constructor with no arguments
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
    a = MAXLINT;
    print(a,'\n');

    puts("Press ENTER to continue");
    getchar();

//. Now check out the rest of the constructors, slipping in a test
//. of assignment from a ÝzLint4 along the way.
    zLint4 b = 'B';      // constructor from char
    a = b;
    zLint4 c = uc;       // from unsigned char
    zLint4 d = nn;       // from int
    zLint4 e = uu;       // from unsigned
    zLint4 f = -2111111111;
                        // from long;
    zLint4 g = 4222222222;
                        // from unsigned long
    zLint4 h = 123456789012.0;
                        // from double
    zLint4 m = MAXLINT;
    zLint4 n = "-0";
                        // from string
    print(a,'\n');
    print(b,'\n');
    print(d,'\n');
    print(e,'\n');
    print(f,'\n');
    print(g,'\n');
    print(h,'\n');
    print(m,'\n');
    puts("Press ENTER to continue");
    getchar();

//. Force overflow using addition and subtraction operators.
    a = MAXLINT;
    a -= 9;
    for (int i = 18; i--;) {
        ++a;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    a = MAXLINT;
    a = -a+9;
    for (i = 18; i--;) {
        --a;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    a = MAXLINT;
    a = a-9;
    for (i = 18; i--;) {
        a = a+1;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    a = MAXLINT;
    a = -a+9;
    for (i = 18; i--;) {
        a = a-1;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    a = MAXLINT;
    a -= 9;
    for (i = 18; i--;) {
        a += 1;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    a = MAXLINT;
    a = -a+9;
    for (i = 18; i--;) {
        a -= 1;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

//. Now investigate zero crossing using the same set of operators.
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
    getchar();
#if o
//. Add some large numbers until overflow then retrace.
    b = 0;
    a = "9876543210987654321";
    for (;;) {
        c = b;
        b += a;
        if (b < 0) break;
        print(b,'\n');
    }
    b = c;
    for (;;) {
        b = b - a;
        if (b < 0) break;
        print(b,'\n');
    }
    puts("Press ENTER to continue");
    getchar();
#endif
//. Then multiply to overflow and divide back using the available
//. operators.
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

//. Test the shift operators, and very briefly some bit operations
    b = 0xffffffff;
    print(b,'\n');
    b >>= 6;
    print(b,'\n');
    b <<= 6;
    print(b,'\n');
    b = 0;
    b |= "0x1000100010001000100010001000";
    print(b,'\n');
    b &= 0xffff;
    print(b,'\n');
    puts("Press ENTER to continue");
    getchar();

//. Check out modulo, not and unary minus
    a = "1000000000031";
    for (i = 5; i > -6; --i)
        print(a%i,'\n');
    puts("Press ENTER to continue");
    getchar();
    b = -a;
    print(b,'\n');
    if (!b) puts("b is zero");
    b = 0;
    if (!b) puts("b is zero");

//. Exersise the conversion functions
    long x;
    b = 2999999999;
    int rv = b.tolong(x);
    printf("%d %ld\n",rv,x);
    b = 2111111111;
    rv = b.tolong(x);
    printf("%d %ld\n",rv,x);
    double v = b;
    printf("v = %f %f\n",v, (double) b);

//. Finally test the set of boolean functions
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
