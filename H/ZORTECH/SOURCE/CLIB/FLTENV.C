/*_ fltenv.c   Sat Mar 30 1991   Modified by: Walter Bright */
/* Copyright (C) 1991 by Walter Bright			*/
/* All Rights Reserved					*/
/* Written by Walter Bright				*/

/* NCEG floating point environment support functions	*/

#include <float.h>
#include <fltenv.h>
#include <signal.h>
#include "mt.h"

#define FE_ALL_ROUND	0xC00	/* mask for rounding mode		*/
#define FE_ALL_PREC	0x300	/* mask for precision mode		*/

#if !_MT
extern fenv_t _fe_cur_env;	/* current floating environment		*/
#endif

#if Afltenv1

/*************************************
 */

int fetestexcept(int excepts)
{   fenv_t fe;

    fegetenv(&fe);
    return fe.status & excepts;
}

/*************************************
 */

int feclearexcept(int excepts)
{   fenv_t fe;

    fegetenv(&fe);
    fe.status &= ~(excepts & FE_ALL_EXCEPT);
    fesetenv(&fe);
    return (excepts & ~FE_ALL_EXCEPT) == 0;
}

/*************************************
 */

int fegetexcept(fexcept_t *flagp,int except)
{
    *flagp = except;
    return (except & ~FE_ALL_EXCEPT) == 0;
}

/*************************************
 */

int fesetexcept(const fexcept_t *flagp,int except)
{
#if _MT
    fenv_t fe;

    fegetenv(&fe);
    fe.status |= *flagp & FE_ALL_EXCEPT;
    fesetenv(&fe);
#else
    _fe_cur_env.status |= *flagp & FE_ALL_EXCEPT;
#endif
    return (except & ~FE_ALL_EXCEPT) == 0;
}

/*************************************
 */

int fegetround()
{   fenv_t fe;

    fegetenv(&fe);
    return fe.control & FE_ALL_ROUND;
}

/*************************************
 */

int fesetround(int round)
{
    if (!(round & ~FE_ALL_ROUND))	/* if valid rounding mode	*/
    {   fenv_t fe;

	fegetenv(&fe);
	fe.control = (fe.control & ~FE_ALL_ROUND) | round;
	fe.round = round;
	fesetenv(&fe);
	return 1;			/* valid rounding mode		*/
    }
    else
	return 0;			/* invalid rounding mode	*/
}

/*************************************
 */

int fegetprec()
{   fenv_t fe;

    fegetenv(&fe);
    return fe.control & FE_ALL_PREC;
}

/*************************************
 */

int fesetprec(int prec)
{
    /* Note that the emulator does not support different precisions	*/
    if (prec & ~FE_ALL_PREC || _8087 == 0)
	return 0;			/* invalid precision		*/
    else
    {	fenv_t fe;

	fegetenv(&fe);
	fe.control = (fe.control & ~FE_ALL_PREC) | prec;
	fesetenv(&fe);
	return 1;
    }
}

/*************************************
 */

void fegetenv(fenv_t *envp)
{
    /* Update the environment with anything from the 8087	*/
#if _MT
    fenv_t *pfe;

    pfe = &_getthreaddata()->t_fenv;
#define _fe_cur_env (*pfe)
#endif
    if (_8087)
    {
	/* Read 8087 status, clear on-chip exceptions	*/
	_fe_cur_env.status = (_fe_cur_env.status & FE_ALL_EXCEPT) | _clear87();

	/* Read 8087 control word without changing it	*/
	_fe_cur_env.control = _control87(0,0);

	/* Having a separate copy of the rounding mode is convenient	*/
	_fe_cur_env.round = _fe_cur_env.control & FE_ALL_ROUND;
    }
    *envp = _fe_cur_env;
}

/*************************************
 */

void fesetenv(const fenv_t *envp)
{
#if _MT
    fenv_t *pfe;

    pfe = &_getthreaddata()->t_fenv;
#define _fe_cur_env (*pfe)
#endif
    _fe_cur_env = *envp;
    /* Update the 8087 with the new environment	*/
    if (_8087)
    {
	/* Only support changing precision and rounding controls	*/
	_control87(_fe_cur_env.control,FE_ALL_PREC | FE_ALL_ROUND);
    }
}

/*************************************
 */

void feprocentry(fenv_t *envp)
{
    fegetenv(envp);		/* save current environment	*/
    fesetenv(&FE_DFL_ENV);	/* install default environment	*/
}

#endif

#if Afeexcept

/*************************************
 */

int feraiseexcept(int excepts)
{
    if (excepts & FE_ALL_EXCEPT)
	raise(SIGFPE);		/* raise floating point exception	*/
    return (excepts & ~FE_ALL_EXCEPT) == 0;
}

/*************************************
 */

void feprocexit(const fenv_t *envp)
{   fenv_t fe;

    fegetenv(&fe);
    fesetenv(envp);
    feraiseexcept(fe.status & FE_ALL_EXCEPT);
}

#endif
