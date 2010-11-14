/* fcntl.h - file control options */

/*
 APPLICATION USAGE

     Although no existing implementation defines AT_SYMLINK_FOLLOW and
     AT_SYMLINK_NOFOLLOW as the same numeric value, POSIX.1-2008 does
     not prohibit that as the two constants are not used with the same
     interfaces.

 RATIONALE

     While many of the symbolic constants introduced in the <fcntl.h>
     header do not strictly need to be used in #if preprocessor directives,
     widespread historic practice has defined them as macros that are
     usable in such constructs, and examination of existing applications
     has shown that they are occasionally used in such a way. Therefore
     it was decided to retain this requirement on an implementation in
     POSIX.1-2008.
 */

#ifndef _POSIX_FCNTL_H
#define _POSIX_FCNTL_H

/*
 * The <fcntl.h> header shall define the mode_t, off_t,
 * and pid_t types as described in <sys.h>.
 */

#include <sys.h>

/*
 * The <fcntl.h> header shall define the symbolic constants
 * for file modes for use as values of mode_t as described
 * in <sys/stat.h>.
 */

#include <sys/stat.h>

/*
 * Inclusion of the <fcntl.h> header may also make visible
 * all symbols from <sys/stat.h> and <unistd.h>.
 */

#include <unistd.h>

/*
 * The <fcntl.h> header shall define the values used for l_whence,
 * SEEK_SET, SEEK_CUR, and SEEK_END as described in <stdio.h>.
 */

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The <fcntl.h> header shall define the following symbolic
 * constants for the cmd argument used by fcntl(). The values
 * shall be unique and shall be suitable for use in #if
 * preprocessing directives.
 */

#define	F_DUPFD			0x0001		/* Duplicate file descriptor. */
#define F_DUPFD_CLOEXEC 0x0002      /* Duplicate file descriptor with the
                                     * close-on-exec flag FD_CLOEXEC set. */
#define	F_GETFD			0x0004		/* Get file descriptor flags. */
#define	F_SETFD			0x0008		/* Set file descriptor flags. */
#define	F_GETFL			0x0010		/* Get file status flags and file access
                                     * modes. */
#define	F_SETFL			0x0020		/* Set file status flags. */
#define F_GETLK         0x0040		/* Get record locking information. */
#define F_SETLK         0x0080		/* Set record locking information. */
#define F_SETLKW        0x0100		/* Set record locking information; wait
                                     * if blocked. */
#define F_GETOWN        0x0200	    /* Get process or process group ID to
                                     * receive SIGURG signals. */
#define F_SETOWN        0x0400	    /* Set process or process group ID to
                                     * receive SIGURG signals. */

/*
 * The <fcntl.h> header shall define the following symbolic
 * constant used for the fcntl() file descriptor flags, which
 * shall be suitable for use in #if preprocessing directives.
 */

#define FD_CLOEXEC		0x0001	    /* Close the file descriptor upon
                                     * execution of an exec family function. */

/*
 * The <fcntl.h> header shall also define the following symbolic
 * constants for the l_type argument used for record locking with
 * fcntl(). The values shall be unique and shall be suitable for
 * use in #if preprocessing directives.
 */

#define F_RDLCK         0x0001		/* Shared or read lock. */
#define F_UNLCK         0x0002		/* Unlock. */
#define F_WRLCK         0x0004		/* Exclusive or write lock. */

/*
 * The <fcntl.h> header shall define the following symbolic
 * constants as file creation flags for use in the oflag value
 * to open() and openat(). The values shall be bitwise-distinct
 * and shall be suitable for use in #if preprocessing directives.
 *
 * The O_TTY_INIT flag can have the value zero and in this case
 * it need not be bitwise-distinct from the other flags.
 */

#define O_TTY_INIT	    0x0000		/* Set the termios structure terminal
                                     * parameters to a state that provides
                                     * conforming behavior; */
#define O_CREAT			0x0001		/* Create file if it does not exist. */
#define	O_EXCL			0x0002		/* Exclusive use flag. */
#define O_NOCTTY		0x0004		/* Do not assign controlling terminal. */
#define O_TRUNC			0x0010		/* Truncate flag. */

/*
 * The <fcntl.h> header shall define the following symbolic
 * constants for use as file status flags for open(), openat(),
 * and fcntl(). The values shall be suitable for use in #if
 * preprocessing directives.
 */

#define O_APPEND		0x00000001	/* Set append mode. */
#define O_DSYNC			0x00000002	/* Write according to synchronized I/O
                                     * data integrity completion. */
#define	O_NONBLOCK		0x00000004	/* Non-blocking mode. */
#define O_RSYNC			0x00000008	/* Synchronized read I/O operations. */
#define O_SYNC			0x00000010	/* Write according to synchronized I/O
                                     * file integrity completion. */

