#ifndef _ARCH_X86_X64_ATOMIC_H
#define _ARCH_X86_X64_ATOMIC_H

#include <sys.h>

typedef struct atomic
    {
    volatile int  counter;
    } atomic_t;

typedef struct atomic64
    {
    volatile long  counter;
    } atomic64_t;

#define xchg(ptr, v) ((__typeof__(*(ptr)))__xchg((unsigned long)(v), \
                         (ptr), sizeof(*(ptr))))

#define __xg(x) ((volatile long *)(x))

static inline void set_64bit(volatile unsigned long *ptr, unsigned long val)
    {
    *ptr = val;
    }

#define _set_64bit set_64bit

/*
 * Note 1: no "lock" prefix even on SMP: xchg always implies lock anyway
 * Note 2: xchg has side effect, so that attribute volatile is necessary,
 *         but generally the primitive is invalid, *ptr is output argument.
 */

static inline unsigned long __xchg(unsigned long x, volatile void *ptr, int size)
    {
    /*
     * XCHG - Exchange Register/Memory with Register
     *
     * If a memory operand is referenced, the processor's locking protocol
     * is automatically implemented for the duration of the exchange operation,
     * regardless of the presence.
     *
     * This instruction is useful for implementing semaphores or similar data
     * structures for process synchronization.
     *
     * In 64-bit mode, the instruction's default operation size is 32 bits.
     * Using a REX prefix in the form of REX.R permits access to additional
     * registers (R8-R15). Using a REX prefix in the form of REX.W promotes
     * operation to 64 bits. See the summary chart at the beginning of this
     * section for encoding data and limits.
     */

    switch (size)
        {
        case 1:
            asm volatile("xchgb %b0,%1"
             : "=q" (x)
                                 : "m" (*__xg(ptr)), "0" (x)
                                 : "memory");
            break;
        case 2:
            asm volatile("xchgw %w0,%1"
             : "=r" (x)
                                 : "m" (*__xg(ptr)), "0" (x)
                                 : "memory");
            break;
        case 4:
            asm volatile("xchgl %k0,%1"
             : "=r" (x)
                                 : "m" (*__xg(ptr)), "0" (x)
                                 : "memory");
            break;
        case 8:
            asm volatile("xchgq %0,%1"
             : "=r" (x)
                                 : "m" (*__xg(ptr)), "0" (x)
                                 : "memory");
            break;
        }
    return x;
    }

/*
 * Atomic compare and exchange.  Compare OLD with MEM, if identical,
 * store NEW in MEM.  Return the initial value in MEM.  Success is
 * indicated by comparing RETURN with OLD.
 */

static inline unsigned long __cmpxchg(volatile void *ptr, unsigned long old,
                                      unsigned long new, int size)
    {
    unsigned long prev;
    /*
     * CMPXCHG - Compare and Exchange
     *
     * Compares the value in the AL, AX, EAX, or RAX register with the first
     * operand (destination operand). If the two values are equal, the second
     * operand (source operand) is loaded into the destination operand.
     * Otherwise, the destination operand is loaded into the AL, AX, EAX or
     * RAX register. RAX register is available only in 64-bit mode.
     *
     * This instruction can be used with a LOCK prefix to allow the instruction
     * to be executed atomically. To simplify the interface to the processor's
     * bus, the destination operand receives a write cycle without regard to
     * the result of the comparison. The destination operand is written back
     * if the comparison fails; otherwise, the source operand is written into
     * the destination. (The processor never produces a locked read without
     * also producing a locked write.)
     *
     * In 64-bit mode, the instruction's default operation size is 32 bits.
     * Use of the REX.R prefix permits access to additional registers (R8-R15).
     * Use of the REX.W prefix promotes operation to 64 bits. See the summary
     * chart at the beginning of this section for encoding data and limits.
     */

    switch (size)
        {
        case 1:
            asm volatile(LOCK_PREFIX "cmpxchgb %b1,%2"
             : "=a"(prev)
                                 : "q"(new), "m"(*__xg(ptr)), "0"(old)
                                 : "memory");
            return prev;
        case 2:
            asm volatile(LOCK_PREFIX "cmpxchgw %w1,%2"
             : "=a"(prev)
                                 : "r"(new), "m"(*__xg(ptr)), "0"(old)
                                 : "memory");
            return prev;
        case 4:
            asm volatile(LOCK_PREFIX "cmpxchgl %k1,%2"
             : "=a"(prev)
                                 : "r"(new), "m"(*__xg(ptr)), "0"(old)
                                 : "memory");
            return prev;
        case 8:
            asm volatile(LOCK_PREFIX "cmpxchgq %1,%2"
             : "=a"(prev)
                                 : "r"(new), "m"(*__xg(ptr)), "0"(old)
                                 : "memory");
            return prev;
        }
    return old;
    }

