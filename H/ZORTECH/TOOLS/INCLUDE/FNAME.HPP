#ifndef __FNAME_HPP
#define __FNAME_HPP
#include <string.h>
#include <errno.h>
#include <tools.hpp>

enum fname_errors {
    FN_OK,
    FN_BADDRIVE = 340,
    FN_BADCOLON,
    FN_PTOOLONG,
    FN_DTOOLONG,
    FN_NTOOLONG,
    FN_XTOOLONG,
    FN_BADCHAR,
    FN_NULL
};

class zFileName {
public:

// There are three constructors, a default constructor (no arguments),
// one which takes a filename string as an argument,  and optionally
// allows characters to be excluded from the filename.  The default
// value for this second parameter is the list of characters which
// are officially prohibited from a DOS file name or extension.
// The final constructor is a copy constructor.
    zFileName();
    zFileName(const char *,
            const char * = " *+=[]:;\",.?/");
    zFileName(const zFileName&);

    zFileName& operator=(const zFileName&);

// The next set of functions give access to the constituent parts of
// the filename.
    char drive() const { return _drive; }
    const char *name() const { return _name; }
    const char *ext() const { return _ext; }
    const char *path() const { return _path; }

// The next three functions modify the zFileName object.  The action
// of the build function is determined by its defaulted final argument.
// If this is non-zero, then the effect is to modify only the components
// of the filename for which there are non-zero arguments.  If the final
// argument is zero, then the arguments override the existing values
// unconditionally.
    int build(char drv, const char *pth ,
            const char *nm, const char *ex, int = 1);

// operator() returns the complete filename.
    const char *operator()();

// The parse function takes apart the supplied string, and determines
// whether it is a legal filename.
    int parse(const char *);

    int error() { int e = err; err = 0; return e; }

    ~zFileName() { delete _path; delete fn; }
private:
    INT16 err;
    char *fn, *_path;
    const char *forbid;
    char _drive;
    char _name[9];
    char _ext[4];
};
#endif
