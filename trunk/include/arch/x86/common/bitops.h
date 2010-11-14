/* bit_ops.h - bit operations */

/*
 * These have to be done with inline assembly: that way the bit-setting
 * is guaranteed to be atomic. All bit operations return 0 if the bit
 * was cleared before the operation and != 0 if it was not.
 *
 * bit 0 is the LSB of addr; bit 32 is the LSB of (addr+1).
 */

#ifndef _ARCH_X86_COMMON_BITOPS_H
#define _ARCH_X86_COMMON_BITOPS_H

#include <config.h>
#include <endian.h>
#include <arch/x86/common/asm.h>

#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 1)
/* Avoids compilation errors on older gcc versions. */
#define BITOP_ADDR(x) "=m" (*(volatile long *) (x))
#else
#define BITOP_ADDR(x) "+m" (*(volatile long *) (x))
#endif

#define ADDR				BITOP_ADDR(addr)

/*
 * We do the locked ops that don't return the old value as
 * a mask operation on a byte.
 */
#define IS_CONSTANT(nr)		(__builtin_constant_p(nr))
#define CONST_MASK_ADDR(nr, addr)	BITOP_ADDR((void *)(addr) + ((nr)>>3))
#define CONST_MASK(nr)			(1 << ((nr) & 7))

/*
 * atomic_variable_test_bit - set a bit in memory atomically
 * 
 * NOTE:
 *
 * <nr> may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static inline void atomic_set_bit
    (
    unsigned int nr, 
    volatile unsigned long *addr
    )
    {
	if (IS_CONSTANT(nr)) 
        {
		asm volatile(LOCK_PREFIX "orb %1,%0"
			: CONST_MASK_ADDR(nr, addr)
			: "iq" ((uint8_t)CONST_MASK(nr))
			: "memory");
	    } 
    else 
        {
		asm volatile(LOCK_PREFIX "bts %1,%0"
			: BITOP_ADDR(addr) : "Ir" (nr) : "memory");
	    }
    }

/*
 * atomic_clear_bit - clear a bit in memory atomically
 *
 * NOTE:
 *
 * atomic_clear_bit() is atomic and may not be reordered.  However, it does
 * not contain a memory barrier, so if it is used for locking purposes,
 * you should call memory_barrier() and/or memory_barrier()
 * in order to ensure changes are visible on other processors.
 */
static inline void atomic_clear_bit
    (
    int nr, 
    volatile unsigned long *addr
    )
    {
	if (IS_CONSTANT(nr)) 
        {
		asm volatile(LOCK_PREFIX "andb %1,%0"
			: CONST_MASK_ADDR(nr, addr)
			: "iq" ((uint8_t)~(CONST_MASK(nr))));
	    } 
    else 
        {
		asm volatile(LOCK_PREFIX "btr %1,%0"
			: BITOP_ADDR(addr)
			: "Ir" (nr));
	    }
    }

/*
 * atomic_toggle_bit - toggle a bit in memory atomically
 *
 * NOTE:
 *
 * @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static inline void atomic_toggle_bit
    (
    int nr,
    volatile unsigned long *addr
    )
    {
	if (IS_CONSTANT(nr)) 
        {
		asm volatile(LOCK_PREFIX "xorb %1,%0"
			: CONST_MASK_ADDR(nr, addr)
			: "iq" ((uint8_t)CONST_MASK(nr)));
	    } 
    else 
	    {
		asm volatile(LOCK_PREFIX "btc %1,%0"
			: BITOP_ADDR(addr)
			: "Ir" (nr));
	    }
    }

/*
 * atomic_test_and_set_bit - set a bit and return its old value atomically
 *
 * NOTE:
 *
 * It is atomic and also implies a memory barrier.
 */
static inline int atomic_test_and_set_bit
    (
    int nr, 
    volatile unsigned long *addr
    )
    {
	int oldbit;

	asm volatile(LOCK_PREFIX "bts %2,%1\n\t"
		     "sbb %0,%0" : "=r" (oldbit), ADDR : "Ir" (nr) : "memory");

	return oldbit;
    }

/*
 * atomic_test_and_clear_bit - clear a bit and return its old value atomically
 *
 * NOTE:
 *
 * It is atomic and also implies a memory barrier.
 */
