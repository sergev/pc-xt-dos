/* unmangle.c (c) 1990 by Walter Bright
   
   You may distribute or use this file without restriction except that
   the above copyright notice should remain in the source file if it is
   electronically or otherwise distributed.

   People writing tools to interface with Zortech C++ are strongly
   encouraged to use this code.
   
   This file contains a number of functions which allow the un-mangling
   of C++ mangled names as used in Zortech C++ v2. The primary function
   is unmangle_ident which takes a character string containing the mangled
   name and returns a pointer to the un-mangled name.

   written by Phil Murray and Dave Mansell of Zortech - March 1990
   Extensively modified by Walter Bright.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char * _near unmangle_type(char **ps);

#define arraysize(a)	(sizeof(a) / sizeof((a)[0]))

/* A (very) small string handling package	*/

static char * _near str_cat(char *s1,char *s2)
{	char *s;

	if (!s1 || !s2)
	    s = NULL;		/* propagate any errors along	*/
	else
	{   s = (char *) malloc(strlen(s1) + strlen(s2) + 1);
	    if (s)
		strcat(strcpy(s,s1),s2);
	}
	free(s1);
	free(s2);
	return s;
}

static char * _near str_cat1(char *s1,char *s2)
{
	return str_cat(strdup(s1),s2);
}

static char * _near str_cat2(char *s1,char *s2)
{
	return str_cat(s1,strdup(s2));
}

static char * _near str_cat12(char *s1,char *s2)
{
	return str_cat1(s1,strdup(s2));
}


typedef struct unmangle_s {
	char	_near *name;
	char	_near *oper;
} UNMANGLE;

static UNMANGLE	um_table[] = {
	{"ct","1"},
	{"dt","2"},
	{"nw","new"},
	{"dl","delete"},
	{"pl","+"},
	{"mi","-"},
	{"ml","*"},
	{"dv","/"},
	{"md","%"},
	{"er","^"},
	{"ad","&"},
	{"or","|"},
	{"co","~"},
	{"nt","!"},
	{"as","="},
	{"lt","<"},
	{"gt",">"},
	{"uno","!<>="},
	{"lg","<>"},
	{"leg","<>="},
	{"ule","!>"},
	{"ul","!>="},
	{"uge","!<"},
	{"ug","!<="},
	{"ue","!<>"},
	{"apl","+="},
	{"ami","-="},
	{"amu","*="},
	{"adv","/="},
	{"amd","%="},
	{"aer","^="},
	{"aad","&="},
	{"aor","|="},
	{"ls","<<"},
	{"rs",">>"},
	{"ars",">>="},
	{"als","<<="},
	{"eq","=="},
	{"ne","!="},
	{"le","<="},
	{"ge",">="},
	{"aa","&&"},
	{"oo","||"},
	{"pp","++"},
	{"mm","--"},
	{"cl","()"},
	{"vc","[]"},
	{"rf","->"},
	{"cm",","},
	{"rm","->*"},
};

/********************************************
	Accepts a mangled C++ name and returns a simplified
	unmangled name. Returns NULL if no unmangling required or an
	error occurred.

	Note this returns a malloc'd buffer which should be free'd when
	finished with if conversion takes place.
*/

