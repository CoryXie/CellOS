/* sys/socket.h - main sockets header */

/*
APPLICATION USAGE

    To forestall portability problems, it is recommended that applications
    not use values larger than 231 -1 for the socklen_t type.

    The sockaddr_storage structure solves the problem of declaring storage
    for automatic variables which is both large enough and aligned enough
    for storing the socket address data structure of any family. For 
    example, code with a file descriptor and without the context of the
    address family can pass a pointer to a variable of this type, where 
    a pointer to a socket address structure is expected in calls such as 
    getpeername(), and determine the address family by accessing the 
    received content after the call.

    The example below illustrates a data structure which aligns on a 
    64-bit boundary. An implementation-defined field _ss_align following
    _ss_pad1 is used to force a 64-bit alignment which covers proper
    alignment good enough for needs of at least sockaddr_in6 (IPv6) 
    and sockaddr_in (IPv4) address data structures. The size of padding
    field _ss_pad1 depends on the chosen alignment boundary. The size 
    of padding field _ss_pad2 depends on the value of overall size 
    chosen for the total size of the structure. This size and alignment
    are represented in the above example by implementation-defined 
    not required) constants _SS_MAXSIZE (chosen value 128) and 
    _SS_ALIGNMENT (with chosen value 8). Constants _SS_PAD1SIZE 
    (derived value 6) and _SS_PAD2SIZE (derived value 112) are also 
    for illustration and not required. The implementation-defined 
    definitions and structure field names above start with an 
    <underscore> to denote implementation private name space.
    Portable code is not expected to access or reference those fields 
    or constants.

    /@
     *  Desired design of maximum size and alignment.
     @/
    #define _SS_MAXSIZE 128
        /@ Implementation-defined maximum size. @/
    #define _SS_ALIGNSIZE (sizeof(int64_t))
        /@ Implementation-defined desired alignment. @/


    /@
     *  Definitions used for sockaddr_storage structure paddings design.
     @/
    #define _SS_PAD1SIZE (_SS_ALIGNSIZE - sizeof(sa_family_t))
    #define _SS_PAD2SIZE (_SS_MAXSIZE - (sizeof(sa_family_t)+ \
                          _SS_PAD1SIZE + _SS_ALIGNSIZE))
    struct sockaddr_storage {
        sa_family_t  ss_family;  /@ Address family. @/
    /@
     *  Following fields are implementation-defined.
     @/
        char _ss_pad1[_SS_PAD1SIZE];
            /@ 6-byte pad; this is to make implementation-defined
               pad up to alignment field that follows explicit in
               the data structure. @/
        int64_t _ss_align;  /@ Field to force desired structure
                               storage alignment. @/
        char _ss_pad2[_SS_PAD2SIZE];
            /@ 112-byte pad to achieve desired size,
               _SS_MAXSIZE value minus size of ss_family
               __ss_pad1, __ss_align fields is 112. @/
    };
*/
#ifndef _POSIX_SYS_SOCKET_H
#define _POSIX_SYS_SOCKET_H

#include <stdint.h>
#include <sys/param.h>
#include <sys.h>
#include <sys/uio.h>

/*
 * The <sys/socket.h> header shall define the socklen_t type, 
 * which is an integer type of width of at least 32 bits; 
 * see APPLICATION USAGE.
 */
 
typedef uint32_t socklen_t;

/* 
 * The <sys/socket.h> header shall define the sa_family_t 
 * unsigned integer type. 
 */

typedef uint8_t sa_family_t;
 
/* Address families */
#define AF_UNSPEC		0
#define AF_INET			1 /* Internet domain sockets for use with IPv4 addresses. */
#define AF_APPLETALK	2
#define AF_ROUTE		3
#define AF_LINK			4
#define AF_INET6		5 /* Internet domain sockets for use with IPv6 addresses. */
#define AF_DLI			6
#define AF_IPX			7
#define AF_NOTIFY		8
#define AF_LOCAL		9
#define AF_UNIX			AF_LOCAL /* UNIX domain sockets.*/
#define AF_BLUETOOTH	10
#define AF_MAX			11
#define AF_UNSPEC       0xFF    /* Unspecified.*/

