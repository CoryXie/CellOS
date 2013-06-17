/* utmpx.h - user accounting database definitions */

#ifndef _POSIX_TAR_H
#define _POSIX_TAR_H

#include <sys.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The <utmpx.h> header shall define the utmpx structure */

typedef struct utmpx
    {
    char            ut_user[];  /* User login name. */
    char            ut_id[];    /* Unspecified initialization process identifier. */
    char            ut_line[];  /* Device name. */
    pid_t           ut_pid;     /* Process ID. */
    short           ut_type;    /* Type of entry. */
    struct timeval  ut_tv;      /* Time entry was made. */
    }utmpx_t;

/*
 * The <utmpx.h> header shall define the following symbolic constants
 * as possible values for the ut_type member of the utmpx structure
 */

#define EMPTY           1   /* No valid user accounting information. */
#define BOOT_TIME       2   /* Identifies time of system boot. */
#define OLD_TIME        3   /* Identifies time when system clock changed. */
#define NEW_TIME        4   /* Identifies time after system clock changed. */
#define USER_PROCESS    5   /* Identifies a process. */
#define INIT_PROCESS    6   /* Identifies a process spawned by the init process. */
#define LOGIN_PROCESS   7   /* Identifies the session leader of a logged-in user. */
#define DEAD_PROCESS    8   /* Identifies a session leader who has exited. */

void          endutxent(void);
struct utmpx *getutxent(void);
struct utmpx *getutxid(const struct utmpx *);
struct utmpx *getutxline(const struct utmpx *);
struct utmpx *pututxline(const struct utmpx *);
void          setutxent(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_TAR_H */

