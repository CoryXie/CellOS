/* wordexp.h - word-expansion types */

#ifndef _POSIX_WORDEXP_H
#define _POSIX_WORDEXP_H

#include <sys.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct wordexp_t
    {
    size_t we_wordc;      /* Count of words matched by words. */
    char **we_wordv;      /* Pointer to list of expanded words. */
    size_t we_offs;       /* Slots to reserve at the beginning of we_wordv. */
    }wordexp_t;

/*
 * The <wordexp.h> header shall define the following symbolic constants
 * for use as flags for the wordexp() function
 */
#define WRDE_DOOFFS     0x0001  /* Number of null pointers to prepend to we_wordv. */
#define WRDE_APPEND     0x0002  /* Append words to those previously generated. */
#define WRDE_NOCMD      0x0004  /* Fail if command substitution is requested. */
#define WRDE_REUSE      0x0008  /* The pwordexp argument was passed to a previous
                                 * successful call to wordexp(), and has not been
                                 * passed to wordfree(). The result is the same
                                 * as if the application had called wordfree()
                                 * and then called wordexp() without WRDE_REUSE. */
#define WRDE_SHOWERR    0x0010  /* Do not redirect stderr to /dev/null. */
#define WRDE_UNDEF      0x0020  /* Report error on an attempt to expand an
                                 * undefined shell variable. */

/*
 * The <wordexp.h> header shall define the following symbolic constants
 * as error return values
 */
enum
    {
    WRDE_SUCCESS = 0,/* OK */
    WRDE_NOSPACE = 1,/* Attempt to allocate memory failed.*/
    WRDE_BADCHAR = 2,/* One of the unquoted characters- <newline>,
                      * '|' , '&' , ';' , '<' , '>' , '(' , ')' , '{' , '}'
                      * - appears in words in an inappropriate context.*/
    WRDE_BADVAL = 3, /* Reference to undefined shell variable when WRDE_UNDEF 
                      * is set in flags.*/
    WRDE_CMDSUB = 4, /* Command substitution requested when WRDE_NOCMD was
                      * set in flags.*/
    WRDE_SYNTAX = 5, /* Shell syntax error, such as unbalanced parentheses
                      * or unterminated string.*/
    };

int  wordexp(const char *restrict, wordexp_t *restrict, int);
void wordfree(wordexp_t *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_WORDEXP_H  */
