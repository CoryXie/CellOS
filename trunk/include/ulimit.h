/* ulimit.h - ulimit commands */

#ifndef _POSIX_TAR_H
#define _POSIX_TAR_H


#ifdef __cplusplus
extern "C" {
#endif

/*
 * The <ulimit.h> header shall define the symbolic constants
 * used by the ulimit() function.
 */

#define UL_GETFSIZE 1   /* Get maximum file size. */
#define UL_SETFSIZE 2   /* Set maximum file size. */

long ulimit(int, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_TAR_H */

