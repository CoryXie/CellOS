#ifndef _NET_IF_MEDIA_H
#define _NET_IF_MEDIA_H


/* bits        usage
 * ----        -----
 * 0-4        Media subtype
 * 5-7        Media type
 * 8-15        Type specific options
 * 16-31    General options
 */

/* Media types */

#define    IFM_ETHER        0x00000020    /* Ethernet */
#define    IFM_TOKEN        0x00000040    /* Token Ring */
#define    IFM_FDDI        0x00000060    /* Fiber Distributed Data Interface */
#define    IFM_IEEE80211    0x00000080    /* Wireless IEEE 802.11 */
#define IFM_ATM            0x000000a0
#define    IFM_CARP        0x000000c0    /* Common Address Redundancy Protocol */

/* Media subtypes */

/* Ethernet */
#define IFM_AUTO        0
#define    IFM_10_T        3            /* 10Base-T - RJ45 */
#define    IFM_100_TX        6            /* 100Base-TX - RJ45 */
#define IFM_1000_T        16            /* 1000Base-T - RJ45 */
#define IFM_1000_SX        18            /* 1000Base-SX - Fiber Optic */

/* General options */

#define    IFM_FULL_DUPLEX    0x00100000    /* Full duplex */
#define    IFM_HALF_DUPLEX    0x00200000    /* Half duplex */
#define    IFM_LOOP        0x00400000    /* hardware in loopback */
#define    IFM_ACTIVE        0x00800000    /* Media link is active */

/* Masks */

#define    IFM_NMASK        0x000000e0    /* Media type */
#define    IFM_TMASK        0x0000001f    /* Media subtype */
#define    IFM_OMASK        0x0000ff00    /* Type specific options */
#define    IFM_GMASK        0xffff0000    /* Generic options */

/* Macros for the masks */

#define    IFM_TYPE(x)        ((x) & IFM_NMASK)
#define    IFM_SUBTYPE(x)    ((x) & IFM_TMASK)
#define    IFM_TYPE_OPTIONS(x)    \
                        ((x) & IFM_OMASK)
#define    IFM_OPTIONS(x)    ((x) & (IFM_OMASK | IFM_GMASK))

#endif    /* _NET_IF_MEDIA_H */
