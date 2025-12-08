#ifndef __IOBJECT_HPP
#define __IOBJECT_HPP
#include <tools.hpp>
#include <event.hpp>
#include <rect.hpp>

// Interactive objects will either handle an event, attempt to handle
// it and fail, be cancelled or interrupted by an event, or completely
// ignore it and effectively pass it on for processing by another
//  object. We need an enumeration to describe these outcomes.
enum Interaction {
    HANDLED = 0,
    ERROR = -1,
    CANCELLED = -2,
    INTERRUPTED = -3,
    IGNORED = -4
};

// The interactive object class has class zRectangle as a public
// virtual base.
class zInteractiveObject : virtual public zRectangle {
public:

// zInteractiveObject is an abstract class with a single virtual
// function which asserts that objects derived from this type have the
// capability to process events.  They may also emit events,  hence
// the arguments to the process event function are references.  The
// return value will have some meaning in the context of the object
// if it is positive,  or will be one of the values from enum
// Interaction.
    virtual int process_event(zEvent &e, INT16 &mbs)
        { return IGNORED; }

// As a precaution for the future there is a function to report
// the objects state.
    int state() { return _state; }

// Since we will wish to deal with interactive objects via generic
// pointers the destructor also needs to be virtual.
    virtual ~zInteractiveObject() {}
protected:
    zInteractiveObject() { _state = 0; }
    int _state;
};
#endif
