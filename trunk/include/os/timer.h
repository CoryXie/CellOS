
#ifndef _K_TIMER_H
#define _K_TIMER_H

#include <sys.h>
#include <time.h>
#include <sys/time.h>
#include <os/clockeventer.h>

/*
 * Time-out element.
 */
 
typedef struct tick_timer 
    {
    struct os_clock_eventer * eventer; /* our clock eventer */
	list_t	        entry;		/* list entry on timer chain */
    id_t            id;         /* Timer ID */
	int		        state;		/* timer state */
	size_t		    expire;		/* expiration time, in ticks */
	size_t		    interval;	/* time interval */
	void		    (*func)(void *); /* function to call */
	void *          arg;		/* function argument */
    }tick_timer_t;

typedef struct tick_timer_list
    {
	list_t	        chain;		/* list head of timer chain */
    size_t          count;      /* Number of active timers on chain */
    size_t          maxium;     /* Maxium of active timers on chain */
    }tick_timer_list_t;

typedef void (*interval_timer_handler_t)(void * arg);

typedef struct interval_timer
    {
    id_t timer_id;  /* ITIMER_PROF, ITIMER_REAL, and ITIMER_VIRTUAL */
    struct os_clock_eventer * eventer; /* our clock eventer */
    struct itimerval timerval;         /* current value */
    BOOL enabled;   /* Is this interval timer enabled */
    size_t remain_intervals;/* Remaining intervals to expire */
    pthread_t pid;  /* thread to send signal to when timer expires */
    interval_timer_handler_t handler; /* handler */
    void * arg;     /* argument to the fire handler */
    }interval_timer_t;

void itimer_callback_handler(void);

extern interval_timer_t global_itimer_ITIMER_PROF;
extern interval_timer_t global_itimer_ITIMER_REAL;
extern interval_timer_t global_itimer_ITIMER_VIRTUAL;

#define TWOSECONDS (NSECS_PER_SEC * 2)
#define ABSTIME_INFINITY 0x7fffFfffFfffFfffLL

static inline void timespec_normalize (timespec_t * t)
    {
    if (t->tv_nsec >= NSECS_PER_SEC)      
        {                                  
        t->tv_nsec -= NSECS_PER_SEC;  
        t->tv_sec++;                  
        }                                   
    else if (t->tv_nsec < 0)               
        {                                   
        t->tv_nsec += NSECS_PER_SEC;  
        t->tv_sec--;                  
        } 
    }

static inline void timespec_add (timespec_t * t1, timespec_t * t2)
    {
	t1->tv_nsec += t2->tv_nsec;         
	t1->tv_sec += t2->tv_sec;           
	timespec_normalize(t1);                 
    }

static inline void timespec_sub (timespec_t * t1, timespec_t * t2)
    {
	t1->tv_nsec -= t2->tv_nsec;         
	t1->tv_sec -= t2->tv_sec;           
	timespec_normalize(t1);                 
    }

static inline void timespec_add_ns (timespec_t * t, long n)
    {
    long long aux = t->tv_nsec + n;             
                                                    
    if ((aux > TWOSECONDS) || (aux < -TWOSECONDS))  
        {                                           
        t->tv_nsec += (n % NSECS_PER_SEC);      
        t->tv_sec += (n / NSECS_PER_SEC);       
        }                                           
    else                                            
        {                                           
        t->tv_nsec = aux;                         
        }                                                                                               
    timespec_normalize(t);                          
    }

static inline abstime_t timespec_to_abstime(timespec_t * t)
    {
    return (abstime_t)(((abstime_t)t->tv_sec * NSECS_PER_SEC) + 
                        (abstime_t)t->tv_nsec);
    }

static inline abstime_t timeval_to_abstime(timeval_t * t)
    {
    return (abstime_t)(((abstime_t)t->tv_sec * NSECS_PER_SEC) + 
                        (abstime_t)USECS2NSECS(t->tv_usec));
    }

static inline timespec_t * abstime_to_timespec(abstime_t abst, timespec_t * ts)
    {
    ts->tv_sec = (abst % NSECS_PER_SEC); 
    ts->tv_nsec = (abst / NSECS_PER_SEC);

    return ts;
    }

static inline timeval_t * abstime_to_timeval(abstime_t abst, timeval_t * tv)
    {
    tv->tv_sec = (abst % USECS_PER_SEC); 
    tv->tv_usec = (abst / USECS_PER_SEC);

    return tv;
    }

#define timespec_nz(t)                  \
    ((t)->tv_sec != 0 || (t)->tv_nsec != 0)

#define timespec_lt(t1, t2)             \
    ((t1)->tv_sec < (t2)->tv_sec ||     \
    ((t1)->tv_sec == (t2)->tv_sec &&    \
    (t1)->tv_nsec < (t2)->tv_nsec))

#define timespec_gt(t1, t2)             \
    (timespec_lt(t2, t1))

#define timespec_ge(t1, t2)             \
    (!timespec_lt(t1, t2))

#define timespec_le(t1, t2)             \
    (!timespec_gt(t1, t2))

#define timespec_eq(t1, t2)             \
    ((t1)->tv_sec == (t2)->tv_sec &&    \
     (t1)->tv_nsec == (t2)->tv_nsec)
     
/* Time Val */
#define timeval_nz(t)                  \
    ((t)->tv_sec != 0 || (t)->tv_usec != 0)

#define timeval_lt(t1, t2)             \
    ((t1)->tv_sec < (t2)->tv_sec ||     \
    ((t1)->tv_sec == (t2)->tv_sec &&    \
    (t1)->tv_usec < (t2)->tv_usec))

#define timeval_gt(t1, t2)             \
    (timeval_lt(t2, t1))

#define timeval_ge(t1, t2)             \
    (!timeval_lt(t1, t2))

#define timeval_le(t1, t2)             \
    (!timeval_gt(t1, t2))

#define timeval_eq(t1, t2)             \
    ((t1)->tv_sec == (t2)->tv_sec &&    \
     (t1)->tv_usec == (t2)->tv_usec)

#endif 

