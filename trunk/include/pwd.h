/* pwd.h - password structure */

#ifndef _POSIX_PWD_H
#define _POSIX_PWD_H

/* 
 * The <pwd.h> header shall define the gid_t, uid_t,
 * and size_t types as described in <sys.h>.
 */

#include <sys.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The <pwd.h> header shall define the struct passwd structure */

struct passwd 
    {
	char	*pw_name; /* User's login name. */
	char	*pw_passwd; /* User's login password. */
	uid_t   pw_uid; /* Numerical user ID. */
	gid_t	pw_gid; /* Numerical group ID. */
	char	*pw_dir; /* Initial working directory. */
	char	*pw_shell; /* Program to use as shell. */
	char	*pw_gecos;
    };

void           endpwent(void);
struct passwd *getpwent(void);

struct passwd *getpwnam(const char *);
int            getpwnam_r(const char *, struct passwd *, char *,
                   size_t, struct passwd **);
struct passwd *getpwuid(uid_t);
int            getpwuid_r(uid_t, struct passwd *, char *,
                   size_t, struct passwd **);
void           setpwent(void);

#ifdef __cplusplus
}
#endif

#endif	/* _POSIX_PWD_H */
