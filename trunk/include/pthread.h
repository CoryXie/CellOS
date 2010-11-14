/* pthread.h - threads */

#ifndef _POSIX_PTHREAD_H
#define _POSIX_PTHREAD_H

#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <signal.h>
#include <sched.h>
#include <os/sched_thread.h>

/*
 * The <pthread.h> header shall define the 
 * pthread_attr_t, pthread_barrier_t,
 * pthread_barrierattr_t, pthread_cond_t, 
 * pthread_condattr_t, pthread_key_t,
 * pthread_mutex_t, pthread_mutexattr_t, 
 * pthread_once_t, pthread_rwlock_t,
 * pthread_rwlockattr_t, pthread_spinlock_t, 
 * and pthread_t types as described in <sys/types.h>.
 */
 
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct	sched_thread;
struct  sched_thread_attr;
struct  sched_mutex;
struct  sched_mutex_attr;

typedef struct	sched_thread		*pthread_t;
typedef struct  sched_thread_attr   *pthread_attr_t;
typedef struct  sched_mutex		    *pthread_mutex_t;
typedef struct  sched_mutex_attr	*pthread_mutexattr_t;
typedef struct  _pthread_cond		pthread_cond_t;
typedef struct  _pthread_condattr	*pthread_condattr_t;
typedef int							pthread_key_t;
typedef struct  _pthread_once		pthread_once_t;
typedef struct  _pthread_rwlock		pthread_rwlock_t;
typedef struct  _pthread_rwlockattr	*pthread_rwlockattr_t;
typedef struct  _pthread_spinlock	pthread_spinlock_t;
typedef struct  _pthread_barrier	*pthread_barrier_t;
typedef struct  _pthread_barrierattr *pthread_barrierattr_t;

struct _pthread_cond 
    {
	uint32_t		flags;
	int32_t			unused;
	pthread_mutex_t	*mutex;
	int32_t			waiter_count;
	int32_t			lock;
    };

struct _pthread_once 
    {
	int32_t		state;
    };

struct _pthread_rwlock 
    {
	uint32_t	flags;
	int32_t		owner;
	union 
        {
		struct 
            {
			int32_t		sem;
		    } shared;
		struct 
            {
			int32_t		lock_sem;
			int32_t		lock_count;
			int32_t		reader_count;
			int32_t		writer_count;
			void*		waiters[2];
		    } local;
	    };
    };

struct _pthread_spinlock 
    {
	spinlock_t lock;
	ipl_t      flags;
    };

typedef struct _pthread_condattr 
    {
	bool		process_shared;
    } pthread_condattr;

typedef struct _pthread_mutexattr 
    {
	int32_t		type;
	bool		process_shared;
    } pthread_mutexattr;

typedef struct _pthread_rwlockattr 
    {
	uint32_t	flags;
    } pthread_rwlockattr;

typedef void (*pthread_key_destructor)(void *data);

struct pthread_key 
    {
	volatile int32_t		sequence;
	pthread_key_destructor destructor;
    };

struct pthread_key_data 
    {
	volatile int32_t		sequence;
	void		*value;
    };

#define PTHREAD_UNUSED_SEQUENCE	0

/* Mutex type attribture */
#define PTHREAD_MUTEX_DEFAULT		0
#define PTHREAD_MUTEX_NORMAL		1
#define PTHREAD_MUTEX_ERRORCHECK	2
#define PTHREAD_MUTEX_RECURSIVE		3
#define PTHREAD_MUTEX_ADAPTIVE      4

/* Mutex robus attribute */
#define PTHREAD_MUTEX_STALLED       0
#define PTHREAD_MUTEX_ROBUST        1

/* Mutex pshared attribute */
#define PTHREAD_PROCESS_PRIVATE		0
#define PTHREAD_PROCESS_SHARED		1

/* Mutex protocol attribute */
#define PTHREAD_PRIO_NONE			0
#define PTHREAD_PRIO_INHERIT		1
#define PTHREAD_PRIO_PROTECT		2

/* Mutex wakeup order attribute */
#define PTHREAD_ORDER_FIFO		    0
#define PTHREAD_ORDER_PRIO		    1

/*
 * Flags for threads and thread attributes.
 */
 
#define PTHREAD_DETACHED			0x1
#define PTHREAD_NOFLOAT				0x8

#define PTHREAD_CREATE_DETACHED		PTHREAD_DETACHED
#define PTHREAD_CREATE_JOINABLE		0

/* Scheduling contention scope */
#define PTHREAD_SCOPE_PROCESS		0
#define PTHREAD_SCOPE_SYSTEM		1

/* Inherit schedule attibutes */
#define PTHREAD_EXPLICIT_SCHED		0
#define PTHREAD_INHERIT_SCHED		1

/*
 * pthread_setcancelstate paramters
 */
