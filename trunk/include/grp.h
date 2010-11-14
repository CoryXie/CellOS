/* grp.h - group structure */

#ifndef _POSIX_GRP_H
#define _POSIX_GRP_H

/*
 * The <grp.h> header shall define the gid_t and size_t
 * types as described in <sys.h>.
 */

#include <sys.h>

/* The <grp.h> header shall declare the group structure */

typedef struct group
    {
    gid_t   gr_gid;     /* Numerical group ID. */
	char	*gr_name;   /* The name of the group. */
	char	*gr_passwd; /* The password of the group. */
	char	**gr_mem;   /* Pointer to a null-terminated array
                         * of character pointers to member names.*/
    }group_t;


#ifdef __cplusplus
extern "C" {
#endif

void setgrent(void);
void endgrent(void);

struct group *getgrgid(gid_t gid);
struct group *getgrnam(const char *name);
struct group *getgrent(void);

int getgrgid_r(gid_t gid, struct group *group, char *buffer,
				size_t bufferSize, struct group **_result);
int getgrnam_r(const char *name, struct group *group, char *buffer,
				size_t bufferSize, struct group **_result);
int getgrent_r(struct group* group, char* buffer, size_t bufferSize,
				struct group** _result);

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_GRP_H */
