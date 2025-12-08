#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <money.hpp>

//. A print function which will be useful in the rest of the test
//. will check the format function to some extent.
void print(zMoney &a, char nl = ' ')
{
    char buf[21];
    a.format(buf,20);
    printf("%s%c",buf,nl);
}

main()
{

//. Start by checking out the default constructor, then each of the
//. assignment operators.
    zMoney a;           // constructor with no arguments
    print(a,'\n');
    a = 0;              // assignment from int
    print(a,'\n');
    a = 123456789.00;   // from double
    print(a,'\n');
    a = "-0";           // from string
    print(a,'\n');
    a = "9999999999.99";
                        // overflow from string
    printf("a.error() = %d\n",a.error());
    puts("Press ENTER to continue");
    getchar();

//. Now go through the constructors, checking assignment from another
//. ÝzMoney along the way.
    zMoney b = 20;          // from int
    a = b;
//    zMoney c(999999999,99); // from explicit long and int
zMoney c = b;
    zMoney d = 987654321.0; // from double
    zMoney e = "-0";        // from string
    zMoney f = "987654321.00";
    zMoney g = f;           // copy constructor

    print(a,'\n');
    print(b,'\n');
//    print(c,'\n');
    print(d,'\n');
    print(e,'\n');
    print(f,'\n');
    puts("Press ENTER to continue");
    getchar();

//. Now watch the overflow behaviour with various operators
    puts("overflow using +");
    a = 0;
    for (int i = 22; i--;) {
        a = a+100000000.00;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    puts("overflow using -");
    a = 0;
    for (i = 22; i--;) {
        a = a-100000000.00;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    puts("overflow using +=");
    a = 0;
    for (i = 22; i--;) {
        a += 100000000.00;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

puts("overflow using -=");
    a = 0;
    for (i = 22; i--;) {
        a -= 100000000.00;
        print(a,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

//. Representations of this sort can have two representations of
//. zero $0.00, and -$0.00, so check zero crossing using various
//. operators to see if behaviour is conventional.
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

//. Multiply by a variety of numbers using the available options
    double dv = 0.00033;
    for (i = 10; i--; dv *= 10) {
        b = 12345.00;
        b = b*dv;
        print(b,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

    b = 99999.99;
    for (i = 10; i--;) {
        b *= 0.95;
        print(b,'\n');
    }
    puts("Press ENTER to continue");
    getchar();

//. Check out divide and modulo - this is one of the areas where
//. rounding errors can still arise.
    a = 999999999.99;
    b = 234.56;
    double q = a/b;
    c = a % b;
    printf("q = %6.0f\n",q);
    print(c,'\n');
    a = 9999999.99;
    b = 0.01;
    q = a/b;
    c = a % b;
    printf("q = %6.0f\n",q);
    puts("This sort of modulo is suspect!");
    print(c,'\n');
    puts("Press ENTER to continue");
    getchar();

//. Exersise the conversion to double, unary minus, and the not function.
    q = sqrt(a);
    printf("q = %f\n",q);
    b = 1;
    if (!b) puts("b is zero");
    else puts("b is not zero");
    if (b) puts("b is not zero");
    else puts("b is zero");
    b = -a;
    print(b,'\n');
    puts("Press ENTER to continue");
    getchar();

// Finally test the various boolean functions.
    a = 1234567.00;
    b = 1234567.00;
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

    a = 1234566.00;
    b = 1234567.00;
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

    a = 1234568.00;
    b = 1234567.00;
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
