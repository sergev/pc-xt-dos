#include <disp.h>
#include <rtc.hpp>

#ifndef __OS2__

#ifdef DOS16RM
	#include <zpmapi.h>

	// Create real mode pointer to system ticker
	static long far *real_clock_ptr=MK_FP(0,0x046c);

	// Convert the above to a protected mode pointer
	// N.B. Both created globally as cannot be allocated in an
	// interupt handler

    static long far *prot_clock_ptr=ZPMProtectedPtr(real_clock_ptr,4);
#else
    #include <dos.h>

	// Create real mode pointer to system ticker
	static long far *real_clock_ptr=MK_FP(0,0x046c);

	// Convert the above to a protected mode pointer
	// N.B. Both created globally as cannot be allocated in an
	// interupt handler

    static long far *prot_clock_ptr=_x386_mk_protected_ptr(real_clock_ptr);

#endif

zRTClock *zRTClock::current_clock = 0;

zRTClock::zRTClock(int row, int col, int attr) {
    clockrow = row; clockcol = col; clockattr = attr;
    if (!disp_inited) disp_open();
    int_off();
    if (!zRTClock::current_clock)
        int_intercept(0x1c,&rtc,256);
    state = CLOCKON;
    prev_clock = zRTClock::current_clock;
    zRTClock::current_clock = this;
    int_on();
}

zRTClock::~zRTClock() {
    if (prev_clock) {
        int_off();
        zRTClock::current_clock = prev_clock;
        int_on();
    } else {
    int_off();
    int_restore(0x1c);
    zRTClock::current_clock = 0;
    int_on();
    }
}
/*-------------------------------*/
/* This is the interrupt handler */
/*-------------------------------*/

extern "C" {
static int rtc(struct INT_DATA *)
{
    long ticks,rem;
    unsigned int hours,mins,secs;
    int a = 256*zRTClock::current_clock->clockattr;
    int tim[8], *p;
    if (zRTClock::current_clock->state) {

        int_off();

#ifdef DOS16RM
		ticks=*prot_clock_ptr;		// retreave ticks from pointer
#else
        #ifdef DOS386
            ppeek(0x046c,&ticks,4);
		#else
            peek(0,0x46c,&ticks,4);
		#endif
#endif

        int_on();
        p = tim;
        hours = ticks/65543;
        rem = ticks%65543;
        mins = rem/1092;
        ticks = 100*(rem%1092);
        secs = (int) (ticks/1821);
        *p++ = hours/10+'0'+a;
        *p++ = hours%10+'0'+a;
        *p++ = ':'+a;
        *p++ = mins/10+'0'+a;
        *p++ = mins%10+'0'+a;
        *p++ = ':'+a;
        *p++ = secs/10+'0'+a;
        *p++ = secs%10+'0'+a;
        int i = 0, j = zRTClock::current_clock->clockcol;
        a = zRTClock::current_clock->clockrow;
        for (; i < 8; i++, j++)
            disp_pokew(a,j,tim[i]);
    }
    return 0;
}
}   // end of extern "C"
#endif
