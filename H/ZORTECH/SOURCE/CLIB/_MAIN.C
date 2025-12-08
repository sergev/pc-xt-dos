/*_ _main.c   */
/* Copyright (C) 1985-1991 by Walter Bright	*/
/* All rights reserved.				*/

#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>

/*********************************
 * This takes
 * argc and argv, and does wild card expansion on any arguments
 * containing a '*' or '?', and then calls main() with a new
 * argc and argv. Any errors result in calling main() with the original
 * arguments intact. Arguments containing wild cards that expand to
 * 0 filenames are deleted. Arguments without wild cards are passed
 * straight through.
 *
 * Arguments which are preceded by a " or ', are passed straight
 * through. (cck)
 */

extern int _argc;
extern char **_argv;

int __wildcard = 1;	/* a dummy that can be used to automatically
			   pull in this module. See macro WILDCARDS
			   in dos.h */

/* Using a name starting with "_STI" causes this function to be put	*/
/* into the static constructor table. (!)				*/

void _STI_wildcard()
{
	struct FIND  *p;
	int i,nargc, path_size, nargvmax;
	char **nargv, path[FILENAME_MAX+1], *cp, *end_path;

	nargc = 0;
	nargvmax = 0;		/* dimension of nargv[]		*/
	nargv = NULL;
	for(i=0; i<_argc; ++i)
	{
	    if (nargc + 2 >= nargvmax)
	    {   nargvmax = nargc + 2;
		nargv = (char **) realloc(nargv,nargvmax * sizeof(char *));
		if (!nargv)
		    goto noexpand;
	    }

		cp = _argv[i];				/* cck */

		/* if have expandable names */
		if( !(cp[-1]=='"' || cp[-1]=='\'') &&	/* cck */
		     (strchr(cp, '*') || strchr(cp, '?'))
				)
		{
			end_path = cp + strlen(cp);
			while (	end_path >= cp &&
				*end_path != '\\' &&
				*end_path != '/' &&
				*end_path != ':')
				end_path--;
			path_size = 0;
			if(end_path >= cp){	/* if got a path */
				path_size = end_path - cp + 1;
				memcpy(path, cp, path_size);
			}
			path[path_size] = 0;
			p = findfirst(cp, 0);
			while (p)
			{
				cp = malloc(path_size + strlen(p->name) + 1);
				if (!cp)
					goto noexpand;
				strcpy(cp, path);
				strcat(cp, p->name);
				if (nargc + 2 >= nargvmax)
				{   nargvmax = nargc + 2;
				    nargv = (char **) realloc(nargv,nargvmax * sizeof(char *));
				    if (!nargv)
					goto noexpand;
				}
				nargv[nargc++]  = cp;
				p = findnext();
			}
		}
		else
		    nargv[nargc++] = _argv[i];
	}
	nargv[nargc] = NULL;
	_argc = nargc;
	_argv = nargv;

noexpand:
    ;	/* give up							*/
	/* Partial nargv[] structure is not free'd, but who cares	*/
	/* because we're out of memory anyway				*/
}
