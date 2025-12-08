/*_ strings2.c   Sun Apr 16 1989   Modified by: Walter Bright */
/* Copyright (C) 1985-1989 by Walter Bright	*/
/* All Rights Reserved					*/
/* Written by Walter Bright				*/

/* More obscure string functions		*/

#include	<stdio.h>
#include	<ctype.h>
#include	<stddef.h>
#include	<string.h>
#include	<stdlib.h>

/**************************
 * Search for char c in first n bytes pointed to by p.
 * Returns:
 *	pointer to char found, else NULL.
 */

#if 0	/* now in strings.asm	*/

void *memchr(const void *p,int c,size_t n)
{
	while (n--)
	{	if (*(const char *)p == c)
			return (void *) p;
		p++;
	}
	return NULL;			/* didn't find it		*/
}

#endif /* MEMCHR */


/**************************
 */

#if Astrcspn

size_t strcspn(p1,p2)
const char *p1,*p2;
{	const char *p1start = p1;
	const char *p2start = p2;

	while (*p1)
	{	while (*p2)
		{	if (*p2 == *p1)
				goto L1;
			p2++;
		}
		p2 = p2start;
		p1++;
	}
    L1:
	return p1 - p1start;
}

#endif

/***********************
 * Return pointer to first occurrence in string p1 of any of the characters
 * in the string p2. Return NULL if no occurrences are found.
 */

#if Astrpbrk

char *strpbrk(p1,p2)
const char *p1,*p2;
{	const char *p2start = p2;

	while (*p1)
	{	while (*p2)
		{	if (*p2 == *p1)
				return (void *) p1;
			p2++;
		}
		p2 = p2start;
		p1++;
	}
	return NULL;
}

#endif

/***********************
 * Find last occurrence of char c in string p.
 * Returns:
 *	pointer to last occurrence
 *	NULL if not found
 */

#if Astrrchr

char *strrchr(p,c)
register const char *p;
int c;
{	size_t u;

	u = strlen(p);
	do
	{   if (p[u] == c)
		return (char *) (p + u);
	} while (u--);
	return NULL;
}

#endif

/**************************
 */

#if Astrspn

size_t strspn(p1,p2)
const char *p1,*p2;
{	const char *p1start = p1;
	const char *p2start = p2;

	while (*p1)
	{	while (*p2)
		{	if (*p2 == *p1)
				goto L1;;
			p2++;
		}
		break;			/* it's not in p2		*/
	    L1:
		p2 = p2start;
		p1++;
	}
	return p1 - p1start;
}

#endif

/************************
 */

#if Astrtok

#if _MT
#include	"mt.h"
#endif

char *strtok(p1,p2)
char *p1;
const char *p2;
{
#if _MT
	char **ps = &_getthreaddata()->t_strtok;
#define s (*ps)
#else
	static char *s = NULL;
#endif
	const char *p2start = p2;

	if (!p1)
	{	p1 = s;			/* use old value		*/
		if (!p1)
			return (char *) NULL;
	}
	p1 += strspn(p1,p2);		/* find first char of token	*/
	if (!*p1)
		return (char *) NULL;
	s = strpbrk(p1,p2);		/* find end of token		*/
	if (s)				/* if there was an end		*/
	{	*s = 0;			/* terminate the token		*/
		s++;			/* start of next token		*/
	}
	return p1;
}

#endif

/************************
 */

#if Astrdup

char *strdup(p)
const char *p;
{	char *pnew;

	pnew = malloc(strlen(p) + 1);
	if (pnew)
		strcpy(pnew,p);
	return pnew;
}

#endif

/*************************
 * Compare strings, ignoring differences in case.
 */

#if 0 /* redone in assembly in strings.asm */
int strcmpl(p1,p2)
register const char *p1,*p2;
{
	while (*p1 && *p2)
	{	if (*p1 != *p2)
		{	if (isalpha(*p1) && isalpha(*p2))
			{	int i;

				i = toupper(*p1) - toupper(*p2);
				if (i)
					return i;
			}
			else
				break;
		}
		p1++;
		p2++;
	}
	return *p1 - *p2;
}
#endif

/************************************************************************/
/*  Compares N bytes of two strings, ignoring case.                     */
/*                                                                      */
/*  Copyright 1989-90 by Robert B. Stout dba MicroFirm                  */
/*  All rights reserved.                                                */
/*                                                                      */
/*  Parameters: 1 - first string                                        */
/*              2 - second string                                       */
/*              3 - max number of characters to compare                 */
/*                                                                      */
/*  Returns: zero     - if strings match                                */
/*           positive - if first > second                               */
/*           negative - if first < second                               */
/*                                                                      */
/*  Notes: If the specified number of characters to compare exceeds     */
/*         the length of either string, the search is terminated and    */
/*         the return value is the same as if strcmpl() were called.    */
/*                                                                      */
/************************************************************************/

#if Astrnicmp

int strnicmp(const char *s1, const char *s2, size_t n)
{
    unsigned char c1, c2;

    c1 = c2 = 0;
    while (n--) 
    {
	c1 = toupper(*(s1++));
	c2 = toupper(*(s2++));
	if (!c1 || (c1 = toupper(c1)) != (c2 = toupper(c2)))
	    break;
    }
    return c1 - c2;
}

#endif

/******************************
 * Convert string p to lower case.
 * Returns:
 *	p
 */

#if Astrlwr

char *strlwr(char *p)
{	char *pold = p;

	while (*p)
	{	*p = tolower(*p);
		p++;
	}
	return pold;
}

#endif

/*************************
 * Convert string p to upper case.
 * Returns:
 *	p
 */

#if Astrupr

