#ifndef _ARCH_X86_X64_SPINLOCK_H
#define _ARCH_X86_X64_SPINLOCK_H

/* Some of the code below comes from http://locklessinc.com/articles/locks/ */

#include <sys.h>

typedef struct spinlock_t
    {
    unsigned int counter;
    } spinlock_t;

/* Initialises a statically-declared spinlock. */
#define SPINLOCK_INITIALISER(_name)     \
    {                                   \
    .counter = 0,                       \
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
    }

/*
 * Ticket locks are conceptually two parts, one indicating the current head of
 * the queue, and the other indicating the current tail. The lock is acquired
 * by atomically noting the tail and incrementing it by one (thus adding
 * ourself to the queue and noting our position), then waiting until the head
 * becomes equal to the the initial value of the tail.
 *
 * We use an xadd covering *both* parts of the lock, to increment the tail and
 * also load the position of the head, which takes care of memory ordering
 * issues and should be optimal for the uncontended case. Note the tail must be
 * in the high part, because a wide xadd increment of the low part would carry
 * up and contaminate the high part.
 *
 * With fewer than 2^8 possible CPUs, we can use x86's partial registers to
 * save some instructions and make the code more elegant. There really isn't
 * much between them in performance though, especially as locks are out of line.
 */

static inline void ticket_spinlock_lock (spinlock_t *lock)
    {
	short inc = 0x0100;

	asm volatile ("lock; "
		"xaddw %w0, %1\n"
		"1:\t"
		"cmpb %h0, %b0\n\t"
		"je 2f\n\t"
		"rep ; nop\n\t"
		"movb %1, %b0\n\t"
		/* don't need lfence here, because loads are in-order */
		"jmp 1b\n"
		"2:"
		: "+Q" (inc), "+m" (lock->counter)
		:
		: "memory", "cc");
    }

