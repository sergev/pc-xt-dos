/*	exitstat.c
 *	pushs and pops exit frame, so exit & atexit return points can
 *	be controlled. Useful in Windows and for turning a standalone program
 *	into a subroutine. A maximum of 16 states can be saved.
 *
 *	(C) Copyright 1991 Zortech, Inc.
 *	All Rights Reserved
 *	Written by: G. Eric Engstrom
 */

#include	<setjmp.h>
#include	<exitstat.h>
#include	<string.h>

jmp_buf   _exit_state;

typedef struct EXITSTATT
  {
  jmp_buf   _exit_state;
     void (* _near *_atexitp)(void);
     void (*_atexit_tbl[32+1])(void);
  }
ExitStateT;

static void exit_exit(int errstatus);

static ExitStateT   _state_tbl[16+1];
static ExitStateT  *_exitstatep = NULL;
static    jmp_buf   _exit_state1;

void (* _near *_atexitp)(void);
void (*_atexit_tbl[32+1])(void);
void (*_hookexitp)(int errstatus);

/* exit_PUSHSTATE
 *
 * returns 0		 if successful
 * returns exit code + 1 if unsuccessful or user exited from with in this frame
 */
int exit_PUSHSTATE(void)
{
int   a;

    if (_exitstatep == NULL)		/* if first call */
      {
      _exitstatep = _state_tbl;		/* initialize to start of table */
      }
    if (_exitstatep >= _state_tbl + 16)	/* if table is full	*/
      {
      return 2;				/* fail			*/
      }
    memcpy(&(_exitstatep->_atexit_tbl[0]),&_atexit_tbl[0],sizeof(_atexit_tbl));	/* save atexit function list */
    memcpy(_exitstatep->_exit_state,_exit_state,sizeof(_exit_state));
    _exitstatep->_atexitp = _atexitp;
    _atexitp = NULL;

    ++_exitstatep;

    _hookexitp = exit_exit;
    return 0;				/* current exit state		*/
}

/* exit_popstate
 *
 * restores the previous return state.
 * should be called by user only when exit_pushstate returns 0
 *
 * returns 0	if successful pop
 * returns 1	if no state to restore
 */
int exit_popstate(void)
{
    if (_exitstatep == NULL
      || _exitstatep == _state_tbl)
      {
      return 1;				/* no states to restore */
      }
    --_exitstatep;

    memcpy(&_atexit_tbl[0],&(_exitstatep->_atexit_tbl[0]),sizeof(_atexit_tbl));
    memcpy(_exit_state,_exitstatep->_exit_state,sizeof(_exit_state));
    _atexitp = _exitstatep->_atexitp;

    if (_exitstatep == _state_tbl)
      {
      _hookexitp = NULL;
      }
    return 0;				/* state restore succesfully */
}

static void exit_exit(int errstatus)
  {
    longjmp(_exit_state,errstatus+1);
  }
