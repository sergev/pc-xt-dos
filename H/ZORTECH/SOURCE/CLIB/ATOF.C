/*_ atof.c   Sun Apr 16 1989   Modified by: Walter Bright */
/* Copyright (C) 1985-1991 by Walter Bright	*/
/* All Rights Reserved					*/

#include	<stdlib.h>
#include	<ctype.h>

/*************************
 * Convert string to double.
 * Terminates on first unrecognized character.
 */

double atof(const char *p)
{   int errnosave = errno;
    double d;

    d = strtod(p,(char **)NULL);
    errno = errnosave;
    return d;
}