/*
 * Always use locked operations when touching memory shared with a
 * hypervisor, since the system may be SMP even if the guest kernel
 * isn't.
 */

static inline unsigned long __sync_cmpxchg(volatile void *ptr,
        unsigned long old,
        unsigned long new, int size)
    {
    unsigned long prev;
    switch (size)
        {
        case 1:
            asm volatile("lock; cmpxchgb %b1,%2"
             : "=a"(prev)
                                 : "q"(new), "m"(*__xg(ptr)), "0"(old)
                                 : "memory");
            return prev;
        case 2:
            asm volatile("lock; cmpxchgw %w1,%2"
             : "=a"(prev)
                                 : "r"(new), "m"(*__xg(ptr)), "0"(old)
                                 : "memory");
            return prev;
        case 4:
            asm volatile("lock; cmpxchgl %1,%2"
             : "=a"(prev)
                                 : "r"(new), "m"(*__xg(ptr)), "0"(old)
                                 : "memory");
            return prev;
        }

    return old;
    }


static inline unsigned long __cmpxchg_local(volatile void *ptr,
        unsigned long old,
        unsigned long new, int size)
    {
    unsigned long prev;

    switch (size)
        {
        case 1:
            asm volatile("cmpxchgb %b1,%2"
             : "=a"(prev)
                                 : "q"(new), "m"(*__xg(ptr)), "0"(old)
                                 : "memory");
            return prev;
        case 2:
            asm volatile("cmpxchgw %w1,%2"
             : "=a"(prev)
                                 : "r"(new), "m"(*__xg(ptr)), "0"(old)
                                 : "memory");
            return prev;
        case 4:
            asm volatile("cmpxchgl %k1,%2"
             : "=a"(prev)
                                 : "r"(new), "m"(*__xg(ptr)), "0"(old)
                                 : "memory");
            return prev;
        case 8:
            asm volatile("cmpxchgq %1,%2"
             : "=a"(prev)
                                 : "r"(new), "m"(*__xg(ptr)), "0"(old)
                                 : "memory");
            return prev;
        }
    return old;
    }

#define cmpxchg(ptr, o, n)                        \
    ((__typeof__(*(ptr)))__cmpxchg((ptr), (unsigned long)(o),    \
                       (unsigned long)(n), sizeof(*(ptr))))
#define cmpxchg64(ptr, o, n)                        \
({                                    \
    BUILD_BUG_ON(sizeof(*(ptr)) != 8);                \
    cmpxchg((ptr), (o), (n));                    \
})
#define cmpxchg_local(ptr, o, n)                    \
    ((__typeof__(*(ptr)))__cmpxchg_local((ptr), (unsigned long)(o),    \
                         (unsigned long)(n),    \
                         sizeof(*(ptr))))
#define sync_cmpxchg(ptr, o, n)                        \
    ((__typeof__(*(ptr)))__sync_cmpxchg((ptr), (unsigned long)(o),    \
                        (unsigned long)(n),        \
                        sizeof(*(ptr))))
#define cmpxchg64_local(ptr, o, n)                    \
({                                    \
    BUILD_BUG_ON(sizeof(*(ptr)) != 8);                \
    cmpxchg_local((ptr), (o), (n));                    \
})