char	*unmangle_ident(char *ident)
{
    char	*result=NULL;
    unsigned	len;
    char	*name,*s,*s1,*cp;
    char	*par=NULL;
    static char opstr[] = "operator ";

    if (*ident != '_')		/* mangled names always start with '_'	*/
	goto ret;
    name = strdup(ident + 1);		/* copy for safety		*/
    if (!name)
	goto done;			/* out of memory		*/

    /* Mangled names have an __ imbedded in them	*/
    if (*name && (s1 = strstr(name + 1,"__")) != NULL)
    {	/* The identifier is mangled	*/

	*s1 = '\0';			/* isolate identifier part	*/
	cp = s1+2;			/* search for class name	*/
	len = 0;
	while (isdigit(*cp))		/* get length of class name	*/
	    len = len * 10 + *(cp++)-'0';
	if (cp - s1 > 2 + 2)		/* if > 2 digit number		*/
	    goto done;			/* don't unmangle		*/

#if !OWNCLASS
	if (len > strlen(cp))
	    goto done;
	else
	{   char *p;

	    p = &cp[len];
	    if (*p)			/* if a type signature follows	*/
	    {
		par = unmangle_type(&p); /* try to unmangle it		*/
		if (!par || *p)		/* if didn't work		*/
		    goto done;
	    }
	}
#endif
	cp[len] = 0;			/* terminate end of class name	*/
	if (!len)
	    cp = NULL;			/* no class name		*/
	else
	    cp = strdup(cp);

	/* Look for special name	*/
	if (name[0] == '_' && name[1] == '_')
	{   char *p;

	    if (name[2] == 'o' && name[3] == 'p')	/* if __op	*/
	    {
		s = name + 4;
		p = str_cat1(opstr,unmangle_type(&s));
		free(name);
		name = p;
	    }
	    else
	    {	int i;

		for (i=0; i < arraysize(um_table); i++)
		{
		    if (strcmp(um_table[i].name,name+2) == 0)
		    {
			free(name);
			p = um_table[i].oper;
			switch (*p)
			{
			    case '2':			/* destructor	*/
				name = str_cat12("~",cp);
				break;
			    case '1':			/* constructor	*/
				name = cp ? strdup(cp) : cp;
				break;
			    default:
				name = str_cat12(opstr,p);
				break;
			}
			break;
		    }
		}
	    }
	}

	if (cp)		  /*  if class pointer else use shortened name  */
	{
	    result = str_cat2(cp,"::");
	    result = str_cat(result,name);	/*  add on actual name  */
	}
	else
	    result = name;
	name = NULL;

	if (par)
	{   result = str_cat(result,par);
	    par = NULL;
	}
    }

done:
    free(par);
    free(name);					/* release copy		*/
ret:
    return result;
}

typedef struct paramangle_s {
	char	type;
	char	_near *desc;
} PARAMANGLE;

static PARAMANGLE type_table[] = {
/* data types */
	{'c',"char"},
	{'s',"short"},
	{'i',"int"},
	{'l',"long"},
	{'f',"float"},
	{'d',"double"},
	{'r',"long double"},
/* pointers and qualifiers */
	{'v',"void"},
	{'b'," __ss *"},
	{'h'," __handle *"},
	{'p'," _near *"},
	{'P'," _far *"},
	{'H'," _huge *"},
	{'R',"&"},
	{'C',"const "},
	{'V',"volatile "},
	{'U',"unsigned "},
	{'S',"signed "},
/* function types */
	{'B',"_near _cdecl "},
	{'D',"_far _cdecl "},
	{'N',"_near "},
	{'F',"_far "},
/* other bits */
	{'e',"..."},
	{'A',""},
};

/*************************************
	Used by unmangle_ident to un-mangle the type safe information.
	
	Note this returns a malloced buffer which should be freed when
	finished with if conversion takes place.
*/

static char * _near unmangle_parmlist(char **str)
{
    char	*result=NULL;
    char	*s;
    char	**op = NULL;
    char	**nop;
    unsigned	i,no;

    no = 0;
    s = *str;
    
    while (isalnum(*s))
    {
	nop = (char **) realloc(op, (no + 1) * sizeof(char *));
	if (!nop)
	    goto done;
	op = nop;
	if (*s == 'T')
	{   unsigned param = s[1] - '0' - 1;

	    if (param >= no)
		goto done;
	    op[no] = strdup(op[param]);	/* copy previous parameter	*/
	    s += 2;
	}
	else
	{   op[no] = unmangle_type(&s);
	    if (!op[no])		/* if unmangle failed		*/
		break;
	}
	no++;
    }

    if (no)
    {
	result = strdup("(");
	for (i = 0; i < no; i++)
	{
	    result = str_cat(result,op[i]);
	    if (i + 1 < no)
		result = str_cat2(result,",");
	}
	result = str_cat2(result,")");
    }
    else
    {

done:
	for (i = 0; i < no; i++)
	    free(op[i]);
    }
    free(op);

    *str = s;
    return result;
}

