/* dirent.h - format of directory entries */

/*
 RATIONALE

     Information similar to that in the <dirent.h> header is contained
     in a file <sys/dir.h> in 4.2 BSD and 4.3 BSD. The equivalent in
     these implementations of struct dirent from this volume of POSIX.1-2008
     is struct direct. The filename was changed because the name <sys/dir.h>
     was also used in earlier implementations to refer to definitions
     related to the older access method; this produced name conflicts.
     The name of the structure was changed because this volume of POSIX.1-2008
     does not completely define what is in the structure, so it could be
     different on some implementations from struct direct.

     The name of an array of char of an unspecified size should not be
     used as an lvalue. Use of:

       sizeof(d_name)

     is incorrect; use:

       strlen(d_name)

     instead.

     The array of char d_name is not a fixed size. Implementations may
     need to declare struct dirent with an array size for d_name of 1,
     but the actual number of characters provided matches (or only slightly
     exceeds) the length of the filename.
 */

#ifndef _POSIX_DIRENT_H
#define _POSIX_DIRENT_H

#include <sys.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * NOTE:
 *
 * The character array d_name is of unspecified size,
 * but the number of bytes preceding the terminating
 * null byte shall not exceed {NAME_MAX}.
 */

typedef struct dirent
    {
	dev_t			d_dev;		/* device */
	dev_t			d_pdev;		/* parent device (only for queries) */
	ino_t			d_ino;		/* inode number - File serial number. */
	ino_t			d_pino;		/* parent inode (only for queries) */
	unsigned short	d_reclen;	/* length of this record, not the name */
	char			d_name[1];	/* name of the entry (null byte terminated) */
    } dirent_t;

/*
 * DIR
 *
 * A type representing a directory stream. The DIR type
 * may be an incomplete type.
 */

typedef struct __DIR DIR;

int            alphasort(const struct dirent **, const struct dirent **);
int            closedir(DIR *);
int            dirfd(DIR *);
DIR           *fdopendir(int);
DIR           *opendir(const char *);
struct dirent *readdir(DIR *);
int            readdir_r(DIR *restrict, struct dirent *restrict,
                   struct dirent **restrict);
void           rewinddir(DIR *);
int            scandir(const char *, struct dirent ***,
                   int (*)(const struct dirent *),
                   int (*)(const struct dirent **,
                   const struct dirent **));
void           seekdir(DIR *, long);
long           telldir(DIR *);

#ifdef __cplusplus
}
#endif

#endif	/* _POSIX_DIRENT_H */