/*
 * Atomic operations that C can't guarantee us.  Useful for
 * resource counting etc..
 */

#define ATOMIC_INIT(i)    { (i) }

/**
 * atomic_read - read atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically reads the value of @v.
 */
static inline int atomic_read(const atomic_t *v)
    {
    return v->counter;
    }

/**
 * atomic_set - set atomic variable
 * @v: pointer of type atomic_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.
 */
static inline void atomic_set(atomic_t *v, int i)
    {
    v->counter = i;
    }

/**
 * atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type atomic_t
 *
 * Atomically adds @i to @v.
 */
static inline void atomic_add(int i, atomic_t *v)
    {
    asm volatile(LOCK_PREFIX "addl %1,%0"
             : "=m" (v->counter)
                         : "ir" (i), "m" (v->counter));
    }

/**
 * atomic_sub - subtract the atomic variable
 * @i: integer value to subtract
 * @v: pointer of type atomic_t
 *
 * Atomically subtracts @i from @v.
 */
static inline void atomic_sub(int i, atomic_t *v)
    {
    asm volatile(LOCK_PREFIX "subl %1,%0"
             : "=m" (v->counter)
                         : "ir" (i), "m" (v->counter));
    }

/**
 * atomic_sub_and_test - subtract value from variable and test result
 * @i: integer value to subtract
 * @v: pointer of type atomic_t
 *
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic_sub_and_test(int i, atomic_t *v)
    {
    unsigned char c;

    asm volatile(LOCK_PREFIX "subl %2,%0; sete %1"
             : "=m" (v->counter), "=qm" (c)
                         : "ir" (i), "m" (v->counter) : "memory");
    return c;
    }

/**
 * atomic_inc - increment atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically increments @v by 1.
 */
static inline void atomic_inc(atomic_t *v)
    {
    asm volatile(LOCK_PREFIX "incl %0"
             : "=m" (v->counter)
                         : "m" (v->counter));
    }

/**
 * atomic_dec - decrement atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1.
 */
static inline void atomic_dec(atomic_t *v)
    {
    asm volatile(LOCK_PREFIX "decl %0"
             : "=m" (v->counter)
                         : "m" (v->counter));
    }

/**
 * atomic_dec_and_test - decrement and test
 * @v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */
static inline int atomic_dec_and_test(atomic_t *v)
    {
    unsigned char c;

    asm volatile(LOCK_PREFIX "decl %0; sete %1"
             : "=m" (v->counter), "=qm" (c)
                         : "m" (v->counter) : "memory");
    return c != 0;
    }

/**
 * atomic_inc_and_test - increment and test
 * @v: pointer of type atomic_t
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic_inc_and_test(atomic_t *v)
    {
    unsigned char c;

    asm volatile(LOCK_PREFIX "incl %0; sete %1"
             : "=m" (v->counter), "=qm" (c)
                         : "m" (v->counter) : "memory");
    return c != 0;
    }

/**
 * atomic_add_negative - add and test if negative
 * @i: integer value to add
 * @v: pointer of type atomic_t
 *
 * Atomically adds @i to @v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.
 */
static inline int atomic_add_negative(int i, atomic_t *v)
    {
    unsigned char c;

    asm volatile(LOCK_PREFIX "addl %2,%0; sets %1"
             : "=m" (v->counter), "=qm" (c)
                         : "ir" (i), "m" (v->counter) : "memory");
    return c;
    }

/**
 * atomic_add_return - add and return
 * @i: integer value to add
 * @v: pointer of type atomic_t
 *
 * Atomically adds @i to @v and returns @i + @v
 */
static inline int atomic_add_return(int i, atomic_t *v)
    {
    int __i = i;
    asm volatile(LOCK_PREFIX "xaddl %0, %1"
             : "+r" (i), "+m" (v->counter)
                         : : "memory");
    return i + __i;
    }

