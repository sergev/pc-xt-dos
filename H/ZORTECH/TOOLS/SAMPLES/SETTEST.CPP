#include <stdio.h>
#include <stdlib.h>
#include <sets.hpp>

// The show function uses the set inclusion test.
void show(zSet &a)
{
    int first = 1;
    printf("[");
    for (int i = 0; i < S_MAX; ++i) {
        if (a.includes(i)) {
            printf("%c%d",first? ' ': ',',i);
            first = 0;
        }
    }
    puts(" ]");
}

int iv[] = {3,6,9,12,18,27};

main()
{
// Test the default constructor
    zSet empty;
    show(empty);
    if (!empty)
        puts("is empty");
    else
        puts("is not empty");

// Test initialization by a string
    zSet a = "1,3,5,7 - 14,19, 25-30, 37,53,59";
    puts("Initializer \"1,3,5,7 - 14,19, 25-30, 37,53,59\", set");
    show(a);

// Test memberwise initialization,
    zSet b = a;
    puts("b was initialized from a, and has:-");
    show(b);

// and memberwise assignment using the implicit conversion from a string.
    b = "2,3,5,13,14";
    puts("b revalued by assignment from \"2,3,5,13,14\":-");
    show(b);

// Initialise from an array of integers.
    zSet x(6,iv);
    puts("X initialized from array of integers:-");
    show(x);

    puts("Press ENTER to continue\n\n");
    getchar();

// Exersise the subset test
    puts("A:-");
    show(a);
    puts("B:-");
    show(b);
    if (b >= a) puts("b is a subset of a");
    else puts("Members in b not in a");

// Then -=
    puts("A:-");
    show(a);
    puts("B -= 2 :-");
    b -= 2;
    show(b);
    if (b >= a) puts("b is a subset of a");
    else puts("Members in b not in a");

// and +=
    puts("A:-");
    show(a);
    puts("B += 20 :-");
    b += 20;
    show(b);
    if (b >= a) puts("b is a subset of a");
    else puts("Members in b not in a");

    puts("Press ENTER to continue\n\n");
    getchar();

    puts("A:-");
    show(a);
    puts("X:-");
    show(x);

// Set union
    b = a+x;
    puts("The union of A and X is");
    show(b);

// Set difference
    b = a-x;
    puts("The difference of A and X is"); show(b);

// Set intersection
    b = a*x;
    puts("The intersection of A and X is"); show(b);
    return EXIT_SUCCESS;
}
