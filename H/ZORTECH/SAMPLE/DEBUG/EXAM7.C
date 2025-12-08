/*
	simple test program to show features of buffer protection
*/

#include <stdlib.h>
#include <stdio.h>

char	**p;
unsigned count = 10;

main()
{
	int		i;

	printf("EXAMPLE 7\n");
	p = (char **)malloc(sizeof(char *)*count);
	for (i=0;i<count;i++) {
		p[i] = malloc((i+1)*100);
	}
	for (i=0;i<count;i++) {
		p[i] = realloc(p[i],(i+2)*100);
	}
	for (i=0;i<count;i++) {
		free(p[i]);
	}
	free(p);
	printf("EXAMPLE 7 finished\n");
	exit(0);
}
