#ifndef __CRITERR_HPP
#define __CRITERR_HPP
#include <int.h>
#include <tools.hpp>

enum CE_errorcode {
    CE_WPROT,       // drive write protected
    CE_NOSUCH,      // no such drive
    CE_NOTREADY,
    CE_UNCOM,       // unknown command
    CE_DATERR,      // data error
    CE_BADREQ,      // bad request structure length
    CE_SEEKERR,
    CE_MEDIA,       // unknown media type
    CE_SECNOTF,     // sector not found
    CE_PAPEROUT,
    CE_WRITE,
    CE_READ,
    CE_GENERAL
};

// The decisions which can be taken by the users error handler
// function are represented by the following enumeration:
enum CE_response { CE_IGNORE, CE_RETRY, CE_ABORT, CE_FAIL };

enum CE_errors {
    CE_OK,
    CE_NOTINST = 200
};

typedef int (*uhp)(int, int, int);

class zCEHandler {
public:
    friend int cdecl errhdlr(struct INT_DATA *);

// The class consists only of constructor and destructor, and an error
// reporting function. The constructor takes as its only argument a
// pointer to the function which is to decide what to do about the
// error condition.  When the function is called, it will receive
// three arguments as described in the discussion.
    zCEHandler(uhp);

    int error() { int e = err; err = 0; return e; }
    ~zCEHandler() { previous(); }
private:
    void previous();

    zCEHandler *predecessor;
    uhp function;
    static uhp useptr;
    static zCEHandler *list;
    INT16 err;
};
#endif
