/* netdb.h - definitions for network database operations */

#ifndef _POSIX_NETDB_H
#define _POSIX_NETDB_H

#include <sys/param.h>
#include <sys.h>
#include <sys/cdefs.h>

/*
 * The <netdb.h> header shall define the socklen_t type 
 * through typedef, as described in <sys/socket.h>.
 */
 
#include <sys/socket.h>

/* 
 * The <netdb.h> header may define the in_port_t type
 * and the in_addr_t type as described in <netinet/in.h>.
 */
 
#include <netinet/in.h>

/* 
 * The <netdb.h> header shall define the uint32_t type
 * as described in <inttypes.h>. 
 */
 
#include <inttypes.h>
 
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * The <netdb.h> header shall define the hostent structure
 *
 * Structures returned by network data base library.  All addresses are
 * supplied in host order, and returned in network order (suitable for
 * use in system calls).
 */
 
struct hostent 
    {
    char   *h_name;      /* Official name of the host. */
    char  **h_aliases;   /* A pointer to an array of pointers to 
                          * alternative host names, terminated by a 
                          * null pointer. */
    int     h_addrtype;  /* Address type. */ 
    int     h_length;    /* The length, in bytes, of the address. */
    char  **h_addr_list; /* A pointer to an array of pointers to network 
                          * addresses (in network byte order) for the host, 
                          * terminated by a null pointer. */
    };

#define	h_addr	h_addr_list[0]		/* address, for backward compatiblity */

/*
 * The <netdb.h> header shall define the netent structure
 *
 * Assumption here is that a network number
 * fits in an unsigned long -- probably a poor one.
 */
 
struct netent 
    {
    char     *n_name;     /* Official, fully-qualified (including the 
                           * domain) name of the host. */
    char    **n_aliases;  /* A pointer to an array of pointers to 
                           * alternative network names, terminated by a 
                           * null pointer. */
    int       n_addrtype; /* The address type of the network. */
    uint32_t  n_net;      /* The network number, in host byte order.*/ 
    };

/* The <netdb.h> header shall define the servent structure */

struct servent 
    {
    char   *s_name;     /* Official name of the service. */
    char  **s_aliases;  /* A pointer to an array of pointers to 
                         * alternative service names, terminated by 
                         * a null pointer. */
    int     s_port;     /* A value which, when converted to uint16_t, 
                         * yields the port number in network byte order 
                         * at which the service resides. */
    char   *s_proto;    /* The name of the protocol to use when 
                         * contacting the service. */
    };

/* The <netdb.h> header shall define the protoent structure */

struct protoent 
    {
    char   *p_name;     /* Official name of the protocol. */
    char  **p_aliases;  /* A pointer to an array of pointers to 
                         * alternative protocol names, terminated by 
                         * a null pointer. */
    int     p_proto;    /* The protocol number. */ 
    };

/* The <netdb.h> header shall define the addrinfo structure */

struct addrinfo 
    {
	int			ai_flags;			/* AI_PASSIVE, AI_CANONNAME */
	int			ai_family;			/* PF_xxx */
	int			ai_socktype;		/* SOCK_xxx */
	int			ai_protocol;		/* 0 or IPPROTO_xxx for IPv4 and IPv6 */
	socklen_t	ai_addrlen;			/* length of ai_addr */
	struct sockaddr	*ai_addr; 		/* binary address */
	char		*ai_canonname;		/* canonical name for hostname */
	struct addrinfo	*ai_next; 		/* next structure in linked list */
    };

/*
 * The <netdb.h> header shall define the IPPORT_RESERVED 
 * symbolic constant with the value of the highest reserved 
 * Internet port number.
 */
 
#define IPPORT_RESERVED       1024
 
/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (left in extern int h_errno).
 */
#define	NETDB_INTERNAL	-1		/* see errno */
#define	NETDB_SUCCESS	0		/* no problem */
#define	HOST_NOT_FOUND	1		/* Authoritative Answer Host not found */
#define	TRY_AGAIN		2
	/* Non-Authoritive Host not found, or SERVERFAIL */
#define	NO_RECOVERY		3
	/* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define	NO_DATA			4
	/* Valid name, no data record of requested type */
#define	NO_ADDRESS		NO_DATA	/* no address, look for MX record */

/*
 * Error return codes from getaddrinfo()
 *
 * The <netdb.h> header shall define the following symbolic 
 * constants for use as error values for getaddrinfo() and 
 * getnameinfo(). The values shall be suitable for use in 
 * #if preprocessing directives.
 */
 
#define	EAI_ADDRFAMILY	1	/* address family for hostname not supported */
#define	EAI_AGAIN	 	2	/* The name could not be resolved at this time. 
                             * Future attempts may succeed. */
#define	EAI_BADFLAGS	3	/* invalid value for ai_flags */
#define	EAI_FAIL		4	/* A non-recoverable error occurred. */
#define	EAI_FAMILY		5	/* The address family was not recognized or the 
                             * address length was invalid for the specified 
                             * family. */
