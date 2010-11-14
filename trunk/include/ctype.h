/* ctype.h - character types */

#ifndef _POSIX_CTYPE_H_
#define _POSIX_CTYPE_H_

#include <locale.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Test if character is lower-case.
 */
 
static inline int islower(int ch) 
    {
    return (ch >= 'a' && ch <= 'z');
    }

/*
 * Test if character is upper-case.
 */
 
static inline int isupper(int ch) 
    {
    return (ch >= 'A' && ch <= 'Z');
    }

/*
 * Test if character is alphabetic.
 */
static inline int isalpha(int ch) 
    {
    return islower(ch) || isupper(ch);
    }

/*
 * Test if character is a digit.
 */
 
static inline int isdigit(int ch) 
    {
    return (ch >= '0' && ch <= '9');
    }

/*
 * Test if character is alpha-numeric.
 */
 
static inline int isalnum(int ch) 
    {
    return (isalpha(ch) || isdigit(ch));
    }

/*
 * Test if character is an ASCII character.
 */
 
static inline int isascii(int ch) 
    {
    return ((unsigned int)ch < 128u);
    }

/*
 * Test if character is blank.
 */
 
static inline int isblank(int ch) 
    {
    return (ch == ' ' || ch == '\t');
    }

/*
 * Test if character is control character.
 */
 
static inline int iscntrl(int ch) 
    {
    return ((unsigned int)ch < 32u || ch == 127);
    }

/*
 * Test if character is a printable character.
 */
 
static inline int isprint(int ch) 
    {
    ch &= 0x7F;
    return (ch >= 0x20 && ch < 0x7F);
    }

/*
 * Check for any printable character except space.
 */
 
static inline int isgraph(int ch) 
    {
    if(ch == ' ') 
        {
        return 0;
        }
    return isprint(ch);
    }

/*
 * Test if character is space.
 */
 
static inline int isspace(int ch) 
    {
    return (ch == '\t' || ch == '\n' || ch == '\v' || ch == '\f' || ch == '\r' || ch == ' ');
    }

/*
 * Test if character is punctuation.
 */
 
static inline int ispunct(int ch) 
    {
    return (isprint(ch) && !isalnum(ch) && !isspace(ch));
    }

/*
 * Test if character is a hexadecimal digit.
 */
 
static inline int isxdigit(int ch) 
    {
    return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
    }

/*
 * Convert a character to ASCII.
 */
 
static inline int toascii(int ch) 
    {
    return (ch & 0x7F);
    }

/*
 * Convert character to lower-case.
 */
 
static inline int tolower(int ch) 
    {
    if(isalpha(ch)) 
        {
        return ch | 0x20;
        } 
    else 
        {
        return ch;
        }
    }

/*
 * Convert character to upper-case.
 */
 
static inline int toupper(int ch)
    {
    if(isalpha(ch)) 
        {
        return ch & ~0x20;
        } 
    else 
        {
        return ch;
        }
    }

#define _tolower(ch)    ((ch) | 0x20)
#define _toupper(ch)    ((ch) & ~0x20)

/*
 * Convert 32 bit value to string.
 */

static inline char *tostring(char *dest, unsigned int val)
    {
    char digits[] = {'0','1','2','3','4','5','6','7','8','9'};
    unsigned int divisor = 1000000000;
    unsigned int num = val;
    int digitfound = 0;
    int i, a;
    int j = 0;

    for (i = 0; i < 10; i++)
        {
        a = num / divisor;
        if ((digitfound) | (a != 0)) 
            {
            dest[j++] = digits[a];
            digitfound = 1;
            }
        num %= divisor;
        divisor /= 10;
        }
    
    if (digitfound == 0)
            dest[j++] = '0';
    dest[j] = 0;
    return(dest);
    }

/*
 * Convert 32 bit value to hex string.
 */

static inline char *tohexstring(char *dest, unsigned int val)
    {
    char hexdigits[] = 
        {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    
    unsigned int i, tmp = val;

    for (i = 0; i < 8; i++) 
        {
        dest[7-i] = hexdigits[tmp & 0xF];
        tmp = tmp >> 4;
        }
    
    dest[9] = '\0';

    return(dest);
    };

int   isalnum_l(int, locale_t);
int   isalpha_l(int, locale_t);
int   isblank_l(int, locale_t);
int   iscntrl_l(int, locale_t);
int   isdigit_l(int, locale_t);
int   isgraph_l(int, locale_t);
int   islower_l(int, locale_t);
int   isprint_l(int, locale_t);
int   ispunct_l(int, locale_t);
int   isspace_l(int, locale_t);
int   isupper_l(int, locale_t);
int   isxdigit_l(int, locale_t);
int   tolower_l(int, locale_t);
int   toupper_l(int, locale_t);

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_CTYPE_H_ */
