/* regex.h - regular expression matching types */

#ifndef _POSIX_REGEX_H
#define _POSIX_REGEX_H 

#include <sys.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The <regex.h> header shall define the structures and symbolic
 * constants used by the regcomp(), regexec(), regerror(), and 
 * regfree() functions.
 */

/* The <regex.h> header shall define the regex_t structure type */

typedef struct 
    {
	size_t    re_nsub;   /* Number of parenthesized subexpressions. */
    } regex_t;

/*
 * The <regex.h> header shall define the regoff_t type as a signed
 * integer type that can hold the largest value that can be stored 
 * in either a ptrdiff_t type or a ssize_t type.
 */
 
typedef off_t regoff_t;

/*
 * The <regex.h> header shall define the regmatch_t structure type
 */
 
 typedef struct 
    {
	regoff_t rm_so;	/* Byte offset from start of string to start 
	                 * of substring. */
	regoff_t rm_eo; /* Byte offset from start of string of the 
                     * first character after the end of substring. */
    } regmatch_t;

/* 
 * The <regex.h> header shall define the following symbolic constants
 * for the cflags parameter to the regcomp() function
 */

/* regcomp() flags */
#define	REG_BASIC	    0000
#define	REG_EXTENDED	0001 /* Use Extended Regular Expressions.*/
#define	REG_ICASE	    0002 /* Ignore case in match. */
#define	REG_NOSUB	    0004 /* Report only success or fail in regexec().*/
#define	REG_NEWLINE	    0010 /* Change the handling of <newline>.*/
#define	REG_NOSPEC	    0020
#define	REG_PEND	    0040
#define	REG_DUMP	    0200

/* 
 * The <regex.h> header shall define the following symbolic constants 
 * as error return values 
 */
 
/* regerror() flags */
#define	REG_NOMATCH	    1 /* regexec() failed to match. */
#define	REG_BADPAT	    2 /* Invalid regular expression. */
#define	REG_ECOLLATE	3 /* Invalid collating element referenced. */
#define	REG_ECTYPE	    4 /* Invalid character class type referenced. */
#define	REG_EESCAPE	    5 /* Trailing <backslash> character in pattern.*/
#define	REG_ESUBREG	    6 /* Number in \digit invalid or in error.*/
#define	REG_EBRACK	    7 /* "[]" imbalance.*/
#define	REG_EPAREN	    8 /* "\(\)" or "()" imbalance. */
#define	REG_EBRACE	    9 /* "\{\}" imbalance. */
#define	REG_BADBR	    10 /* Content of "\{\}" invalid: not a number, number 
                            * too large, more than two numbers, first larger
                            * than second. */
#define	REG_ERANGE	    11 /* Invalid endpoint in range expression. */
#define	REG_ESPACE	    12 /* Out of memory. */
#define	REG_BADRPT	    13 /* '?' , '*' , or '+' not preceded by valid 
                            * regular expression. */
#define	REG_EMPTY	    14
#define	REG_ASSERT	    15
#define	REG_INVARG	    16
#define	REG_ATOI	    255	    /* convert name to number (!) */
#define	REG_ITOA	    0400	/* convert number to name (!) */

/* 
 * The <regex.h> header shall define the following symbolic constants 
 * for the eflags parameter to the regexec() function
 */
 
/* regexec() flags */
#define	REG_NOTBOL	    00001 /* The <circumflex> character ( '^' ), 
                               * when taken as a special character, does 
                               * not match the beginning of string.*/
#define	REG_NOTEOL	    00002 /* The <dollar-sign> ( '$' ), when taken 
                               * as a special character, does not match
                               * the end of string.*/
#define	REG_STARTEND	00004
#define	REG_TRACE	    00400	/* tracing of execution */
#define	REG_LARGE	    01000	/* force large representation */
#define	REG_BACKR	    02000	/* force use of backref code */

int    regcomp(regex_t *restrict, const char *restrict, int);
size_t regerror(int, const regex_t *restrict, char *restrict, size_t);
int    regexec(const regex_t *restrict, const char *restrict, size_t,
           regmatch_t [restrict], int);
void   regfree(regex_t *);

/* 
 * The implementation may define additional macros or constants 
 * using names beginning with REG_.
 */
 
#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif /* regex.h */
