/* queue.h - generic queque operations with safe lock option */

#ifndef _QUEUE_H
#define _QUEUE_H

#include <stddef.h>
#include <arch.h>

struct qhead;

typedef struct qelement 
    {
	struct qelement   *next; /* Next element on the queque */
	struct qelement   *prev; /* Previous element on the queque */
    struct qhead      *head; /* Head of the queque I am on */
    }qelement_t;

typedef struct qhead 
    {
    char               name[NAME_MAX]; /* Optional queue name */
    size_t             count; /* Number of elements on this queue */
    spinlock_t         safe; /* Per queue specific locking */
    int                lock; /* Is it created with locking ? */
    qelement_t         self; /* Dummy element of the head itself */
    }qhead_t;

/* 
 * The QUEUE_LOCK and QUEUE_UNLOCK can be used by external code with 
 * broader scope of locking than provided by the internal per routine
 * specific locking. The 'lock' filed in the 'qhead_t' strcuture is
 * only set to true if the queue_init() specified so. This alows external
 * code to provide module specific locking, or even without locking if
 * that module is believed to be lock safe!
 */

#define QUEUE_LOCK(head)        do {            \
        if ((head)->lock)                       \
            spinlock_lock(&(head)->safe);       \
        }while(0)
        
#define QUEUE_UNLOCK(head)      do {            \
        if ((head)->lock)                       \
            spinlock_unlock(&(head)->safe);     \
        }while(0)

#define QUEUE_ISR_LOCK(head)        do {                \
                __cpu_intr_flags = intr_disable();      \
                if ((head)->lock)                       \
                    spinlock_lock(&(head)->safe);       \
                }while(0)
                
#define QUEUE_ISR_UNLOCK(head)      do {                \
                if ((head)->lock)                       \
                    spinlock_unlock(&(head)->safe);     \
                    intr_restore(__cpu_intr_flags);     \
                }while(0)

/* The following os assume the lock of the queue head has been locked */

#define queue_empty(head)	    ((head)->self.next == (&(head)->self))
#define queue_next(item)		((item)->next)
#define queue_prev(item)		((item)->prev)
#define queue_first(head)	    ((head)->self.next)
#define queue_last(head)	    ((head)->self.prev)
#define queue_end(head,item)	((item) == &(head)->self)

/* Get the struct for this entry */

#define queue_entry(item, type, member) \
    (type *)((char *)item - offsetof(type, member))

#define QUEUE_ITERATE(head, iter)        \
    for(qelement_t *iter = (head)->self.next; \
        iter != &(head)->self; iter = iter->next)

/*
 * Initialize the queue head
 */
 
static inline void queue_init (qhead_t *head, char * name, int lock)
    {
    memset(head, 0, sizeof(qhead_t));
    
    head->self.next = head->self.prev = &head->self;
    
    head->count = 0;

    if (lock)
        spinlock_init(&head->safe);
    
    if ((name == NULL) || strlen(name) == 0)
        name = "NoNameQ";  
    
    strncpy(head->name, name, NAME_MAX);
    }

/*
 * Check if the queue is empty with locking
 */
 
static inline bool queue_empty_safe (qhead_t *head)
    {
    bool empty ;

    QUEUE_LOCK(head);

    empty = (head->self.next == &head->self);

    QUEUE_UNLOCK(head);

    return empty;
    }

/*
 * Get the next queue element of this element with locking
 */
 
static inline qelement_t * queue_next_safe (qelement_t * item)
    {
    qelement_t * next;

    QUEUE_LOCK(item->head);

    next = item->next;

    QUEUE_UNLOCK(item->head);

    return next;
    }

/*
 * Get the previous queue element of this element with locking
 */
 
static inline qelement_t * queue_prev_safe (qelement_t * item)
    {
    qelement_t * prev;

    QUEUE_LOCK(item->head);

    prev = item->prev;

    QUEUE_UNLOCK(item->head);

    return prev;
    }

/*
 * Get the first queue element of this queue with locking
 */
 
static inline qelement_t * queue_first_safe (qhead_t * head)
    {
    qelement_t * first;

    QUEUE_LOCK(head);

    first = head->self.next;

    QUEUE_UNLOCK(head);

    return first;
    }

/*
 * Get the last queue element of this queue with locking
 */
 
static inline qelement_t * queue_last_safe (qhead_t * head)
    {
    qelement_t * last;

    QUEUE_LOCK(head);

    last = head->self.prev;

    QUEUE_UNLOCK(head);

    return last;
    }

/*
 * Insert element at tail of queue, with optional locking 
 */
 
static inline void enqueue (qhead_t * head, qelement_t * item, int lock)
    {
    if (lock) 
        QUEUE_LOCK(head);
    
	item->next = &head->self;
	item->prev = head->self.prev;
	item->prev->next = item;
	head->self.prev = item;
    
    head->count++;
    
    item->head = head;
    
    if (lock) 
        QUEUE_UNLOCK(head);
    }

/*
 * Remove and return element of head of queue, with optional locking
 */
 
static inline  qelement_t * dequeue (qhead_t * head, int lock)
    {
	qelement_t * item;

    if (lock) 
        QUEUE_LOCK(head);

	if (head->self.next == &head->self)
        {  
        if (lock) 
            QUEUE_UNLOCK(head);
        
		return ((qelement_t *)NULL);
        }
    
	item = head->self.next;
	item->next->prev = &head->self;
	head->self.next = item->next;

    head->count--;

    item->head = NULL;

    if (lock) 
        QUEUE_UNLOCK(head);

	return item;
    }

/*
 * Insert element after specified element, with optional locking
 */
 
static inline void queue_insert (qelement_t * prev, qelement_t * item, int lock)
    {   
    if (lock) 
        QUEUE_LOCK(prev->head);
    
	item->prev = prev;
	item->next = prev->next;
	prev->next->prev = item;
	prev->next = item;

    prev->head->count++;

    if (lock) 
        QUEUE_UNLOCK(prev->head);
    }

/*
 * Remove specified element from queue, with optional locking 
 */
 
static inline void queue_remove (qelement_t * item, int lock)
    {
    qhead_t * head = item->head;
    
    if (head == NULL) 
        return;
    
    if (lock) 
        QUEUE_LOCK(head);

    //printk("queue_remove %p\n", item);
    
    if (item->head)
        {
    	item->prev->next = item->next;
        if (item->next)
    	    item->next->prev = item->prev;
        item->head->count--;
        item->head = NULL;
        }

    if (lock) 
        QUEUE_UNLOCK(head);

    //printk("queue_remove %p done\n", item);
    }

#endif /* _QUEUE_H */
