#include <intvec.hpp>

#ifndef __OS2__

zIntVector::zIntVector()
{
    _state = VOID;
}

zIntVector::zIntVector(unsigned v,
    ihandler fp, unsigned s)
{
    _state = VOID;
    install(v,fp,s);
} 

int zIntVector::install(unsigned vect, ihandler funcptr,
                                unsigned stacksize)
{
    if (_state & INITED) {   // use newhandler to update
        err = IV_ALREADY;
        return IV_ALREADY;
    }
    _state |= INITED;        // set initialised flag
    vector = vect; intserver = funcptr; stack = stacksize;
                            // and copy in parameters
    if (!int_intercept(vect,funcptr,stacksize)) {
        _state |= ACTIVE;    // success - new server now in charge
        err = IV_OK;
        return IV_OK;
    } else {
        err = IV_FAILURE;
        return IV_FAILURE;
    }
}

int zIntVector::suspend()
{
    if (!(_state & ACTIVE)) {
        err = IV_NOTACTIVE;
        return IV_NOTACTIVE;
    }
#if __ZTC__ < 0x215
    if (!int_restore(vector)) {
        _state &= ~ACTIVE;
        err = 0;
        return IV_OK;
    } else {
        err = IV_FAILURE;
        return IV_FAILURE;
    }
#else
    int_restore(vector);
    err = 0;
    return IV_OK;
#endif
}

int zIntVector::reinstate()
{
    if (_state != INITED) {  // can only reinstate if inited and not active
        err = (_state & ACTIVE)? IV_ALREADY: IV_NOCANDO;
        return err;
    }
    if (!int_intercept(vector,intserver,stack)) {
        _state |= ACTIVE;
        return IV_OK;
    } else {
        err = IV_FAILURE;
        return IV_FAILURE;
    }
}

int zIntVector::newhandler(ihandler funcptr,
                                    unsigned stacksize)
{
    if (!(_state & INITED)) { // not initialised, so vector unknown
        err = IV_NOCANDO;
        return IV_NOCANDO;
    }
    if (_state & ACTIVE) {       // active, so put original handler back first
#if __ZTC__ < 0x215
        if (int_restore(vector)) {
            err = IV_FAILURE;
            return IV_FAILURE;
        }
#else
        int_restore(vector);
#endif
    }
    _state &= ~ACTIVE;
    intserver = funcptr;
    if (!int_intercept(vector,intserver,stack)) {
        _state |= ACTIVE;        // new handler
        return IV_OK;
    } else {
        err = IV_FAILURE;
        return IV_FAILURE;
    }
}
#endif
