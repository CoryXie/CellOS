/* sys/un.h - definitions for UNIX domain sockets */

#ifndef _POSIX_SYS_UN_H
#define _POSIX_SYS_UN_H

#include <sys/socket.h>

/*
 * The <sys/un.h> header shall define the sockaddr_un structure.
 *
 * The sockaddr_un structure is used to store addresses for UNIX
 * domain sockets. Values of this type shall be cast by applications
 * to struct sockaddr for use with socket functions.
 */
struct sockaddr_un
    {
	uint8_t		sun_len;
	uint8_t		sun_family; /* Address family. */
	char		sun_path[126]; /* Socket pathname. */
    };

#endif	/* _POSIX_SYS_UN_H */