static inline int atomic_sub_return(int i, atomic_t *v)
    {
    return atomic_add_return(-i, v);
    }

#define atomic_inc_return(v)  (atomic_add_return(1, v))
#define atomic_dec_return(v)  (atomic_sub_return(1, v))

/* The 64-bit atomic type */

#define ATOMIC64_INIT(i)    { (i) }

/**
 * atomic64_read - read atomic64 variable
 * @v: pointer of type atomic64_t
 *
 * Atomically reads the value of @v.
 * Doesn't imply a read memory barrier.
 */
static inline long atomic64_read(const atomic64_t *v)
    {
    return v->counter;
    }

/**
 * atomic64_set - set atomic64 variable
 * @v: pointer to type atomic64_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.
 */
static inline void atomic64_set(atomic64_t *v, long i)
    {
    v->counter = i;
    }

/**
 * atomic64_add - add integer to atomic64 variable
 * @i: integer value to add
 * @v: pointer to type atomic64_t
 *
 * Atomically adds @i to @v.
 */
static inline void atomic64_add(long i, atomic64_t *v)
    {
    asm volatile(LOCK_PREFIX "addq %1,%0"
             : "=m" (v->counter)
                         : "er" (i), "m" (v->counter));
    }

/**
 * atomic64_sub - subtract the atomic64 variable
 * @i: integer value to subtract
 * @v: pointer to type atomic64_t
 *
 * Atomically subtracts @i from @v.
 */
static inline void atomic64_sub(long i, atomic64_t *v)
    {
    asm volatile(LOCK_PREFIX "subq %1,%0"
             : "=m" (v->counter)
                         : "er" (i), "m" (v->counter));
    }

/**
 * atomic64_sub_and_test - subtract value from variable and test result
 * @i: integer value to subtract
 * @v: pointer to type atomic64_t
 *
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic64_sub_and_test(long i, atomic64_t *v)
    {
    unsigned char c;

    asm volatile(LOCK_PREFIX "subq %2,%0; sete %1"
             : "=m" (v->counter), "=qm" (c)
                         : "er" (i), "m" (v->counter) : "memory");
    return c;
    }

/**
 * atomic64_inc - increment atomic64 variable
 * @v: pointer to type atomic64_t
 *
 * Atomically increments @v by 1.
 */
static inline void atomic64_inc(atomic64_t *v)
    {
    asm volatile(LOCK_PREFIX "incq %0"
             : "=m" (v->counter)
                         : "m" (v->counter));
    }

/**
 * atomic64_dec - decrement atomic64 variable
 * @v: pointer to type atomic64_t
 *
 * Atomically decrements @v by 1.
 */
static inline void atomic64_dec(atomic64_t *v)
    {
    asm volatile(LOCK_PREFIX "decq %0"
             : "=m" (v->counter)
                         : "m" (v->counter));
    }

/**
 * atomic64_dec_and_test - decrement and test
 * @v: pointer to type atomic64_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */
static inline int atomic64_dec_and_test(atomic64_t *v)
    {
    unsigned char c;

    asm volatile(LOCK_PREFIX "decq %0; sete %1"
             : "=m" (v->counter), "=qm" (c)
                         : "m" (v->counter) : "memory");
    return c != 0;
    }

/**
 * atomic64_inc_and_test - increment and test
 * @v: pointer to type atomic64_t
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic64_inc_and_test(atomic64_t *v)
    {
    unsigned char c;

    asm volatile(LOCK_PREFIX "incq %0; sete %1"
             : "=m" (v->counter), "=qm" (c)
                         : "m" (v->counter) : "memory");
    return c != 0;
    }

/**
 * atomic64_add_negative - add and test if negative
 * @i: integer value to add
 * @v: pointer to type atomic64_t
 *
 * Atomically adds @i to @v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.
 */
static inline int atomic64_add_negative(long i, atomic64_t *v)
    {
    unsigned char c;

    asm volatile(LOCK_PREFIX "addq %2,%0; sets %1"
             : "=m" (v->counter), "=qm" (c)
                         : "er" (i), "m" (v->counter) : "memory");
    return c;
    }