/* Protocol families, deprecated */
#define PF_UNSPEC		AF_UNSPEC
#define PF_INET			AF_INET
#define PF_ROUTE		AF_ROUTE
#define PF_LINK			AF_LINK
#define PF_INET6		AF_INET6
#define PF_LOCAL		AF_LOCAL
#define PF_UNIX			AF_UNIX
#define PF_BLUETOOTH	AF_BLUETOOTH

/* Socket types */
#define	SOCK_STREAM	    1 /* Byte-stream socket. */
#define	SOCK_DGRAM	    2 /* Datagram socket. */
#define	SOCK_RAW	    3 /* Raw Protocol Interface.*/
#define	SOCK_SEQPACKET	4 /* Sequenced-packet socket. */
#define SOCK_MISC	    255 

/* 
 * The <sys/socket.h> header shall define the following symbolic
 * constant for use as the level argument of setsockopt() and 
 * getsockopt()
 */
 
/* Options to be accessed at socket level, not protocol level. */
#define	SOL_SOCKET		-1		

/* 
 * The <sys/socket.h> header shall define the following symbolic 
 * constants with distinct values for use as the option_name 
 * argument in getsockopt() or setsockopt() calls
 */
 
#define SO_ACCEPTCONN	0x00000001	/* Socket is accepting connections. */
#define SO_BROADCAST	0x00000002	/* Transmission of broadcast messages is supported. */
#define	SO_DEBUG		0x00000004	/* Debugging information is being recorded. */
#define	SO_DONTROUTE	0x00000008	/* Bypass normal routing. */
#define	SO_KEEPALIVE	0x00000010	/* Connections are kept alive with periodic messages. */
#define SO_OOBINLINE	0x00000020	/* Out-of-band data is transmitted in line. */
#define	SO_REUSEADDR	0x00000040	/* Reuse of local addresses is supported. */
#define SO_REUSEPORT	0x00000080	/* allow local address & port reuse */
#define SO_USELOOPBACK	0x00000100	/* bypass hardware when possible */
#define SO_LINGER		0x00000200	/* Socket lingers on close. */

#define SO_SNDBUF		0x40000001	/* Send buffer size. */
#define SO_SNDLOWAT		0x40000002	/* Send 'low-water mark' */
#define SO_SNDTIMEO		0x40000003	/* Send timeout */
#define SO_RCVBUF		0x40000004	/* Receive buffer size. */
#define SO_RCVLOWAT		0x40000005	/* Receive 'low water mark'. */
#define SO_RCVTIMEO		0x40000006	/* Receive timeout. */
#define	SO_ERROR		0x40000007	/* Socket error status. */
#define	SO_TYPE			0x40000008	/* Socket type. */
#define SO_NONBLOCK		0x40000009
#define SO_BINDTODEVICE	0x4000000a	/* binds the socket to a specific device index */
#define SO_PEERCRED		0x4000000b	/* get peer credentials, param: ucred */

/* Shutdown options */
#define SHUT_RD			0 /* Disables further receive operations. */
#define SHUT_WR			1 /* Disables further send operations. */
#define SHUT_RDWR		2 /* Disables further send and receive operations.*/

/*
 * The <sys/socket.h> header shall define the following symbolic 
 * constant for use as the maximum backlog queue length which may 
 * be specified by the backlog field of the listen() function
 */
#define SOMAXCONN		32		/* Max listen queue for a socket */

/* The <sys/socket.h> header shall define the linger structure */

typedef struct linger 
    {
	int			l_onoff; /* Indicates whether linger option is enabled. */
	int			l_linger;/* Linger time, in seconds. */
    }linger_t;

/*
 * The sockaddr structure is used to define a socket address 
 * which is used in the bind(), connect(), getpeername(), 
 * getsockname(), recvfrom(), and sendto() functions
 */
typedef struct sockaddr 
    {
	uint8_t		sa_len;	        /* Total length */
	sa_family_t sa_family;      /* Address family.  */
	char        sa_data[30];    /* Socket address (variable-length data).*/
    }sockaddr_t;

