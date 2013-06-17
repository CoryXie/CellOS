/* sys/utsname.h - system name structure */

#ifndef _POSIX_SYS_UTSNAME_H
#define _POSIX_SYS_UTSNAME_H

#define _SYS_NAMELEN 32

/*
 * The <sys/utsname.h> header shall define the structure utsname.
 *
 * The character arrays are of unspecified size, but the data stored
 * in them shall be terminated by a null byte.
 */
typedef struct utsname
    {
    char  sysname[_SYS_NAMELEN];  /* Name of this implementation of the operating system. */
    char  nodename[_SYS_NAMELEN]; /* Name of this node within the communications
                                   * network to which this node is attached, if any. */
    char  release[_SYS_NAMELEN];  /* Current release level of this implementation. */
    char  version[_SYS_NAMELEN];  /* Current version level of this release. */
    char  machine[_SYS_NAMELEN];  /* Name of the hardware type on which the system is running.*/

    }utsname_t;


#ifdef __cplusplus
extern "C" {
#endif

int uname(struct utsname *);

#ifdef __cplusplus
}
#endif

#endif	/* _POSIX_SYS_UTSNAME_H */
