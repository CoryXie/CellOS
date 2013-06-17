/* nl_types.h - data types */

#ifndef _POSIX_NL_TYPES_H
#define _POSIX_NL_TYPES_H

#include <sys/cdefs.h>

/*
 * NL_SETD
 *
 * Used by gencat when no $set directive is specified in a 
 * message text source file. This constant can be passed as 
 * the value of set_id on subsequent calls to catgets() 
 * (that is, to retrieve messages from the default message set).
 * The value of NL_SETD is implementation-defined.
 */
 
#define NL_SETD         0

/*
 * NL_CAT_LOCALE
 *
 * Value that must be passed as the oflag argument to catopen()
 * to ensure that message catalog selection depends on the 
 * LC_MESSAGES locale category, rather than directly on the LANG 
 * environment variable.
 */
 
#define NL_CAT_LOCALE   1

/*
 * nl_item
 *
 * Used by nl_langinfo() to identify items of langinfo data.
 * Values of objects of type nl_item are defined in <langinfo.h>.
 */
 
typedef int 	nl_item;

/*
 * nl_catd
 *
 * Used by the message catalog functions catopen(), catgets(),
 * and catclose() to identify a catalog descriptor.
 */
 
typedef void*	nl_catd;

__BEGIN_DECLS

int       catclose(nl_catd);
char     *catgets(nl_catd, int, int, const char *);
nl_catd   catopen(const char *, int);

__END_DECLS


#endif /* _POSIX_NL_TYPES_H */
