/* spawn.h - spawn (ADVANCED REALTIME) */

#ifndef _POSIX_SPAWN_H
#define _POSIX_SPAWN_H

/*
 * The <spawn.h> header shall define the mode_t and pid_t 
 * types as described in <sys.h>.
 */
 
#include <sys.h>

/* 
 * The <spawn.h> header shall define the sigset_t type as 
 * described in <signal.h>
 */
 
#include <signal.h>

/*
 * The tag sched_param shall be declared as naming an incomplete
 * structure type, the contents of which are described in the 
 * <sched.h> header
 */
 
#include <sched.h>
 
#ifdef __cplusplus
extern "C" {
#endif

/*
 * The <spawn.h> header shall define the following symbolic 
 * constants for use as the flags that may be set in a 
 * posix_spawnattr_t object using the posix_spawnattr_setflags()
 * function
 */

#define POSIX_SPAWN_RESETIDS        0x00
#define POSIX_SPAWN_SETPGROUP       0x01
#define POSIX_SPAWN_SETSCHEDPARAM   0x02
#define POSIX_SPAWN_SETSCHEDULER    0x03
#define POSIX_SPAWN_SETSIGDEF       0x04
#define POSIX_SPAWN_SETSIGMASK      0x05

/* 
 * The <spawn.h> header shall define the posix_spawnattr_t 
 * and posix_spawn_file_actions_t types used in performing 
 * spawn operations.
 */

/* Data structure to contain attributes for thread creation. */

typedef struct posix_spawnattr
    {
    short int _flags;
    pid_t _pgrp;
    sigset_t _sd;
    sigset_t _ss;
    struct sched_param _sp;
    int _policy;
    int __pad[16];
    } posix_spawnattr_t;

/* Data structure to contain the action information.  */
struct __spawn_action
    {
      enum
        {
        spawn_do_close,
        spawn_do_dup2,
        spawn_do_open
        } tag;

      union
        {
        struct
            {
            int fd;
            } close_action;
        struct
            {
            int fd;
            int newfd;
            } dup2_action;
        struct
            {
            int fd;
            const char *path;
            int oflag;
            mode_t mode;
            } open_action;
        } action;
    };

typedef struct posix_spawn_file_actions
    {
    int _allocated;
    int _used;
    struct __spawn_action *_actions;
    int __pad[16];
    } posix_spawn_file_actions_t;

int   posix_spawn(pid_t *restrict, const char *restrict,
          const posix_spawn_file_actions_t *,
          const posix_spawnattr_t *restrict, char *const [restrict],
          char *const [restrict]);
int   posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *,
          int);
int   posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *,
          int, int);
int   posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *restrict,
          int, const char *restrict, int, mode_t);
int   posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *);
int   posix_spawn_file_actions_init(posix_spawn_file_actions_t *);
int   posix_spawnattr_destroy(posix_spawnattr_t *);
int   posix_spawnattr_getflags(const posix_spawnattr_t *restrict,
          short *restrict);
int   posix_spawnattr_getpgroup(const posix_spawnattr_t *restrict,
          pid_t *restrict);
int   posix_spawnattr_getschedparam(const posix_spawnattr_t *restrict,
          struct sched_param *restrict);
int   posix_spawnattr_getschedpolicy(const posix_spawnattr_t *restrict,
          int *restrict);

int   posix_spawnattr_getsigdefault(const posix_spawnattr_t *restrict,
          sigset_t *restrict);
int   posix_spawnattr_getsigmask(const posix_spawnattr_t *restrict,
          sigset_t *restrict);
int   posix_spawnattr_init(posix_spawnattr_t *);
int   posix_spawnattr_setflags(posix_spawnattr_t *, short);
int   posix_spawnattr_setpgroup(posix_spawnattr_t *, pid_t);

int   posix_spawnattr_setschedparam(posix_spawnattr_t *restrict,
          const struct sched_param *restrict);
int   posix_spawnattr_setschedpolicy(posix_spawnattr_t *, int);

int   posix_spawnattr_setsigdefault(posix_spawnattr_t *restrict,
          const sigset_t *restrict);
int   posix_spawnattr_setsigmask(posix_spawnattr_t *restrict,
          const sigset_t *restrict);
int   posix_spawnp(pid_t *restrict, const char *restrict,
          const posix_spawn_file_actions_t *,
          const posix_spawnattr_t *restrict,
          char *const [restrict], char *const [restrict]);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_SPAWN_H */

