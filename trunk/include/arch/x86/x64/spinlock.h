#ifndef _ARCH_X86_X64_SPINLOCK_H
#define _ARCH_X86_X64_SPINLOCK_H

/* Some of the code below comes from http://locklessinc.com/articles/locks/ */

#include <sys.h>

typedef struct spinlock
    {
    unsigned int counter;
    ipl_t flags;
    } spinlock_t;

/* Initialises a statically-declared spinlock. */
#define SPINLOCK_INITIALISER(_name)     \
    {                                   \
    .counter = 0,                       \
    .flags = 0                          \
    }

/* Statically declares a new spinlock. */
#define SPINLOCK_DECLARE(_var)          \
    spinlock_t _var = SPINLOCK_INITIALISER(#_var)


/* spinlock_init - initialize spinlock
 *
 * Unlock spinlock.
 *
 * @param lock Pointer to spinlock_t structure.
 */

static inline void spinlock_init (spinlock_t *lock)
    {
    lock->counter = 0;
    lock->flags = 0;
    }

/* Compile read-write barrier */
#define barrier() asm volatile("": : :"memory")

/* Pause instruction to prevent excess processor bus usage */ 
#define cpu_relax() asm volatile("pause\n": : :"memory")

/* Atomic exchange (of various sizes) */
static inline void *xchg_64(void *ptr, void *x)
    {
	__asm__ __volatile__("xchgq %0,%1"
				:"=r" ((unsigned long long) x)
				:"m" (*(volatile long long *)ptr), "0" ((unsigned long long) x)
				:"memory");

	return x;
    }

static inline unsigned xchg_32(void *ptr, unsigned x)
    {
	__asm__ __volatile__("xchgl %0,%1"
				:"=r" ((unsigned) x)
				:"m" (*(volatile unsigned *)ptr), "0" (x)
				:"memory");

	return x;
    }

static inline unsigned short xchg_16(void *ptr, unsigned short x)
    {
	__asm__ __volatile__("xchgw %0,%1"
				:"=r" ((unsigned short) x)
				:"m" (*(volatile unsigned short *)ptr), "0" (x)
				:"memory");

	return x;
    }

/* Test and set a bit */
static inline char atomic_bitsetandtest(void *ptr, int x)
    {
	char out;
	__asm__ __volatile__("lock; bts %2,%1\n"
						"sbb %0,%0\n"
				:"=r" (out), "=m" (*(volatile long long *)ptr)
				:"Ir" (x)
				:"memory");

	return out;
    }

#define SPINLOCK_EBUSY 1

static inline void basic_spinlock_lock(spinlock_t *lock)
    {
	while (1)
	    {
		if (!xchg_32(&lock->counter, SPINLOCK_EBUSY)) 
            return;
	
		while (lock->counter) cpu_relax();
	    }
    }

static inline void basic_spinlock_unlock(spinlock_t *lock)
    {
	barrier();
	lock->counter = 0;
    }

static inline int basic_spinlock_trylock(spinlock_t *lock)
    {
	return xchg_32(&lock->counter, SPINLOCK_EBUSY);
    }

/*
 * How ticket locks work:
 * 
 * 1) The lock variable structure "ticketlock" is split into two parts, 
 * we use "unsigned int u" which is 32 bits in both 32 bit and 64 bit systems; 
 * and we split into two parts called "unsigned short ticket" and
 * "unsigned short users" which are both 16 bits.
 * 2) When someone tries to take the lock, it becomes the "user" of the lock,
 * thus the "users" part is increased by 1 firstly; however, it has to check
 * if the current "tick" number(index) belongs to it (the "tick" number must 
 * be equal to the previous "users" number (count), which means all the 
 * previous users has relased the lock.)
 * 3) When release the lock, the "tick" number is increased by 1, meaning
 * one "user" has given up the lock. This "tick" number is then given to the
 * waiting caller who has tried to take the lock with the previous "users"
 * equal to that "tick" number.
 * 4) The "tick" number means the lock has been used that many of times if it
 * does not overlap!
 * 5) the "users" part has to be in the higher part of the lock variable, so
 * that it does not "carry out" to the "ticket" part when it overflows. 
 */
typedef union ticketlock ticketlock;

/* 
 * Note: This has to be on 64 bit! On 32 bit, it should be unsigned int
 * and split into two unsigned shorts!
 */
union ticketlock
    {
	unsigned int u;
	struct
	    {
		unsigned short ticket;
		unsigned short users;
	    } s;
    };

/*
 * Exchanges the contents of a register (second operand) with the 
 * contents of a register or memory location (first operand), computes
 * the sum of the two values, and stores the result in the first operand
 * location.
 */

static inline unsigned short atomic_xadd16(unsigned short *var, unsigned short value)
    {
	__asm__ __volatile__ (
	    "lock;"
		"xaddw %w1,%0"
		: "=m" (*var), "=r" (value)
		: "1" (value)
		: "cc"
	    );
	return value;
    }

static inline unsigned int atomic_xadd32(unsigned int *var, unsigned int value)
    {
	__asm__ __volatile__ (
	    "lock;"
		"xaddl %1,%0"
		: "=m" (*var), "=r" (value)
		: "1" (value)
		: "cc"
	    );
	return value;
    }

static inline void ticket_lock_init(ticketlock *t)
    {
    t->u = 0;
    }

static inline void ticket_lock(ticketlock *t)
    {
	unsigned short me = atomic_xadd16(&t->s.users, 1);
	
	while (t->s.ticket != me) 
        cpu_relax();
    }

static inline void ticket_unlock(ticketlock *t)
    {
	barrier();
	t->s.ticket++;
    }

static inline int ticket_trylock(ticketlock *t)
    {
	unsigned short me = t->s.users;
	unsigned short menew = me + 1;
	unsigned int cmp = ((unsigned int) me << 16) + me;
	unsigned int cmpnew = ((unsigned int) menew << 16) + me;

	if (cmpxchg(&t->u, cmp, cmpnew) == cmp) 
        return OK;
	
	return EBUSY;
    }

static inline int ticket_lockable(ticketlock *t)
    {
	ticketlock u = *t;
	barrier();
	return (u.s.ticket == u.s.users);
    }

#define spinlock_lock basic_spinlock_lock
#define spinlock_unlock basic_spinlock_unlock
#define spinlock_trylock  basic_spinlock_trylock

typedef union rwticket rwticket;

union rwticket
    {
	unsigned int u;
	struct
	    {
		unsigned char write;
		unsigned char read;
		unsigned char users;
	    } s;
    };

static inline void rwticket_wrlock(rwticket *l)
    {
	unsigned int me = atomic_xadd32(&l->u, (1<<16));
	unsigned char val = me >> 16;
	
	while (val != l->s.write) 
        cpu_relax();
    }

static inline void rwticket_wrunlock(rwticket *l)
    {	
	barrier();

	l->s.write++;
	l->s.read++;
    }

static inline int rwticket_wrtrylock(rwticket *l)
    {
	unsigned me = l->s.users;
	unsigned char menew = me + 1;
	unsigned read = l->s.read << 8;
	unsigned cmp = (me << 16) + read + me;
	unsigned cmpnew = (menew << 16) + read + me;

	if (cmpxchg(&l->u, cmp, cmpnew) == cmp) 
        return OK;
	
	return EBUSY;
    }

static inline void rwticket_rdlock(rwticket *l)
    {
	unsigned me = atomic_xadd32(&l->u, (1<<16));
	unsigned char val = me >> 16;
	
	while (val != l->s.read) cpu_relax();
	    l->s.read++;
    }

static inline void rwticket_rdunlock(rwticket *l)
    {
    asm volatile(LOCK_PREFIX "incb %0"
             : "=m" (l->s.write)
             : "m" (l->s.write));
    }

static inline int rwticket_rdtrylock(rwticket *l)
    {
	unsigned me = l->s.users;
	unsigned write = l->s.write;
	unsigned char menew = me + 1;
	unsigned cmp = (me << 16) + (me << 8) + write;
	unsigned cmpnew = ((unsigned) menew << 16) + (menew << 8) + write;

	if (cmpxchg(&l->u, cmp, cmpnew) == cmp) 
        return OK;
	
	return EBUSY;
    }

typedef rwticket rwlock_t; 

#define rwlock_write_lock(lock) rwticket_wrlock(lock)
#define rwlock_write_unlock(lock) rwticket_wrunlock(lock)
#define rwlock_write_trylock(lock) rwticket_wrtrylock(lock)
#define rwlock_read_lock(lock) rwticket_rdlock(lock)
#define rwlock_read_unlock(lock) rwticket_rdunlock(lock)
#define rwlock_read_trylock(lock) rwticket_rdtrylock(lock)

#endif /* _ARCH_X86_X64_SPINLOCK_H */
