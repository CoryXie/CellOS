/* ctrlregs.h - X86-64 specific system control registers management */

#ifndef _ARCH_X86_X64_CTRLREGS_H
#define _ARCH_X86_X64_CTRLREGS_H

/*********************System registers and bits (start)**********************/

#define CR0_PG      (1 << 31) /* PG Paging (bit 31 of CR0) */
#define CR0_CD      (1 << 30) /* CD Cache Disable (bit 30 of CR0) */
#define CR0_NW      (1 << 29) /* NW Not Write-through (bit 29 of CR0) */
#define CR0_AM      (1 << 18) /* AM Alignment Mask (bit 18 of CR0) */
#define CR0_WP      (1 << 16) /* WP Write Protect (bit 16 of CR0) */
#define CR0_NE      (1 << 5)  /* NE Numeric Error (bit 5 of CR0) */
#define CR0_ET      (1 << 4)  /* ET Extension Type (bit 4 of CR0) */
#define CR0_TS      (1 << 3)  /* TS Task Switched (bit 3 of CR0) */
#define CR0_EM      (1 << 2)  /* EM Emulation (bit 2 of CR0) */
#define CR0_MP      (1 << 1)  /* MP Monitor Coprocessor (bit 1 of CR0) */
#define CR0_PE      (1 << 0)  /* PE Protection Enable (bit 0 of CR0) */

/* 
 * The CR3 register is used to point to the base address of the 
 * highest-level page-translation table
 */
#define CR3_PCD     (1 << 4) /* PCD Page-level Cache Disable (bit 4 of CR3) */
#define CR3_PWT     (1 << 3) /* PWT Page-level Write-Through (bit 3 of CR3) */

#define CR4_VME     (1 << 0) /* VME Virtual-8086 Mode Extensions (bit 0 of CR4) */
#define CR4_PVI     (1 << 1) /* PVI Protected-Mode Virtual Interrupts (bit 1 of CR4) */
#define CR4_TSD     (1 << 2) /* TSD Time Stamp Disable (bit 2 of CR4) */
#define CR4_DE      (1 << 3) /* DE Debugging Extensions (bit 3 of CR4) */
#define CR4_PSE     (1 << 4) /* PSE Page Size Extensions (bit 4 of CR4) */
#define CR4_PAE     (1 << 5) /* PAE Physical Address Extension (bit 5 of CR4)  */
#define CR4_MCE     (1 << 6) /* MCE Machine-Check Enable (bit 6 of CR4)  */
#define CR4_PGE     (1 << 7) /* PGE Page Global Enable (bit 7 of CR4) */
#define CR4_PCE     (1 << 8) /* PCE Performance-Monitoring Counter Enable (bit 8 of CR4) */
#define CR4_OSFXSR  (1 << 9) /* OSFXSR Operating System Support for FXSAVE and FXRSTOR
                               * instructions (bit 9 of CR4) */
#define CR4_OSXMMEXCPT  (1 << 10) /* OSXMMEXCPT Operating System Support for Unmasked SIMD
                                   * Floating-Point Exceptions (bit 10 of CR4) */
#define CR4_VMXE    (1 << 13) /* VMXE VMX-Enable Bit (bit 13 of CR4) */
#define CR4_SMXE    (1 << 14) /* SMXE SMX-Enable Bit (bit 14 of CR4) */
#define CR4_OSXSAVE (1 << 18) /* OSXSAVE XSAVE and Processor Extended States-Enable
                                * Bit (bit 18 of CR4) */


#define CR8_TPL_MASK  (0xF) /* TPL Task Priority Level (bit 3:0 of CR8)
                             * This sets the threshold value corresponding to the
                             * highest-priority interrupt to be blocked. A value of 0
                             * means all interrupts are enabled. This field is
                             * available in 64-bit mode. A value of 15 means
                             * all interrupts will be disabled.*/


#ifndef __ASM__
#define GEN_READ_SYS_REG(reg) \
static inline unative_t sys_read_##reg (void) \
    { \
        unative_t res; \
        asm volatile ( \
            "movq %%" #reg ", %[res]" \
            : [res] "=r" (res) \
        ); \
        return res; \
    }

#define GEN_WRITE_SYS_REG(reg) \
static inline void sys_write_##reg (unative_t regn) \
    { \
        asm volatile ( \
            "movq %[regn], %%" #reg \
            :: [regn] "r" (regn) \
        ); \
    }

/** CR0 - Contains system control flags that control operating mode and states
  * of the processor.
  */
GEN_READ_SYS_REG(cr0)
GEN_WRITE_SYS_REG(cr0)

/** CR1 - Reserved. */

/** CR2 - Contains the page-fault linear address (the linear address that
  * caused a page fault).
  */
GEN_READ_SYS_REG(cr2)

/** CR3 - Contains the physical address of the base of the paging-structure
  * hierarchy and two flags (PCD and PWT). Only the most-significant bits (less the
  * lower 12 bits) of the base address are specified; the lower 12 bits of the address
  * are assumed to be 0. The first paging structure must thus be aligned to a page
  * (4-KByte) boundary. The PCD and PWT flags control caching of that paging
  * structure in the processor's internal data caches (they do not control TLB
  * caching of page-directory information).
  * When using the physical address extension, the CR3 register contains the base
  * address of the page-directory-pointer table In IA-32e mode, the CR3 register
  * contains the base address of the PML4 table.
  */
GEN_READ_SYS_REG(cr3)
GEN_WRITE_SYS_REG(cr3)