#define PTHREAD_CANCEL_ENABLE		0
#define PTHREAD_CANCEL_DISABLE		1

/*
 * pthread_setcanceltype parameters
 */
#define PTHREAD_CANCEL_ASYNCHRONOUS	0
#define PTHREAD_CANCEL_DEFERRED		1

#define PTHREAD_CANCELED		    (void *)(-1)

#define PTHREAD_ONCE_INIT 			{ -1 }

#define PTHREAD_BARRIER_SERIAL_THREAD   -1

/* private structure */

struct sched_thread_cleanup 
    {
	void (*routine)(void*);
	void *arg;
	struct sched_thread_cleanup *next;
    };

#define pthread_cleanup_push(p_routine, p_arg)  \
    do                                          \
    {                                           \
	ipl_t __flags;                              \
	struct sched_thread_cleanup __cleanup;      \
	__cleanup.routine = (p_routine);            \
	__cleanup.arg = (p_arg);                    \
	__flags = interrupts_disable(void);         \
	__cleanup.next = pthread_self()->cleanup;   \
	pthread_self()->cleanup = &__cleanup;       \
	interrupts_restore(__flags);


#define pthread_cleanup_pop(execute)            \
	__flags = interrupts_disable(void);         \
	pthread_self()->cleanup =                   \
	    pthread_self()->cleanup->next;          \
	if (execute)                                \
		__cleanup.routine(__cleanup.arg);       \
	interrupts_restore(__flags);                \
    }while(0)

/*
 * The <pthread.h> header shall define the following 
 * compile-time constant expressions valid as initializers
 * for the following types:
 *
 * PTHREAD_COND_INITIALIZER - pthread_cond_t
 * 
 * PTHREAD_MUTEX_INITIALIZER - pthread_mutex_t
 * 
 * PTHREAD_RWLOCK_INITIALIZER - pthread_rwlock_t
 */
 
#define PTHREAD_MUTEX_INITIALIZER \
	{ PTHREAD_MUTEX_DEFAULT, 0, -42, -1, 0 }
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER \
	{ PTHREAD_MUTEX_RECURSIVE, 0, -42, -1, 0 }
#define PTHREAD_COND_INITIALIZER	\
	{ 0, -42, NULL, 0, 0 }

int   pthread_atfork(void (*)(void), void (*)(void),
          void(*)(void));
int   pthread_attr_destroy(pthread_attr_t *);
int   pthread_attr_getdetachstate(const pthread_attr_t *, int *);
int   pthread_attr_getguardsize(const pthread_attr_t *restrict,
          size_t *restrict);
int   pthread_attr_getinheritsched(const pthread_attr_t *restrict,
          int *restrict);

int   pthread_attr_getschedparam(const pthread_attr_t *restrict,
          struct sched_param *restrict);
int   pthread_attr_getschedpolicy(const pthread_attr_t *restrict,
          int *restrict);
int   pthread_attr_getscope(const pthread_attr_t *restrict,
          int *restrict);

int   pthread_attr_getstack(const pthread_attr_t *restrict,
          void **restrict, size_t *restrict);

int   pthread_attr_getstacksize(const pthread_attr_t *restrict,
          size_t *restrict);

int   pthread_attr_init(pthread_attr_t *);
int   pthread_attr_setdetachstate(pthread_attr_t *, int);
int   pthread_attr_setguardsize(pthread_attr_t *, size_t);
int   pthread_attr_setinheritsched(pthread_attr_t *, int);

int   pthread_attr_setschedparam(pthread_attr_t *restrict,
          const struct sched_param *restrict);
int   pthread_attr_setschedpolicy(pthread_attr_t *, int);
int   pthread_attr_setscope(pthread_attr_t *, int);

int   pthread_attr_setstack(pthread_attr_t *, void *, size_t);

int   pthread_attr_setstacksize(pthread_attr_t *, size_t);

int   pthread_barrier_destroy(pthread_barrier_t *);
int   pthread_barrier_init(pthread_barrier_t *restrict,
          const pthread_barrierattr_t *restrict, unsigned);
int   pthread_barrier_wait(pthread_barrier_t *);
int   pthread_barrierattr_destroy(pthread_barrierattr_t *);
int   pthread_barrierattr_getpshared(
          const pthread_barrierattr_t *restrict, int *restrict);

int   pthread_barrierattr_init(pthread_barrierattr_t *);
int   pthread_barrierattr_setpshared(pthread_barrierattr_t *, int);

int   pthread_cancel(pthread_t);
int   pthread_cond_broadcast(pthread_cond_t *);
int   pthread_cond_destroy(pthread_cond_t *);
int   pthread_cond_init(pthread_cond_t *restrict,
          const pthread_condattr_t *restrict);
int   pthread_cond_signal(pthread_cond_t *);
int   pthread_cond_timedwait(pthread_cond_t *restrict,
          pthread_mutex_t *restrict, const struct timespec *restrict);
