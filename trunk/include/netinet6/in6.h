/* netinet/in.h - Internet address family for IPv6 */

#ifndef _POSIX_NETINET6_IN6_H
#define _POSIX_NETINET6_IN6_H

#include <sys.h>
#include <stdint.h>

/*
 * The <netinet/in.h> header shall define the in6_addr structure
 * This array is used to contain a 128-bit IPv6 address, stored
 * in network byte order.
 */
 
struct in6_addr 
    {
    uint8_t        s6_addr[16];
    };

/* 
 * The <netinet/in.h> header shall define the sockaddr_in6 structure
 * IP Version 6 socket address.
 *
 * The sin6_port and sin6_addr members shall be in network byte order.
 *
 * The sockaddr_in6 structure shall be set to zero by an application
 * prior to using it, since implementations are free to have additional,
 * implementation-defined fields in sockaddr_in6.
 *
 * The sin6_scope_id field is a 32-bit integer that identifies a set
 * of interfaces as appropriate for the scope of the address carried 
 * in the sin6_addr field. For a link scope sin6_addr, the application
 * shall ensure that sin6_scope_id is a link index. For a site scope 
 * sin6_addr, the application shall ensure that sin6_scope_id is a 
 * site index. The mapping of sin6_scope_id to an interface or set of 
 * interfaces is implementation-defined.
 */
 
struct sockaddr_in6 
    {
    uint8_t            sin6_len;
    uint8_t            sin6_family;    /* AF_INET6. */
    uint16_t        sin6_port;      /* Port number. */
    uint32_t        sin6_flowinfo;  /* IPv6 traffic class and flow information.*/
    struct in6_addr    sin6_addr;      /* IPv6 address. */
    uint32_t        sin6_scope_id;  /* Set of interfaces for a scope. */
    };


#define IN6ADDR_ANY_INIT {{ \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}
#define IN6ADDR_LOOPBACK_INIT {{ \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }}

/*
 * The <netinet/in.h> header shall declare the following 
 * external variables.
 */

/*
 * in6addr_any
 *
 * This variable is initialized by the system to contain 
 * the wildcard IPv6 address. The <netinet/in.h> header 
 * also defines the IN6ADDR_ANY_INIT macro. This macro 
 * must be constant at compile time and can be used to 
 * initialize a variable of type struct in6_addr to the 
 * IPv6 wildcard address.
 */
 
extern const struct in6_addr in6addr_any;

/*
 * in6addr_loopback
 * 
 * This variable is initialized by the system to contain 
 * the loopback IPv6 address. The <netinet/in.h> header 
 * also defines the IN6ADDR_LOOPBACK_INIT macro. This 
 * macro must be constant at compile time and can be
 * used to initialize a variable of type struct in6_addr 
 * to the IPv6 loopback address.
 */
 
extern const struct in6_addr in6addr_loopback;

/* The <netinet/in.h> header shall define the ipv6_mreq structure */

struct ipv6_mreq 
    {
    struct in6_addr ipv6mr_multiaddr; /* Pv6 multicast address. */
    unsigned        ipv6mr_interface; /* Interface index. */
    };

struct in6_pktinfo 
    {
    struct in6_addr ipi6_addr;      /* src/dst IPv6 address */
    unsigned int    ipi6_ifindex;   /* send/recv interface index */
    };


/* Non-standard helper defines (same as in FreeBSD, though) */
#define __IPV6_ADDR_SCOPE_NODELOCAL            0x01
#define __IPV6_ADDR_SCOPE_INTFACELOCAL        0x01
#define __IPV6_ADDR_SCOPE_LINKLOCAL            0x02
#define __IPV6_ADDR_SCOPE_SITELOCAL            0x05
#define __IPV6_ADDR_SCOPE_ORGLOCAL            0x08
#define __IPV6_ADDR_SCOPE_GLOBAL            0x0e

/*
 * The <netinet/in.h> header shall define the following symbolic 
 * constants, with distinct integer values, for use in the 
 * option_name argument in the getsockopt() or setsockopt() 
 * functions at protocol level IPPROTO_IPV6
 */

#define IPV6_JOIN_GROUP     1   /* Join a multicast group. */
#define IPV6_LEAVE_GROUP    2   /* Quit a multicast group. */
#define IPV6_MULTICAST_HOPS 3   /* Multicast hop limit. */
#define IPV6_MULTICAST_IF   4   /* Interface to use for outgoing 
                                 * multicast packets. */
#define IPV6_MULTICAST_LOOP 5   /* Multicast packets are delivered 
                                 * back to the local application.
                                 */
