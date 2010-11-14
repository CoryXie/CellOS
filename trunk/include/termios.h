/* termios.h - define values for termios */

/*
 * The <termios.h> header shall contain the definitions used
 * by the terminal I/O interfaces.
 */

#ifndef _POSIX_TERMIOS_H
#define _POSIX_TERMIOS_H

#include <sys.h>

typedef uint32_t tcflag_t; /* Used for terminal modes.*/
typedef unsigned char speed_t; /* Used for terminal baud rates.*/
typedef unsigned char cc_t; /* Used for terminal special characters. */

#define NCCS	11		/* Size of the array c_cc for control characters. */

/* The <termios.h> header shall define the termios structure */
typedef struct termios
    {
	tcflag_t	c_iflag;	/* input modes */
	tcflag_t	c_oflag;	/* output modes */
	tcflag_t	c_cflag;	/* control modes */
	tcflag_t	c_lflag;	/* local modes */
	char		c_line;		/* line discipline */
	speed_t		c_ispeed;	/* (unused) */
	speed_t		c_ospeed;	/* (unused) */
	cc_t		c_cc[NCCS];	/* control characters */
    }termios_t;

/* Indices into c_cc array.  Default values in parentheses. */
#define VEOF                 0	/* cc_c[VEOF] = EOF char (^D) */
#define VEOL                 1	/* cc_c[VEOL] = EOL char (undef) */
#define VERASE               2	/* cc_c[VERASE] = ERASE char (^H) */
#define VINTR                3	/* cc_c[VINTR] = INTR char (DEL) */
#define VKILL                4	/* cc_c[VKILL] = KILL char (^U) */
#define VMIN                 5	/* cc_c[VMIN] = MIN value for timer */
#define VQUIT                6	/* cc_c[VQUIT] = QUIT char (^\) */
#define VTIME                7	/* cc_c[VTIME] = TIME value for timer */
#define VSUSP                8	/* cc_c[VSUSP] = SUSP (^Z, ignored) */
#define VSTART               9	/* cc_c[VSTART] = START char (^S) */
#define VSTOP               10	/* cc_c[VSTOP] = STOP char (^Q) */

/* Values for termios c_iflag bit map. */
#define BRKINT		0x0001	/* signal interrupt on break */
#define ICRNL		0x0002	/* map CR to NL on input */
#define IGNBRK		0x0004	/* ignore break */
#define IGNCR		0x0008	/* ignore CR */
#define IGNPAR		0x0010	/* ignore characters with parity errors */
#define INLCR		0x0020	/* map NL to CR on input */
#define INPCK		0x0040	/* enable input parity check */
#define ISTRIP		0x0080	/* mask off 8th bit */
#define IXOFF		0x0100	/* enable start/stop input control */
#define IXON		0x0200	/* enable start/stop output control */
#define PARMRK		0x0400	/* mark parity errors in the input queue */


/* c_oflag - output control modes */
#define	OPOST		0x01		/* Post-process output. */
#define	OLCUC		0x02		/* Map lowercase to uppercase */
#define	ONLCR		0x04		/* Map NL to CR-NL on output */
#define	OCRNL		0x08		/* Map CR to NL on output */
#define	ONOCR		0x10		/* No CR output when at column 0 */
#define	ONLRET		0x20		/* Newline performs CR function */
#define	OFILL		0x40		/* Use fill characters for delays */
#define	OFDEL		0x80		/* Fills are DEL, otherwise NUL */
#define	NLDLY		0x100		/* Newline delays: */
#define	NL0			0x000
#define	NL1			0x100
#define	CRDLY		0x600		/* Carriage return delays: */
#define	CR0			0x000
#define	CR1			0x200
#define	CR2			0x400
#define	CR3			0x600
#define	TABDLY		0x1800		/* Tab delays: */
#define	TAB0		0x0000
#define	TAB1		0x0800
#define	TAB2		0x1000
#define	TAB3		0x1800
#define	BSDLY		0x2000		/* Backspace delays: */
#define	BS0			0x0000
#define	BS1			0x2000
#define	VTDLY		0x4000		/* Vertical tab delays: */
#define	VT0			0x0000
#define	VT1			0x4000
#define	FFDLY		0x8000		/* Form feed delays: */
#define	FF0			0x0000
#define	FF1			0x8000

/* c_cflag - control modes */
#define	CBAUD		0x1F			/* line speed definitions */

#define	B0			0x00			/* hang up */
#define	B50			0x01			/* 50 baud */
#define	B75			0x02
#define	B110		0x03
#define	B134		0x04
#define	B150		0x05
#define	B200		0x06
#define	B300		0x07
#define	B600		0x08
#define	B1200		0x09
#define	B1800		0x0A
#define	B2400		0x0B
#define	B4800		0x0C
#define	B9600		0x0D
#define	B19200		0x0E
#define	B31250		0x0F			/* for MIDI */
#define B38400		0x10
#define B57600		0x11
#define B115200		0x12
#define B230400		0x13