/*
 * The <sys/socket.h> header shall define the sockaddr_storage 
 * structure, which shall be:
 * 
 * - Large enough to accommodate all supported protocol-specific 
 *    address structures
 *
 * - Aligned at an appropriate boundary so that pointers to it
 *   can be cast as pointers to protocol-specific address structures
 *   and used to access the fields of those structures without
 *   alignment problems
 *
 * When a sockaddr_storage structure is cast as a sockaddr structure,
 * the ss_family field of the sockaddr_storage structure shall map 
 * onto the sa_family field of the sockaddr structure. When a 
 * sockaddr_storage structure is cast as a protocol-specific address
 * structure, the ss_family field shall map onto a field of that 
 * structure that is of type sa_family_t and that identifies the 
 * protocol's address family.
 */
 
typedef struct sockaddr_storage 
    {
	uint8_t		ss_len;			/* Total length */
	sa_family_t ss_family;		/* Address family */
	uint8_t		__ss_pad1[6];	/* Align to quad */
	uint64_t	__ss_pad2;		/* Force alignment to 64 bit */
	uint8_t		__ss_pad3[112];	/* Pad to a total of 128 bytes */
    }sockaddr_storage_t;

/* 
 * The <sys/socket.h> header shall define the msghdr structure.
 *
 * The msghdr structure is used to minimize the number of directly
 * supplied parameters to the recvmsg() and sendmsg() functions. 
 * This structure is used as a value- result parameter in the 
 * recvmsg() function and value only for the sendmsg() function.
 */
typedef struct msghdr 
    {
    void          *msg_name;        /* Optional address. */ 
    socklen_t      msg_namelen;     /* Size of address. */ 
    struct iovec  *msg_iov;         /* Scatter/gather array. */ 
    int            msg_iovlen;      /* Members in msg_iov. */ 
    void          *msg_control;     /* Ancillary data; see below. */ 
    socklen_t      msg_controllen;  /* Ancillary data buffer len. */ 
    int            msg_flags;       /* Flags on received message. */ 
    }msghdr_t;

/*
 * Flags for the msghdr.msg_flags field 
 *
 * The <sys/socket.h> header shall define the following symbolic
 * constants with distinct values for use as the valid values for 
 * the msg_flags field in the msghdr structure, or the flags 
 * parameter in recv(), recvfrom(), recvmsg(), send(), sendmsg(),
 * or sendto() calls
 */
 
#define MSG_OOB			0x0001	/* Out-of-band data. */
#define MSG_PEEK		0x0002	/* Leave received data in queue. */
#define MSG_DONTROUTE	0x0004	/* Send without using routing tables. */
#define MSG_EOR			0x0008	/* Terminates a record (if supported by the protocol). */
#define MSG_TRUNC		0x0010	/* Normal data truncated. */
#define MSG_CTRUNC		0x0020	/* Control data truncated. */
#define MSG_WAITALL		0x0040	/* Attempt to fill the read buffer. */
#define MSG_DONTWAIT	0x0080	/* this message should be nonblocking */
#define MSG_BCAST		0x0100	/* this message rec'd as broadcast */
#define MSG_MCAST		0x0200	/* this message rec'd as multicast */
#define	MSG_EOF			0x0400	/* data completes connection */
#define	MSG_NOSIGNAL	0x0800  /* No SIGPIPE generated when an attempt to
                                 * send is made on a stream-oriented socket 
                                 * that is no longer connected.*/
/* 
 * The <sys/socket.h> header shall define the cmsghdr structure. 
 *
 * The cmsghdr structure is used for storage of ancillary data 
 * object information.
 * 
 * Ancillary data consists of a sequence of pairs, each consisting 
 * of a cmsghdr structure followed by a data array. The data array
 * contains the ancillary data message, and the cmsghdr structure 
 * contains descriptive information that allows an application to 
 * correctly parse the data.
 *
 * The values for cmsg_level shall be legal values for the level
 * argument to the getsockopt() and setsockopt() functions. The 
 * system documentation shall specify the cmsg_type definitions 
 * for the supported protocols.
 */
typedef struct cmsghdr 
    {
    socklen_t  cmsg_len;    /* Data byte count, including the cmsghdr. */ 
    int        cmsg_level;  /* Originating protocol. */ 
    int        cmsg_type;   /* Protocol-specific type. */ 
	/* data follows */
    }cmsghdr_t;

/*
 * The <sys/socket.h> header shall define the following macros
 * to gain access to the data arrays in the ancillary data 
 * associated with a message header
 */

