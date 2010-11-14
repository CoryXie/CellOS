/* netinet/in.h - Internet address family */

#ifndef _POSIX_NETINET_IN_H
#define _POSIX_NETINET_IN_H

#include <net/if.h>
#include <endian.h>
#include <stdint.h>
#include <sys.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* 
 * RFC 2553 states that these must be available through 
 * <netinet/in.h> 
 */
 
#include <netinet6/in6.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;

/* We can't include <ByteOrder.h> since we are a posix file,
 * and we are not allowed to import all the BeOS types here.
 */
 
#ifndef htonl
	extern unsigned long __swap_int32(unsigned long);	/* private */
	extern uint16_t __swap_int16(uint16_t);	/* private */
#	if BYTE_ORDER == LITTLE_ENDIAN
#		define htonl(x) ((uint32_t)__swap_int32(x))
#		define ntohl(x) ((uint32_t)__swap_int32(x))
#		define htons(x) __swap_int16(x)
#		define ntohs(x) __swap_int16(x)
#	elif BYTE_ORDER == BIG_ENDIAN
#		define htonl(x) (x)
#		define ntohl(x) (x)
#		define htons(x) (x)
#		define ntohs(x) (x)
#	else
#		error Unknown byte order.
#	endif
#endif

/*
 * The <netinet/in.h> header shall define the following symbolic
 * constants for use as values of the level argument of getsockopt()
 * and setsockopt()
 */
 
/* Protocol definitions */

#define IPPROTO_IP				0	/* 0, IPv4 */
#define IPPROTO_HOPOPTS			0	/* 0, IPv6 hop-by-hop options */
#define IPPROTO_ICMP			1	/* 1, ICMP (v4) */
#define IPPROTO_IGMP			2	/* 2, IGMP (group management) */
#define IPPROTO_TCP				6	/* 6, tcp */
#define IPPROTO_UDP				17	/* 17, UDP */
#define IPPROTO_IPV6			41	/* 41, IPv6 in IPv6 */
#define IPPROTO_ROUTING			43	/* 43, Routing */
#define IPPROTO_FRAGMENT		44	/* 44, IPv6 fragmentation header */
#define IPPROTO_ESP				50	/* 50, Encap Sec. Payload */
#define IPPROTO_AH				51	/* 51, Auth Header */
#define IPPROTO_ICMPV6			58	/* 58, IPv6 ICMP */
#define IPPROTO_NONE			59	/* 59, IPv6 no next header */
#define IPPROTO_DSTOPTS			60	/* 60, IPv6 destination option */
#define IPPROTO_ETHERIP			97	/* 97, Ethernet in IPv4 */
#define IPPROTO_RAW				255	/* 255 */

#define IPPROTO_MAX				256

/* Port numbers */

#define IPPORT_RESERVED			1024 /* < IPPORT_RESERVED are privileged 
                                      * and should be accessible only by root */
#define IPPORT_USERRESERVED		49151 /* > IPPORT_USERRESERVED are reserved 
                                       * for servers, though not requiring
	                                   * privileged status
	                                   */

/* 
 * The <netinet/in.h> header shall define the in_addr structure
 * IP Version 4 address
 */

struct in_addr 
    {
	in_addr_t s_addr;
    };

/* 
 * The <netinet/in.h> header shall define the sockaddr_in structure
 * IP Version 4 socket address.
 *
 * The sin_port and sin_addr members shall be in network byte order.
 *
 * The sockaddr_in structure is used to store addresses for the 
 * Internet address family. Values of this type shall be cast by 
 * applications to struct sockaddr for use with socket functions.
 */
 
struct sockaddr_in 
    {
	uint8_t		    sin_len;
	uint8_t		    sin_family; /* AF_INET. */
	uint16_t	    sin_port;   /* Port number. */
	struct in_addr 	sin_addr;   /* IP address. */
	int8_t		    sin_zero[24];
    };

/* 
 * RFC 3678 - Socket Interface Extensions for Multicast Source Filters 
 */

struct ip_mreq 
    {
	struct in_addr imr_multiaddr;   /* IP address of group */
	struct in_addr imr_interface;   /* IP address of interface */
    };

struct ip_mreq_source 
    {
	struct in_addr imr_multiaddr;	/* IP address of group */
	struct in_addr imr_sourceaddr;	/* IP address of source */
	struct in_addr imr_interface;	/* IP address of interface */
    };

struct group_req 
    {
	uint32_t                gr_interface; /* interface index */
	struct sockaddr_storage gr_group;     /* group address */
    };

struct group_source_req 
    {
	uint32_t                gsr_interface; /* interface index */
	struct sockaddr_storage gsr_group;     /* group address */
	struct sockaddr_storage gsr_source;    /* source address */
    };

/*
 * Options for use with [gs]etsockopt at the IP level.
 * First word of comment is data type; bool is stored in int.
 */
 