/**
 * atomic64_add_return - add and return
 * @i: integer value to add
 * @v: pointer to type atomic64_t
 *
 * Atomically adds @i to @v and returns @i + @v
 */
static inline long atomic64_add_return(long i, atomic64_t *v)
    {
    long __i = i;
    asm volatile(LOCK_PREFIX "xaddq %0, %1;"
             : "+r" (i), "+m" (v->counter)
                         : : "memory");
    return i + __i;
    }

static inline long atomic64_sub_return(long i, atomic64_t *v)
    {
    return atomic64_add_return(-i, v);
    }

#define atomic64_inc_return(v)  (atomic64_add_return(1, (v)))
#define atomic64_dec_return(v)  (atomic64_sub_return(1, (v)))

static inline long atomic64_cmpxchg(atomic64_t *v, long old, long new)
    {
    return cmpxchg(&v->counter, old, new);
    }

static inline long atomic64_xchg(atomic64_t *v, long new)
    {
    return xchg(&v->counter, new);
    }

static inline long atomic_cmpxchg(atomic_t *v, int old, int new)
    {
    return cmpxchg(&v->counter, old, new);
    }

static inline long atomic_xchg(atomic_t *v, int new)
    {
    return xchg(&v->counter, new);
    }

/**
 * atomic_add_unless - add unless the number is a given value
 * @v: pointer of type atomic_t
 * @a: the amount to add to v...
 * @u: ...unless v is equal to u.
 *
 * Atomically adds @a to @v, so long as it was not @u.
 * Returns non-zero if @v was not @u, and zero otherwise.
 */
static inline int atomic_add_unless(atomic_t *v, int a, int u)
    {
    int c, old;
    c = atomic_read(v);
    for (;;)
        {
        if (unlikely(c == (u)))
            break;
        old = atomic_cmpxchg((v), c, c + (a));
        if (likely(old == c))
            break;
        c = old;
        }
    return c != (u);
    }

#define atomic_inc_not_zero(v) atomic_add_unless((v), 1, 0)

/**
 * atomic64_add_unless - add unless the number is a given value
 * @v: pointer of type atomic64_t
 * @a: the amount to add to v...
 * @u: ...unless v is equal to u.
 *
 * Atomically adds @a to @v, so long as it was not @u.
 * Returns non-zero if @v was not @u, and zero otherwise.
 */
static inline int atomic64_add_unless(atomic64_t *v, long a, long u)
    {
    long c, old;
    c = atomic64_read(v);
    for (;;)
        {
        if (unlikely(c == (u)))
            break;
        old = atomic64_cmpxchg((v), c, c + (a));
        if (likely(old == c))
            break;
        c = old;
        }
    return c != (u);
    }

/**
 * atomic_inc_short - increment of a short integer
 * @v: pointer to type int
 *
 * Atomically adds 1 to @v
 * Returns the new value of @u
 */
static inline short int atomic_inc_short(short int *v)
    {
    asm(LOCK_PREFIX "addw $1, %0" : "+m" (*v));
    return *v;
    }

/**
 * atomic_or_long - OR of two long integers
 * @v1: pointer to type unsigned long
 * @v2: pointer to type unsigned long
 *
 * Atomically ORs @v1 and @v2
 * Returns the result of the OR
 */
static inline void atomic_or_long(unsigned long *v1, unsigned long v2)
    {
    asm(LOCK_PREFIX "orq %1, %0" : "+m" (*v1) : "r" (v2));
    }

#define atomic64_inc_not_zero(v) atomic64_add_unless((v), 1, 0)

/* These are x86-specific, used by some header files */
#define atomic_clear_mask(mask, addr)                    \
    asm volatile(LOCK_PREFIX "andl %0,%1"                \
             : : "r" (~(mask)), "m" (*(addr)) : "memory")

