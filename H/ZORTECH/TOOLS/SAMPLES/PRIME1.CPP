#include <stdio.h>
#include <stdlib.h>
#include <intnn.hpp>

zInt20 prime[500];

void print(zInt20 &a, char nl = ' ')
{
    char buf[22];
    a.format(buf);
    printf("%s%c",buf,nl);
}

main()
{
    prime[0] = 1;
    prime[1] = 2;
    zInt20 n = 3;
    int k, j = 2;

//. The algorithm for finding prime numbers is that given by Knuth
//. (The Art of Computer Programming, Vol 1, pp 143-146).
    for (;;) {
        prime[j++] = n;
        if (j > 500) break;
        for (;;) {
            n += 2;
            k = 2;
            for (;; ++k) {
                if (!(n%prime[k])) break;
                if (n/prime[k] <= prime[k]) goto found;
            }
        }
found:
        continue;
    }

//. Now find the biggest gap between successive prime numbers in
//. the list of the first 500.
    n = 0;
    zInt20 before, after;
    for (j = 1; j < 500; ++j) {
        zInt20 gap = prime[j] - prime[j-1];
        if (gap > n) {
            n = gap;
            before = prime[j-1];
            after = prime[j];
        }
        print(prime[j],'\n');
    }
    printf("Biggest gap was ");
    print(n,'\n');
    print(before);
    print(after);
    return EXIT_SUCCESS;
}
