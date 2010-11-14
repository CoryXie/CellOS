#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H

#include <termios.h>

/* These only work on sockets for now */
#define FIONBIO 	0xbe000000
#define FIONREAD	0xbe000001

#endif	/* _SYS_IOCTL_H */
