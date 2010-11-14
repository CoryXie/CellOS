/*  locale.h - category macros */

#ifndef _POSIX_LOCALE_H
#define _POSIX_LOCALE_H

/* The <locale.h> header shall define NULL (as described in <stddef.h>) */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The <locale.h> header shall define the lconv structure,
 * which shall include at least the following members.
 * (See the definitions of LC_MONETARY in LC_MONETARY and LC_NUMERIC .)
 */

/* Structure giving information about numeric and monetary notation.  */

struct lconv
    {
    /* Numeric (non-monetary) information. */
    char *decimal_point;
    char *thousands_sep;

    /*
     * Each element is the number of digits in each group;
     * elements with higher indices are farther left.
     * An element with value CHAR_MAX means that no further
     * grouping is done. An element with value 0 means that
     * the previous element is used for all groups farther
     * left.
     */
    char *grouping;

    /* Monetary information.  */

    /*
     * First three chars are a currency symbol from ISO 4217.
     * Fourth char is the separator.  Fifth char is '\0'.
     */
    char *int_curr_symbol;

    char *currency_symbol;	    /* Local currency symbol. */
    char *mon_decimal_point;	/* Decimal point character. */
    char *mon_thousands_sep;	/* Thousands separator.  */
    char *mon_grouping;		    /* Like `grouping' element (above). */
    char *positive_sign;		/* Sign for positive values. */
    char *negative_sign;		/* Sign for negative values. */
    char int_frac_digits;		/* Int'l fractional digits. */
    char frac_digits;		    /* Local fractional digits. */

    /* 1 if currency_symbol precedes a positive value, 0 if succeeds.  */
    char p_cs_precedes;

    /* 1 iff a space separates currency_symbol from a positive value.  */
    char p_sep_by_space;

    /* 1 if currency_symbol precedes a negative value, 0 if succeeds.  */
    char n_cs_precedes;

    /* 1 iff a space separates currency_symbol from a negative value.  */
    char n_sep_by_space;

    /*
     * Positive and negative sign positions:
     * 0 Parentheses surround the quantity and currency_symbol.
     * 1 The sign string precedes the quantity and currency_symbol.
     * 2 The sign string follows the quantity and currency_symbol.
     * 3 The sign string immediately precedes the currency_symbol.
     * 4 The sign string immediately follows the currency_symbol.
     */
    char p_sign_posn;
    char n_sign_posn;

    /* 1 if int_curr_symbol precedes a positive value, 0 if succeeds.  */
    char int_p_cs_precedes;

    /* 1 iff a space separates int_curr_symbol from a positive value.  */
    char int_p_sep_by_space;

    /* 1 if int_curr_symbol precedes a negative value, 0 if succeeds.  */
    char int_n_cs_precedes;

    /* 1 iff a space separates int_curr_symbol from a negative value.  */
    char int_n_sep_by_space;

    /*
     * Positive and negative sign positions:
     * 0 Parentheses surround the quantity and int_curr_symbol.
     * 1 The sign string precedes the quantity and int_curr_symbol.
     * 2 The sign string follows the quantity and int_curr_symbol.
     * 3 The sign string immediately precedes the int_curr_symbol.
     * 4 The sign string immediately follows the int_curr_symbol.
     */
    char int_p_sign_posn;
    char int_n_sign_posn;
    }lconv_t;

/*
 * The <locale.h> header shall define at least the following macros
 * which shall expand to integer constant expressions with distinct
 * values for use as the first argument to the setlocale() function.
 */

enum
    {
    LC_ALL = 0,
    LC_COLLATE = 1,
    LC_CTYPE = 2,
    LC_MESSAGES = 3,
    LC_MONETARY = 4,
    LC_NUMERIC = 5,
    LC_TIME = 6,
    LC_COUNT
    };

#define LC_LAST (LC_COUNT - 1)

/*
 * Implementations may add additional masks using the form LC_*
 * and an uppercase letter.
 */

/*
 * The <locale.h> header shall contain at least the following
 * macros representing bitmasks for use with the newlocale()
 * function for each supported locale category:
 *
 * LC_COLLATE_MASK
 * LC_CTYPE_MASK
 * LC_MESSAGES_MASK
 * LC_MONETARY_MASK
 * LC_NUMERIC_MASK
 * LC_TIME_MASK
 *
 * Implementations may add additional masks using the form LC_*_MASK.
 *
 * In addition, a macro to set the bits for all categories set
 * shall be defined: LC_ALL_MASK
 */

#define  LC_COLLATE_MASK    0x0001
#define  LC_CTYPE_MASK      0x0002
#define  LC_MESSAGES_MASK   0x0004
#define  LC_MONETARY_MASK   0x0008
#define  LC_NUMERIC_MASK    0x0010
#define  LC_TIME_MASK       0x0020
#define  LC_ALL_MASK        (LC_COLLATE_MASK | \
                             LC_CTYPE_MASK |\
                             LC_MESSAGES_MASK |\
                             LC_MONETARY_MASK |\
                             LC_NUMERIC_MASK |\
                             LC_TIME_MASK)

/*
 * The <locale.h> header shall define LC_GLOBAL_LOCALE, a special
 * locale object descriptor used by the uselocale() function.
 */
#define  LC_GLOBAL_LOCALE    0xFFFF

/*
 * The <locale.h> header shall define the locale_t type,
 * representing a locale object.
 */

typedef void * locale_t;

/*
 * The following shall be declared as functions and may
 * also be defined as macros. Function prototypes shall
 * be provided for use with ISO C standard compilers.
 */

locale_t      duplocale(locale_t);
void          freelocale(locale_t);
struct lconv *localeconv(void);
locale_t      newlocale(int, const char *, locale_t);
char         *setlocale(int category, const char * locale);
locale_t      uselocale (locale_t);

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_LOCALE_H */
