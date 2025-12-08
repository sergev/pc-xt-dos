/*_ direct.h   Mon Jun  4 1990   Modified by: Walter Bright */

#ifndef __DIRECT_H
#define __DIRECT_H

#if __cplusplus
extern "C" {
#endif

typedef unsigned size_t;

char * _cdecl getcwd(char *,size_t);
int _cdecl _chdrive(int);
int _cdecl _getdrive(void);
int _cdecl chdir(const char *);
int _cdecl mkdir(const char *);
int _cdecl rmdir(const char *);

#if __cplusplus
}
#endif

#endif
