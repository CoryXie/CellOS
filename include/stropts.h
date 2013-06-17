/* stropts.h - STREAMS interface (STREAMS) */

#ifndef _POSIX_STROPTS_H
#define _POSIX_STROPTS_H

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * FMNAMESZ
 *
 * The minimum size in bytes of the buffer referred to 
 * by the arg argument.
 */
#define  FMNAMESZ 8

/*
 * The <stropts.h> header shall define the t_scalar_t and
 * t_uscalar_t types, respectively, as signed and unsigned 
 * opaque types of equal length of at least 32 bits.
 */

typedef int_least32_t     t_scalar_t;
typedef uint_least32_t    t_uscalar_t;

/* The <stropts.h> header shall define the bandinfo structure */

typedef struct bandinfo
    {
    int            bi_flag;  /* Flushing type. */
    unsigned char  bi_pri;   /* Priority band. */
    }bandinfo_t ;

/* The <stropts.h> header shall define the strbuf structure */

typedef struct strbuf
    {
    char  *buf;     /* Pointer to buffer. */
    int    len;     /* Length of data. */
    int    maxlen;  /* Maximum buffer length. */
    }strbuf_t ;

/* The <stropts.h> header shall define the strpeek structure */

typedef struct strpeek
    {
    struct strbuf  ctlbuf;  /* The control portion of the message. */
    struct strbuf  databuf; /* The data portion of the message. */
    t_uscalar_t    flags;   /* RS_HIPRI or 0. */
    }strpeek_t ;

/* The <stropts.h> header shall define the strfdinsert structure */

typedef struct strfdinsert
    {
    struct strbuf  ctlbuf;   /* The control portion of the message.*/ 
    struct strbuf  databuf;  /* The data portion of the message.*/ 
    int            fildes;   /* File descriptor of the other STREAM.*/ 
    t_uscalar_t    flags;    /* RS_HIPRI or 0. */
    int            offset;   /* Relative location of the stored value. */
    }strfdinsert_t ;

/* The <stropts.h> header shall define the strioctl structure */

typedef struct strioctl
    {
    int    ic_cmd;     /* ioctl() command. */ 
    char  *ic_dp;      /* Pointer to buffer. */ 
    int    ic_len;     /* Length of data. */ 
    int    ic_timout;  /* Timeout for response. */ 
    }strioctl_t ;

/* The <stropts.h> header shall define the strrecvfd structure */

typedef struct strrecvfd
    {
    int    fd;   /* Received file descriptor. */ 
    gid_t  gid;  /* GID of sender. */ 
    uid_t  uid;  /* UID of sender. */
    }strrecvfd_t ;

/* The <stropts.h> header shall define the str_mlist structure */

typedef struct str_mlist
    {
    char  l_name[FMNAMESZ+1];  /* A STREAMS module name. */ 
    }str_mlist_t ;

/* The <stropts.h> header shall define the str_list structure */

typedef struct str_list
    {
    struct str_mlist  *sl_modlist;  /* STREAMS module names. */ 
    int                sl_nmods;    /* Number of STREAMS module names. */ 
    }str_list_t ;

/*
 * The <stropts.h> header shall define at least the following
 * symbolic constants for use with I_FLUSH
 */

#define FLUSHR  0x00 /* Flush read queues.*/
#define FLUSHRW 0x01 /* Flush read and write queues.*/
#define FLUSHW  0x02 /* Flush write queues. */

/* Macros used as `request' argument to `ioctl'. */
#define __SID		('S' << 8)

#define I_NREAD	    (__SID | 1)	/* Size the top message. */
#define I_PUSH	    (__SID | 2)	/* Push a STREAMS module. */
#define I_POP	    (__SID | 3)	/* Pop a STREAMS module. */
#define I_LOOK	    (__SID | 4)	/* Get the top module name.*/
#define I_FLUSH	    (__SID | 5)	/* Flush a STREAM. */
#define I_SRDOPT    (__SID | 6)	/* Set the read mode. */
#define I_GRDOPT    (__SID | 7)	/* Returns the current read mode setting.  */
#define I_STR	    (__SID | 8)	/* Send a STREAMS ioctl(). */
#define I_SETSIG    (__SID | 9)	/* Ask for notification signals. */
#define I_GETSIG    (__SID |10) /* Retrieve current notification signals. */
#define I_FIND	    (__SID |11) /* Look for a STREAMS module. */
#define I_LINK	    (__SID |12) /* Connect two STREAMs. */
#define I_UNLINK    (__SID |13) /* Disconnect two STREAMs. */
#define I_PEEK	    (__SID |15) /* Peek at the top message on a STREAM. */
#define I_FDINSERT  (__SID |16) /* Send implementation-defined information about another STREAM. */
#define I_SENDFD    (__SID |17) /* Pass a file descriptor through a STREAMS pipe. */
#define I_RECVFD    (__SID |14) /* Get a file descriptor sent via I_SENDFD.  */
#define I_SWROPT    (__SID |19) /* Set the write mode.  */
#define I_GWROPT    (__SID |20) /* Get the write mode. */
#define I_LIST	    (__SID |21) /* Get all the module names on a STREAM. */
#define I_PLINK	    (__SID |22) /* Persistently connect two STREAMs. */
#define I_PUNLINK   (__SID |23) /* Dismantle a persistent STREAMS link. */
#define I_FLUSHBAND (__SID |28) /* Flush one band of a STREAM.*/
#define I_CKBAND    (__SID |29) /* See if any messages exist in a band. */
#define I_GETBAND   (__SID |30) /* Get the band of the top message on a STREAM. */
#define I_ATMARK    (__SID |31) /* Is the top message "marked''? */
#define I_SETCLTIME (__SID |32) /* Set close time delay. */
#define I_GETCLTIME (__SID |33) /* Get close time delay. */
#define I_CANPUT    (__SID |34) /* Is a band writable? */

