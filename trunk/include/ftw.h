/* ftw.h - file tree traversal */

#ifndef _POSIX_FTW_H
#define _POSIX_FTW_H

/*
 * The <ftw.h> header shall define the stat structure
 * and the symbolic names for st_mode and the file type
 * test macros as described in <sys/stat.h>.
 */

#include <sys/cdefs.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The <ftw.h> header shall define the FTW structure */

struct FTW
    {
    int  base;
    int  level;
    };

/*
 * The <ftw.h> header shall define the following symbolic constants
 * for use as values of the third argument to the application-supplied
 * function that is passed as the second argument to ftw() and nftw().
 */

#define FTW_F   0x00 /* File */
#define FTW_D   0x01 /* Directory */
#define FTW_DNR 0x02 /* Directory without read permission. */
#define FTW_DP  0x03 /* Directory with subdirectories visited. */
#define FTW_NS  0x04 /* Unknown type; stat() failed. */
#define FTW_SL  0x05 /* Symbolic link. */
#define FTW_SLN 0x06 /* Symbolic link that names a nonexistent file. */


/*
 * The <ftw.h> header shall define the following symbolic constants
 * for use as values of the fourth argument to nftw().
 */

#define FTW_PHYS    0x10 /* Physical walk, does not follow symbolic links.
                          * Otherwise, nftw() follows links but does not
                          * walk down any path that crosses itself.
                          */
#define FTW_MOUNT   0x20 /* The walk does not cross a mount point. */
#define FTW_DEPTH   0x30 /* All subdirectories are visited before the
                          * directory itself. */
#define FTW_CHDIR   0x40 /* The walk changes to each directory before
                          * reading it. */


int ftw(const char *, int (*)(const char *, const struct stat *,
    int), int);

int nftw(const char *, int (*)(const char *, const struct stat *,
    int, struct FTW *), int, int);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_FTW_H */
