/* sys/wait.h - declarations for waiting */

#ifndef _POSIX_SYS_WAIT_H
#define _POSIX_SYS_WAIT_H

#include <sys.h>
#include <signal.h>


/* waitpid()/waitid() options */
#define WNOHANG		0x01 /* Do not hang if no status is available; return immediately */
#define WUNTRACED	0x02 /* Report status of stopped child process. */
#define WCONTINUED	0x04 /* Report status of continued child process. */

#define WEXITED		0x08 /* Wait for processes that have exited.*/
#define WSTOPPED	0x10 /* Status is returned for any child that has stopped upon receipt of a signal.*/
#define WNOWAIT		0x20 /* Keep the process whose status is returned in infop in a waitable state.*/

/* macros to interprete wait()/waitpid() status */
#define WIFEXITED(value)	(((value) & ~0xff) == 0) /* True if child exited normally.*/
#define WEXITSTATUS(value)	((value) & 0xff) /* Return exit status.*/
#define WIFSIGNALED(value)	((((value) >> 8) & 0xff) != 0) /* True if child exited due to uncaught signal.*/
#define WTERMSIG(value)		(((value) >> 8) & 0xff) /* Return signal number that caused process to terminate.*/
#define WIFSTOPPED(value)	((((value) >> 16) & 0xff) != 0) /* True if child is currently stopped.*/
#define WSTOPSIG(value)		(((value) >> 16) & 0xff) /* Return signal number that caused process to stop.*/
#define WIFCORED(value)		((value) & 0x10000)
#define WIFCONTINUED(value)	((value) & 0x20000) /* True if child has been continued.*/

/*
 * The type idtype_t shall be defined as an enumeration type
 * whose possible values shall include at least the following:
 * P_ALL P_PGID P_PID
 */

/* ID types for waitid() */
typedef enum
    {
	P_ALL,		/* Wait for any children, ignore ID */
	P_PID,		/* Wait for the child whose process ID matches */
	P_PGID		/* Wait for any child whose process group ID matches */
    } idtype_t;


#ifdef __cplusplus
extern "C" {
#endif

pid_t  wait(int *);
int    waitid(idtype_t, id_t, siginfo_t *, int);
pid_t  waitpid(pid_t, int *, int);

#ifdef __cplusplus
}
#endif

#endif	/* _POSIX_SYS_WAIT_H */
