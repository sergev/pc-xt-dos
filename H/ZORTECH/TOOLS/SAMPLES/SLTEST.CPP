#include <stdio.h>
#include <stdlib.h>
#include <slist.hpp>

//. First of all we need some "objects" to attach to the list. In
//. this case we will just use some character strings.
char *x[] = {
    "first",
    "second",
    "third",
    "fourth",
    "fifth"
};

main()
{
//. Now we can declare a ÝzSList and an iterator associated with it.
//. Notice that this use of the Ý= sign is an INITIALIZATION, and
//. uses the ÝzSListIterator constructor.
    zSList w;
    zSListIterator z = w;
    int i;
    void *p;

//. Then insert some items into the list, use the iterator to scan them,
//. and use the Ýget function to take them out again.
    puts("Testing class zSList\n");
    for (i = 0; i < 5; ++i)
        w.insert(x[i]);
    while ((p = z()) != 0)
        printf("%s\n",(char *) p);
    for (i = 5; i--; )
        printf("%s\n",(char *) w.get());

    puts("\nPress ENTER to continue");
    getchar();

//. Repeat the sequence of operations, but this time add the items
//. at the tail of the list.
    for (i = 0; i < 5; ++i)
        w.append(x[i]);
    while ((p = z()) != 0)
        printf("%s\n",(char *) p);
    for (i = 5; i--; )
        printf("%s\n",(char *) w.get());
    return EXIT_SUCCESS;
}
