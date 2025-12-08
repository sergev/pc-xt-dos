#ifndef __RTC_HPP
#define __RTC_HPP
#include <int.h>
#include <tools.hpp>

#ifdef DOS386
extern "C" void ppeek(unsigned, void *, size_t);
#endif

enum clock_states { CLOCKOFF, CLOCKON };

class zRTClock {
    friend int cdecl rtc(struct INT_DATA *);

public:

// The default arguments of the constructor place a clock in the
// lower right corner of the screen.
    zRTClock(int = 24, int = 72, int = 0x70);

    void on() { state = CLOCKON; }
    void off() { state = CLOCKOFF; }
    ~zRTClock();
private:
    char state;
    char clockrow, clockcol;
    char clockattr;
    zRTClock *prev_clock;
    static zRTClock *current_clock;
};
#endif
