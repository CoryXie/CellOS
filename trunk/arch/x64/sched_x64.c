#include <sys.h>
#include <arch.h>
#include <os.h>

void sched_thread_arch_init
    (
    sched_thread_t *t
    )
    {    
    t->saved_context.ipl |= RFLAGS_IF;
    
    t->arch.tls = 0;
    t->arch.syscall_rsp[SYSCALL_USTACK_RSP] = 0;
    /*
     * Kernel RSP can be precalculated at thread creation time.
     */
    t->arch.syscall_rsp[SYSCALL_KSTACK_RSP] =
        (uintptr_t) &t->stack_base[CONFIG_KSTACK_SIZE - sizeof(uint64_t)];
    }

