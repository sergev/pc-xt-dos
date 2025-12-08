/*_ zpmstub.c */

#include <stdio.h>
#include <process.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>

#ifdef __ZTC__
unsigned _stack = 0;	/* special value so the '=nnnn' parameter is	*/
			/* passed through to argv[]			*/
int _okbigbuf = 0;	/* disallow big buffers				*/
#endif

/* Add environment strings to be searched here */
char *paths_to_check[] = { "PATH" };

void msg_err(const char *p)
{
#if 1
	write(3,p,strlen(p));
#else
	fputs(p,stderr);
#endif
}

/********************************
 * Search for name in environment variables
 * and store result in path.
 */

void assign_path(char *name, char *path)
{
#if 1
	strcpy(path,name);
#else
	int i;
	for (i = 0; i < sizeof(paths_to_check)/sizeof(paths_to_check[0]); i++)
	{
		_searchenv(name,paths_to_check[i],path);
		if (path[0])
			return;
	}	
	msg_err("Cannot find ");
	msg_err(name);
	msg_err(".\r\nSet the PATH to the directory which contains ");
	msg_err(name);
	msg_err(" and try again.\r\n");
	exit(EXIT_FAILURE);
#endif
}

int _cdecl main(int argc, char **argv)
{
	char **av;
	char *name;
	char arg0path[128];
	int ac;

#if 0
	/* Print out banner	*/
	static const char copyright[] =
		"ZPM Protected Mode Run-time\r\n"
		"Copyright (c) 1990-1991 Rational Systems, Inc.\r\n";
	write(2,copyright,sizeof(copyright) - 1);
#endif

	/* Allocate arg vector with room for new av[0] and terminal NULL */
	av = (char **) malloc(sizeof(char*) * (argc+2));
	if (!av)
	{
		msg_err("Out of memory.\r\n");
		exit(EXIT_FAILURE);
	}

	/* Locate the loader/debugger */
	name = "ZPM.EXE";
	assign_path(name,arg0path);
	av[0] = arg0path;
			
	/* Copy arguments */
	for(ac = 0; ac < argc; ac++)
		av[ac + 1] = argv[ac];
	av[ac + 1] = NULL;			/* Terminate list */

	execvp(av[0], av);

	/* If the execvp() fails, we return here	*/
	msg_err("Cannot EXEC ");
	msg_err(av[0]);
	msg_err(". DOS error = ");
	msg_err(itoa(errno,arg0path,10));
	msg_err(".\r\n");

	return EXIT_FAILURE;
}
