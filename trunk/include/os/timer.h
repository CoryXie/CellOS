
#ifndef _K_TIMER_H
#define _K_TIMER_H

#include <sys.h>
#include <time.h>
#include <os.h>

/*
 * Time-out element.
 */
 
typedef struct ktimer 
    {
	struct list	    link;		/* linkage on timer chain */
	int		        state;		/* timer state */
	size_t		    expire;		/* expiration time, in ticks */
	size_t		    interval;	/* time interval */
	void		    (*func)(void *); /* function to call */
	void *          arg;		/* function argument */
    }ktimer_t;

#define NSECS_PER_SEC 1000000000
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

#endif 