/*
 * CMSG_DATA
 *
 * If the argument is a pointer to a cmsghdr structure, 
 * this macro shall return an unsigned character pointer 
 * to the data array associated with the cmsghdr structure.
 */
#define	CMSG_DATA(cmsg) ((unsigned char *)(cmsg) \
	                    + _ALIGN(sizeof(struct cmsghdr)))

/*
 * CMSG_NXTHDR(mhdr,cmsg)
 *
 * If the first argument is a pointer to a msghdr structure
 * and the second argument is a pointer to a cmsghdr structure 
 * in the ancillary data pointed to by the msg_control field 
 * of that msghdr structure, this macro shall return a pointer
 * to the next cmsghdr structure, or a null pointer if this
 * structure is the last cmsghdr in the ancillary data.
 */
#define	CMSG_NXTHDR(mhdr, cmsg)	                \
	(((char *)(cmsg) + _ALIGN((cmsg)->cmsg_len) \
	    + _ALIGN(sizeof(struct cmsghdr)) \
		> (char *)(mhdr)->msg_control + (mhdr)->msg_controllen) \
		? (struct cmsghdr *)NULL \
		: (struct cmsghdr *)((char *)(cmsg) + _ALIGN((cmsg)->cmsg_len)))

/*
 * CMSG_FIRSTHDR(mhdr)
 *
 * If the argument is a pointer to a msghdr structure, this macro 
 * shall return a pointer to the first cmsghdr structure in the 
 * ancillary data associated with this msghdr structure, or a null
 * pointer if there is no ancillary data associated with the msghdr
 * structure.
 */
#define	CMSG_FIRSTHDR(mhdr) \
	((mhdr)->msg_controllen >= sizeof(struct cmsghdr) \
	? (struct cmsghdr *)(mhdr)->msg_control \
	: (struct cmsghdr *)NULL)
	
#define	CMSG_SPACE(len) (_ALIGN(sizeof(struct cmsghdr)) + _ALIGN(len))
#define	CMSG_LEN(len)	(_ALIGN(sizeof(struct cmsghdr)) + (len))
#define	CMSG_ALIGN(len)	_ALIGN(len)

/*
 * Ancillary data is also possible at the socket level. 
 * The <sys/socket.h> header shall define the following 
 * symbolic constant for use as the cmsg_type value when
 * cmsg_level is SOL_SOCKET
 */

/* 
 * Indicates that the data array contains the access rights
 * to be sent or received.
 */
#define SCM_RIGHTS	0x01

/* Parameter to SO_PEERCRED */

typedef struct ucred 
    {
	pid_t	pid;	/* PID of sender */
	uid_t	uid;	/* UID of sender */
	gid_t	gid;	/* GID of sender */
    }ucred_t;


#if __cplusplus
extern "C" {
#endif

int 	accept(int socket, struct sockaddr *address, socklen_t *_addressLength);
int		bind(int socket, const struct sockaddr *address,
			socklen_t addressLength);
int		connect(int socket, const struct sockaddr *address,
			socklen_t addressLength);
int     getpeername(int socket, struct sockaddr *address,
			socklen_t *_addressLength);
int     getsockname(int socket, struct sockaddr *address,
			socklen_t *_addressLength);
int     getsockopt(int socket, int level, int option, void *value,
			socklen_t *_length);
int		listen(int socket, int backlog);
ssize_t recv(int socket, void *buffer, size_t length, int flags);
ssize_t recvfrom(int socket, void *buffer, size_t bufferLength, int flags,
			struct sockaddr *address, socklen_t *_addressLength);
ssize_t recvmsg(int socket, struct msghdr *message, int flags);
ssize_t send(int socket, const void *buffer, size_t length, int flags);
ssize_t	sendmsg(int socket, const struct msghdr *message, int flags);
ssize_t sendto(int socket, const void *message, size_t length, int flags,
			const struct sockaddr *address, socklen_t addressLength);
int     setsockopt(int socket, int level, int option, const void *value,
			socklen_t length);
int		shutdown(int socket, int how);
int		socket(int domain, int type, int protocol);
int		sockatmark(int descriptor);
int		socketpair(int domain, int type, int protocol, int socketVector[2]);

#if __cplusplus
}
#endif

#endif	/* _POSIX_SYS_SOCKET_H */
