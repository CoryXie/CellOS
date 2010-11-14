#ifndef _ARCH_X86_X64_SIGNAL_H
#define _ARCH_X86_X64_SIGNAL_H
  
typedef struct mcontext 
    {
    /*
     * The first 20 fields must match the definition of
     * sigcontext. So that we can support sigcontext
     * and ucontext_t at the same time.
     */
    unsigned long    mc_onstack;    /* sigcontext compat. */
    unsigned long    mc_rdi;        /* machine state (struct trapframe) */
    unsigned long    mc_rsi;
    unsigned long    mc_rdx;
    unsigned long    mc_rcx;
    unsigned long    mc_r8;
    unsigned long    mc_r9;
    unsigned long    mc_rax;
    unsigned long    mc_rbx;
    unsigned long    mc_rbp;
    unsigned long    mc_r10;
    unsigned long    mc_r11;
    unsigned long    mc_r12;
    unsigned long    mc_r13;
    unsigned long    mc_r14;
    unsigned long    mc_r15;
    unsigned long    mc_trapno;
    unsigned long    mc_addr;
    unsigned long    mc_flags;
    unsigned long    mc_err;
    unsigned long    mc_rip;
    unsigned long    mc_cs;
    unsigned long    mc_rflags;
    unsigned long    mc_rsp;
    unsigned long    mc_ss;

    long    mc_len;                 /* sizeof(mcontext_t) */
    long    mc_fpformat;
    long    mc_ownedfp;

    long    mc_fpstate[64] __aligned(16);
    long    mc_spare[8];
    } mcontext_t; 

#define _MC_FPFMT_NODEV         0x10000 /* device not present or configured */
#define _MC_FPFMT_XMM           0x10002

#define _MC_FPOWNED_NONE        0x20000 /* FP state not used */
#define _MC_FPOWNED_FPU         0x20001 /* FP state came from FPU */
#define _MC_FPOWNED_PCB         0x20002 /* FP state came from PCB */

#endif /* _ARCH_X86_X64_SIGNAL_H */