/* Possible arguments for `I_SETSIG'.  */
#define S_INPUT		0x0001	/* A message, other than a high-priority message, 
                             * has arrived at the head of a STREAM head read 
                             * queue. */
#define S_HIPRI		0x0002	/* A high-priority message is present on a STREAM 
                             * head read queue. */
#define S_OUTPUT	0x0004	/* The write queue for normal data (priority band 0)
                             * just below the STREAM head is no longer full. 
                             * This notifies the process that there is room on 
                             * the queue for sending (or writing) normal data 
                             * downstream.*/
#define S_MSG		0x0008	/* A STREAMS signal message that contains the 
                             * SIGPOLL signal reaches the front of the STREAM
                             * head read queue. */
#define S_ERROR		0x0010	/* Notification of an error condition reaches the STREAM head. */
#define S_HANGUP	0x0020	/* Notification of a hangup reaches the STREAM head. */
#define S_RDNORM	0x0040	/* A normal (priority band set to 0) message has 
                             * arrived at the head of a STREAM head read queue. */
#define S_WRNORM	S_OUTPUT/* Equivalent to S_OUTPUT. */
#define S_RDBAND	0x0080	/* A message with a non-zero priority band has 
                             * arrived at the head of a STREAM head read queue.*/
#define S_WRBAND	0x0100	/* The write queue for a non-zero priority band 
                             * just below the STREAM head is no longer full. */
#define S_BANDURG	0x0200	/* When used in conjunction with S_RDBAND, 
                             * SIGURG is generated instead of SIGPOLL
                             * when a priority message reaches the front 
                             * of the STREAM head read queue. */

/* Option for `I_PEEK'.  */
#define RS_HIPRI	0x01	/* Only look for high-priority messages.  */

/* Options for `I_SRDOPT'.  */
#define RNORM		0x0000	/* Byte-STREAM mode, the default. */
#define RMSGD		0x0001	/* Message-discard mode. */
#define RMSGN		0x0002	/* Message-nondiscard mode. */
#define RPROTDAT	0x0004	/* Deliver the control part of a message as data 
                             * when a process issues a read().*/
#define RPROTDIS	0x0008	/* Discard the control part of a message, delivering
                             * any data part, when a process issues a read(). */
#define RPROTNORM	0x0010	/* Fail read() with [EBADMSG] if a message 
                             * containing a control part is at the front of 
                             * the STREAM head read queue.*/

/* Possible mode for `I_SWROPT'.  */
#define SNDZERO		0x001	/* Send a zero-length message downstream when 
                             * a write() of 0 bytes occurs. */

/* Arguments for `I_ATMARK'.  */
#define ANYMARK		0x01	/* Check if the message is marked.  */
#define LASTMARK	0x02	/* Check if the message is the last one marked on the queue.*/

/* Argument for `I_UNLINK'.  */

#define MUXID_ALL	(-1)	/* Unlink all STREAMs linked to the STREAM associated with fildes. */


/* Macros for `getmsg', `getpmsg', `putmsg' and `putpmsg'. */
#define MSG_HIPRI	0x01	/* Send/receive high priority message. */
#define MSG_ANY		0x02	/* Receive any message.*/
#define MSG_BAND	0x04	/* Receive message from specified band. */

/* Values returned by getmsg and getpmsg */
#define MORECTL		1	/* More control information is left in message. */
#define MOREDATA	2	/* More data is left in message. */

int    fattach(int, const char *);
int    fdetach(const char *);
int    getmsg(int, struct strbuf *restrict, struct strbuf *restrict,
           int *restrict);
int    getpmsg(int, struct strbuf *restrict, struct strbuf *restrict,
           int *restrict, int *restrict);
int    ioctl(int, int, ...);
int    isastream(int);
int    putmsg(int, const struct strbuf *, const struct strbuf *, int);
int    putpmsg(int, const struct strbuf *, const struct strbuf *, int,
           int);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_STROPTS_H */

