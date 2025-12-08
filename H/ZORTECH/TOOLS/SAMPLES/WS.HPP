#include <tools.hpp>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

class wordsource {
public:
    wordsource(const char *filename);
    ~wordsource();
    const char *next();
    void reset();
    int error() { int e = err; err = 0; return e; }
private:
    void fillbuf();
    void nextalpha();
    FILE *fp;
    char buffer[133];
    char *p, *q;
    INT16 done, err;
};
