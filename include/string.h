/* string.h - string operations */

#ifndef _POSIX_STRING_H
#define _POSIX_STRING_H

#include <sys.h>
#include <locale.h>

#ifdef __cplusplus
extern "C" {
#endif

void    *memccpy(void *restrict, const void *restrict, int, size_t);

void    *memchr(const void *, int, size_t);
int      memcmp(const void *, const void *, size_t);
void    *memcpy(void *restrict, const void *restrict, size_t);
void    *memmove(void *, const void *, size_t);
void    *memset(void *, int, size_t);
char    *stpcpy(char *restrict, const char *restrict);
char    *stpncpy(char *restrict, const char *restrict, size_t);

char    *strcat(char *restrict, const char *restrict);
char    *strchr(const char *, int);
int      strcmp(const char *, const char *);
int      strcoll(const char *, const char *);
int      strcoll_l(const char *, const char *, locale_t);
char    *strsep(char **stringp, const char *delim);

char    *strcpy(char *restrict, const char *restrict);
size_t   strcspn(const char *, const char *);
char    *strdup(const char *);

char    *strerror(int);
char    *strerror_l(int, locale_t);
int      strerror_r(int, char *, size_t);

size_t   strlen(const char *);
char    *strncat(char *restrict, const char *restrict, size_t);
int      strncmp(const char *, const char *, size_t);
char    *strncpy(char *restrict, const char *restrict, size_t);
char    *strndup(const char *, size_t);
size_t   strnlen(const char *, size_t);

char    *strpbrk(const char *, const char *);
char    *strrchr(const char *, int);
char    *strsignal(int);

size_t   strspn(const char *, const char *);
char    *strstr(const char *, const char *);
char    *strtok(char *restrict, const char *restrict);
char    *strtok_r(char *restrict, const char *restrict, char **restrict);

size_t   strxfrm(char *restrict, const char *restrict, size_t);
size_t   strxfrm_l(char *restrict, const char *restrict,
             size_t, locale_t);

/* BSD compatible */

#ifndef bcopy
#define bcopy(s, d, l)  memmove(d, s, l)
#endif

#ifndef bzero
#define bzero(s, l)  memset(s, 0, l)
#endif

#ifndef bcmp
#define bcmp(a, b, len) memcmp(a, b, len)
#endif

#ifndef index
#define index(str, chr) strchr(str, chr)
#endif

#ifndef rindex
#define rindex(str, chr) strrchr(str, chr)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* _POSIX_STRING_H */