char *strupr(char *p)
{	char *pold = p;

	while (*p)
	{	*p = toupper(*p);
		p++;
	}
	return pold;
}

#endif

/***********************
 */

#if Astrnset

char *strnset(char *p,int c,size_t n)
{	char *pstart = p;

	while (n-- && *p)
		*p++ = c;
	return pstart;
}

#endif

/**********************
 * Reverse the characters in the string p.
 * Returns:
 *	p
 */

#if Astrrev

char *strrev(char *p)
{	char *pend,*pstart = p;

	pend = p + strlen(p) - 1;
	while (p < pend)
	{	*p ^= *pend;
		*pend ^= *p;
		*p++ ^= *pend--;	/* swap *p and *pend		*/
	}
	return pstart;
}

#endif

/********************
 * Set all the characters in string p to c.
 */

#if Astrset

char *strset(char *p,int c)
{	char *pstart = p;

	while (*p)
		*p++ = c;
	return pstart;
}

#endif

/*********************
 */

#if Aswab

void swab(char *src,char *dst,size_t n)
{	char c;

	n >>= 1;
	while (n--)
	{	c = src[0];	/* in case src and dst are the same	*/
		dst[0] = src[1];
		dst[1] = c;
		dst += 2;
		src += 2;
	}
}

#endif

/**********************************
 * Return pointer to first occurrence of string s2 in s1.
 * Return NULL if s1 is not in s2.
 */

#if Astrstr

#if 0 /* Smaller but slower under many circumstances. */
char *strstr(const char *s1,const char *s2)
{   size_t len2;
    size_t len1;
    char c2 = *s2;

    len1 = strlen(s1);
    len2 = strlen(s2);
    if (!len2)
	return (char *) s1;
    while (len2 <= len1)
    {
	if (c2 == *s1)
	    if (memcmp(s2,s1,len2) == 0)
		return (char *) s1;
	s1++;
	len1--;
    }
    return NULL;
}
#else
#include <limits.h>    /* defines UCHAR_MAX */
/****************************************************************************
See "Algorithms" Second Edition by Robert Sedgewick.  Boyer-Moore string 
search routine.
Modified by Joe Huffman November 3, 1990
****************************************************************************/
char *strstr(const char *text, const char * pattern)
{
  const size_t M = strlen(pattern);
  const unsigned long N = strlen(text);
  const char *p_p = pattern;
  const char *t_p;
  size_t skip[UCHAR_MAX + 1];
  size_t i, j;

  if(M == 0)
    return (char *)text;

  if(M > N)   /* If pattern is longer than the text string. */
    return 0;

#if M_I386 || M_I486
  _memintset((int *)skip, M, UCHAR_MAX + 1);
#else
  { size_t *s_p = skip + UCHAR_MAX;
    do
    {
      *s_p = M;
    }while(s_p-- > skip);
  }
#endif

  p_p = pattern;
  do
  {
    skip[*(const unsigned char *)p_p] = M - 1 - (p_p - pattern);
  } while(*++p_p);

  p_p = pattern + M - 1;
  t_p = text + M - 1;

  while(1)
  {
    char c;

    c = *t_p;
    if(c == *p_p)
    {
      if(p_p - pattern == 0)
        return (char *)t_p;

      t_p--; p_p--;
    }
    else
    {
      size_t step = M - (p_p - pattern);

      if (step < skip[(unsigned char)c])
        step = skip[(unsigned char)c];

      /* If we have run out of text to search in. */
      /* Need cast for case of large strings with 16 bit size_t... */
      if((unsigned long)(t_p - text) + step >= N)
        return 0;

      t_p += step;

      p_p = pattern + M - 1;
    }
  }
}
#endif  /* #if 0 */
#endif  /* #if Astrstr */

/********************************
 * Convert integer i to ascii, and store in string pointed to by a.
 * Input:
 *	a	pointer to string of sufficient length, 17 is always sufficient
 *	base	radix, between 2 and 36 inclusive
 * Returns:
 *	a
 */

#if Aitoa

char *itoa(int i,char *a,int base)
{	unsigned u;
	char buf[1 + sizeof(i) * 8 + 1];
	char __ss *p;
	int sign;

	sign = 1;
	u = i;				/* assume positive		*/
	buf[sizeof(buf) - 1] = 0;
	p = &buf[sizeof(buf) - 2];	/* last character position	*/
	if (base == 10 && (int) u < 0)
	{   sign--;
	    u = -u;
	}
	while (1)
	{	*p = (u % base) + '0';
		if (*p > '9')
		    *p += 'A'-'0'-10;
		if ((u /= base) == 0)
			break;
		p--;
	}
	*--p = '-';
	p += sign;
	return memcpy(a,p,&buf[sizeof(buf)] - p);
}

#endif

/***************************************
 * Converts a long integer to a string.
 * Input:	i	number to be converted
 *		a	buffer in which to build the converted string
 *		base	number base to use for conversion
 *
 * Returns:	a
 */

#if Altoa

char *ltoa(long i, char *a, int base)
{
        unsigned long u;
	char buf[sizeof(long) * 8 + 1];
        char __ss *p;
	char *q = a;

        u = i;
        buf[sizeof(buf) - 1] = 0;
        p = &buf[sizeof(buf) - 2];          /* last character position      */
        if (10 == base && (long) u < 0)
        {
	    *q++ = '-';
	    u = -u;
        }
	while (1)
	{
	    int n;

	    n   = u % base;
	    *p  = n + ((9 < n) ? ('A' - 10) : '0');
	    if ((u /= base) == 0)
		break;
	    p--;
	}
        memcpy(q, p, &buf[sizeof(buf)] - p);
        return a;
}

#endif

