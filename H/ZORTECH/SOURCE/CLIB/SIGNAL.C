/*_ signal.c   Tue May 22 1990   Modified by: Walter Bright */
/* Copyright (c) 1987-1990 by Walter Bright	*/
/* All Rights Reserved				*/
/* Written by Walter Bright			*/

/* Signal handling routines			*/

#include	<signal.h>
#include	<errno.h>
#include	<dos.h>
#include	<int.h>
#include	<process.h>
#include	<stdlib.h>

static void (*sighandler[])(int) =
{	SIG_DFL,SIG_DFL,SIG_DFL,
	SIG_DFL,SIG_DFL,SIG_DFL,
	SIG_DFL,
};

#if __OS2__

/* Type of OS/2 signal handler	*/
typedef void (_pascal _far _loadds *sh)(unsigned short,unsigned short);

extern int _far _pascal DosSetSigHandler(sh,sh,unsigned short _far *,unsigned short,unsigned short);

/***************************
 * Convert from OS/2 signal number to our signal number.
 */

static int os2_sig[] = {
	-1,			/* not used			*/
	SIGINT,			/* corresponding to ^C		*/
	-1,			/* broken pipe			*/
	SIGTERM,		/* program terminated		*/
	SIGBREAK,		/* control-break		*/
	-1,			/* event flag A			*/
	-1,			/* event flag B			*/
	-1,			/* event flag C			*/
};

/***************************
 * Signal handler for OS/2
 */

void _pascal _far _loadds os2sighandler(unsigned short SigArg, unsigned short SigNum)
{   void (*handler)(int);
    int signum;

    DosSetSigHandler(os2sighandler,0L,NULL,4,SigNum);	/* acknowledge	*/
    signum = os2_sig[SigNum];			/* translate		*/
    if (signum != -1)
    {
	handler = sighandler[signum];
	signal(signum,SIG_IGN);			/* reset to default	*/
	if (handler != SIG_DFL && handler != SIG_IGN)
	    (*handler)(signum);
    }
}

#else

static void (*cb_handler)(int) = 0;

static int ctrl_break_handler(struct INT_DATA *id)
{	void (*handler)(int);

	handler = cb_handler;
	signal(SIGINT,SIG_IGN);		/* reset to default		*/
	if (handler && handler != SIG_IGN)
	    (*handler)(SIGINT);
	return 1;			/* return from interrupt	*/
}

#endif

/****************************
 * Set handler for a signal.
 */

void (*signal(sig,func))(int)
int sig;
void (*func)(int);
{	void (*previous)(int);

	if ((unsigned) sig > sizeof(sighandler)/sizeof(sighandler[0]))
	{	errno = ENOMEM;		/* need to use a better value here */
		return SIG_ERR;		/* unsuccessful			*/
	}
	previous = sighandler[sig];
#if __OS2__
	{
	    int result;
	    int action;
	    int os2sig;

	    /* Determine OS/2 equivalents	*/
	    switch (sig)
	    {   case SIGINT:	os2sig = 1;	break;
		case SIGBREAK:	os2sig = 4;	break;
		case SIGTERM:	os2sig = 3;	break;
		default:
		    goto L1;
	    }
	    if (func == SIG_DFL)
		action = 0;
	    else if (func == SIG_IGN)
		action = 1;
	    else
		action = 2;

	    result = DosSetSigHandler(os2sighandler,0,NULL,action,1);
	    if (result)
	    {	errno = result;
		return SIG_ERR;
	    }
	}
    L1: ;
#else
	if (sig == SIGINT)		/* if control-break		*/
	{
	    if (cb_handler)		/* if already a handler		*/
		int_restore(0x23);	/* restore previous handler	*/
	    cb_handler = 0;
	    if (func != SIG_DFL)
	    {
		if (int_intercept(0x23,ctrl_break_handler,1000) != 0)
		{   /* failed to intercept the interrupt	*/
		    errno = ENOMEM;	/* probably out of memory	*/
		    return SIG_ERR;
		}
		cb_handler = func;
	    }
	}
#endif
	sighandler[sig] = func;
	return previous;
}

/***********************
 * Raise signal sig.
 * Input:
 *	sig	SIGXXXX
 * Returns:
 *	0	successful
 *	!= 0	unsuccessful
 */

int raise(sig)
int sig;
{	void (*func)(int);

	if ((unsigned) sig > sizeof(sighandler)/sizeof(sighandler[0]))
		return 1;		/* unsuccessful			*/
	func = sighandler[sig];
	sighandler[sig] = SIG_DFL;	/* reset to default		*/
	if (func == SIG_DFL)		/* if default			*/
	{   switch (sig)
	    {
		case SIGFPE:
		    break;		/* ignore			*/
		case SIGABRT:
		case SIGILL:
		case SIGINT:
		case SIGSEGV:
		case SIGTERM:
		case SIGBREAK:
		default:
		    _exit(EXIT_FAILURE); /* abort with error		*/
	    }
	}
	else if (func != SIG_IGN)	/* else if not ignored		*/
	    (*func)(sig);
	return 0;
}

/**********************
 * Exit with an error level of 1.
 */

void abort()
{
	raise(SIGABRT);
}


