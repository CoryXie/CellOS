/**
 * @file
 * @brief        Compiler-specific macros/definitions.
 */

#ifndef __COMPILER_H
#define __COMPILER_H

#define __unused        __attribute__((unused))
#define __used          __attribute__((used))
#define __packed        __attribute__((packed))
#define __aligned(a)    __attribute__((aligned(a)))
#define __noreturn      __attribute__((noreturn))
#define __malloc        __attribute__((malloc))
#define __printf(a, b)  __attribute__((format(printf, a, b)))
#define __deprecated    __attribute__((deprecated))
#define __init_text     __attribute__((section(".init.text")))
#define __init_data     __attribute__((section(".init.data")))
#define __section(s)    __attribute__((section(s)))
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

/* Force a compilation error if condition is true */
#define BUILD_BUG_ON(condition) ((void)BUILD_BUG_ON_ZERO(condition))

/* Force a compilation error if condition is constant and true */
#define MAYBE_BUILD_BUG_ON(cond) ((void)sizeof(char[1 - 2 * !!(cond)]))

/* Force a compilation error if condition is true, but also produce a
   result (of value 0 and type size_t), so the expression can be used
   e.g. in a structure initializer (or where-ever else comma expressions
   aren't permitted). */
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))
#define BUILD_BUG_ON_NULL(e) ((void *)sizeof(struct { int:-!!(e); }))

/** Round a value up. */
#define ROUND_UP(value, nearest) \
    __extension__ \
    ({ \
        typeof(value) __n = value; \
        if(__n % (nearest)) { \
            __n -= __n % (nearest); \
            __n += nearest; \
        } \
        __n; \
    })

/** Round a value down. */
#define ROUND_DOWN(value, nearest) \
    __extension__ \
    ({ \
        typeof(value) __n = value; \
        if(__n % (nearest)) { \
            __n -= __n % (nearest); \
        } \
        __n; \
    })

/** Get the number of bits in a type. */
#define BITS(t)        (sizeof(t) * 8)

/** Get the number of elements in an array. */
#define ARRAYSZ(a)    (sizeof((a)) / sizeof((a)[0]))

/** Get the lowest value out of a pair of values. */
#define MIN(a, b)    ((a) < (b) ? (a) : (b))

/** Get the highest value out of a pair of values. */
#define MAX(a, b)    ((a) < (b) ? (b) : (a))

//! This macro returns the absolute value of \a a.
#define ABS(a)        ((a) < 0 ? -(a) : (a))
//! This macro truncates \a addr to the \a align boundary.
#define TRUNC(addr, align)    ((addr) & ~((align) - 1))
//! This macro rounds down \a addr to the \a align boundary.
#define ALIGN_DOWN(addr, align)    TRUNC(addr, align)
//! This macro rounds up \a addr to the \a align boundary.
#define ALIGN_UP(addr, align)    ( ((addr) + (align) - 1) & (~((align) - 1)) )

/* Number of elements in an array */ 

#define NELEMENTS(array)        \
        (sizeof (array) / sizeof ((array) [0]))

#define CONTAINER_OF(node, type, member)        \
    (type *)((char *)node - offsetof(type, member))

#endif /* __COMPILER_H */
