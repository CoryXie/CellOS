/* sys/stat.h - data returned by the stat() function */

/*
APPLICATION USAGE

    Use of the macros is recommended for determining the type of a file.

RATIONALE

    A conforming C-language application must include <sys/stat.h> for
    functions that have arguments or return values of type mode_t, so
    that symbolic values for that type can be used. An alternative would
    be to require that these constants are also defined by including
    <sys.h>.

    The S_ISUID and S_ISGID bits may be cleared on any write, not just
    on open(), as some historical implementations do.

    System calls that update the time entry fields in the stat structure
    must be documented by the implementors. POSIX-conforming systems
    should not update the time entry fields for functions listed in the
    System Interfaces volume of POSIX.1-2008 unless the standard requires
    that they do, except in the case of documented extensions to the
    standard.

    Upon assignment, file timestamps are immediately converted to the
    resolution of the file system by truncation (i.e., the recorded time
    can be older than the actual time). For example, if the file system
    resolution is 1 microsecond, then a conforming stat() must always
    return an st_mtim.tv_nsec that is a multiple of 1000. Some older
    implementations returned higher-resolution timestamps while the inode
    information was cached, and then spontaneously truncated the tv_nsec
    fields when they were stored to and retrieved from disk, but this
    behavior does not conform.

    Note that st_dev must be unique within a Local Area Network (LAN)
    in a "system'' made up of multiple computers' file systems connected
    by a LAN.

    Networked implementations of a POSIX-conforming system must guarantee
    that all files visible within the file tree (including parts of the
    tree that may be remotely mounted from other machines on the network)
    on each individual processor are uniquely identified by the combination
    of the st_ino and st_dev fields.

    The unit for the st_blocks member of the stat structure is not defined
    within POSIX.1-2008. In some implementations it is 512 bytes. It may
    differ on a file system basis. There is no correlation between values
    of the st_blocks and st_blksize, and the f_bsize (from <sys/statvfs.h>)
    structure members.

    Traditionally, some implementations defined the multiplier for st_blocks
    in <sys/param.h> as the symbol DEV_BSIZE.

    Some earlier versions of this standard did not specify values for the
    file mode bit macros. The expectation was that some implementors might
    choose to use a different encoding for these bits than the traditional
    one, and that new applications would use symbolic file modes instead
    of numeric. This version of the standard specifies the traditional
    encoding, in recognition that nearly 20 years after the first
    publication of this standard numeric file modes are still in
    widespread use by application developers, and that all conforming
    implementations still use the traditional encoding.

FUTURE DIRECTIONS

    No new S_IFMT symbolic names for the file type values of mode_t will
    be defined by POSIX.1-2008; if new file types are required, they will
    only be testable through S_ISxx() or S_TYPEISxxx() macros instead.
*/
#ifndef _POSIX_SYS_STAT_H
#define _POSIX_SYS_STAT_H


#include <sys.h>
#include <time.h>

/* The <sys/stat.h> header shall define the stat structure */

typedef struct stat
    {
    dev_t st_dev;            /* Device ID of device containing file. */
    ino_t st_ino;            /* File serial number. */
    mode_t st_mode;          /* Mode of file (see below). */
    nlink_t st_nlink;        /* Number of hard links to the file. */
    uid_t st_uid;            /* User ID of file. */
    gid_t st_gid;            /* Group ID of file. */
    dev_t st_rdev;           /* Device ID (if file is character or block special). */
    off_t st_size;           /* For regular files, the file size in bytes.
                              * For symbolic links, the length in bytes of the
                              * pathname contained in the symbolic link.
                              * For a shared memory object, the length in bytes.
                              * For a typed memory object, the length in bytes.
                              * For other file types, the use of this field is
                              * unspecified. */
    struct timespec st_atim; /* Last data access timestamp. */
    struct timespec st_mtim; /* Last data modification timestamp. */
    struct timespec st_ctim; /* Last file status change timestamp. */
    blksize_t st_blksize;    /* A file system-specific preferred I/O block size
                              * for this object. In some file system types, this
                              * may vary from file to file. */
    blkcnt_t st_blocks;      /* Number of blocks allocated for this object. */
    }stat_t;

/*
 * The st_ino and st_dev fields taken together uniquely identify
 * the file within the system.
 */

/*
 * For compatibility with earlier versions of this standard,
 * the st_atime macro shall be defined with the value st_atim.tv_sec.
 * Similarly, st_ctime and st_mtime shall be defined as macros
 * with the values st_ctim.tv_sec and st_mtim.tv_sec, respectively.
 */

#define st_atime	st_atim.tv_sec
#define st_mtime	st_mtim.tv_sec
#define st_ctime	st_ctim.tv_sec
#define st_crtime	st_crtim.tv_sec


