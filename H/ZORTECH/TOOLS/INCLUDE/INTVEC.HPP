#ifndef __INTVEC_HPP
#define __INTVEC_HPP
#include <int.h>
#include <errno.h>
#include <tools.hpp>

// The interrupt package expects the user handler function to have
// "C" linkage, and the following signature.
typedef int (cdecl *ihandler)(struct INT_DATA *);

enum iv_states {
    IV_OK,
    IV_ALREADY = 200,   // cant install - already active
    IV_NOTACTIVE,       // cant disable - not active
    IV_NOCANDO,         // don't have a vector number yet
    IV_FAILURE          // failed to redirect vector
};

// Some bit descriptions are also required to describe the objects
// current state.
enum iv_flags {
    VOID, 
    INITED,
    ACTIVE
};

class zIntVector {
public:

// There are two constructors, a default constructor which just
// establishes an object for which the install function can be
// called later.  The second constructor takes arguments specifying
// the required vector, a pointer to the user handler function, and
// the stack size to be reserved for the interrupt handler.  This
// latter is defaulted to 256 bytes, which will be adequate for
// many purposes.  The effect of this second constructor is to install
// the supplied handler immediately.
    zIntVector();
    zIntVector(unsigned v, ihandler fp,
            unsigned s = 256);

// The install function is used with a zIntVector object which has
// not already intercepted a vector.  Its arguments are similar to
// those of the second constructor.
    int install(unsigned v,
            ihandler fp, unsigned s = 256);

// Functions suspend and reinstate put the vector and interrupt
// handler back as they were before the zIntVector object took
// control, and establish control again respectively.
    int suspend();
    int reinstate();

// The newhandler function allows the interrupt handler associated
// with a zIntVector object to be changed on the fly.
    int newhandler(ihandler fp,
                unsigned s = 256);

// Finally there are a group of reporting functions. Ýoperator()
// reports which vector the ÝzIntVector object is tied to,
// state returns the state of the object as a bit pattern made up
// from the elements of enum iv_flags, and error, as usual, reports
// the state of the error flag and resets it.
    unsigned operator()() const
        { return vector; }
    int state() const { return _state; }
    int error() { int e = err; err = 0; return e; }
    ~zIntVector() { suspend(); }
private:
    char err;
    char _state;
                    // use bits 1 and 2 for inited and active
    unsigned INT16 vector;
                    // only 255 vectors max
    unsigned INT16 stack;
                    // stack size needed by current server
    ihandler intserver;
                    // pointer to current server
};
#endif
