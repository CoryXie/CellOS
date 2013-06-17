/* device.h - device management */

#ifndef _OS_DEVICE_H
#define _OS_DEVICE_H

#include <sys.h>
#include <arch.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generic device abstraction */
typedef struct os_device 
    { 
    char name[NAME_MAX];
    
    }os_device_t;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _OS_DEVICE_H */



