/* sched_task.h - scheduler task management */

#ifndef _OS_SCHED_TASK_H
#define _OS_SCHED_TASK_H

#include <sys.h>
#include <arch.h>
#include <sched.h>
#include <os/sched_core.h>

/*
 * Task struct
 */
typedef struct sched_task 
    {
	char            name[NAME_MAX];     /* Task name */
	struct list	    node;		        /* node on task list in system */
	struct sched_task *	parent;		    /* parent process */
	struct pmap	    asp;		        /* address space description */
	size_t		    suscnt;		        /* suspend count */
	uint32_t		flags;		        /* flags defined below */
	ktimer_t	    alarm;		        /* timer for alarm exception */
	void            (*handler)(ptr_t);  /* pointer to exception handler */
	list_t	        threads;	        /* list of threads */
	list_t	        mutexes;	        /* mutexes owned by this task */
	list_t	        conditions;	        /* condition variables owned by this task */
	list_t	        semaphores;	        /* semaphores owned by this task */
    }sched_task_t;

#endif /* _OS_SCHED_TASK_H */

