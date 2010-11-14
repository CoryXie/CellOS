/* dlfcn.h - dynamic linking */

#ifndef _POSIX_DLFCN_H
#define	_POSIX_DLFCN_H

#include <sys.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * The <dlfcn.h> header shall define at least the following symbolic
 * constants for use in the construction of a dlopen() mode argument.
 * 
 * RTLD_LAZY
 *
 * Relocations are performed at an implementation-defined time.
 *
 * RTLD_NOW
 *
 * Relocations are performed when the object is loaded.
 *
 * RTLD_GLOBAL
 *
 * All symbols are available for relocation processing of other modules.
 *
 * RTLD_LOCAL
 *
 * All symbols are not made available for relocation processing by 
 * other modules. 
 */
 
#define RTLD_LAZY	0	
#define RTLD_NOW	1	
#define RTLD_GLOBAL	2	
#define RTLD_LOCAL	3	

int    dlclose(void *);
char  *dlerror(void);
void  *dlopen(const char *, int);
void  *dlsym(void *restrict, const char *restrict);

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_DLFCN_H */
