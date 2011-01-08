/* glob.h - pathname pattern-matching types */

#ifndef _POSIX_GLOB_H
#define    _POSIX_GLOB_H

#include <sys/cdefs.h>

/*
 * The <glob.h> header shall define the size_t type
 * as described in <sys.h>.
 */

#include <sys.h>

/*
 * The <glob.h> header shall define the structures and symbolic
 * constants used by the glob() function.
 */

struct stat;

/* The <glob.h> header shall define the glob_t structure type */

typedef struct glob
    {
    size_t gl_pathc;    /* Count of paths matched by pattern */
    size_t gl_offs;        /* Slots to reserve at the beginning of gl_pathv */
    char **gl_pathv;    /* Pointer to a list of matched pathnames */
    size_t gl_matchc;    /* Count of paths matching pattern. */
    int    gl_flags;    /* Copy of flags parameter to glob. */
    int (*gl_errfunc)(const char *, int);

    /*
     * Alternate filesystem access methods for glob; replacement
     * versions of closedir(3), readdir(3), opendir(3), stat(2)
     * and lstat(2).
     */

    void (*gl_closedir)(void *);
    struct dirent *(*gl_readdir)(void *);
    void *(*gl_opendir)(const char *);
    int (*gl_lstat)(const char *, struct stat *);
    int (*gl_stat)(const char *, struct stat *);
    } glob_t;

/*
 * The <glob.h> header shall define the following symbolic constants
 * as values for the flags argument.
 */

#define    GLOB_APPEND        0x0001    /* Append generated pathnames to
                                 * those previously obtained. */
#define    GLOB_DOOFFS        0x0002    /* Specify how many null pointers to
                                 * add to the beginning of gl_pathv. */
#define    GLOB_ERR        0x0004    /* Cause glob() to return on error. */
#define    GLOB_MARK        0x0008    /* Each pathname that is a directory that
                                 * matches pattern has a <slash> appended. */
#define    GLOB_NOCHECK    0x0010    /* If pattern does not match any pathname, then
                                 * return a list consisting of only pattern. */
#define    GLOB_NOSORT        0x0020    /* Do not sort the pathnames returned. */
#define    GLOB_NOESCAPE    0x2000    /* Disable backslash escaping. */

/*
 * Error values returned by glob(3)
 *
 * The <glob.h> header shall define the following symbolic constants
 * as error return values.
 */

#define    GLOB_NOSPACE    (-1)    /* An attempt to allocate memory failed. */
#define    GLOB_ABORTED    (-2)    /* The scan was stopped because GLOB_ERR was
                                 * set or (*errfunc)() returned non-zero. */
#define    GLOB_NOMATCH    (-3)    /* The pattern does not match any existing
                                 * pathname, and GLOB_NOCHECK was not set
                                 * in flags. */
#define    GLOB_NOSYS        (-4)    /* Obsolete: source comptability only. */

#define    GLOB_ALTDIRFUNC    0x0040    /* Use alternately specified directory funcs. */
#define    GLOB_BRACE        0x0080    /* Expand braces ala csh. */
#define    GLOB_MAGCHAR    0x0100    /* Pattern had globbing characters. */
#define    GLOB_NOMAGIC    0x0200    /* GLOB_NOCHECK without magic chars (csh). */
#define    GLOB_QUOTE        0x0400    /* Quote special chars with \. */
#define    GLOB_TILDE        0x0800    /* Expand tilde names from the passwd file. */
#define    GLOB_LIMIT        0x1000    /* limit number of returned paths */

/* Source compatibility, these are the old names */

#define GLOB_MAXPATH    GLOB_LIMIT
#define    GLOB_ABEND        GLOB_ABORTED

__BEGIN_DECLS
int    glob(const char *, int, int (*)(const char *, int), glob_t *);
void    globfree(glob_t *);
__END_DECLS

#endif /* _POSIX_GLOB_H */
