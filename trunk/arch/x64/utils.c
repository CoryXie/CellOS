#include <sys.h>
#include <arch.h>
#include <os.h>

/** get_ip - get current RIP
 *
 *  Get current RIP.
 */

unative_t  get_ip(void)
    {
    unative_t sp;

    asm volatile (
        "mov %%rsp, %[sp]"
        : [sp] "=r" (sp)
        );

    return *(unative_t*)sp;
    }

