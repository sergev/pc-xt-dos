#include <bios.h>
#include <stdlib.h>
#include <bucket.hpp>

struct thing {
    char key[4];
    char dummy[20];
};

declare(zGBucketFile,thing);

void createit()
{
    zGBucketFile(thing) pf(59,3,"bktest2");
}

main()
{
    if (access("bktest2.bkt",F_OK) == -1) {
        puts("File not found - creating");
        createit();
    }
    thing t;
    zGBucketFile(thing) bf("bktest2",2);
    for (;;) {
        printf("\n\nItem: ");
        fflush(stdout);
        scanf("%s",t.key);
        if (t.key[0] == '.') break;
        int rv = bf.seek(&t);
        if (!rv)
            printf("Found %s\n",t.dummy);
        printf("Dummy: ");
        fflush(stdout);
        scanf("%s",t.dummy);
        if (rv) {
            bf.add(&t);
        } else  {
            bf.update(&t);
        }
    }

    const thing *p = bf.traverse(1);
    while (p) {
        printf("%4s %s\n", p->key, p->dummy);
        p = bf.traverse();
    }

    return EXIT_SUCCESS;
}