static inline void ticket_spinlock_unlock(spinlock_t *lock)
    {
	asm volatile("incb %0"
		     : "+m" (lock->counter)
		     :
		     : "memory", "cc");
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

typedef struct k42lock k42lock;
struct k42lock
    {
	k42lock *next;
	k42lock *tail;
    };

static inline void k42_lock(k42lock *l)
    {
	k42lock me;
	k42lock *pred, *succ;
	me.next = NULL;
	
	barrier();
	
	pred = xchg_64(&l->tail, &me);
	if (pred)
	    {
		me.tail = (void *) 1;
		
		barrier();
		pred->next = &me;
		barrier();
		
		while (me.tail) cpu_relax();
	    }
	
	succ = me.next;

	if (!succ)
	    {
		barrier();
		l->next = NULL;
		
		if (cmpxchg(&l->tail, &me, &l->next) != &me)
		    {
			while (!me.next) 
                cpu_relax();
			
			l->next = me.next;
		    }
	    }
	else
	    {
		l->next = succ;
	    }
    }


static inline void k42_unlock(k42lock *l)
    {
	k42lock *succ = l->next;
	
	barrier();
	
	if (!succ)
	    {
		if (cmpxchg(&l->tail, &l->next, NULL) == (void *) &l->next) return;
		
		while (!l->next) cpu_relax();
		    succ = l->next;
	    }
	
	succ->tail = NULL;
    }

static inline int k42_trylock(k42lock *l)
    {
	if (!cmpxchg(&l->tail, NULL, &l->next)) 
        return OK;
	
	return EBUSY;
    }

typedef struct listlock_t listlock_t;
struct listlock_t
    {
	listlock_t *next;
	int spin;
    };
typedef struct listlock_t *listlock;

#define LLOCK_FLAG	(void *)1

static inline void listlock_lock(listlock *l)
    {
	listlock_t me;
	listlock_t *tail;

	/* Fast path - no users  */
	if (!cmpxchg(l, NULL, LLOCK_FLAG)) 
        return;
	
	me.next = LLOCK_FLAG;
	me.spin = 0;
	
	/* Convert into a wait list */
	tail = xchg_64(l, &me);
	
	if (tail)
	    {
		/* Add myself to the list of waiters */
		if (tail == LLOCK_FLAG) tail = NULL;
		me.next = tail;
			
		/* Wait for being able to go */
		while (!me.spin) cpu_relax();
		
		return;
	    }
	
	/* Try to convert to an exclusive lock */
	if (cmpxchg(l, &me, LLOCK_FLAG) == &me) 
        return;
		
	/* Failed - there is now a wait list */
	tail = *l;
		
	/* Scan to find who is after me */
	while (1)
	    {
		/* Wait for them to enter their next link */
		while (tail->next == LLOCK_FLAG) cpu_relax();
			
		if (tail->next == &me)
		    {
			/* Fix their next pointer */
			tail->next = NULL;
			
			return;
		    }
			
		tail = tail->next;
	    }
    }

static inline void listlock_unlock(listlock *l)
    {
	listlock_t *tail;
	listlock_t *tp;
	
	while (1)
	    {
		tail = *l;
		
		barrier();
	
		/* Fast path */
		if (tail == LLOCK_FLAG)
		    {
			if (cmpxchg(l, LLOCK_FLAG, NULL) == LLOCK_FLAG)
                return;
		
			continue;
		    }
				
		tp = NULL;
		
		/* Wait for partially added waiter */
		while (tail->next == LLOCK_FLAG) 
            cpu_relax();
		
		/* There is a wait list */
		if (tail->next) 
            break;
		
		/* Try to convert to a single-waiter lock */
		if (cmpxchg(l, tail, LLOCK_FLAG) == tail)
		    {
			/* Unlock */
			tail->spin = 1;
				
			return;
		    }
			
		cpu_relax();
	    }
		
	/* A long list */
	tp = tail;
	tail = tail->next;
	
	/* Scan wait list */
	while (1)
	    {
		/* Wait for partially added waiter */
		while (tail->next == LLOCK_FLAG) 
            cpu_relax();
			
		if (!tail->next) 
            break;
			
		tp = tail;
		tail = tail->next;
	    }
	
	tp->next = NULL;
		
	barrier();
	
	/* Unlock */
	tail->spin = 1;
    }

static inline int listlock_trylock(listlock *l)
    {
	/* Simple part of a spin-lock */
	if (!cmpxchg(l, NULL, LLOCK_FLAG)) 
        return OK;
	
	/* Failure! */
	return EBUSY;
    }

typedef struct bitlistlock_t bitlistlock_t;
struct bitlistlock_t
    {
	bitlistlock_t *next;
	int spin;
    };

typedef bitlistlock_t *bitlistlock;

#define BLL_USED	((bitlistlock_t *) -2LL)

static inline void bitlistlock_lock(bitlistlock *l)
    {
	bitlistlock_t me;
	bitlistlock_t *tail;
	
	/* Grab control of list */
	while (atomic_bitsetandtest(l, 0)) 
        cpu_relax();
	
	/* Remove locked bit */
	tail = (bitlistlock_t *) ((uintptr_t) *l & ~1LL);
	
	/* Fast path, no waiters */
	if (!tail)
	    {
		/* Set to be a flag value */
		*l = BLL_USED;
		return;
	    }
	
	if (tail == BLL_USED) 
        tail = NULL;
    
	me.next = tail;
	me.spin = 0;
	
	barrier();
	
	/* Unlock, and add myself to the wait list */
	*l = &me;
	
	/* Wait for the go-ahead */
	while (!me.spin) 
        cpu_relax();
    }

static inline void bitlistlock_unlock(bitlistlock *l)
    {
	bitlistlock_t *tail;
	bitlistlock_t *tp;
	
	/* Fast path - no wait list */
	if (cmpxchg(l, BLL_USED, NULL) == BLL_USED) 
        return;
	
	/* Grab control of list */
	while (atomic_bitsetandtest(l, 0)) 
        cpu_relax();
	
	tp = *l;
	
	barrier();
	
	/* Get end of list */
	tail = (bitlistlock_t *) ((uintptr_t) tp & ~1LL);
	
	/* Actually no users? */
	if (tail == BLL_USED)
	    {
		barrier();
		*l = NULL;
		return;
	    }
	
	/* Only one entry on wait list? */
	if (!tail->next)
	    {
		barrier();
		
		/* Unlock bitlock */
		*l = BLL_USED;
		
		barrier();
		
		/* Unlock lock */
		tail->spin = 1;
		
		return;
	    }
	
	barrier();

	/* Unlock bitlock */
	*l = tail;
	
	barrier();
		
	/* Scan wait list for start */
	do
	    {
		tp = tail;
		tail = tail->next;
	    }
	while (tail->next);
	
	tp->next = NULL;
	
	barrier();
	
	/* Unlock */
	tail->spin = 1;
    }

static inline int bitlistlock_trylock(bitlistlock *l)
    {
	if (!*l && (cmpxchg(l, NULL, BLL_USED) == NULL)) 
        return 0;
	
	return EBUSY;
    }

typedef struct plock_t plock_t;
struct plock_t
    {
	plock_t *next;
    };

typedef struct plock plock;
struct plock
    {
	plock_t *next;
	plock_t *prev;
	plock_t *last;
    };

static inline void plock_lock(plock *l)
    {
	plock_t *me = NULL;
	plock_t *prev;
	
	barrier();
	me = xchg_64(l, &me);
	
	prev = NULL;
	
	/* Wait until we get the lock */
	while (me)
	    {
		/* Scan wait list for my previous */
		if (l->next != (plock_t *) &me)
		    {
			plock_t *t = l->next;
			
			while (me)
			    {
				if (t->next == (plock_t *) &me)
				    {
					prev = t;
				
					while (me) 
                        cpu_relax();
					
					goto done;
				    }
				
				if (t->next) 
                    t = t->next;
                
				cpu_relax();
			    }
		    }
		cpu_relax();
	    }
	
done:	
	l->prev = prev;
	l->last = (plock_t *) &me;
    }

static inline void plock_unlock(plock *l)
    {
	plock_t *tail;
	
	/* Do I know my previous? */
	if (l->prev)
	    {
		/* Unlock */
		l->prev->next = NULL;
		return;
	    }
	
	tail = l->next;
	barrier();
	
	/* Fast case */
	if (tail == l->last)
	    {
		/* Try to remove the wait list */
		if (cmpxchg(&l->next, tail, NULL) == tail) return;
		
		tail = l->next;
	    }
	
	/* Scan wait list */
	while (1)
	    {
		/* Wait for partially added waiter */
		while (!tail->next) 
            cpu_relax();
		
		if (tail->next == l->last) break;
		
		tail = tail->next;
	    }
		
	barrier();
	
	/* Unlock */
	tail->next = NULL;
    }

static inline int plock_trylock(plock *l)
    {
	plock_t me;
	
	if (!cmpxchg(&l->next, NULL, &me))
	    {
		l->last = &me;
		return 0;
	    }
	
	return EBUSY;
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
