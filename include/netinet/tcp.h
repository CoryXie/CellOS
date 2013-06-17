/* netinet/tcp.h - definitions for the Transmission Control Protocol (TCP)*/

#ifndef _POSIX_NETINET_TCP_H
#define _POSIX_NETINET_TCP_H

#include <endian.h>
#include <stdint.h>

struct tcphdr 
    {
	uint16_t	th_sport;	/* source port */
	uint16_t	th_dport;	/* destination port */
	uint32_t	th_seq;
	uint32_t	th_ack;
	
#if BIG_ENDIAN
	uint8_t		th_off : 4;	/* data offset */
	uint8_t		th_x2 : 4;	/* unused */
#else
	uint8_t		th_x2 : 4;
	uint8_t		th_off : 4;
#endif
	uint8_t		th_flags;
	uint16_t	th_win;
	uint16_t	th_sum;
	uint16_t	th_urp;		/* end of urgent data offset */
    } _PACKED;

/*
 * The <netinet/tcp.h> header shall define the following 
 * symbolic constant for use as a socket option at the 
 * IPPROTO_TCP level.
 */
 
#define	TCP_NODELAY				0x01 /* Avoid coalescing of small segments.*/
#define	TCP_MAXSEG				0x02 /* Set maximum segment size */
#define TCP_NOPUSH				0x04 /* Don't use TH_PUSH */
#define TCP_NOOPT				0x08 /* Don't use any TCP options */

#endif	/* _POSIX_NETINET_TCP_H */