/* Extended file types */
#define S_ATTR_DIR			01000000000	/* Attribute directory */
#define S_ATTR				02000000000	/* Attribute */
#define S_INDEX_DIR			04000000000	/* Index (or index directory) */
#define S_STR_INDEX			00100000000	/* String index */
#define S_INT_INDEX			00200000000	/* int32 index */
#define S_UINT_INDEX		00400000000	/* uint32 index */
#define S_LONG_LONG_INDEX	00010000000	/* int64 index */
#define S_ULONG_LONG_INDEX	00020000000	/* uint64 index */
#define S_FLOAT_INDEX		00040000000	/* float index */
#define S_DOUBLE_INDEX		00001000000	/* double index */
#define S_ALLOW_DUPS		00002000000	/* Allow duplicate entries (currently unused) */

/* Link types */
#define	S_LINK_SELF_HEALING	00001000000	/* Link will be updated if you move its target */
#define S_LINK_AUTO_DELETE	00002000000	/* Link will be deleted if you delete its target */

/*
 * The <sys/stat.h> header shall define the following symbolic
 * constants for the file types encoded in type mode_t.
 */
#define S_IFMT				00000170000 /* Type of file */
#define	S_IFSOCK			00000140000 /* Socket */
#define	S_IFLNK				00000120000 /* Symbolic link */
#define S_IFREG 			00000100000 /* Regular */
#define S_IFBLK 			00000060000 /* Block special */
#define S_IFDIR 			00000040000 /* Directory */
#define S_IFCHR 			00000020000 /* Character special */
#define S_IFIFO 			00000010000 /* Fifo */

#define S_ISREG(mode)		(((mode) & S_IFMT) == S_IFREG)
#define S_ISLNK(mode)		(((mode) & S_IFMT) == S_IFLNK)
#define S_ISBLK(mode)		(((mode) & S_IFMT) == S_IFBLK)
#define S_ISDIR(mode)		(((mode) & S_IFMT) == S_IFDIR)
#define S_ISCHR(mode)		(((mode) & S_IFMT) == S_IFCHR)
#define S_ISFIFO(mode)		(((mode) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(mode)		(((mode) & S_IFMT) == S_IFSOCK)
#define S_ISINDEX(mode)		(((mode) & S_INDEX_DIR) == S_INDEX_DIR)

/*
 * The <sys/stat.h> header shall define the following symbolic
 * constants for the file mode bits encoded in type mode_t,
 * with the indicated numeric values. These macros shall expand
 * to an expression which has a type that allows them to be used,
 * either singly or OR'ed together, as the third argument to open()
 * without the need for a mode_t cast.
 */
#define	S_IUMSK 07777		/* User settable bits */

#define S_ISUID 04000       /* Set user id on execution */
#define S_ISGID 02000       /* Set group id on execution */

#define S_ISVTX 01000       /* Save swapped text even after use (sticky bit) */

#define S_IRWXU 00700       /* Read, write, execute: owner */
#define S_IRUSR 00400       /* Read permission: owner */
#define S_IWUSR 00200       /* Write permission: owner */
#define S_IXUSR 00100       /* Execute permission: owner */
#define S_IRWXG 00070       /* Read, write, execute: group */
#define S_IRGRP 00040       /* Read permission: group */
#define S_IWGRP 00020       /* Write permission: group */
#define S_IXGRP 00010       /* Execute permission: group */
#define S_IRWXO 00007       /* Read, write, execute: other */
#define S_IROTH 00004       /* Read permission: other */
#define S_IWOTH 00002       /* Write permission: other */
#define S_IXOTH 00001       /* Execute permission: other */

/* BSD extensions */
#define S_IREAD		S_IRUSR
#define S_IWRITE	S_IWUSR
#define S_IEXEC		S_IXUSR

#define ACCESSPERMS (S_IRWXU | S_IRWXG | S_IRWXO)
#define ALLPERMS    (S_ISUID | S_ISGID | S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO)
#define	DEFFILEMODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
	                /* default file mode, everyone can read/write */

/* Special values for timespec::tv_nsec passed to utimensat(), futimens() */
#define UTIME_NOW	1000000000
#define UTIME_OMIT	1000000001

#ifdef __cplusplus
extern "C" {
#endif

int    chmod(const char *, mode_t);
int    fchmod(int, mode_t);
int    fchmodat(int, const char *, mode_t, int);
int    fstat(int, struct stat *);
int    fstatat(int, const char *restrict, struct stat *restrict, int);
int    futimens(int, const struct timespec [2]);
int    lstat(const char *restrict, struct stat *restrict);
int    mkdir(const char *, mode_t);
int    mkdirat(int, const char *, mode_t);
int    mkfifo(const char *, mode_t);
int    mkfifoat(int, const char *, mode_t);
int    mknod(const char *, mode_t, dev_t);
int    mknodat(int, const char *, mode_t, dev_t);
int    stat(const char *restrict, struct stat *restrict);
mode_t umask(mode_t);
int    utimensat(int, const char *, const struct timespec [2], int);

#ifdef __cplusplus
}
#endif

#endif	/* _POSIX_SYS_STAT_H */