#define	CSIZE		0x20    /* Character size */
#define	CS5	        0x0000	/* If CSIZE is CS5, characters are 5 bits */
#define	CS6	        0x0004	/* If CSIZE is CS6, characters are 6 bits */
#define	CS7	        0x0008	/* If CSIZE is CS7, characters are 7 bits */
#define	CS8	        0x000C	/* If CSIZE is CS8, characters are 8 bits */
#define	CSTOPB		0x40	/* Send 2 stop bits, not 1 */
#define	CREAD		0x80	/* Enable receiver */
#define	PARENB		0x100	/* Parity enable */
#define	PARODD		0x200	/* Odd parity, else even */
#define	HUPCL		0x400	/* Hangs up on last close */
#define	CLOCAL		0x800	/* Indicates local line */
#define	XLOBLK		0x1000	/* Block layer output ?*/
#define	CTSFLOW		0x2000	/* Enable CTS flow */
#define	RTSFLOW		0x4000	/* Enable RTS flow */
#define	CRTSCTS		(RTSFLOW | CTSFLOW)

/* c_lflag - local modes */
#define ISIG		0x01			/* Nnable signals */
#define ICANON		0x02			/* Canonical input */
#define XCASE		0x04			/* Canonical u/l case */
#define ECHO		0x08			/* Enable echo */
#define ECHOE		0x10			/* Echo erase as bs-sp-bs */
#define ECHOK		0x20			/* Echo nl after kill */
#define ECHONL		0x40			/* Echo nl */
#define NOFLSH		0x80			/* Disable flush after int or quit */
#define TOSTOP		0x100			/* Stop bg processes that write to tty */
#define IEXTEN		0x200			/* Implementation defined extensions */
#define ECHOCTL		0x400
#define ECHOPRT		0x800
#define ECHOKE		0x1000
#define FLUSHO		0x2000
#define PENDIN		0x4000

/* options to tcsetattr() */
#define TCSANOW		0x01			/* Make change immediate */
#define TCSADRAIN	0x02			/* Drain output, then change */
#define TCSAFLUSH	0x04			/* Drain output, flush input */

/* actions for tcflow() */
#define TCOOFF		0x01			/* Suspend output */
#define TCOON		0x02			/* Restart output */
#define TCIOFF		0x04			/* Transmit STOP character, intended to stop input data */
#define TCION		0x08			/* Transmit START character, intended to resume input data */

/* values for tcflush() */
#define TCIFLUSH	0x01			/* Flush pending input */
#define TCOFLUSH	0x02			/* Flush untransmitted output */
#define TCIOFLUSH	0x03			/* Flush both */


/* ioctl() identifiers to control the TTY */
#define TCGETA				0x8000
#define TCSETA				(TCGETA + 1)
#define TCSETAF				(TCGETA + 2)
#define TCSETAW				(TCGETA + 3)
#define TCWAITEVENT			(TCGETA + 4)
#define TCSBRK				(TCGETA + 5)
#define TCFLSH				(TCGETA + 6)
#define TCXONC				(TCGETA + 7)
#define TCQUERYCONNECTED	(TCGETA + 8)
#define TCGETBITS			(TCGETA + 9)
#define	TCSETDTR			(TCGETA + 10)
#define TCSETRTS			(TCGETA + 11)
#define TIOCGWINSZ			(TCGETA + 12)	/* Pass in a struct winsize */
#define TIOCSWINSZ			(TCGETA + 13)	/* Pass in a struct winsize */
#define TCVTIME				(TCGETA + 14)	/* Pass in bigtime_t, old value saved */
#define TIOCGPGRP			(TCGETA + 15)	/* Gets the process group ID of the TTY device */
#define TIOCSPGRP			(TCGETA + 16)	/* Sets the process group ID ('pgid' in BeOS) */
#define TIOCSCTTY			(TCGETA + 17)	/* Become controlling TTY */

/* Event codes.  Returned from TCWAITEVENT */
#define EV_RING			0x0001
#define EV_BREAK		0x0002
#define EV_CARRIER		0x0004
#define EV_CARRIERLOST	0x0008

/* for TIOCGWINSZ */
typedef struct winsize
    {
	unsigned short	ws_row;
	unsigned short	ws_col;
	unsigned short	ws_xpixel;
	unsigned short	ws_ypixel;
    }winsize_t;

/* Bits for the TCGETBITS control */
#define	TCGB_CTS		0x01
#define TCGB_DSR		0x02
#define TCGB_RI			0x04
#define TCGB_DCD		0x08


#ifdef __cplusplus
extern "C" {
#endif

speed_t cfgetispeed(const struct termios *);
speed_t cfgetospeed(const struct termios *);
int     cfsetispeed(struct termios *, speed_t);
int     cfsetospeed(struct termios *, speed_t);
int     tcdrain(int);
int     tcflow(int, int);
int     tcflush(int, int);
int     tcgetattr(int, struct termios *);
pid_t   tcgetsid(int);
int     tcsendbreak(int, int);
int     tcsetattr(int, int, const struct termios *);

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_TERMIOS_H */
