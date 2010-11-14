#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <config.h>

#define CALLER ((uintptr_t) __builtin_return_address(0))

/** Debugging ASSERT macro
 *
 * If CONFIG_DEBUG is set, the ASSERT() macro
 * evaluates expr and if it is false raises
 * kernel panic.
 *
 * @param expr Expression which is expected to be true.
 *
 */
#ifdef CONFIG_DEBUG
#    define ASSERT(expr) \
        if (!(expr)) { \
            panic("Assertion failed (%s), caller=%p.", #expr, CALLER); \
        }
#else
#    define ASSERT(expr)
#endif

/** Extensive logging output macro
 *
 * If CONFIG_LOG is set, the LOG() macro
 * will print whatever message is indicated plus
 * an information about the location.
 *
 */

#ifdef CONFIG_LOG
#    define LOG(format, ...) \
        printk("%s() at %s:%u: " format "\n", __func__, __FILE__, \
            __LINE__, ##__VA_ARGS__);
#else
#    define LOG(format, ...)
#endif

/** Extensive logging execute macro
 *
 * If CONFIG_LOG is set, the LOG_EXEC() macro
 * will print an information about calling a given
 * function and call it.
 *
 */

#ifdef CONFIG_LOG
#    define LOG_EXEC(fnc) \
        { \
            printk("%s() at %s:%u: " #fnc "\n", __func__, __FILE__, \
            __LINE__); \
            fnc; \
        }
#else
#    define LOG_EXEC(fnc) fnc
#endif

#endif /* __DEBUG_H_ */
