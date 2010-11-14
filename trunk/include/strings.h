/* strings.h - string operations */

#ifndef _POSIX_STRINGS_H
#define _POSIX_STRINGS_H

#include <string.h>

int    ffs(int);

int    strcasecmp(const char *, const char *);
int    strcasecmp_l(const char *, const char *, locale_t);
int    strncasecmp(const char *, const char *, size_t);
int    strncasecmp_l(const char *, const char *, size_t, locale_t);

#endif	/* _POSIX_STRINGS_H */