#define atomic_set_mask(mask, addr)                    \
    asm volatile(LOCK_PREFIX "orl %0,%1"                \
             : : "r" ((unsigned)(mask)), "m" (*(addr))        \
             : "memory")

/* Atomic operations are already serializing on x86 */
#define smp_mb__before_atomic_dec()    barrier()
#define smp_mb__after_atomic_dec()    barrier()
#define smp_mb__before_atomic_inc()    barrier()
#define smp_mb__after_atomic_inc()    barrier()

/*
 * Suppport for atomic_long_t
 *
 * Casts for parameters are avoided for existing atomic functions in order to
 * avoid issues with cast-as-lval under gcc 4.x and other limitations that the
 * macros of a platform may have.
 */

typedef atomic64_t atomic_long_t;

#define ATOMIC_LONG_INIT(i)    ATOMIC64_INIT(i)

static inline long atomic_long_read(atomic_long_t *l)
    {
    atomic64_t *v = (atomic64_t *)l;

    return (long)atomic64_read(v);
    }

static inline void atomic_long_set(atomic_long_t *l, long i)
    {
    atomic64_t *v = (atomic64_t *)l;

    atomic64_set(v, i);
    }

static inline void atomic_long_inc(atomic_long_t *l)
    {
    atomic64_t *v = (atomic64_t *)l;

    atomic64_inc(v);
    }

static inline void atomic_long_dec(atomic_long_t *l)
    {
    atomic64_t *v = (atomic64_t *)l;

    atomic64_dec(v);
    }

static inline void atomic_long_add(long i, atomic_long_t *l)
    {
    atomic64_t *v = (atomic64_t *)l;

    atomic64_add(i, v);
    }

static inline void atomic_long_sub(long i, atomic_long_t *l)
    {
    atomic64_t *v = (atomic64_t *)l;

    atomic64_sub(i, v);
    }

static inline int atomic_long_sub_and_test(long i, atomic_long_t *l)
    {
    atomic64_t *v = (atomic64_t *)l;

    return atomic64_sub_and_test(i, v);
    }

static inline int atomic_long_dec_and_test(atomic_long_t *l)
    {
    atomic64_t *v = (atomic64_t *)l;

    return atomic64_dec_and_test(v);
    }

static inline int atomic_long_inc_and_test(atomic_long_t *l)
    {
    atomic64_t *v = (atomic64_t *)l;

    return atomic64_inc_and_test(v);
    }

static inline int atomic_long_add_negative(long i, atomic_long_t *l)
    {
    atomic64_t *v = (atomic64_t *)l;

    return atomic64_add_negative(i, v);
    }

static inline long atomic_long_add_return(long i, atomic_long_t *l)
    {
    atomic64_t *v = (atomic64_t *)l;

    return (long)atomic64_add_return(i, v);
    }

static inline long atomic_long_sub_return(long i, atomic_long_t *l)
    {
    atomic64_t *v = (atomic64_t *)l;

    return (long)atomic64_sub_return(i, v);
    }

static inline long atomic_long_inc_return(atomic_long_t *l)
    {
    atomic64_t *v = (atomic64_t *)l;

    return (long)atomic64_inc_return(v);
    }

static inline long atomic_long_dec_return(atomic_long_t *l)
    {
    atomic64_t *v = (atomic64_t *)l;

    return (long)atomic64_dec_return(v);
    }

static inline long atomic_long_add_unless(atomic_long_t *l, long a, long u)
    {
    atomic64_t *v = (atomic64_t *)l;

    return (long)atomic64_add_unless(v, a, u);
    }

#define atomic_long_inc_not_zero(l) atomic64_inc_not_zero((atomic64_t *)(l))

#define atomic_long_cmpxchg(l, old, new) \
    (atomic64_cmpxchg((atomic64_t *)(l), (old), (new)))
#define atomic_long_xchg(v, new) \
    (atomic64_xchg((atomic64_t *)(v), (new)))

#endif /* _ARCH_X86_X64_ATOMIC_H */

