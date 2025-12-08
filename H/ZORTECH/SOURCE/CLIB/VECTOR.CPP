/*_ vector.c   Mon Jan 23 1989   Modified by: Walter Bright */

#include	<stddef.h>

/****************************
 * Call constructors/destructors on an array of objects.
 * Input:
 *	Parray		pointer to array of objects
 *	Nelem		number of those objects
 *	Sizelem		size of each of those objects
 *	Ctor		constructor to call for each of those objects
 */

typedef void *(*ctor_t)(void *);
typedef void *(*dtor_t)(int,void *);

#if 0

/* New ARM 2.1 style, where Nelem is stored as the first
   element in the array
 */
void *_vec_newa(void *Parray,unsigned Nelem,unsigned Sizelem,void *(*Ctor)(void))
{   unsigned n;
    void *Pret;

    if (Parray == 0)
    {	Parray = new char[((size_t) Nelem) * Sizelem + sizeof(size_t)];
	Pret = Parray;
	if (!Pret)
	    return Pret;
	*((size_t *)Parray)++ = Nelem;
    }
    else
	Pret = Parray;
    if (Ctor != 0)
    {
	for (n = 0; n < Nelem; n++)
	    (*(ctor_t)Ctor)((char *)Parray + n * Sizelem);
    }
    return Pret;
}

void _vec_deletea(void *Parray,unsigned Nelem,unsigned Sizelem,
	int (*Dtor)(void),int Free)
{   unsigned int n;

    if (Parray)
    {
	if (Dtor != 0)
	{
	    if (Free & 1)
		Nelem = ((size_t *)Parray)[-1];	/* retrieve number of objects	*/
	    for (n = Nelem; n--;)
		(*(dtor_t)Dtor)(2,(char *)Parray + n * Sizelem);
	}
	if (Free & 1)
	{   ((size_t *)Parray)--;
	    delete Parray;
	}
    }
}

#endif

/* 2.0 style */
void *_vec_new(void *Parray,unsigned Nelem,unsigned Sizelem,void *(*Ctor)(void))
{   unsigned n;
	
    if (Parray == 0)
    {	Parray = new char[((size_t) Nelem) * Sizelem + sizeof(size_t)];
	if (!Parray)
	    return Parray;
    }
    if (Ctor != 0)
    {	for (n = 0; n < Nelem; n++)
	    (*(ctor_t)Ctor)((char *)Parray + n * Sizelem);
    }
    return Parray;
}

void _vec_delete(void *Parray,unsigned Nelem,unsigned Sizelem,
	int (*Dtor)(void),int Free)
{   unsigned int n;

    if (Parray)
    {
	if (Dtor != 0)
	{   for (n = Nelem; n--;)
		(*(dtor_t)Dtor)(2,(char *)Parray + n * Sizelem);
	}
	if (Free & 1)
	    delete Parray;
    }
}
