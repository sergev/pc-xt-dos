/*_ _handler.c   Sat Jan  2 1988   Modified by: Walter Bright */

#include <new.hpp>

void (*_new_handler)(void);

/******************************
 * Called to set a new value of _new_handler.
 * Returns:
 *	previous value of _new_handler.
 */

void (*set_new_handler(void (*p)(void)))(void)
{	void (*old_handler)(void);

	old_handler = _new_handler;
	_new_handler = p;
	return old_handler;
}