#define	EAI_MEMORY		6	/* There was a memory allocation failure. */
#define	EAI_NODATA		7	/* no address associated with hostname */
#define	EAI_NONAME		8	/* The name does not resolve for the supplied 
                             * parameters. NI_NAMEREQD is set and the host's 
                             * name cannot be located, or both nodename and 
                             * servname were null.*/
#define	EAI_SERVICE		9	/* The service passed was not recognized for 
                             * the specified socket type. */
#define	EAI_SOCKTYPE	10	/* The intended socket type was not recognized. */
#define	EAI_SYSTEM		11	/* A system error occurred. The error code can 
                             * be found in errno. */
#define EAI_BADHINTS	12  /* Invalid value for hints */
#define EAI_PROTOCOL	13  /* Resolved protocol is unknown */
#define EAI_OVERFLOW	14  /* An argument buffer overflowed. */
#define EAI_BADFLAGS	15  /* The flags had an invalid value. */
#define EAI_MAX			15  /* Maxium number error codes */

/* 
 * The <netdb.h> header shall define the following symbolic
 * constants that evaluate to bitwise-distinct integer 
 * constants for use in the flags field of the addrinfo 
 * structure
 */ 

/*
 * Flag values for getaddrinfo()
 */
 
#define	AI_PASSIVE		0x00000001 /* Socket address is intended for bind(). */
#define	AI_CANONNAME	0x00000002 /* Request for canonical name.*/
#define AI_NUMERICHOST	0x00000004 /* Return numeric host address as name.*/
#define AI_NUMERICSERV	0x00000008 /* Inhibit service name resolution. */                                    
#define AI_V4MAPPED		0x00000010 /* If no IPv6 addresses are found, query
                                    * for IPv4 addresses and return them to 
                                    * the caller as IPv4-mapped IPv6 addresses. 
                                    */
#define AI_ALL			0x00000020 /* Query for both IPv4 and IPv6 addresses. */
#define AI_ADDRCONFIG	0x00000040 /* Query for IPv4 addresses only when an 
                                    * IPv4 address is configured; query for
                                    * IPv6 addresses only when an IPv6 address
                                    * is configured.*/

#define	AI_MASK			\
	(AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST | AI_NUMERICSERV |\
	 AI_V4MAPPED| AI_ADDRCONFIG| AI_ALL | AI_ADDRCONFIG)

/*
 * Flag values for getipnodebyname()
 */
#define AI_V4MAPPED_CFG	0x00000200 /* Accept IPv4-mapped if kernel supports */
#define AI_DEFAULT		(AI_V4MAPPED_CFG | AI_ADDRCONFIG)

/*
 * Constants for getnameinfo()
 */
#define	NI_MAXHOST		1025
#define	NI_MAXSERV		32

/*
 * Flag values for getnameinfo()
 *
 * The <netdb.h> header shall define the following symbolic
 * constants that evaluate to bitwise-distinct integer 
 * constants for use in the flags argument to getnameinfo()
 */
 
#define	NI_NOFQDN		0x00000001 /* Only the nodename portion of the FQDN
                                    * is returned for local hosts.*/
#define	NI_NUMERICHOST	0x00000002 /* The numeric form of the node's address
                                    * is returned instead of its name.*/
#define	NI_NAMEREQD		0x00000004 /* Return an error if the node's name 
                                    * cannot be located in the database.*/
#define	NI_NUMERICSERV	0x00000008 /* The numeric form of the service address
                                    * is returned instead of its name.*/
#define	NI_DGRAM		0x00000010 /* Indicates that the service is a datagram 
                                    * service (SOCK_DGRAM).*/
#define	NI_WITHSCOPEID	0x00000020 /* Backward compatibility with old systems */
#define NI_NUMERICSCOPE	0x00000040 /* For IPv6 addresses, the numeric form 
                                    * of the scope identifier is returned 
                                    * instead of its name.*/

/*
 * Scope delimit character
 */
 
#define SCOPE_DELIMITER	'%'

#ifndef _PATH_NETWORKS
#define	_PATH_NETWORKS	"/etc/networks"
#endif

void              endhostent(void);
void              endnetent(void);
void              endprotoent(void);
void              endservent(void);
void              freeaddrinfo(struct addrinfo *);
const char       *gai_strerror(int);
int               getaddrinfo(const char *restrict, const char *restrict,
                      const struct addrinfo *restrict,
                      struct addrinfo **restrict);
struct hostent   *gethostent(void);
int               getnameinfo(const struct sockaddr *restrict, socklen_t,
                      char *restrict, socklen_t, char *restrict,
                      socklen_t, int);
struct netent    *getnetbyaddr(uint32_t, int);
struct netent    *getnetbyname(const char *);
struct netent    *getnetent(void);
struct protoent  *getprotobyname(const char *);
struct protoent  *getprotobynumber(int);
struct protoent  *getprotoent(void);
struct servent   *getservbyname(const char *, const char *);
struct servent   *getservbyport(int, const char *);
struct servent   *getservent(void);
void              sethostent(int);
void              setnetent(int);
void              setprotoent(int);
void              setservent(int);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_NETDB_H */
