#include <stdio.h>
#include <stdlib.h>
#include <moneynn.hpp>

//. As before we define a print function utilizing Ýformat.
void print(zMoney18 &a, char nl = ' ')
{
    char buf[24];
    a.format(buf);
    printf("%s%c",buf,nl);
}

main()
{

// Then declare a zMoney18 using the default constructor, and use that
// to check out the assignment operators.
    puts("Assignments");
    zMoney18 a;         // constructor with no arguments
    a = 0;              // assignment from int
    print(a,'\n');
    a = 1.23456e16;     // assignment from double
    print(a,'\n');
    a = "123456789123456789.99"; // from string
    print(a,'\n');
    a = "99999999999999999999.00";  // overflow from string
    printf("a.error() = %d\n",a.error());
    puts("Press ENTER to continue");
    getchar();

// Now the rest of the constructors, sneaking in a test of assignment
// from the existing zMoney18 object.
    puts("Constructors");
    zMoney18 b = 0;     // from int
    zMoney18 c = 1.23456e16;  // constructor from double
    a = c;
    zMoney18 d = "123456789123456789.99";  // from string
    print(a,'\n');
    print(b,'\n');
    print(c,'\n');
    puts("Press ENTER to continue");
    getchar();

// Now push to overflow using addition and subtraction.
    puts("Overflow via additions/subtractions");
    a = "999999999999999991.00";
    for (int i = 18; i--;) {
        a = a+1.00;
        if (a.error()) break;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    a = "-999999999999999991.00";
    for (i = 18; i--;) {
        a = a-1.00;
        if (a.error()) break;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    a = "999999999999999991.00";
    for (i = 18; i--;) {
        a += 1.00;
        if (a.error()) break;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    a = "-999999999999999991.00";
    for (i = 18; i--;) {
        a -= 1.00;
        if (a.error()) break;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

//. Check out zero crossing using the same operators
    puts("Zero crossing");
    a = 1.00;
    for (i = 10; i--;) {
        a = a-0.20;
        print(a,'\n');
    }
    for (i = 10; i--;) {
        a = a+0.20;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    for (i = 10; i--;) {
        a -= 0.20;
        print(a,'\n');
    }
    for (i = 10; i--;) {
        a += 0.20;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

//. Add some large sums to overflow then retrace.
    puts("Add large numbers to overflow");
    b = 0;
    a = "98765432109876543.50";
    for (;;) {
        c = b;
        b += a;
        if (b.error()) break;
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

//. Now multiply to overflow then divide back using the various options.
    puts("Multiply to overflow, then divide back");
    b = 1;
a = 13.0;
    for (;;) {
        c = b;
        b *= a;
        if (b.error()) break;
        print(b,'\n');
    }
    b = c;
    for (;;) {
        b /= a;
        if (b <= 13) break;
        print(b,'\n');
    }
    print(b,'\n');
    puts("Press ENTER to continue");
    getchar();

//. Check out money division and modulo.
    char buf[22];
    a = "999999999999999999.99";
    b = 0.19;
    zInt20 goodies = a/b;
    goodies.format(buf);
    puts(buf);
    printf("modulo - ");
    print(a%b,'\n');
    a = "100000000000000000.00";
    b = "9999999999999999.99";
    goodies = a/b;
    goodies.format(buf);
    puts(buf);
    printf("modulo - ");
    print(a%b,'\n');
    puts("Press ENTER to continue");
    getchar();

//. Exersise the conversion functions
    double v = b;
    printf("v = %f %f\n",v,(double) b);
    b = 0.00;
    if (!b) puts("b is zero");
    else puts("b is not zero");
    if (b) puts("b is not zero");
    else puts("b is zero");
    puts("Press ENTER to continue");
    getchar();

//. Finally test the set of boolean functions
    a = "1234567.99";
    b = "1234567.99";
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

    a = "1234566.99";
    b = "1234567.99";
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

    a = "1234568.99";
    b = "1234567.99";
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