#define IPV6_UNICAST_HOPS   6   /* Unicast hop limit. */
#define IPV6_V6ONLY         7   /* Restrict AF_INET6 socket to IPv6
                                 * communications only. */

/*
 * The <netinet/in.h> header shall define the following macros
 * that test for special IPv6 addresses. Each macro is of type 
 * int and takes a single argument of type const struct in6_addr *
 */
 
#define __IPV6_ADDR_MC_SCOPE(a)                ((a)->s6_addr[1] & 0x0f)


/* Unspecified address. */
#define IN6_IS_ADDR_UNSPECIFIED(a) \
    (!memcmp((a)->s6_addr, in6addr_any.s6_addr, sizeof(struct in6_addr)))

/* Loopback address. */
#define IN6_IS_ADDR_LOOPBACK(a) \
    (!memcmp((a)->s6_addr, in6addr_loopback.s6_addr, sizeof(struct in6_addr)))

/* Multicast address. */
#define IN6_IS_ADDR_MULTICAST(a) \
    ((a)->s6_addr[0] == 0xff)

/* Unicast link-local address. */
#define IN6_IS_ADDR_LINKLOCAL(a) \
    (((a)->s6_addr[0] == 0xfe) && (((a)->s6_addr[1] & 0xc0) == 0x80))

/* Unicast site-local address. */
#define IN6_IS_ADDR_SITELOCAL(a) \
    (((a)->s6_addr[0] == 0xfe) && (((a)->s6_addr[1] & 0xc0) == 0xc0))

/* IPv4 mapped address. */
#define IN6_IS_ADDR_V4MAPPED(a) \
    ((a)->s6_addr[0] == 0x00 && (a)->s6_addr[1] == 0x00 \
    && (a)->s6_addr[2] == 0x00 && (a)->s6_addr[3] == 0x00 \
    && (a)->s6_addr[4] == 0x00 && (a)->s6_addr[5] == 0x00 \
    && (a)->s6_addr[6] == 0x00 && (a)->s6_addr[9] == 0x00 \
    && (a)->s6_addr[8] == 0x00 && (a)->s6_addr[9] == 0x00 \
    && (a)->s6_addr[10] == 0xff && (a)->s6_addr[11] == 0xff)

/* IPv4-compatible address. */
#define IN6_IS_ADDR_V4COMPAT(a) \
    ((a)->s6_addr[0] == 0x00 && (a)->s6_addr[1] == 0x00 \
    && (a)->s6_addr[2] == 0x00 && (a)->s6_addr[3] == 0x00 \
    && (a)->s6_addr[4] == 0x00 && (a)->s6_addr[5] == 0x00 \
    && (a)->s6_addr[6] == 0x00 && (a)->s6_addr[9] == 0x00 \
    && (a)->s6_addr[8] == 0x00 && (a)->s6_addr[9] == 0x00 \
    && (a)->s6_addr[10] == 0x00 && (a)->s6_addr[11] == 0x01)

/* Multicast node-local address. */
#define IN6_IS_ADDR_MC_NODELOCAL(a) \
    (IN6_IS_ADDR_MULTICAST(a) && __IPV6_ADDR_MC_SCOPE(a) \
        == __IPV6_ADDR_SCOPE_NODELOCAL)

/* Multicast link-local address. */
#define IN6_IS_ADDR_MC_LINKLOCAL(a) \
    (IN6_IS_ADDR_MULTICAST(a) && __IPV6_ADDR_MC_SCOPE(a) \
        == __IPV6_ADDR_SCOPE_LINKLOCAL)

/* Multicast site-local address. */
#define IN6_IS_ADDR_MC_SITELOCAL(a) \
    (IN6_IS_ADDR_MULTICAST(a) && __IPV6_ADDR_MC_SCOPE(a) \
        == __IPV6_ADDR_SCOPE_SITELOCAL)

/* Multicast organization-local address. */
#define IN6_IS_ADDR_MC_ORGLOCAL(a) \
    (IN6_IS_ADDR_MULTICAST(a) && __IPV6_ADDR_MC_SCOPE(a) \
        == __IPV6_ADDR_SCOPE_ORGLOCAL)

/* Multicast global address. */
#define IN6_IS_ADDR_MC_GLOBAL(a) \
    (IN6_IS_ADDR_MULTICAST(a) && __IPV6_ADDR_MC_SCOPE(a) \
        == __IPV6_ADDR_SCOPE_GLOBAL)

/*
 * The <netinet/in.h> header shall define the following symbolic 
 * constant, with the value specified, to help applications 
 * declare buffers of the proper size to store IPv6 addresses 
 * in string form 
 */
 
#define INET6_ADDRSTRLEN                46


#endif    /* _POSIX_NETINET6_IN6_H */
