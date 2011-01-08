/* context.h - X86-64 specific context management */

#ifndef _ARCH_X86_X64_CONTEXT_H
#define _ARCH_X86_X64_CONTEXT_H

#define OFFSET_SP  0x0
#define OFFSET_PC  0x8
#define OFFSET_RBX 0x10
#define OFFSET_RBP 0x18
#define OFFSET_R12 0x20
#define OFFSET_R13 0x28
#define OFFSET_R14 0x30
#define OFFSET_R15 0x38

#ifdef KERNEL
# define OFFSET_IPL 0x40
#else
# define OFFSET_TLS 0x40
#endif

/*
 * CAUTION: keep these in sync with low level assembly code in syscall_entry 
 */
#define SYSCALL_USTACK_RSP    0
#define SYSCALL_KSTACK_RSP    1

#define ISTATE_OFFSET_RAX            0
#define ISTATE_OFFSET_RBX            8
#define ISTATE_OFFSET_RCX            16
#define ISTATE_OFFSET_RDX            24
#define ISTATE_OFFSET_RSI            32
#define ISTATE_OFFSET_RDI            40
#define ISTATE_OFFSET_RBP            48
#define ISTATE_OFFSET_R8            56
#define ISTATE_OFFSET_R9            64
#define ISTATE_OFFSET_R10            72
#define ISTATE_OFFSET_R11            80
#define ISTATE_OFFSET_R12            88    
#define ISTATE_OFFSET_R13             96
#define ISTATE_OFFSET_R14            104
#define ISTATE_OFFSET_R15            112
#define ISTATE_OFFSET_INTNO            120
#define ISTATE_OFFSET_RBP_FRAME        128
#define ISTATE_OFFSET_RIP_FRAME        136
#define ISTATE_OFFSET_ERROR_WORD    144
#define ISTATE_OFFSET_RIP            152
#define ISTATE_OFFSET_CS            160
#define ISTATE_OFFSET_RFLAGS        168
#define ISTATE_OFFSET_RSP            176
#define ISTATE_OFFSET_SS            184

#define IREGISTER_SPACE                144

#ifndef __ASM__

/* 
 * According to ABI the stack MUST be aligned on 
 * 16-byte boundary. If it is not, the va_arg calling will
 * panic sooner or later
 */
#define ARCH_STACK_ALIGN     16

#define context_update(c, rip, stack, size)  \
    do  {                                    \
        (c)->pc = (uintptr_t) (rip);         \
        (c)->sp = ALIGN_DOWN(((uintptr_t) (stack)) + (size),\
                         ARCH_STACK_ALIGN);  \
        (c)->rbp = 0;                        \
        } while (0)

/* 
 * We include only registers that must be preserved
 * during function call
 */
typedef struct sched_context
    {
    uint64_t sp;
    uint64_t pc;
    
    uint64_t rbx;
    uint64_t rbp;

    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;

    ipl_t    ipl;
    } __attribute__ ((packed)) sched_context_t;

/** This is passed to interrupt handlers */
typedef struct stack_frame 
    {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t int_no;
    uint64_t rbp_frame;
    uint64_t rip_frame;
    uint64_t error;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp; 
    uint64_t ss; 
    } __attribute__ ((packed)) stack_frame_t;

typedef struct thread_arch_t
    {
    unative_t tls;
    /** User and kernel RSP for syscalls. */
    uint64_t syscall_rsp[2];    
    } thread_arch_t;

/** Return true if exception happened while in userspace */
static inline int istate_from_uspace(stack_frame_t *istate)
    {
    return !(istate->rip & 0x8000000000000000);
    }

static inline void istate_set_retaddr(stack_frame_t *istate, uintptr_t retaddr)
    {
    istate->rip = retaddr;
    }

static inline unative_t istate_get_pc(stack_frame_t *istate)
    {
    return istate->rip;
    }

static inline unative_t istate_get_fp(stack_frame_t *istate)
    {
    return istate->rbp;
    }

#endif /* __ASM__ */

#endif /* _ARCH_X86_X64_CONTEXT_H */
