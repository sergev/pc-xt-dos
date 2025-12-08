/*_ response.c   Fri Aug 24 1990   Modified by: Walter Bright */
/* Copyright (C) 1990 by Walter Bright		*/
/* All rights reserved.				*/
/* Written by Walter Bright			*/

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

/*********************************
 * #include <stdlib.h>
 * int _pascal response_expand(int *pargc,char ***pargv);
 *
 * Expand any response files in command line.
 * Response files are arguments that look like:
 *	@NAME
 * The name is first searched for in the environment. If it is not
 * there, it is searched for as a file name.
 * Arguments are separated by spaces, tabs, or newlines. These can be
 * imbedded within arguments by enclosing the argument in '' or "".
 * Recursively expands nested response files.
 *
 * To use, put the line:
 *	response_expand(&argc,&argv);
 * as the first executable statement in main(int argc, char **argv).
 * argc and argv are adjusted to be the new command line arguments
 * after response file expansion.
 *
 * Zortech's MAKE program can be notified that a program can accept
 * long command lines via environment variables by preceding the rule
 * line for the program with a *.
 *
 * Returns:
 *	0	success
 *	!=0	failure (argc, argv unchanged)
 */

struct Narg
{	int argc;	/* arg count		*/
	int argvmax;	/* dimension of nargv[]	*/
	char **argv;
};

static int _near _pascal addargp(struct Narg __ss *n,char *p)
{
    /* The 2 is to always allow room for a NULL argp at the end	*/
    if (n->argc + 2 >= n->argvmax)
    {   n->argvmax = n->argc + 2;
	n->argv = (char **) realloc(n->argv,n->argvmax * sizeof(char *));
	if (!n->argv)
	    return 1;
    }
    n->argv[n->argc++] = p;
    return 0;
}

int _pascal response_expand(pargc,pargv)
int *pargc;
char ***pargv;
{
    struct Narg n;
    int i;
    char *cp;
    int recurse = 0;

    n.argc = 0;
    n.argvmax = 0;		/* dimension of n.argv[]		*/
    n.argv = NULL;
    for(i=0; i<*pargc; ++i)
    {
	cp = (*pargv)[i];
	if (*cp == '@')
	{
	    char *buffer;
	    char *bufend;
	    char *p;
	    char tc;

	    cp++;
	    p = getenv(cp);
	    if (p)
	    {
		buffer = strdup(p);
		if (!buffer)
		    goto noexpand;
		bufend = buffer + strlen(buffer);
	    }
	    else
	    {   long length;
		int fd;
		int nread;
		size_t len;

		length = filesize(cp);
		if (length & 0xFFFF0000)	/* error or file too big */
		    goto noexpand;
		len = length;
		buffer = malloc(len + 1);
		if (!buffer)
		    goto noexpand;
		bufend = &buffer[len];

		/* Read file into buffer	*/
		fd = open(cp,O_RDONLY);
		if (fd == -1)
		    goto noexpand;
		nread = read(fd,buffer,len);
		close(fd);

		if (nread != len)
		    goto noexpand;
	    }

	    for (p = buffer; p < bufend; p++)
	    {
		switch (*p)
		{   case 26:		/* ^Z marks end of file		*/
			goto L2;
		    case 0xD:
		    case 0:
		    case ' ':
		    case '\t':
		    case 0xA:
			*p = 0;		/* separate arguments		*/
			break;
		    case '\'':
		    case '"':
			tc = *p++;
			if (addargp(&n,p))
			    goto noexpand;
			for (; 1; p++)
			{   if (p >= bufend)
				goto L2;
			    switch (*p)
			    {	case 26:
				    goto L2;
				case '\'':
				case '"':
				    if (tc == *p)
				    {	*p = 0;		/* remove trailing quote */
					goto L1;
				    }
				break;
			    }
			}
		    L1:
			break;
		    case '@':
			recurse = 1;
			/* FALL-THROUGH */
		    default:		/* start of new argument	*/
			if (addargp(&n,p))
			    goto noexpand;
			for (; 1; p++)
			{
			    if (p >= bufend)
				goto L2;
			    switch (*p)
			    {	case 26:
				    goto L2;
				case 0xD:
				case 0:
				case ' ':
				case '\t':
				case 0xA:
				    *p = 0;	/* separate arguments	*/
				    goto L1;
			    }
			}
			break;
		}
	    }
	L2:
	    *p = 0;			/* terminate last argument	*/
	}
	else if (addargp(&n,(*pargv)[i]))
	    goto noexpand;
    }
    n.argv[n.argc] = NULL;
    if (recurse)
    {	/* Recursively expand @filename	*/
	if (response_expand(&n.argc,&n.argv))
	    goto noexpand;
    }
    *pargc = n.argc;
    *pargv = n.argv;
    return 0;				/* success			*/

noexpand:				/* error			*/
    free(n.argv);
    /* BUG: any file buffers are not free'd	*/
    return 1;
}
