#include <stdio.h>
#include <stdlib.h>

void	vcm_test3(int i)
{
	void	*p;

	do {												/*  soak up memory  */
		p = malloc(0x4000);
	} while (p);
}
