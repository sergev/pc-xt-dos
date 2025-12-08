/***************************
 * Generate a unique filename.
 * Input:
 *	template	Pointer to a string that ends with six X's.
 *			The trailing X's are successively replaced with
 *			AA.AAA, then AA.AAB, until a file that doesn't exist is
 *			referred to. If ZZ.ZZZ is reached, mktemp will return
 *			NULL (though you'd be hard pressed to have that
 *			many files!). The filename portion to the left of
 *			XXXXXX must be well-formed, i.e. the path must
 *			exist and the filename characters must be valid
 *			MS-DOS filename characters, and a maximum of 6
 *			filename characters to the left of XXXXXX.
 * Returns:
 *	template if successful.
 *	NULL if unsuccessful.
 */

char *mktemp(char *template)
{   int templen;
    char *p;

    templen = strlen(template);
    if (templen < 6 ||
	strcmp((p = template + templen - 6),"XXXXXX"))
	return 0;			/* invalid format	*/
    strcpy(p,"AA.AAA");
    while (1)
    {	int i;

	if (access(template,0x1FF) == -1)
	    return template;
	i = 5;
	while (p[i] == 'Z')
	{
	    if (i == 0)
		return 0;
	    p[i] = 'A';
	    (i == 3) ? (i -= 2) : i--;
	}
	p[i]++;
    }
}