/** CR4 - Contains a group of flags that enable several architectural extensions,
  * and indicate operating system or executive support for specific processor capabilities.
  * The control registers can be read and loaded (or modified) using the
  * move-to-or-from-control-registers forms of the MOV instruction. In protected mode,
  * the MOV instructions allow the control registers to be read or loaded (at privilege
  * level 0 only). This restriction means that application programs or operating-system
  * procedures (running at privilege levels 1, 2, or 3) are prevented from reading or
  * loading the control registers.
  */
GEN_READ_SYS_REG(cr4)
GEN_WRITE_SYS_REG(cr4)

/** CR8 - Provides read and write access to the Task Priority Register (TPR).
  * It specifies the priority threshold value that operating systems use to control
  * the priority class of external interrupts allowed to interrupt the processor.
  * This register is available only in 64-bit mode. However, interrupt filtering
  * continues to apply in compatibility mode.
  */
GEN_READ_SYS_REG(cr8)
GEN_WRITE_SYS_REG(cr8)

GEN_READ_SYS_REG(dr0)
GEN_READ_SYS_REG(dr1)
GEN_READ_SYS_REG(dr2)
GEN_READ_SYS_REG(dr3)
GEN_READ_SYS_REG(dr6)
GEN_READ_SYS_REG(dr7)

GEN_WRITE_SYS_REG(dr0)
GEN_WRITE_SYS_REG(dr1)
GEN_WRITE_SYS_REG(dr2)
GEN_WRITE_SYS_REG(dr3)
GEN_WRITE_SYS_REG(dr6)
GEN_WRITE_SYS_REG(dr7)
#endif /* __ASM__*/

#define RFLAGS_CF       (1 << 0) /* CF Carry Flag (bit 0) */
#define RFLAGS_ALWAYS1  (1 << 1) /* Fixed flags */
#define RFLAGS_PF       (1 << 2) /* PF Parity flag (bit 2)*/
#define RFLAGS_AF       (1 << 4) /* AF Adjust flag (bit 4) */
#define RFLAGS_ZF       (1 << 6) /* ZF Zero flag (bit 6) */
#define RFLAGS_SF       (1 << 7) /* SF Sign flag (bit 7) */
#define RFLAGS_TF       (1 << 8) /* TF Trap (bit 8) */
#define RFLAGS_IF       (1 << 9) /* IF Interrupt enable (bit 9) */
#define RFLAGS_DF       (1 << 10) /* DF Direction flag (bit 10) */
#define RFLAGS_OF       (1 << 11) /* OF Overflow flag (bit 11) */
#define RFLAGS_IOPL0    (0 << 12) /* I/O privilege flags */
#define RFLAGS_IOPL1    (1 << 12) /* (bits 12 and 13) */
#define RFLAGS_IOPL2    (2 << 12)
#define RFLAGS_IOPL3    (3 << 12)
#define RFLAGS_IOPL_MSK (3 << 12)  /* IOPL I/O privilege level field  */
#define RFLAGS_NT       (1 << 14)  /* NT Nested task (bit 14) */
#define RFLAGS_RF       (1 << 16)  /* RF Resume (bit 16) */
#define RFLAGS_VM86     (1 << 17)  /* VM Virtual-8086 mode (bit 17) */
#define RFLAGS_AC       (1 << 18)  /* AC Alignment check (bit 18) */
#define RFLAGS_VIF      (1 << 19)  /* VIF Virtual Interrupt (bit 19) */
#define RFLAGS_VIP      (1 << 20)  /* VIP Virtual interrupt pending (bit 20) */
#define RFLAGS_ID       (1 << 21)  /* ID Identification (bit 21) */

#define USER_RFLAGS     (RFLAGS_ALWAYS1 | RFLAGS_IF)

/** interrupts_enable - enable interrupts
 *
 * Enable interrupts and return previous value of rFLAGS.
 *
 * @return Old rFLAGS (interrupt priority level).
 */

static inline ipl_t interrupts_enable(void)
    {
    ipl_t v;

    asm volatile (
        "pushfq\n"
        "popq %[v]\n"
        "sti\n"
        : [v] "=r" (v)
        );

    return v;
    }

/** interrupts_disable - disable interrupts
 *
 * Disable interrupts and return previous value of rFLAGS.
 *
 * @return Old rFLAGS (interrupt priority level).
 */

static inline ipl_t interrupts_disable(void)
    {
    ipl_t v;

    asm volatile (
        "pushfq\n"
        "popq %[v]\n"
        "cli\n"
        : [v] "=r" (v)
        );
    return v;
    }

/** interrupts_restore - restore rFLAGS (interrupt priority level)
 *
 * Restore EFLAGS.
 *
 * @param ipl Saved interrupt priority level.
 */

static inline void interrupts_restore(ipl_t ipl)
    {
    asm volatile (
        "pushq %[ipl]\n"
        "popfq\n"
        :: [ipl] "r" (ipl)
        );
    }

/** interrupts_read - return interrupt priority level
  *
  * Return rFLAFS.
  *
  * @return Current rFLAFS (interrupt priority level).
  */

static inline ipl_t interrupts_read(void)
    {
    ipl_t v;

    asm volatile (
        "pushfq\n"
        "popq %[v]\n"
        : [v] "=r" (v)
        );

    return v;
    }

/** Check interrupts state.
 *
 * @return True if interrupts are disabled.
 *
 */
static inline bool interrupts_disabled(void)
    {
	ipl_t v;
	
	asm volatile (
		"pushfq\n"
		"popq %[v]\n"
		: [v] "=r" (v)
	    );
	
	return ((v & RFLAGS_IF) == 0);
    }

#endif /* _ARCH_X86_X64_CTRLREGS_H */
