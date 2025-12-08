#include <stdlib.h>
#include <iostream.hpp>
#include <io.h>
#include <fname.hpp>

//. It will be convenient for the test program to have some descriptive
//. strings for the various errors which can arise.
char *errmess[] = {
    "No memory",
    "Drive letter out of range",
    "Drive specifier incorrect",
    "Path > 64 characters",
    "Directory names 8 chars only",
    "File name too long",
    "Extension too long",
    "Illegal character",
    ""
};

main(int argc, char *argv[])
{

//. Then declare a null ÝzFileName using no arguments, and use it to
//. parse the filename hopefully supplied as part of the command line.
    zFileName f;
    int x;
    long len;

    if (argc > 1) {
        if ((x = f.parse(argv[1])) != 0) {

//. If parsing finds fault with the filename then describe why, otherwise
//. determine if the file exists, and if it does, then display its
//. size.
            int i = x > ENOMEM? x+1-FN_BADDRIVE: 0;
            cout << errmess[i];
        } else {
            len = filesize((char *) f());
            if (len >= 0L)
                cout << "File length is " << len << endl;
            else
                cout << "File does not exist";
        }
    } else
        cout <<
"file name argument required - bad or good!";
    return EXIT_SUCCESS;
}
