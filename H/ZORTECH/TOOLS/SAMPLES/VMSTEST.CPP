#include <iostream.hpp>
#include <stdlib.h>
#include <bios.h>
#include <vms.hpp>


// For the test we'll define an arbitrary, but quite large data type.
// Our initialization function sets up one of these objects to be
// in a recognizeable initial state.
struct example {
    int id;
    char text[502];
};

declare(zGVMS,example);


void zeroit(void *p)
{
    example *lp = p;

    lp->id = -1;
    memset(lp->text,'\0',502);
    strcpy(lp->text,
"This element has its text in initial state");
}

main(int argc, char *argv[])
{
    int create = (argc > 1 && !strcmp(argv[1],"-c"));

// Set up a sparse virtual array for a maximum of 1400 elements, 32
// of them able to reside in memory.  Hash table width 100, file
// system name THING.???, initialization function zeroit.
// The file is to be created or opened according to the command
// line argument.
    zGVMS(example)
        v(1400,32,100,"vmsthing",
          create,zeroit);
    int rv;
    if ((rv = v.error()) != 0) {
        printf(
"Constructor failed, error = %d\n", rv);
        return EXIT_FAILURE;
    }
    long i,j;
    int rand();
    example e, *p;

    if (create) {
        for (i = 0; i < 1400; i += 3) {
              p = &v[i];       // get a pointer to avoid multiple accesses
              p->id = i;
              strcpy(p->text,"This has been altered");
        }
        v.flush();
    } else 
        printf(
"%lu elements have been actively accessed\n",v.nelems());
    for (i = 300; i < 500; ++i)
        printf("%d %s\n",v(i).id,v(i).text);
    j = 300;
    while (v(j).id != -1)
        ++j;
    p = &v[j];
    p->id = (int) j;
    strcpy(p->text,"This is a supplementary");
    printf(
"%lu elements have been actively accessed\n",
        v.nelems());
    return EXIT_SUCCESS;
}
// This should modify one record each time the test is run without
// the -c argument.