/**************************************
 */

static char * _near unmangle_type(char **ps)
{
    char *s = *ps;
    char *result = strdup("");
    char *s1,*p;
    int i;
    char c;
    char *pc = "";
    char *pv = "";

    while (1)
    {

    /* Leading digits indicate a class name is coming       */
    if (isdigit(*s))
    { char num[3];

        i = 0;
	do
	{ num[i] = *s;
	    i++;
	    s++;
	    if (i == arraysize(num))        /* class name is too long */
		goto done;
	} while (isdigit(*s));
	num[i] = 0;
	i = atoi(num);              /* get length of class name     */
	c = s[i];
	s[i] = 0;
	result = str_cat1(s,result);
	s += i;
	*s = c;
	goto prepcv;			/* class name forms end of type	*/
    }
    else
    {		
	for (i = 0; 1; i++)
	{   if (i == arraysize(type_table))
		goto error;
	    if (type_table[i].type == *s)
		break;
	}
	s1 = type_table[i].desc;

	switch (*s++)
	{
	    case 'R':				/* ref		*/
	    case 'b':				/* ss ptr	*/
	    case 'h':				/* handle ptr	*/
	    case 'p':				/* near ptr	*/
	    case 'P':				/* far ptr	*/
	    case 'H':				/* huge ptr	*/
			result = str_cat2(result,s1);
			result = str_cat2(result,pc);
			result = str_cat2(result,pv);
			pc = "";
			pv = "";
			continue;
	    case 'C':				/* const	*/
			pc = s1;
			continue;
	    case 'V':				/* volatile	*/
			pv = s1;
			continue;
	    case 'S':				/* signed	*/
			if (*s != 'c')
			    goto error;
			goto L1;
	    case 'U':				/* unsigned	*/
			if (!strchr("csil",*s))
			    goto error;
	    L1:
			result = str_cat(unmangle_type(&s),result);
			/* FALL-THROUGH */
	    default:
			result = str_cat1(s1,result);
	    prepcv:
			result = str_cat1(pc,result);
			result = str_cat1(pv,result);
			pc = "";
			pv = "";
			break;
	    case 'A':				/* array	*/
			if (*result)
			{
			    result = str_cat1("(",result);
			    result = str_cat2(result,")");
			}
			while (1)
			{
			    s1 = s;
			    while (isdigit(*s))
				s++;
			    c = *s;
			    *s = 0;
			    result = str_cat2(result,"[");
			    result = str_cat2(result,s1);
			    result = str_cat2(result,"]");
			    *s = c;
			    if (c != 'A')	/* not array of array	*/
				break;
			    s++;
			}
			result = str_cat(unmangle_type(&s),result);
			break;
	    case 'D':			/* far c function	*/
	    case 'N':			/* near c++ function	*/
	    case 'F':			/* far c++ function	*/
	    case 'B':			/* near c function	*/
			if (*result)
			{
			    result = str_cat2(str_cat1("(",result),")");
			}
			result = str_cat(result,unmangle_parmlist(&s));
			result = str_cat2(result,pc);
			result = str_cat2(result,pv);
			pc = "";
			pv = "";
			if (*s == '_')	/* if return type coming	*/
			{   s++;
			    result = str_cat(unmangle_type(&s),result);
			}
			break;
	}
	break;
    }
    }

done:
    *ps = s;
    return result;

error:
    free(result);
    return NULL;
}

#if 0
void main()
{	char *p;
	p = unmangle_ident("_exe__FSc");
	printf("p = '%s'\n",p);
}
#endif
