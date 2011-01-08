
#ifndef _K_TIMER_H
#define _K_TIMER_H

#include <sys.h>
#include <time.h>
#include <sys/time.h>
#include <os/clockeventer.h>
#include <os/rbtree.h>

#if 0
/*
 * Time-out element.
 */
 
typedef struct tick_timer 
    {
    struct clockeventer *   eventer;         /* our clock eventer */
    list_t                      entry;           /* list entry on timer chain */
    id_t                        id;              /* Timer ID */
    int                         state;           /* timer state */
    size_t                      expire;          /* expiration time, in ticks */
    size_t                      interval;        /* time interval */
    void                        (*func)(void *); /* function to call */
    void *                      arg;             /* function argument */
    }tick_timer_t;

typedef struct tick_timer_list
    {
    list_t          chain;      /* list head of timer chain */
    size_t          count;      /* Number of active timers on chain */
    size_t          maxium;     /* Maxium of active timers on chain */
    }tick_timer_list_t;
#endif

struct timerchain_node 
    {
    struct rbnode rbnode;    /* node in the rbtree */
    abstime_t expires;       /* expiration time, in nanosecond */
    abstime_t interval;      /* time interval */
    int       flags;         /* timer flags */
    void    (*func)(void *); /* function to call */
    void *    arg;             /* function argument */
    };

struct timerchain_head 
    {
    struct clockeventer * eventer;         /* our clock eventer */
    struct rbtree rbtree;
    struct timerchain_node *earliest;
    spinlock_t lock;
    };

extern void timerchain_subsystem_init(void);

extern int timerchain_compare(struct timerchain_node *t1, 
    struct timerchain_node *t2);

extern void timerchain_add(struct timerchain_head *head,
                struct timerchain_node *node);
extern void timerchain_remove(struct timerchain_head *head,
                struct timerchain_node *node);
extern struct timerchain_node *timerchain_iterate_next(
                        struct timerchain_node *node);

/**
 * timerchain_get_earliest - Returns the timer with the earlies expiration time
 *
 * @head: head of timerchain
 *
 * Returns a pointer to the timer node that has the earliest expiration time.
 */
static inline struct timerchain_node *timerchain_get_earliest
    (struct timerchain_head *head)
    {
    return head->earliest;
    }

static inline void timerchain_init_node(struct timerchain_node *node)
    {
    rbnode_init(&node->rbnode, node);
    }

static inline void timerchain_init_head(struct timerchain_head *head)
    {
    rbtree_init(&head->rbtree, (rb_compare)&timerchain_compare);
    spinlock_init(&head->lock);
    head->earliest = NULL;
    }

typedef void (*interval_timer_handler_t)(void * arg);

typedef struct interval_timer
    {
    struct timerchain_node timer_node; /* timer node */
    id_t timer_id;  /* ITIMER_PROF, ITIMER_REAL, and ITIMER_VIRTUAL */
    struct itimerval timerval;         /* current value */
    BOOL enabled;   /* Is this interval timer enabled */
    size_t remain_intervals;/* Remaining intervals to expire */
    pthread_t pid;  /* thread to send signal to when timer expires */
    interval_timer_handler_t handler; /* handler */
    void * arg;     /* argument to the fire handler */
    }interval_timer_t;

void itimer_callback_handler(void);

#define TWO_SECONDS_NS (NSECS_PER_SEC * 2)
#define TWO_SECONDS_US (USECS_PER_SEC * 2)
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
                                                    
    if ((aux > TWO_SECONDS_NS) || (aux < -TWO_SECONDS_NS))  
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

static inline void timeval_normalize (timeval_t * t)
    {
    if (t->tv_usec >= USECS_PER_SEC)      
        {                                  
        t->tv_usec -= USECS_PER_SEC;  
        t->tv_sec++;                  
        }                                   
    else if (t->tv_usec < 0)               
        {                                   
        t->tv_usec += USECS_PER_SEC;  
        t->tv_sec--;                  
        } 
    }

static inline void timeval_add (timeval_t * t1, timeval_t * t2)
    {
    t1->tv_usec += t2->tv_usec;         
    t1->tv_sec += t2->tv_sec;           
    timeval_normalize(t1);                 
    }

static inline void timeval_sub (timeval_t * t1, timeval_t * t2)
    {
    t1->tv_usec -= t2->tv_usec;         
    t1->tv_sec -= t2->tv_sec;           
    timeval_normalize(t1);                 
    }

static inline void timeval_add_us (timeval_t * t, long n)
    {
    long long aux = t->tv_usec + n;             
                                                    
    if ((aux > TWO_SECONDS_US) || (aux < -TWO_SECONDS_US))  
        {                                           
        t->tv_usec += (n % USECS_PER_SEC);      
        t->tv_sec += (n / USECS_PER_SEC);       
        }                                           
    else                                            
        {                                           
        t->tv_usec = aux;                         
        }                                                                                               
    timeval_normalize(t);                          
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

