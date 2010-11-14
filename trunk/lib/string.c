#include <sys.h>
#include <arch.h>
#include <os.h>

void * memcpy(void * dest, const void *src, size_t count)
    {
    char *tmp = (char *) dest, *s = (char *) src;

    while (count--)
        *tmp++ = *s++;

    return dest;
    }

void *memset(void *m, int c, size_t n)
    {
    char *s = (char *) m;

    while (n-- != 0)
        {
        *s++ = (char) c;
        }

    return m;
    }

int memcmp(const void *ptr1, const void *ptr2, size_t count)
    {
    int v = 0;
    char *p1 = (char *)ptr1;
    char *p2 = (char *)ptr2;

    while (count-- > 0 && v == 0)
        {
        v = *(p1++) - *(p2++);
        }

    return v;
    }

/** Macro to implement strtoul() and strtoull(). */
#define __strtoux(type, cp, endp, base)        \
    __extension__ \
    ({ \
        type result = 0, value; \
        if(!base) { \
            if(*cp == '0') { \
                if((tolower(*(++cp)) == 'x') && isxdigit(cp[1])) { \
                    cp++; \
                    base = 16; \
                } else { \
                    base = 8; \
                } \
            } else { \
                base = 10; \
            } \
        } else if(base == 16) { \
            if(cp[0] == '0' && tolower(cp[1]) == 'x') { \
                cp += 2; \
            } \
        } \
        while(isxdigit(*cp) && (value = isdigit(*cp) ? *cp - '0' : tolower(*cp) - 'a' + 10) < base) { \
            result = result * base + value; \
            cp++; \
        } \
        if(endp) { \
            *endp = (char *)cp; \
        } \
        result; \
    })

/** Convert a string to an unsigned long.
 *
 * Converts a string to an unsigned long using the specified number base.
 *
 * @param cp        The start of the string.
 * @param endp        Pointer to the end of the parsed string placed here.
 * @param base        The number base to use (if zero will guess).
 *
 * @return        Converted value.
 */
unsigned long strtoul(const char *cp, char **endp, int base)
    {
    return __strtoux(unsigned long, cp, endp, base);
    }

/** Convert a string to a signed long.
 *
 * Converts a string to an signed long using the specified number base.
 *
 * @param cp        The start of the string.
 * @param endp        Pointer to the end of the parsed string placed here.
 * @param base        The number base to use.
 *
 * @return        Converted value.
 */
long strtol(const char *cp, char **endp, int base)
    {
    if (*cp == '-')
        {
        return -strtoul(cp + 1, endp, base);
        }
    return strtoul(cp, endp, base);
    }

/** Convert a string to an unsigned long long.
 *
 * Converts a string to an unsigned long long using the specified number base.
 *
 * @param cp        The start of the string.
 * @param endp        Pointer to the end of the parsed string placed here.
 * @param base        The number base to use.
 *
 * @return        Converted value.
 */
unsigned long long strtoull(const char *cp, char **endp, int base)
    {
    return __strtoux(unsigned long long, cp, endp, base);
    }

/** Convert a string to an signed long long.
 *
 * Converts a string to an signed long long using the specified number base.
 *
 * @param cp        The start of the string.
 * @param endp        Pointer to the end of the parsed string placed here.
 * @param base        The number base to use.
 *
 * @return        Converted value.
 */
long long strtoll(const char *cp, char **endp, int base)
    {
    if (*cp == '-')
        {
        return -strtoull(cp + 1, endp, base);
        }
    return strtoull(cp, endp, base);
    }

/** Get length of string.
 *
 * Gets the length of the string specified. The length is the number of
 * characters found before a NULL byte.
 *
 * @param str        Pointer to the string.
 *
 * @return        Length of the string.
 */
size_t strlen(const char *str)
    {
    size_t retval;
    for (retval = 0; *str != '\0'; str++) retval++;
    return retval;
    }

/** Get length of string with limit.
 *
 * Gets the length of the string specified. The length is the number of
 * characters found either before a NULL byte or before the maximum length
 * specified.
 *
 * @param str        Pointer to the string.
 * @param count        Maximum length of the string.
 *
 * @return        Length of the string.
 */
size_t strnlen(const char *str, size_t count)
    {
    size_t retval;
    for (retval = 0; *str != '\0' && retval < count; str++) retval++;
    return retval;
    }

/** Compare 2 strings.
 *
 * Compares the two strings specified.
 *
 * @param s1        Pointer to the first string.
 * @param s2        Pointer to the second string.
 *
 * @return        An integer less than, equal to or greater than 0 if
 *            s1 is found, respectively, to be less than, to match,
 *            or to be greater than s2.
 */
int strcmp(const char *s1, const char *s2)
    {
    char x;

    for (;;)
        {
        x = *s1;
        if (x != *s2)
            break;
        if (!x)
            break;
        s1++;
        s2++;
        }
    return x - *s2;
    }

/** Compare 2 strings with a length limit.
 *
 * Compares the two strings specified. Compares at most the number of bytes
 * specified.
 *
 * @param s1        Pointer to the first string.
 * @param s2        Pointer to the second string.
 * @param count        Maximum number of bytes to compare.
 *
 * @return        An integer less than, equal to or greater than 0 if
 *            s1 is found, respectively, to be less than, to match,
 *            or to be greater than s2.
 */
int strncmp(const char *s1, const char *s2, size_t count)
    {
    const char *a = s1;
    const char *b = s2;
    const char *fini = a + count;

    while (a < fini)
        {
        int res = *a - *b;
        if (res)
            return res;
        if (!*a)
            return 0;
        a++;
        b++;
        }
    return 0;
    }

