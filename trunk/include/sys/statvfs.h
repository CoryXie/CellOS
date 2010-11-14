/* sys/statvfs.h - VFS File System information structure */

#ifndef _POSIX_SYS_STAT_VFS_H
#define _POSIX_SYS_STAT_VFS_H

/*
 * The <sys/statvfs.h> header shall define the fsblkcnt_t
 * and fsfilcnt_t types as described in <sys.h>.
 */

#include <sys.h>

/* The <sys/statvfs.h> header shall define the statvfs structure */
typedef struct statvfs
    {
    unsigned long f_bsize;    /* File system block size. */
    unsigned long f_frsize;   /* Fundamental file system block size. */
    fsblkcnt_t    f_blocks;   /* Total number of blocks on file system
                               * in units of f_frsize.  */
    fsblkcnt_t    f_bfree;    /* Total number of free blocks. */
    fsblkcnt_t    f_bavail;   /* Number of free blocks available to
                               * non-privileged process. */
    fsfilcnt_t    f_files;    /* Total number of file serial numbers. */
    fsfilcnt_t    f_ffree;    /* Total number of free file serial numbers. */
    fsfilcnt_t    f_favail;   /* Number of file serial numbers available to
                               * non-privileged process. */
    unsigned long f_fsid;     /* File system ID. */
    unsigned long f_flag;     /* Bit mask of f_flag values. */
    unsigned long f_namemax;  /* Maximum filename length. */

    }statvfs_t;

/*
 * The <sys/statvfs.h> header shall define the following symbolic
 * constants for the f_flag member
 */
#define ST_RDONLY	1   /* Read-only file system. */
#define ST_NOSUID	2   /* Does not support the semantics of the ST_ISUID
                         * and ST_ISGID file mode bits.*/


#ifdef __cplusplus
extern "C" {
#endif

int statvfs(const char *path, struct statvfs *buffer);
int fstatvfs(int descriptor, struct statvfs *buffer);

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_SYS_STAT_VFS_H */
