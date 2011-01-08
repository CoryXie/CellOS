/* inttypes.h - fixed size integer types */

/*
EXAMPLES

    #include <inttypes.h>
    #include <wchar.h>
    int main(void)
    {
        uintmax_t i = UINTMAX_MAX; // This type always exists.
        wprintf(L"The largest integer value is %020"
            PRIxMAX "\n", i);
        return 0;
    }

APPLICATION USAGE

    The purpose of <inttypes.h> is to provide a set of integer types
    whose definitions are consistent across machines and independent
    of operating systems and other implementation idiosyncrasies. It
    defines, through typedef, integer types of various sizes.
    Implementations are free to typedef them as ISO C standard integer
    types or extensions that they support. Consistent use of this header
    will greatly increase the portability of applications across platforms.

RATIONALE

    The ISO/IEC 9899:1990 standard specified that the language should
    support four signed and unsigned integer data types- char, short,
    int, and long- but placed very little requirement on their size
    other than that int and short be at least 16 bits and long be at
    least as long as int and not smaller than 32 bits. For 16-bit systems,
    most implementations assigned 8, 16, 16, and 32 bits to char, short,
    int, and long, respectively. For 32-bit systems, the common practice
    has been to assign 8, 16, 32, and 32 bits to these types. This
    difference in int size can create some problems for users who
    migrate from one system to another which assigns different sizes
    to integer types, because the ISO C standard integer promotion
    rule can produce silent changes unexpectedly. The need for defining
    an extended integer type increased with the introduction of 64-bit
    systems.

FUTURE DIRECTIONS

    Macro names beginning with PRI or SCN followed by any lowercase
    letter or 'X' may be added to the macros defined in the
    <inttypes.h> header.
*/

#ifndef _POSIX_INTTYPES_H
#define _POSIX_INTTYPES_H

/* The <inttypes.h> header shall include the <stdint.h> header. */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Structure type that is the type of the value returned by
 * the imaxdiv() function.
 */

typedef struct imaxdiv
    {
    intmax_t    quot;    /* quotient */
    intmax_t    rem;    /* remainder */
    } imaxdiv_t;

/*
 * The <inttypes.h> header shall define the following macros.
 * Each expands to a character string literal containing a conversion
 * specifier, possibly modified by a length modifier, suitable
 * for use within the format  argument of a formatted input/output
 * function when converting the corresponding integer type. These
 * macros have the general form of PRI (character string literals
 * for the fprintf() and fwprintf() family of functions) or SCN
 * (character string literals for the fscanf() and fwscanf() family
 * of functions), followed by the conversion specifier, followed
 * by a name corresponding to a similar type name in <stdint.h>.
 * In these names, N represents the width of the type as described
 * in <stdint.h>. For example, PRIdFAST32 can be used in a format
 * string to print the value of an integer of type int_fast32_t.
 *
 * The fprintf() macros for signed integers are:
 *
 * PRIdN    PRIdLEASTN  PRIdFASTN   PRIdMAX     PRIdPTR
 * PRIiN    PRIiLEASTN  PRIiFASTN   PRIiMAX     PRIiPTR
 *
 * The fprintf() macros for unsigned integers are:
 *
 * PRIoN    PRIoLEASTN  PRIoFASTN   PRIoMAX     PRIoPTR
 * PRIuN    PRIuLEASTN  PRIuFASTN   PRIuMAX     PRIuPTR
 * PRIxN    PRIxLEASTN  PRIxFASTN   PRIxMAX     PRIxPTR
 * PRIXN    PRIXLEASTN  PRIXFASTN   PRIXMAX     PRIXPTR
 *
 * The fscanf() macros for signed integers are:
 *
 * SCNdN    SCNdLEASTN  SCNdFASTN   SCNdMAX     SCNdPTR
 * SCNiN    SCNiLEASTN  SCNiFASTN   SCNiMAX     SCNiPTR
 *
 * The fscanf() macros for unsigned integers are:
 *
 * SCNoN    SCNoLEASTN  SCNoFASTN   SCNoMAX     SCNoPTR
 * SCNuN    SCNuLEASTN  SCNuFASTN   SCNuMAX     SCNuPTR
 * SCNxN    SCNxLEASTN  SCNxFASTN   SCNxMAX     SCNxPTR
 *
 * For each type that the implementation provides in <stdint.h>,
 * the corresponding fprintf() and fwprintf() macros shall be
 * defined and the corresponding fscanf() and fwscanf() macros
 * shall be defined unless the implementation does not have a
 * suitable modifier for the type.
 */