/** Compare 2 strings ignorning case.
 *
 * Compares the two strings specified, ignorning the case of the characters
 * in the strings.
 *
 * @param s1        Pointer to the first string.
 * @param s2        Pointer to the second string.
 *
 * @return        An integer less than, equal to or greater than 0 if
 *            s1 is found, respectively, to be less than, to match,
 *            or to be greater than s2.
 */
int strcasecmp(const char *s1, const char *s2)
    {
    for (;;)
        {
        if (!*s2 || (tolower(*s1) != tolower(*s2)))
            break;
        s1++;
        s2++;
        }
    return tolower(*s1) - tolower(*s2);
    }

/** Compare 2 strings with a length limit ignoring case.
 *
 * Compares the two strings specified, ignorning the case of the characters
 * in the strings. Compares at most the number of bytes specified.
 *
 * @param s1        Pointer to the first string.
 * @param s2        Pointer to the second string.
 * @param count        Maximum number of bytes to compare.
 *
 * @return        An integer less than, equal to or greater than 0 if
 *            s1 is found, respectively, to be less than, to match,
 *            or to be greater than s2.
 */
int strncasecmp(const char *s1, const char *s2, size_t count)
    {
    const char *a = s1;
    const char *b = s2;
    const char *fini = a + count;

    while (a < fini)
        {
        int res = tolower(*a) - tolower(*b);
        if (res)
            return res;
        if (!*a)
            return 0;
        a++;
        b++;
        }
    return 0;
    }

/** Separate a string.
 *
 * Finds the first occurrence of a symbol in the string delim in *stringp.
 * If one is found, the delimeter is replaced by a NULL byte and the pointer
 * pointed to by stringp is updated to point past the string. If no delimeter
 * is found *stringp is made NULL and the token is taken to be the entire
 * string.
 *
 * @param stringp    Pointer to a pointer to the string to separate.
 * @param delim        String containing all possible delimeters.
 *
 * @return        NULL if stringp is NULL, otherwise a pointer to the
 *            token found.
 */
char *strsep(char **stringp, const char *delim)
    {
    char *s;
    const char *spanp;
    int c, sc;
    char *tok;

    if ((s = *stringp) == NULL)
        return (NULL);

    for (tok = s;;)
        {
        c = *s++;
        spanp = delim;
        do
            {
            if ((sc = *spanp++) == c)
                {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;

                *stringp = s;
                return (tok);
                }
            }
        while (sc != 0);
        }
    }

/** Find a character in a string.
 *
 * Finds the first occurrence of a character in the specified string.
 *
 * @param s        Pointer to the string to search.
 * @param c        Character to search for.
 *
 * @return        NULL if token not found, otherwise pointer to token.
 */
char *strchr(const char *s, int c)
    {
    char ch = c;

    for (;;)
        {
        if (*s == ch)
            break;
        else if (!*s)
            return NULL;
        else
            s++;
        }

    return (char *)s;
    }

/** Find a character in a string.
 *
 * Finds the last occurrence of a character in the specified string.
 *
 * @param s        Pointer to the string to search.
 * @param c        Character to search for.
 *
 * @return        NULL if token not found, otherwise pointer to token.
 */
char *strrchr(const char *s, int c)
    {
    const char *l = NULL;

    for (;;)
        {
        if (*s == c)
            l = s;
        if (!*s)
            return (char *)l;
        s++;
        }

    return (char *)l;
    }

/** Copy a string.
 *
 * Copies a string from one place to another. Assumes that the destination
 * is big enough to hold the string.
 *
 * @param dest        Pointer to the destination buffer.
 * @param src        Pointer to the source buffer.
 *
 * @return        The value specified for dest.
 */
char *strcpy(char *dest, const char *src)
    {
    char *d = dest;

    while ((*d++ = *src++));
    
    return dest;
    }

/** Copy a string with a length limit.
 *
 * Copies a string from one place to another. Will copy at most the number
 * of bytes specified.
 *
 * @param dest        Pointer to the destination buffer.
 * @param src        Pointer to the source buffer.
 * @param count        Maximum number of bytes to copy.
 *
 * @return        The value specified for dest.
 */
char *strncpy(char *dest, const char *src, size_t count)
    {
    size_t i;

    for (i = 0; i < count; i++)
        {
        dest[i] = src[i];
        if (!src[i])
            {
            break;
            }
        }
    return dest;
    }

/** Concatenate 2 strings.
 *
 * Appends one string to another.
 *
 * @param dest        Pointer to the string to append to.
 * @param src        Pointer to the string to append.
 *
 * @return        Pointer to dest.
 */
char *strcat(char *dest, const char *src)
    {
    size_t destlen = strlen(dest);
    char *d = dest + destlen;

    while ((*d++ = *src++));
    return dest;
    }

/** Convert string to integer.
 *
 * Converts the string specified to an integer. If a part of the string is
 * encountered that cannot be converted, i.e. not a number, then the function
 * returns the current value.
 *
 * @param s        Pointer to the string to convert.
 *
 * @return        The converted value.
 */
int atoi(const char *s)
    {
    int v = 0;
    int sign = 1;

    while (*s == ' ' || (unsigned int)(*s - 9) < 5u)
        {
        s++;
        }

    switch (*s)
        {
        case '-':
            sign=-1;
        case '+':
            s++;
        }

    while ((unsigned int) (*s - '0') < 10u)
        {
        v = v * 10 + *s - '0';
        s++;
        }

    return (sign == -1)? -v:v;
    }
