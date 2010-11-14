/* langinfo.h - language information constants */

/*
APPLICATION USAGE

    Wherever possible, users are advised to use functions compatible
    with those in the ISO C standard to access items of langinfo data.
    In particular, the strftime() function should be used to access
    date and time information defined in category LC_TIME. The localeconv()
    function should be used to access information corresponding to
    RADIXCHAR, THOUSEP, and CRNCYSTR.
*/

#ifndef _POSIX_LANGINFO_H
#define _POSIX_LANGINFO_H

/*
 * The <langinfo.h> header shall define the symbolic constants used to
 * identify items of langinfo data (see nl_langinfo()).
 *
 * The <langinfo.h> header shall define the locale_t type as described
 * in <locale.h>.
 *
 * The <langinfo.h> header shall define the nl_item type as described 
 * in <nl/types.h>.
 */

#include <locale.h>
#include <nl_types.h>
#include <sys/cdefs.h>

/*
 * The <langinfo.h> header shall define the following symbolic constants
 * with type nl_item. The entries under Category indicate in which
 * setlocale() category each item is defined.
 */

enum {
	CODESET,	/* Codeset name. */
	D_T_FMT,	/* String for formatting date and time. */
	D_FMT,		/* Date format string. */
	T_FMT,		/* Time format string */
	T_FMT_AMPM,	/* a.m. or p.m. time formatting string */
	AM_STR,		/* Ante Meridian affix */
	PM_STR,		/* Post Meridian affix */

	/* Week day names */
	DAY_1,
	DAY_2,
	DAY_3,
	DAY_4,
	DAY_5,
	DAY_6,
	DAY_7,

	/* Abbreviated week day names */
	ABDAY_1,
	ABDAY_2,
	ABDAY_3,
	ABDAY_4,
	ABDAY_5,
	ABDAY_6,
	ABDAY_7,

	/* Month names */
	MON_1,
	MON_2,
	MON_3,
	MON_4,
	MON_5,
	MON_6,
	MON_7,
	MON_8,
	MON_9,
	MON_10,
	MON_11,
	MON_12,

	/* Abbreviated month names */
	ABMON_1,
	ABMON_2,
	ABMON_3,
	ABMON_4,
	ABMON_5,
	ABMON_6,
	ABMON_7,
	ABMON_8,
	ABMON_9,
	ABMON_10,
	ABMON_11,
	ABMON_12,

	ERA,			/* Era description segments */
	ERA_D_FMT,		/* Era date format string */
	ERA_D_T_FMT,	/* Era date and time format string */
	ERA_T_FMT,		/* Era time format string */
	ALT_DIGITS,		/* Alternative symbols for digits */

	RADIXCHAR,		/* Radix character. */
	THOUSEP,		/* Separator for thousands */

	YESEXPR,		/* Affirmative response expression */
	NOEXPR,			/* Negative response expression */

   /*
    * Local currency symbol, preceded by '-' if the symbol should
    * appear before the value, '+' if the symbol should appear
    * after the value, or '.' if the symbol should replace the
    * radix character. If the local currency symbol is the empty
    * string, implementations may return the empty string ( "" ).
    */
	CRNCYSTR,		/* Currency symbol */

	_NL_LANGINFO_LAST
    };
/*
 * If the locale's values for p_cs_precedes and n_cs_precedes do not match,
 * the value of nl_langinfo(CRNCYSTR) and nl_langinfo_l(CRNCYSTR,loc) is
 * unspecified.
 */

#ifdef __cplusplus
extern "C" {
#endif

char *nl_langinfo(nl_item item);
char *nl_langinfo_l(nl_item item, locale_t locale);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_LANGINFO_H */