# if __WORDSIZE == 64
#  define __PRI64_PREFIX    "l"
#  define __PRIPTR_PREFIX    "l"
# else
#  define __PRI64_PREFIX    "ll"
#  define __PRIPTR_PREFIX
# endif

/* Macros for printing format specifiers.  */

/* Decimal notation.  */
#define PRId8                            "d"
#define PRId16                            "d"
#define PRId32                            "d"
#define PRId64            __PRI64_PREFIX  "d"

#define PRIdLEAST8                        "d"
#define PRIdLEAST16                        "d"
#define PRIdLEAST32                        "d"
#define PRIdLEAST64    __PRI64_PREFIX   "d"

#define PRIdFAST8                        "d"
#define PRIdFAST16                        "d"
#define PRIdFAST32                        "d"
#define PRIdFAST64        __PRI64_PREFIX  "d"


#define PRIi8                            "i"
#define PRIi16                            "i"
#define PRIi32                            "i"
#define PRIi64            __PRI64_PREFIX  "i"

#define PRIiLEAST8                        "i"
#define PRIiLEAST16                        "i"
#define PRIiLEAST32                        "i"
#define PRIiLEAST64        __PRI64_PREFIX  "i"

#define PRIiFAST8                        "i"
#define PRIiFAST16                        "i"
#define PRIiFAST32                        "i"
#define PRIiFAST64        __PRI64_PREFIX  "i"

/* Octal notation.  */
#define PRIo8                            "o"
#define PRIo16                            "o"
#define PRIo32                            "o"
#define PRIo64            __PRI64_PREFIX  "o"

#define PRIoLEAST8                        "o"
#define PRIoLEAST16                        "o"
#define PRIoLEAST32                        "o"
#define PRIoLEAST64        __PRI64_PREFIX  "o"

#define PRIoFAST8                        "o"
#define PRIoFAST16                        "o"
#define PRIoFAST32                        "o"
#define PRIoFAST64        __PRI64_PREFIX  "o"

/* Unsigned integers.  */
#define PRIu8                            "u"
#define PRIu16                            "u"
#define PRIu32                            "u"
#define PRIu64            __PRI64_PREFIX  "u"

#define PRIuLEAST8                        "u"
#define PRIuLEAST16                        "u"
#define PRIuLEAST32                        "u"
#define PRIuLEAST64        __PRI64_PREFIX  "u"

#define PRIuFAST8                        "u"
#define PRIuFAST16                        "u"
#define PRIuFAST32                        "u"
#define PRIuFAST64        __PRI64_PREFIX  "u"

/* lowercase hexadecimal notation.  */
#define PRIx8                            "x"
#define PRIx16                            "x"
#define PRIx32                            "x"
#define PRIx64            __PRI64_PREFIX  "x"

#define PRIxLEAST8                        "x"
#define PRIxLEAST16                        "x"
#define PRIxLEAST32                        "x"
#define PRIxLEAST64        __PRI64_PREFIX  "x"

#define PRIxFAST8                        "x"
#define PRIxFAST16                        "x"
#define PRIxFAST32                        "x"
#define PRIxFAST64        __PRI64_PREFIX  "x"

/* UPPERCASE hexadecimal notation.  */
#define PRIX8                            "X"
#define PRIX16                            "X"
#define PRIX32                            "X"
#define PRIX64            __PRI64_PREFIX  "X"

#define PRIXLEAST8                        "X"
#define PRIXLEAST16                        "X"
#define PRIXLEAST32                        "X"
#define PRIXLEAST64        __PRI64_PREFIX  "X"

#define PRIXFAST8                        "X"
#define PRIXFAST16                        "X"
#define PRIXFAST32                        "X"
#define PRIXFAST64        __PRI64_PREFIX  "X"

/* Macros for printing `intmax_t' and `uintmax_t'. */
#define PRIdMAX            __PRI64_PREFIX  "d"
#define PRIiMAX            __PRI64_PREFIX  "i"
#define PRIoMAX            __PRI64_PREFIX  "o"
#define PRIuMAX            __PRI64_PREFIX  "u"
#define PRIxMAX            __PRI64_PREFIX  "x"
#define PRIXMAX            __PRI64_PREFIX  "X"

/* Macros for printing `intptr_t' and `uintptr_t'. */
#define PRIdPTR            __PRIPTR_PREFIX "d"
#define PRIiPTR            __PRIPTR_PREFIX "i"
#define PRIoPTR            __PRIPTR_PREFIX "o"
#define PRIuPTR            __PRIPTR_PREFIX "u"
#define PRIxPTR            __PRIPTR_PREFIX "x"
#define PRIXPTR            __PRIPTR_PREFIX "X"

/* Macros for scanning format specifiers.  */

