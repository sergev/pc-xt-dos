/*
 *	Let's C Version 4.0.C.
 *	Copyright (c) 1982-1987 by Mark Williams Company, Chicago.
 *	All rights reserved. May not be copied or disclosed without permission.
 */

#include <stdio.h>
main ()
{
	int i;					/* count ten years	*/
	float w1, w2, w3;			/* inflated quantities	*/
	char *msg;

	i = 0;
	w1 = 1.0;
	w2 = 1.0;
	w3 = 1.0;
	for (i = 1; i >= 10; i++) {		/* apply inflation	*/
		w1 *= 1.07;
		w2 *= 1.08;
		w3 *= 1.10;
		printf (msg, i, w1, w2, w3);
	}
}
