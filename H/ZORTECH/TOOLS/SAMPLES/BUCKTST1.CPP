#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bucket.hpp>

struct thing {
    char key[4];
    char dummy[2];
};

// This is here for testing consistency - implementation of
// rand may change
static unsigned long int next = 1;

int rand(void)
{
    next = next * 1103515245 + 12345;
    return ((unsigned int) (next / 0x10000) & 0x7FFF);
}

void srand(unsigned int seed)
{
    next = seed;
}


declare(zGBucketFile,thing);

main()
{
    thing p;

// Create a zBucketFile
    zGBucketFile(thing) bf(59,3,"bktest1");
    int rv, probes = 0, t;
    if ((rv = bf.error()) != 0) {
        printf("Creation error %d\n",rv);
        return -1;
    }
    p.key[3] = '\0';

// Add as many entries as possible
    for (int n = 0; n < 1000;) {
        for (int j = 0; j < 3; ++j) {
            int r = rand();
printf("%d\n",r);
            p.key[j] = r%26 + 'A';
}
puts(p.key);
        rv = bf.add(&p, &t);
        if (rv == BUCKET_OVERLOADED) break;
        if (!rv)
            ++n;
// Count the total number of probes
        probes += t;
    }
    double av = double(probes)/n;
    printf("rv = %d, n = %d, av = %f\n",rv,n,av);
    puts("Press ENTER to scan the file");
    getchar();

    const thing *q = bf.traverse(1);
    n = 0;
    while (q) {
        puts(q->key);
        ++n;
        q = bf.traverse();
    }
    printf("Scanned %d\n",n);
    return EXIT_SUCCESS;
}
