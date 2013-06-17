/* monetary.h - monetary types */

#ifndef _POSIX_MONETARY_H
#define _POSIX_MONETARY_H

/*
 * The <monetary.h> header shall define the locale_t type 
 * as described in <locale.h>.
 *
 * The <monetary.h> header shall define the size_t type as 
 * described in <stddef.h>.
 *
 * The <monetary.h> header shall define the ssize_t type as
 * described in <sys.h>.
 */

#include <locale.h>
#include <stddef.h>
#include <sys/cdefs.h>
#include <sys.h>

__BEGIN_DECLS

ssize_t  strfmon(char *restrict, size_t, const char *restrict, ...);
ssize_t  strfmon_l(char *restrict, size_t, locale_t,
             const char *restrict, ...);

__END_DECLS


#endif /* _POSIX_MONETARY_H */
