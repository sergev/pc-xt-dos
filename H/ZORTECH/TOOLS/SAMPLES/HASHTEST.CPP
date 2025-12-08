#include <iostream.hpp>
#include <ctype.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <bios.h>
#include <hash.hpp>
#include "ws.hpp"

#define NL endl

// First we need an arbitrary data type to test the hash table class
// out on.  Then we can declare a hash table of the appropriate sort.
struct name {
    int len;
    char str[1];
};

declare(zGHSearch,name);

char hold[50];

// We also need to provide functions to compare name objects, a
// visit function to be executed when table entries are visited by
// the inorder function, and a hash number calculation function
// for name objects.  Notice that the match function does
// not have to do anything sophisticated in this case.  It just needs
// to return zero if the objects are equal, nonzero otherwise.
int match(name *a, name *b)
{
    return strcmp(a->str,b->str);
}

void foo(name *p)
{
    cout << p->str << endl;
}

unsigned name_hash(name *p, unsigned tablesize)
{
    return zAlphaHash(p->str, tablesize);
}

// The reporting function needs a pointer to a function it can use to
// make a string out of an object stored in the table.
const char *xlate(name *p)
{
    return (const char *) p->str;
}

int insert(zGHSearch(name) &h, const char *s)
{
    name *dp;
    int n = strlen(s);

    dp = (name *) new char[sizeof(name)+n];
    strcpy(dp->str,s);
    dp->len = n;

    if (!h.insert(dp))
        return 1;
    return 0;
}

main(int argc, char* argv[])
{

// Declare a hash table for type name, with matching function match,
// table size 1024, automatic purging of table contents, and visit
// function foo.
    zGHSearch(name) hasher(match,1024,name_hash,1,foo);
    FILE *fp;
    name *np;
    int wordcount = 0, missing = 0;
    const char *p;

// The test program needs a text file to build the tree from, and the file
// name should be given as a command line argument.
    if (argc != 2) {
        cout << "Give a text file name as argument" << endl;
        return EXIT_FAILURE;
    }
    wordsource ws(argv[1]);
    if (ws.error()) {
        cout << "Source open failed" << endl;
        return EXIT_FAILURE;
    }

// Now extract words from the source file, and populate the table,
// keeping an item to delete.
    while ((p = ws.next()) != 0) {
        wordcount += insert(hasher,p);
        if (wordcount == 1)
            strcpy(hold+sizeof(int),p);
    }

// Delete the saved item, then scan through the table to make sure
// that each object derived from the text can be found in the table.
    hasher.remove((name *) hold);

// Then return to the start of the file and make sure we can look up
// all the words.
    ws.reset();
    while ((p = ws.next()) != 0) {
        name *t;
        np = (name *) (p-sizeof(int));
        t = hasher.seek(np);
        if (!t)
            ++missing;
    }

// Finally, exersise the inorder function, report the findings, and
// show that copying produces a compile time error message.
    hasher.inorder();

    cout << wordcount
         <<
" distinct words, first word in text was repeated "
         << missing
         << " times" << endl;

    zHashTable_report(hasher,&xlate);

//    zGHSearch(name) another = hasher;
// Copying is not allowed. Remove the comment to check.
    return EXIT_SUCCESS;
}
