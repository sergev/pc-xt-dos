#include <iostream.hpp>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <bios.h>
#include <bintree.hpp>
#include "ws.hpp"

//. First we define the rather arbitrary data type, ÝnameÞ, which the tree
//. is to hold.  Then we can declare a binary tree object for this type.
struct name {
    int len;
    char str[1];
};

declare(zGBSearch,name);

char hold[50];      // somewhere to park one

//. Next is the comparison function required to order objects added to
//. the tree. In this case it just compares the strings embedded in the
//. name type.
int namecmp(name *a, name *b)
{
    return(strcmp(a->str,b->str));
}

//. Two slightly different print functions will serve to demonstrate the
//. capability to switch "visit" functions.
void printit(name *a)
{
    cout << a->len << " " << a->str << endl;
}

void print_again(void* p)
{
    name *t = (name *) p;
    cout << t->str << endl;
}

int insert(zGBSearch(name) &b, const char *s)
{
    name *np;
    int n = strlen(s);

    np = (name *) new char[sizeof(name)+n];
    if (!np) {
        cout << "Out of memory" << endl;
        exit(1);
    }
    strcpy(np->str,s);
    np->len = n;
    if ((n = b.insert(np)) == 0)
        return 1;
    if (n != BINTREE_DUP)
        cout << "Insert error on " << s << " " << n << endl;
    return 0;
}

main(int argc, char* argv[])
{

// Declare a zGBSearchname object with comparison function namecmp,
// and initially set to use printit for visits to nodes.  In this case
// all the objects we put into the tree will be dynamically allocated,
// so we ask the tree to take care of deleting them.
    zGBSearch(name) tree(namecmp,PURGE,printit);
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
        wordcount += insert(tree,p);
        if (wordcount == 1)
            strcpy(hold+sizeof(int),p);
    }

// Now remove the word we noted. Fix up hold so it looks like a name
// object.
    np = tree.remove((name *) &hold);
    if (!np)
        cout << "Error " << tree.error() << " removing "
            << hold+sizeof(int) << endl;

// Then return to the start of the file and make sure we can look up
// all the words.
    ws.reset();
    while ((p = ws.next()) != 0) {
        name *t;
        np = (name *) (p-sizeof(int));
        t = tree.seek(np);
        if (!t)
            ++missing;
    }

//. Now visit the nodes in order, executing the printit function for each.
//. Then set a new visit function and repeat the process.
    tree.inorder();

    tree.set_process(print_again);
    tree.inorder();

//. Finally report the findings, then show that attempting to copy
//. the ÝzGBSearch(name) object produces a compile time error message.
    cout << wordcount
         <<
" distinct words, first word in text was repeated "
         << missing
         << " times" << endl;

//    zGBSearch(name) another = tree;
// remove the comment on the line above to see what happens.
    return EXIT_SUCCESS;  
}
