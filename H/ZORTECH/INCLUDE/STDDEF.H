/*_ stddef.h   Tue May  9 1989   Modified by: Walter Bright */

#ifndef __STDDEF_H
#define __STDDEF_H	1

#ifdef __STDC__
#define __CDECL
#else
#define __CDECL _cdecl
#endif

#if _MT
extern int * __CDECL _errno(void);
#define errno (*_errno())
extern int _far * __CDECL _threadid;
#else
extern int __CDECL errno;
#endif

typedef int ptrdiff_t;
typedef unsigned size_t;
typedef char wchar_t;

#define offsetof(t,i)	((size_t)((char *)&((t *)0)->i - (char *)0))

#ifndef NULL
#if __COMPACT__ || __LARGE__ || __VCM__
#define NULL 0L
#else
#define NULL 0
#endif
#endif

#if __cplusplus && !__NEW_HPP
extern "C++" {
	inline void *operator new(size_t s, void *at)
	{ return at; }
}
#endif

#endif