static inline int atomic_test_and_clear_bit
    (
    int nr, 
    volatile unsigned long *addr
    )
    {
	int oldbit;

	asm volatile(LOCK_PREFIX "btr %2,%1\n\t"
		     "sbb %0,%0"
		     : "=r" (oldbit), ADDR : "Ir" (nr) : "memory");

	return oldbit;
    }

/*
 * atomic_test_and_toggle_bit - toggle a bit and return its old value atomically
 *
 * NOTE:
 *
 * It is atomic and also implies a memory barrier.
 */
static inline int atomic_test_and_toggle_bit
    (
    int nr, volatile unsigned long *addr
    )
    {
	int oldbit;

	asm volatile(LOCK_PREFIX "btc %2,%1\n\t"
		     "sbb %0,%0"
		     : "=r" (oldbit), ADDR : "Ir" (nr) : "memory");

	return oldbit;
    }

/*
 * atomic_test_bit - test a bit and return its value atomically
 *
 * NOTE:
 *
 * It is atomic and also implies a memory barrier.
 */

static inline int atomic_test_bit
    (
    int nr, 
    volatile const unsigned long *addr
    )
    {
	if (IS_CONSTANT(nr)) 
        {
        return ((1UL << (nr % BITS_PER_LONG)) &
            (((unsigned long *)addr)[nr / BITS_PER_LONG])) != 0;
	    } 
    else 
	    {
        int oldbit;
        
        asm volatile("bt %2,%1\n\t"
                 "sbb %0,%0"
                 : "=r" (oldbit)
                 : "m" (*(unsigned long *)addr), "Ir" (nr));
        
        return oldbit;
	    }
    }

/*
 * atomic_test_bit_range - test a bit range and see if any bit set in the range
 *
 * NOTE:
 *
 * It is atomic and also implies a memory barrier.
 */

static inline int atomic_test_bit_range
    (
    int low,
    int high,
    volatile const unsigned long *addr
    )
    {
    int old = 0;
    int nr; 

    for (nr = low; nr <= high; nr++)
        {
        asm volatile("bt %2,%1\n\t"
                 "sbb %0,%0"
                 : "=r" (old)
                 : "m" (*(unsigned long *)addr), "Ir" (nr));
        
        if (old)
            return 1;
        }
    
    return 0;
    }

/*
 * atomic_cmp_bit_range - comapre a bit range and see if two areas are different
 *
 * NOTE:
 *
 * It is atomic and also implies a memory barrier.
 */

static inline int atomic_cmp_bit_range
    (
    int low,
    int high,
    volatile const unsigned long *addr1,
    volatile const unsigned long *addr2
    )
    {
    int old1 = 0;
    int old2 = 0;
    int nr; 

    for (nr = low; nr <= high; nr++)
        {
        asm volatile("bt %2,%1\n\t"
                 "sbb %0,%0"
                 : "=r" (old1)
                 : "m" (*(unsigned long *)addr1), "Ir" (nr));

        
        asm volatile("bt %2,%1\n\t"
                 "sbb %0,%0"
                 : "=r" (old2)
                 : "m" (*(unsigned long *)addr2), "Ir" (nr));
        
        if (old1 != old2)
            return 1;
        }
    
    return 0;
    }

/*
 * find_first_bit_set - find first set bit in word
 *
 * NOTE:
 *
 * find_first_bit_set(value) returns 0 if value is 0 or the position
 * of the first set bit if value is nonzero. The first (least significant) 
 * bit is at position 1.
 */
static inline int find_first_bit_set 
    (
    int x
    )
    {
	int r;
    
	asm("bsfl %1,%0\n\t"
	    "cmovzl %2,%0"
	    : "=r" (r) : "rm" (x), "r" (-1));
    
	return r + 1;
    }

/*
 * find_last_bit_set - find last set bit in word
 *
 * NOTE:
 *
 * find_last_bit_set(value) returns 0 if value is 0 or the position 
 * of the last set bit if value is nonzero. The last (most significant)
 * bit is at position 32.
 */
static inline int find_last_bit_set
    (
    int x
    )
    {
	int r;
	asm("bsrl %1,%0\n\t"
	    "cmovzl %2,%0"
	    : "=&r" (r) : "rm" (x), "rm" (-1));
	return r + 1;
    }

#endif /* _ARCH_X86_COMMON_BITOPS_H */
