/* stddef.h - standard type definitions */

/*
RATIONALE

The ISO C standard does not require the NULL macro to include
the cast to type void * and specifies that the NULL macro be 
implementation-defined. POSIX.1-2008 requires the cast and 
therefore need not be implementation-defined.
*/

#ifndef _POSIX_STDDEF_H
#define _POSIX_STDDEF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * NULL
 * 
 * Null pointer constant.
 *
 * The macro shall expand to an integer constant expression 
 * with the value 0 cast to type void. 
 */
 
#define NULL           ((void *)0)

typedef uint32_t        BOOL;
#define TRUE            1
#define FALSE           0

#define MAGIC_VALID     (magic_t)(0xFEEDF00D)
#define MAGIC_INVALID   (magic_t)(0xDEADBEEF)

#define TMO_NO_WAIT       (wait_t)(0)  
#define TMO_WAIT_FOREVER  (wait_t)(-1) 

/*
 * offsetof(type, member-designator)
 *
 * Integer constant expression of type size_t, the value of 
 * which is the offset in bytes to the structure member 
 *(member-designator), from the beginning of its structure (type).
 */
#if 0
#define offsetof(type,member) ((unsigned long) &(((type*)0)->member))
#else
#define offsetof(type, member)    __builtin_offsetof(type, member)
#endif
/*
 * ptrdiff_t
 *
 * Signed integer type of the result of subtracting two pointers.
 */

typedef long            ptrdiff_t;

/*
 * wchar_t
 *
 * Integer type whose range of values can represent distinct codes 
 * for all members of the largest extended character set specified 
 * among the supported locales; the null character shall have the
 * code value zero. Each member of the basic character set shall
 * have a code value equal to its value when used as the lone 
 * character in an integer character constant if an implementation
 * does not define __STDC_MB_MIGHT_NEQ_WC__.
 */

typedef int wchar_t;

/*
 * size_t
 *
 * Unsigned integer type of the result of the sizeof operator.
 */
 
typedef unsigned long size_t; /* Used for sizes of objects. */
typedef long ssize_t; /* Used for a count of bytes or an error indication. */

/*
 * The implementation shall support one or more programming
 * environments in which the widths of ptrdiff_t, size_t, and 
 * wchar_t are no greater than the width of type long. The 
 * names of these programming environments can be obtained 
 * using the confstr() function or the getconf utility.
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_STDDEF_H */