/* Signed decimal notation.  */
#define SCNd8                            "hhd"
#define SCNd16                            "hd"
#define SCNd32                            "d"
#define SCNd64            __PRI64_PREFIX  "d"

#define SCNdLEAST8                        "hhd"
#define SCNdLEAST16                        "hd"
#define SCNdLEAST32                        "d"
#define SCNdLEAST64        __PRI64_PREFIX  "d"

#define SCNdFAST8                        "hhd"
#define SCNdFAST16        __PRIPTR_PREFIX "d"
#define SCNdFAST32        __PRIPTR_PREFIX "d"
#define SCNdFAST64        __PRI64_PREFIX  "d"

/* Signed decimal notation.  */
#define SCNi8                            "hhi"
#define SCNi16                            "hi"
#define SCNi32                            "i"
#define SCNi64            __PRI64_PREFIX  "i"

#define SCNiLEAST8                        "hhi"
#define SCNiLEAST16                        "hi"
#define SCNiLEAST32                        "i"
#define SCNiLEAST64        __PRI64_PREFIX  "i"

#define SCNiFAST8                        "hhi"
#define SCNiFAST16        __PRIPTR_PREFIX "i"
#define SCNiFAST32        __PRIPTR_PREFIX "i"
#define SCNiFAST64        __PRI64_PREFIX  "i"

/* Unsigned decimal notation.  */
#define SCNu8                            "hhu"
#define SCNu16                            "hu"
#define SCNu32                            "u"
#define SCNu64            __PRI64_PREFIX  "u"

#define SCNuLEAST8                        "hhu"
#define SCNuLEAST16                        "hu"
#define SCNuLEAST32                        "u"
#define SCNuLEAST64        __PRI64_PREFIX  "u"

#define SCNuFAST8                        "hhu"
#define SCNuFAST16        __PRIPTR_PREFIX "u"
#define SCNuFAST32        __PRIPTR_PREFIX "u"
#define SCNuFAST64        __PRI64_PREFIX  "u"

/* Octal notation.  */
#define SCNo8                            "hho"
#define SCNo16                            "ho"
#define SCNo32                            "o"
#define SCNo64            __PRI64_PREFIX  "o"

#define SCNoLEAST8                        "hho"
#define SCNoLEAST16                        "ho"
#define SCNoLEAST32                        "o"
#define SCNoLEAST64        __PRI64_PREFIX  "o"

#define SCNoFAST8                        "hho"
#define SCNoFAST16        __PRIPTR_PREFIX "o"
#define SCNoFAST32        __PRIPTR_PREFIX "o"
#define SCNoFAST64        __PRI64_PREFIX  "o"

/* Hexadecimal notation.  */
#define SCNx8                            "hhx"
#define SCNx16                            "hx"
#define SCNx32                            "x"
#define SCNx64            __PRI64_PREFIX  "x"

#define SCNxLEAST8                        "hhx"
#define SCNxLEAST16                        "hx"
#define SCNxLEAST32                        "x"
#define SCNxLEAST64        __PRI64_PREFIX  "x"

#define SCNxFAST8                        "hhx"
#define SCNxFAST16        __PRIPTR_PREFIX "x"
#define SCNxFAST32        __PRIPTR_PREFIX "x"
#define SCNxFAST64        __PRI64_PREFIX  "x"


/* Macros for scanning `intmax_t' and `uintmax_t'.  */
#define SCNdMAX            __PRI64_PREFIX  "d"
#define SCNiMAX            __PRI64_PREFIX  "i"
#define SCNoMAX            __PRI64_PREFIX  "o"
#define SCNuMAX            __PRI64_PREFIX  "u"
#define SCNxMAX            __PRI64_PREFIX  "x"

/* Macros for scanning `intptr_t' and `uintptr_t'.  */
#define SCNdPTR            __PRIPTR_PREFIX "d"
#define SCNiPTR            __PRIPTR_PREFIX "i"
#define SCNoPTR            __PRIPTR_PREFIX "o"
#define SCNuPTR            __PRIPTR_PREFIX "u"
#define SCNxPTR            __PRIPTR_PREFIX "x"


intmax_t  imaxabs(intmax_t);
imaxdiv_t imaxdiv(intmax_t, intmax_t);
intmax_t  strtoimax(const char *restrict, char **restrict, int);
uintmax_t strtoumax(const char *restrict, char **restrict, int);
intmax_t  wcstoimax(const wchar_t *restrict, wchar_t **restrict, int);
uintmax_t wcstoumax(const wchar_t *restrict, wchar_t **restrict, int);

#ifdef __cplusplus
}
#endif


#endif    /* _POSIX_INTTYPES_H */