int   pthread_cond_wait(pthread_cond_t *restrict,
          pthread_mutex_t *restrict);
int   pthread_condattr_destroy(pthread_condattr_t *);
int   pthread_condattr_getclock(const pthread_condattr_t *restrict,
          clockid_t *restrict);
int   pthread_condattr_getpshared(const pthread_condattr_t *restrict,
          int *restrict);

int   pthread_condattr_init(pthread_condattr_t *);
int   pthread_condattr_setclock(pthread_condattr_t *, clockid_t);
int   pthread_condattr_setpshared(pthread_condattr_t *, int);

int   pthread_create(pthread_t *restrict, const pthread_attr_t *restrict,
          void *(*)(void*), void *restrict);
int   pthread_detach(pthread_t);
int   pthread_equal(pthread_t, pthread_t);
void  pthread_exit(void *);
int   pthread_getconcurrency(void);

int   pthread_getcpuclockid(pthread_t, clockid_t *);

int   pthread_getschedparam(pthread_t, int *restrict,
          struct sched_param *restrict);

void *pthread_getspecific(pthread_key_t);
int   pthread_join(pthread_t, void **);
int   pthread_key_create(pthread_key_t *, void (*)(void*));
int   pthread_key_delete(pthread_key_t);
int   pthread_mutex_consistent(pthread_mutex_t *);
int   pthread_mutex_destroy(pthread_mutex_t *);
int   pthread_mutex_getprioceiling(const pthread_mutex_t *restrict,
          int *restrict);

int   pthread_mutex_init(pthread_mutex_t *restrict,
          const pthread_mutexattr_t *restrict);
int   pthread_mutex_lock(pthread_mutex_t *);
int   pthread_mutex_setprioceiling(pthread_mutex_t *restrict, int,
          int *restrict);

int   pthread_mutex_timedlock(pthread_mutex_t *restrict,
          const struct timespec *restrict);
int   pthread_mutex_trylock(pthread_mutex_t *);
int   pthread_mutex_unlock(pthread_mutex_t *);
int   pthread_mutexattr_destroy(pthread_mutexattr_t *);
int   pthread_mutexattr_getprioceiling(
          const pthread_mutexattr_t *restrict, int *restrict);

int   pthread_mutexattr_getprotocol(const pthread_mutexattr_t *restrict,
          int *restrict);

int   pthread_mutexattr_getpshared(const pthread_mutexattr_t *restrict,
          int *restrict);

int   pthread_mutexattr_getrobust(const pthread_mutexattr_t *restrict,
          int *restrict);
int   pthread_mutexattr_gettype(const pthread_mutexattr_t *restrict,
          int *restrict);
int   pthread_mutexattr_init(pthread_mutexattr_t *);
int   pthread_mutexattr_setprioceiling(pthread_mutexattr_t *, int);

int   pthread_mutexattr_setprotocol(pthread_mutexattr_t *, int);

int   pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);

int   pthread_mutexattr_setrobust(pthread_mutexattr_t *, int);
int   pthread_mutexattr_settype(pthread_mutexattr_t *, int);
int   pthread_once(pthread_once_t *, void (*)(void));
int   pthread_rwlock_destroy(pthread_rwlock_t *);
int   pthread_rwlock_init(pthread_rwlock_t *restrict,
          const pthread_rwlockattr_t *restrict);
int   pthread_rwlock_rdlock(pthread_rwlock_t *);
int   pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict,
          const struct timespec *restrict);
int   pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict,
          const struct timespec *restrict);
int   pthread_rwlock_tryrdlock(pthread_rwlock_t *);
int   pthread_rwlock_trywrlock(pthread_rwlock_t *);
int   pthread_rwlock_unlock(pthread_rwlock_t *);
int   pthread_rwlock_wrlock(pthread_rwlock_t *);
int   pthread_rwlockattr_destroy(pthread_rwlockattr_t *);
int   pthread_rwlockattr_getpshared(
          const pthread_rwlockattr_t *restrict, int *restrict);

int   pthread_rwlockattr_init(pthread_rwlockattr_t *);
int   pthread_rwlockattr_setpshared(pthread_rwlockattr_t *, int);

/* pthread_t pthread_self(void); */
#define pthread_self() kurrent

int   pthread_setcancelstate(int, int *);
int   pthread_setcanceltype(int, int *);
int   pthread_setconcurrency(int);
int   pthread_setschedparam(pthread_t, int,
          const struct sched_param *);
int   pthread_setschedprio(pthread_t, int);

int   pthread_setspecific(pthread_key_t, const void *);

int pthread_spin_destroy(pthread_spinlock_t *lock);
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);


void  pthread_testcancel(void);

#ifdef __cplusplus
}
#endif

#endif	/* _PTHREAD_ */
