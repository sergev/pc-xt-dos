#include "ws.hpp"

wordsource::wordsource(const char *filename)
{
    if ((fp = fopen(filename,"r")) == 0) {
        err = 1;
        return;
    }
    err = done = 0;
    fillbuf();
}

void wordsource::reset()
{
    rewind(fp);
    done = 0;
    fillbuf();
}

wordsource::~wordsource()
{
    fclose(fp);
}

void wordsource::fillbuf()
{
    memset(buffer,0,133);
    if (fgets(buffer,132,fp) == 0) {
        done = 1;
        return;
    }
    for (char *cp = buffer; *cp; ++cp) {
        if (*cp < 0) *cp = ' ';
        *cp = tolower(*cp);
    }
    p = buffer;
}

void wordsource::nextalpha()
{
    for (;;) {
        while (*p && !isalpha(*p)) ++p;
        if (!*p) {
            fillbuf();
            if (done) break;
        } else
            break;
    }
}

const char *wordsource::next()
{
    char *t;
    nextalpha();
    if (done) return 0;
    q = p;
    while (*q && isalpha(*q)) ++q;
    *q = '\0';
    t = p;
    p = q+1;;
    return t;
}
