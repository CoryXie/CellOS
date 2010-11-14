
#ifndef _OS_PRINTK_H
#define _OS_PRINTK_H

#include <stdarg.h>

#define LOG_DEBUG       1               /**< Debug message. */
#define LOG_NORMAL      2               /**< Normal message. */
#define LOG_WARN        3               /**< Warning message. */
#define LOG_NONE        4               /**< Do not log the message (for fatal/KDBG). */ 

/** Type for a do_printf() helper function. */
typedef void (*printf_helper_t)(char, void *, int *);

int do_printf(printf_helper_t helper, void *data, const char *fmt, va_list args);

int printk(const char *format, ...); 

int panic(const char *format, ...); 

#endif /* _OS_PRINTK_H */