/*
 * The <fcntl.h> header shall define the following symbolic
 * constant for use as the mask for file access modes. The value
 * shall be suitable for use in #if preprocessing directives.
 */

#define O_ACCMODE		0x00000001	/* Mask for file access modes. */

/*
 * The <fcntl.h> header shall define the following symbolic
 * constants for use as the file access modes for open(),
 * openat(), and fcntl(). The values shall be suitable for
 * use in #if preprocessing directives.
 */

#define O_RDONLY		0x0000		/* Open for reading only. */
#define O_WRONLY		0x0001		/* Open for writing only. */
#define O_RDWR			0x0002		/* Open for reading and writing. */
#define O_EXEC			0x0004		/* Open for execute only (non-directory
                                     * files). The result is unspecified if
                                     * this flag is applied to a directory. */
#define O_SEARCH	    0x0008		/* Open directory for search only. The
                                     * result is unspecified if this flag is
                                     * applied to a non-directory file. */

/*
 * The <fcntl.h> header shall define the following symbolic
 * constants as values for the flag used by open() and openat():
 */

#define O_CLOEXEC		0x00001000	/* The FD_CLOEXEC flag associated with
                                     * the new descriptor shall be set to
                                     * close the file descriptor upon
                                     * execution of an exec family function. */
#define	O_DIRECTORY		0x00002000  /* Fail if not a directory. */
#define O_NOFOLLOW		0x00004000	/* Do not follow symbolic links. */

/*
 * The <fcntl.h> header shall define the following symbolic
 * constant as a special value used in place of a file descriptor
 * for the *at() functions which take a directory file descriptor
 * as a parameter.
 */

#define AT_FDCWD		(-1)		/* Use the current working directory
                                     * to determine the target of relative
                                     * file paths. */

/*
 * The <fcntl.h> header shall define the following symbolic
 * constant as a value for the flag used by fstatat(), fchmodat(),
 * fchownat(), and utimensat():
 */

#define AT_SYMLINK_NOFOLLOW	0x01	/* Do not follow symbolic links. */

/*
 * The <fcntl.h> header shall define the following symbolic
 * constant as a value for the flag used by linkat():
 */

#define AT_SYMLINK_FOLLOW	0x02	/* Follow symbolic link. */

/*
 * The <fcntl.h> header shall define the following symbolic
 * constant as a value for the flag used by unlinkat():
 */

#define AT_REMOVEDIR		0x04	/* unlinkat() */

/*
 * The <fcntl.h> header shall define the following symbolic
 * constant as a value for the flag used by faccessat():
 */

#define AT_EACCESS			0x08	/* Check access using effective user
                                     * and group ID. */

/*
 * The <fcntl.h> header shall define the following symbolic
 * constants for the advice argument used by posix_fadvise():
 *
 * POSIX_FADV_DONTNEED
 *
 *    The application expects that it will not access the specified
 * data in the near future.
 *
 * POSIX_FADV_NOREUSE
 *
 *    The application expects to access the specified data once and
 * then not reuse it thereafter.
 *
 * POSIX_FADV_NORMAL
 *
 *    The application has no advice to give on its behavior with
 * respect to the specified data. It is the default characteristic
 * if no advice is given for an open file.
 *
 * POSIX_FADV_RANDOM
 *
 *    The application expects to access the specified data in a random order.
 *
 * POSIX_FADV_SEQUENTIAL
 *
 *    The application expects to access the specified data sequentially
 * from lower offsets to higher offsets.
 *
 * POSIX_FADV_WILLNEED
 *
 *    The application expects to access the specified data in the near future.
 */

#define POSIX_FADV_DONTNEED	    0x01
#define POSIX_FADV_NOREUSE	    0x02
#define POSIX_FADV_NORMAL	    0x03
#define POSIX_FADV_RANDOM	    0x04
#define POSIX_FADV_SEQUENTIAL	0x05
#define POSIX_FADV_WILLNEED	    0x01

/*
 * The <fcntl.h> header shall define the flock structure
 * describing a file lock.
 */

typedef struct flock
    {
	short	l_type;     /* Type of lock; F_RDLCK, F_WRLCK, F_UNLCK. */
	short	l_whence;   /* Flag for starting offset. */
	off_t	l_start;    /* Relative offset in bytes. */
	off_t	l_len;      /* Size; if 0 then until EOF. */
	pid_t	l_pid;      /* Process ID of the process holding the lock;
	                     * returned with F_GETLK.*/
    }flock_t;

/*
 * The following shall be declared as functions and may
 * also be defined as macros. Function prototypes shall
 * be provided.
 */

int  creat(const char *, mode_t);
int  fcntl(int, int, ...);
int  open(const char *, int, ...);
int  openat(int, const char *, int, ...);
int  posix_fadvise(int, off_t, off_t, int);
int  posix_fallocate(int, off_t, off_t);

#ifdef __cplusplus
}
#endif

#endif	/* _FCNTL_H */