#define IP_OPTIONS					1	/* buf/ip_opts; set/get IP options */
#define IP_HDRINCL					2	/* int; header is included with data */
#define IP_TOS						3   /* int; IP type of service and preced. */
#define IP_TTL						4	/* int; IP time to live */
#define IP_RECVOPTS					5	/* bool; receive all IP opts w/dgram */
#define IP_RECVRETOPTS				6	/* bool; receive IP opts for response */
#define IP_RECVDSTADDR				7	/* bool; receive IP dst addr w/dgram */
#define IP_RETOPTS					8	/* ip_opts; set/get IP options */
#define IP_MULTICAST_IF				9	/* in_addr; set/get IP multicast i/f  */
#define IP_MULTICAST_TTL			10	/* u_char; set/get IP multicast ttl */
#define IP_MULTICAST_LOOP			11  /* u_char; set/get IP multicast loopback */
#define IP_ADD_MEMBERSHIP			12  /* ip_mreq; add an IP group membership */
#define IP_DROP_MEMBERSHIP			13  /* ip_mreq; drop an IP group membership */
#define IP_BLOCK_SOURCE				14	/* ip_mreq_source */
#define IP_UNBLOCK_SOURCE			15	/* ip_mreq_source */
#define IP_ADD_SOURCE_MEMBERSHIP	16	/* ip_mreq_source */
#define IP_DROP_SOURCE_MEMBERSHIP	17	/* ip_mreq_source */
#define MCAST_JOIN_GROUP			18	/* group_req */
#define MCAST_BLOCK_SOURCE			19	/* group_source_req */
#define MCAST_UNBLOCK_SOURCE		20	/* group_source_req */
#define MCAST_LEAVE_GROUP			21	/* group_req */
#define MCAST_JOIN_SOURCE_GROUP		22	/* group_source_req */
#define MCAST_LEAVE_SOURCE_GROUP	23	/* group_source_req */

/* IPPROTO_IPV6 options */
#define IPV6_MULTICAST_IF			24	/* int */
#define IPV6_MULTICAST_HOPS			25	/* int */
#define IPV6_MULTICAST_LOOP			26	/* int */

#define IPV6_UNICAST_HOPS			27	/* int */
#define IPV6_JOIN_GROUP				28	/* struct ipv6_mreq */
#define IPV6_LEAVE_GROUP			29	/* struct ipv6_mreq */
#define IPV6_V6ONLY					30	/* int */

#define IPV6_PKTINFO				31	/* struct ipv6_pktinfo */
#define IPV6_RECVPKTINFO			32	/* struct ipv6_pktinfo */
#define IPV6_HOPLIMIT				33	/* int */
#define IPV6_RECVHOPLIMIT			34	/* int */

#define IPV6_HOPOPTS				35  /* struct ip6_hbh */
#define IPV6_DSTOPTS				36  /* struct ip6_dest */
#define IPV6_RTHDR					37  /* struct ip6_rthdr */

/*
 * The <netinet/in.h> header shall define the following symbolic 
 * constants for use as destination addresses for connect(), 
 * sendmsg(), and sendto()
 */
 
#define INADDR_ANY					((in_addr_t)0x00000000)
#define INADDR_LOOPBACK				((in_addr_t)0x7f000001)
#define INADDR_BROADCAST			((in_addr_t)0xffffffff)	/* must be masked */

#define INADDR_UNSPEC_GROUP			((in_addr_t)0xe0000000)	/* 224.0.0.0 */
#define INADDR_ALLHOSTS_GROUP		((in_addr_t)0xe0000001)	/* 224.0.0.1 */
#define INADDR_ALLROUTERS_GROUP		((in_addr_t)0xe0000002)	/* 224.0.0.2 */
#define INADDR_MAX_LOCAL_GROUP		((in_addr_t)0xe00000ff)	/* 224.0.0.255 */

#define IN_LOOPBACKNET				127

#define INADDR_NONE					((in_addr_t)0xffffffff)

#define IN_CLASSA(i)				(((in_addr_t)(i) & 0x80000000) == 0)
#define IN_CLASSA_NET				0xff000000
#define IN_CLASSA_NSHIFT			24
#define IN_CLASSA_HOST				0x00ffffff
#define IN_CLASSA_MAX				128

#define IN_CLASSB(i)				(((in_addr_t)(i) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET				0xffff0000
#define IN_CLASSB_NSHIFT			16
#define IN_CLASSB_HOST				0x0000ffff
#define IN_CLASSB_MAX				65536

#define IN_CLASSC(i)				(((in_addr_t)(i) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET				0xffffff00
#define IN_CLASSC_NSHIFT			8
#define IN_CLASSC_HOST				0x000000ff

#define IN_CLASSD(i)				(((in_addr_t)(i) & 0xf0000000) == 0xe0000000)

/* These ones aren't really net and host fields, but routing needn't know. */
#define IN_CLASSD_NET				0xf0000000
#define IN_CLASSD_NSHIFT			28
#define IN_CLASSD_HOST				0x0fffffff

#define IN_MULTICAST(i)				IN_CLASSD(i)

#define IN_EXPERIMENTAL(i)			(((in_addr_t)(i) & 0xf0000000) == 0xf0000000)
#define IN_BADCLASS(i)				(((in_addr_t)(i) & 0xf0000000) == 0xf0000000)

#define IP_MAX_MEMBERSHIPS			20

/* 
 * The <netinet/in.h> header shall define the following 
 * symbolic constant, with the value specified, to help
 * applications declare buffers of the proper size to 
 * store IPv4 addresses in string form 
 */
 
#define INET_ADDRSTRLEN				16

/* Some helpful macros */

#define in_hosteq(s, t)				((s).s_addr == (t).s_addr)
#define in_nullhost(x)				((x).s_addr == INADDR_ANY)
#define satosin(sa)					((struct sockaddr_in *)(sa))
#define sintosa(sin)				((struct sockaddr *)(sin))

#ifdef __cplusplus
}
#endif

#endif	/* _POSIX_NETINET_IN_H */
