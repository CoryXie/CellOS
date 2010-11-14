/* x64.h - Intel 64 and AMD64 common definitions and access routines */

#ifndef __INCLUDE_ARCH_X64_X64_H
#define __INCLUDE_ARCH_X64_X64_H

#include <sys.h>
#include <arch.h>


/*********************Descriptor data structures (start) *********************/

#define KERNEL_CS   0x8
#define KERNEL_DS   0x10
#define USER_CS     0x18
#define USER_DS     0x20

#define PL_KERNEL  0   /* Ring 0 */
#define PL_USER    3   /* Ring 3 */

#define SEG_DESC_ATTR_PRESENT    (1 << 7)
#define SEG_DESC_ATTR_DATA       (2 << 3)
#define SEG_DESC_ATTR_CODE       (3 << 3)
#define SEG_DESC_ATTR_WRITABLE   (1 << 1)

#define SEG_DESC_ATTR_TSS        (0x09)
#define SEG_DESC_ATTR_INTERRUPT  (0x0e)
#define SEG_DESC_ATTR_TRAP       (0x0f)

#define DPL_KERNEL               (PL_KERNEL << 5)
#define DPL_USER                 (PL_USER << 5)

#define TSS_BASIC_SIZE  104
#define TSS_IOMAP_SIZE  (16 * 1024 + 1)  /* 16K for bitmap + 1 terminating byte
                                                                          * for convenience */

#define IO_PORTS        (64 * 1024)

/*
 * This is the same as ARCH_NUMREGS, but it is used by assembler stubs,
 * so needs to be defined here as a constant.
 */

/* Number of registers to be saved for this architecture */
#define NUM_REGS                        20

/* Number of FPU registers to be saved for this architecture */
#define NUM_FPU_REGS                    64

#ifndef __ASM__

typedef struct x64_seg_descriptor
    {
    unsigned limit_0_15: 16;
    unsigned base_0_15: 16;
    unsigned base_16_23: 8;
    unsigned access: 8;
    unsigned limit_16_19: 4;
    unsigned available: 1;
    unsigned longmode: 1;
    unsigned special: 1;
    unsigned granularity : 1;
    unsigned base_24_31: 8;
    } __attribute__ ((packed)) x64_seg_descriptor_t;

typedef struct x64_tss_descriptor
    {
    unsigned limit_0_15: 16;
    unsigned base_0_15: 16;
    unsigned base_16_23: 8;
    unsigned type: 4;
    unsigned reserved0: 1;
    unsigned dpl : 2;
    unsigned present : 1;
    unsigned limit_16_19: 4;
    unsigned available: 1;
    unsigned reserved1: 2;
    unsigned granularity : 1;
    unsigned base_24_31: 8;
    unsigned base_32_63 : 32;
    unsigned reserved2: 32;
    } __attribute__ ((packed)) x64_tss_descriptor_t;

typedef struct x64_int_descriptor
    {
    unsigned offset_0_15: 16;
    unsigned selector: 16;
    unsigned ist:3;
    unsigned reserved0: 5;
    unsigned type: 5;
    unsigned dpl: 2;
    unsigned present : 1;
    unsigned offset_16_31: 16;
    unsigned offset_32_63: 32;
    unsigned reserved1: 32;
    } __attribute__ ((packed)) x64_int_descriptor_t;

typedef struct x64_desc_ptr_64
    {
    uint16_t limit;
    uint64_t base;
    } __attribute__ ((packed)) x64_desc_ptr_64_t;

/*
  * Although hardware task-switching is not supported in 64-bit mode, a 64-bit task
  * state segment (TSS) must exist.
  *
  * The operating system must create at least one 64-bit TSS after activating IA-32e
  * mode. It must execute the LTR instruction (in 64-bit mode) to load the TR register
  * with a pointer to the 64-bit TSS responsible for both 64-bit-mode programs and
  * compatibility-mode programs.
  */
typedef struct x64_tss
    {
    uint32_t reserve1;
    /* The full 64-bit canonical forms of the stack pointers (RSP) for privilege levels 0-2. */
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserve2;
    /* The full 64-bit canonical forms of the interrupt stack table (IST) pointers. */
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserve3;
    uint16_t reserve4;
    /* The 16-bit offset to the I/O permission bit map from the 64-bit TSS base. */
    uint16_t iomap_base;
    uint8_t iomap[TSS_IOMAP_SIZE];
    } __attribute__ ((packed)) x64_tss_t;

#if 0
typedef struct stack_frame
    {
    uint64_t gs;    /* + 0x00 */
    uint64_t fs;    /* + 0x08 */
    uint64_t r15;   /* + 0x10 */
    uint64_t r14;   /* + 0x18 */
    uint64_t r13;   /* + 0x20 */
    uint64_t r12;   /* + 0x28 */
    uint64_t r11;   /* + 0x30 */
    uint64_t r10;   /* + 0x38 */
    uint64_t r9;    /* + 0x40 */
    uint64_t r8;    /* + 0x48 */
    uint64_t rbp;   /* + 0x50 */
    uint64_t rsi;   /* + 0x58 */
    uint64_t rdi;   /* + 0x60 */
    uint64_t rdx;   /* + 0x68 */
    uint64_t rcx;   /* + 0x70 */
    uint64_t rbx;   /* + 0x78 */
    uint64_t rax;   /* + 0x80 */
    uint64_t int_no; /*+ 0x88 */
    uint64_t error; /* + 0x90 */
    uint64_t rip;   /* + 0x98 */
    uint64_t cs;    /* + 0xA0 */
    uint64_t flags; /* + 0xA8 */
    uint64_t rsp;   /* + 0xB0 */
    uint64_t ss;    /* + 0xB8 */
    } __attribute__ ((packed)) stack_frame_t;
#endif
#endif

#ifndef __ASM__
/** SYSTEM FLAGS AND FIELDS IN THE EFLAGS REGISTER
  *
  * The system flags and IOPL field of the EFLAGS register control I/O, maskable
  * hardware interrupts, debugging, task switching, and the virtual-8086 mode.
  * Only privileged code (typically operating system or executive code) should be
  * allowed to modify these bits.
  */
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
#define RFLAGS_IOPL_MASK (3 << 12) /* IOPL I/O privilege level field  */
#define RFLAGS_NT       (1 << 14)  /* NT Nested task (bit 14) */
#define RFLAGS_RF       (1 << 16)  /* RF Resume (bit 16) */
#define RFLAGS_VM86     (1 << 17)  /* VM Virtual-8086 mode (bit 17) */
#define RFLAGS_AC       (1 << 18)  /* AC Alignment check (bit 18) */
#define RFLAGS_VIF      (1 << 19)  /* VIF Virtual Interrupt (bit 19) */
#define RFLAGS_VIP      (1 << 20)  /* VIP Virtual interrupt pending (bit 20) */
#define RFLAGS_ID       (1 << 21)  /* ID Identification (bit 21) */

/**
 * State of RFLAGS when executing a user-space program: Enable interrupts
 */
#define USER_RFLAGS     (RFLAGS_ALWAYS1 | RFLAGS_IF)

/*********************Descriptor data structures (end) **********************/

/** get_stack_base - get current stack base address
 *
 * Return the base address of the current stack. The stack is assumed to be
 * CONFIG_KSTACK_SIZE bytes long. The stack must start on page boundary.
 */

static inline cpu_addr_t get_stack_base(void)
    {
    cpu_addr_t v;

    asm volatile (
        "andq %%rsp, %[v]\n"
        : [v] "=r" (v)
        : "0" (~((size_t) CONFIG_KSTACK_SIZE-1))
        );

    return v;
    }

/** cpu_sleep - halt the CPU until an external interrupt is received
 *
 * The HLT instruction is run when an operating system enters its idle loop and
 * there is no immediate work to be done.
 *
 * Since issuing the HLT instruction requires ring 0 access, it can only be run by
 * privileged system software, such as the kernel.
 */

static inline void cpu_sleep(void)
    {
    asm volatile ("hlt\n");
    }

/** cpu_halt - really halt the CPU
 *
 * This routines puts the CPU into endless halt state.
 */

static inline void cpu_halt(void)
    {
    asm volatile (
        "0:\n"
        "    hlt\n"
        "    jmp 0b\n"
        );
    }


/** ioport_in8 - read 8 bit byte from port
 *
 * Get byte from port.
 *
 * @param port Port to read from
 * @return Value read
 */

static inline uint8_t ioport_in8(uint16_t port)
    {
    uint8_t val;

    asm volatile (
        "inb %w[port], %b[val]\n"
        : [val] "=a" (val)
        : [port] "d" (port)
        );

    return val;
    }

/** ioport_in16 - read 16 bit word from port
 *
 * Get word from port.
 *
 * @param port Port to read from
 * @return Value read
 */

static inline uint16_t ioport_in16(uint16_t port)
    {
    uint16_t val;

    asm volatile (
        "inw %w[port], %w[val]\n"
        : [val] "=a" (val)
        : [port] "d" (port)
        );

    return val;
    }

/** ioport_in32 - read 32 bit double word from port
  *
  * Get double word from port.
  *
  * @param port Port to read from
  * @return Value read
  */

static inline uint32_t ioport_in32(uint16_t port)
    {
    uint32_t val;

    asm volatile (
        "inl %w[port], %[val]\n"
        : [val] "=a" (val)
        : [port] "d" (port)
        );

    return val;
    }

/** ioport_out8 - write 8 bit byte to port
 *
 * Output byte to port
 *
 * @param port Port to write to
 * @param val Value to write
 */
static inline void ioport_out8(uint16_t port, uint8_t val)
    {
    asm volatile (
        "outb %b[val], %w[port]\n"
        :: [val] "a" (val), [port] "d" (port)
        );
    }

/** ioport_out16 - write 16 bit word to port
 *
 * Output word to port
 *
 * @param port Port to write to
 * @param val Value to write
 */

static inline void ioport_out16(uint16_t port, uint16_t val)
    {
    asm volatile (
        "outw %w[val], %w[port]\n"
        :: [val] "a" (val), [port] "d" (port)
        );
    }

/** ioport_out32 - write 32 bit double word to port
 *
 * Output double word to port
 *
 * @param port Port to write to
 * @param val Value to write
 */

static inline void ioport_out32(uint16_t port, uint32_t val)
    {
    asm volatile (
        "outl %[val], %w[port]\n"
        :: [val] "a" (val), [port] "d" (port)
        );
    }

/** swapgs - swap the 64-bit 'KernelGSbase' MSR with the 64-bit GS segment base
 *
 * A new instruction (SwapGS) is described below. It is designed to be used by
 * an OS kernel. It provides a method  for the kernel to obtain a pointer to
 * kernel data structures in 64-bit mode.
 *
 * SwapGS exchanges the kernel data structure pointer from the KernelGSbase MSR
 * with GS base register. The kernel can then use the GS prefix on normal memory
 * references to access the kernel data structures.
 *
 * The need for SwapGS arises from the requirement that, upon entry to the OS
 * kernel,the kernel needs to get a 64-bit pointer to its key data structures.
 *
 * When using SYSCALL to implement system calls, no kernel stack exists at the
 * OS entry point. Neither is there a straightforward method to obtain a pointer
 * to kernel structures, from which the kernel stack pointer could be read.Thus,
 * the kernel can't save GPRs or reference memory.  By design, SwapGS does not
 * require any GPR or memory operands, therefore no registers need to be saved
 * before using it.
 *
 * Similarly, when the OS kernel is entered via an interrupt or exception (and
 * the kernel stack is already set up), SwapGS can be used to quickly get a
 * pointer to the kernel data structures.
 *
 * Swaps the 64-bit 'KernelGSbase' MSR with the 64-bit GS segment base.
 *
 * The SwapGS instruction is a privileged instruction intended for use by system
 * software. It is used upon entry to the kernel (via SYSCALL, interrupt or
 * exceptions) to obtain a pointer to kernel data structures.
 *
 * SwapGS exchanges the kernel data structure pointer from the KernelGSbase MSR
 * with GS base register. The kernel can then use the GS prefix on normal memory
 * references to access the kernel data structures.
 *
 * SwapGS is a serializing instruction.
 *
 * Operation
 * ---------
 *        if mode <> 64 then #UD;
 *        if CPL <> 0 then #GP (0);
 *        temp = GS base;
 *        GS base = MSR_KernelGSbase;
 *        MSR_KernelGSbase = temp;
 *
 * Example usage
 * -------------
 *  At a kernel entry point the OS can use SwapGS to obtain a pointer to kernel
 *  data structures and simultaneously save the user's GS base. Upon exit it
 *  can use SwapGS to restore the user's GS base:
 *
 *    SystemCallEntryPoint:
 *       SwapGS              ; set up kernel pointer, save user's GS base
 *       mov gs:[SavedUserRSP], rsp    ; save user's stack pointer
 *       mov rsp, gs:[KernelStackPtr]  ; set up kernel stack
 *       push rax            ; now that we have a stack, save user's GPRs
 *       mov rax, gs:[CPUnumber]       ; get CPU number     < or whatever >
 *       .                             ; perform system service
 *       .
 *       SwapGS                        ; restore user's GS, save kernel pointer
 *
 * The KernelGSbase MSR itself is only accessible via the normal RDMSR/WRMSR
 * instructions. Those instructions are privileged so KernelGSbase MSR is only
 * readable/writable by the OS. WRMSR will cause a #GP(0) if the value written
 * into KernelGSbase is not canonical; the SwapGS instruction itself does not
 * perform a canonical check.
 *
 * The KernelGSbase is MSR C000_0102h.
 **/

static inline void swapgs(void)
    {
    asm volatile("swapgs");
    }

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

static inline uint64_t rdtsc(void)
    {
	uint32_t lower;
	uint32_t upper;
	
	asm volatile (
		"rdtsc\n"
		: "=a" (lower), /* EAX */
		  "=d" (upper)  /* EDX */
	    );
	
	return ((uint64_t) lower) | (((uint64_t) upper) << 32);
    }

/** write_msr - write to MSR
 *
 * @param msr MSR register to write to
 * @param value MSR value to be written into MSR register
 */

static inline void write_msr(uint32_t msr, uint64_t value)
    {
    asm volatile (
        "wrmsr\n"
        :: "c" (msr),
           "a" ((uint32_t) (value)),
           "d" ((uint32_t) (value >> 32))
        );
    }

/** read_msr - read from MSR
 *
 * @param msr MSR register to read from
 * @return MSR value
 */

static inline uint64_t read_msr(uint32_t msr)
    {
    uint32_t ax, dx;

    asm volatile (
        "rdmsr\n"
        : "=a" (ax), "=d" (dx)
        : "c" (msr)
    );

    return ((uint64_t) dx << 32) | ax;
    }


/** enable_lapic_in_msr - enable local APIC
 *
 * Enable local APIC in MSR.
 */

static inline void enable_lapic_in_msr(void)
    {
    asm volatile (
        "movl $0x1b, %%ecx\n"
        "rdmsr\n"
        "orl $(1 << 11),%%eax\n"
        "orl $(0xfee00000),%%eax\n"
        "wrmsr\n"
        ::: "%eax","%ecx","%edx"
        );
    }

/** get_ip - get current RIP
 *
 *  Get current RIP.
 */

unative_t get_ip(void);

/** invlpg - invalidate TLB Entry
 *
 * @param addr Address on a page whose TLB entry is to be invalidated.
 */

static inline void invlpg(uintptr_t addr)
    {
    asm volatile (
        "invlpg %[addr]\n"
        :: [addr] "m" (*((unative_t *) addr))
        );
    }

/** load_gdtr - load GDTR register from memory
 *
 * @param gdtr_reg Address of memory from where to load GDTR.
 */

static inline void load_gdtr(x64_desc_ptr_64_t *gdtr_reg)
    {
    asm volatile (
        "lgdtq %[gdtr_reg]\n"
        :: [gdtr_reg] "m" (*gdtr_reg)
        );
    }

/** store_gdtr - store GDTR register to memory.
 *
 * When the GDTR register is stored (using the SGDT instruction), a 48-bit
 * "pseudo-descriptor" is stored in memory.
 *
 * To avoid alignment check faults in user mode (privilege level 3), the
 * pseudo-descriptor should be located at an odd word address (that is,
 * address MOD 4 is equal to 2).
 *
 * @param gdtr_reg Address of memory to where to store GDTR.
 */

static inline void store_gdtr(x64_desc_ptr_64_t *gdtr_reg)
    {
    asm volatile (
        "sgdtq %[gdtr_reg]\n"
        :: [gdtr_reg] "m" (*gdtr_reg)
        );
    }

/** load_idtr - load IDTR register from memory.
 *
 * @param idtr_reg Address of memory from where to load IDTR.
 */

static inline void load_idtr(x64_desc_ptr_64_t *idtr_reg)
    {
    asm volatile (
        "lidtq %[idtr_reg]\n"
        :: [idtr_reg] "m" (*idtr_reg));
    }

/** load_tr - load TR from descriptor table.
 *
 * @param sel Selector specifying descriptor of TSS segment.
 */

static inline void load_tr(uint16_t sel)
    {
    asm volatile (
        "ltr %[sel]"
        :: [sel] "r" (sel)
        );
    }

/** load_ldtr - load LDTR from descriptor table.
 *
 * @param sel Selector specifying descriptor of LDT segment.
 */

static inline void load_ldtr(uint16_t sel)
    {
    asm volatile (
        "lldt %[sel]"
        :: [sel] "r" (sel)
        );
    }
#if 0
/** atomic_set - set an atomic_t value atomicly
 *
 * Set an atomic_t value atomicly.
 *
 * @param val atomic_t variable to set the value
 * @param i the new value to set into the atomic_t variable
 */

static inline void atomic_set(atomic_t *val, long i)
    {
    val->counter = i;
    }

/** atomic_get - get an atomic_t value atomicly
  *
  * Set an atomic_t value atomicly.
  *
  *@param val atomic_t variable to get the value
  *@return  the new value to set into the atomic_t variable
  */
static inline long atomic_get(atomic_t *val)
    {
    return val->counter;
    }

/** atomic_inc - increase an atomic_t value atomicly
  *
  * Increase an atomic_t value atomicly.
  *
  *@param val atomic_t variable to increase the value
  */
static inline void atomic_inc(atomic_t *val)
    {
    asm volatile (
        LOCK_PREFIX
        " incq %[count]\n"
        : [count] "+m" (val->counter)
        );
    }

/** atomic_dec - decrease an atomic_t value atomicly
  *
  * Decrease an atomic_t value atomicly.
  *
  *@param val atomic_t variable to decrease the value
  */
static inline void atomic_dec(atomic_t *val)
    {
    asm volatile (
        LOCK_PREFIX
        " decq %[count]\n"
        : [count] "+m" (val->counter)
        );
    }

/** atomic_get_and_add - get the atomic_t counter and add a new value to it
  *
  * Get the old atomic_t variable value and add a new value to it.
  *
  *@param val atomic_t variable to get the value
  *@param r  the new value to add into the atomic_t variable
  *@return the old atomic_t variable value
  */
static inline long atomic_get_and_add(atomic_t *val, long r)
    {
    /*
     * "xaddq  r, [m]":
     *
     *     temp = [m];
     *     [m] += r;
     *     r = temp;
     *
     *
     * The "+r" is any register, %rax (%r0) - %r16.
     * The "cc" means that flags were changed.
     */
    asm volatile (
        LOCK_PREFIX
        " xaddq %[r], %[count]\n" /* Exchange and addition */
        : [count] "+m" (val->counter), [r] "+r" (r)
        ::"cc", "memory"
        );

    return r;
    }


#define atomic_preinc(val)  atomic_get_and_add(val, 1)
#define atomic_predec(val)  atomic_get_and_add(val, -1)

/** atomic_test_and_set - test and set the atomic_t value
  *
  * Test and set the atomic_t value atomicly.
  *
  *@return the old value in the atomic_t variable
  */
static inline uint64_t atomic_test_and_set(atomic_t *val)
    {
    uint64_t v;

   /* When a memory operand is used with the XCHG
       * instruction, the processor's LOCK signal is automatically asserted.
       * This instruction is thus useful for implementing semaphores or similar
       * data structures for process synchronization.
       */
    asm volatile (
        "movq $1, %[v]\n"
        "xchgq %[v], %[count]\n"
        : [v] "=r" (v), [count] "+m" (val->counter)
        );

    return v;
    }

 /** atomic_cmp_and_set - conditionally set atomic_t value if it equals an old value
   *
   * Check if the atomic_t value equals 'old', if so update the atomic_t value with a new
   * value 'set'';otherwise return the current atomic_t value.
   *
   *@return true if the atomic_t is set to the new value (was equal to the old one befpre)
   */
 static inline long atomic_cmp_and_set(atomic_t *lock, long old, long set)
    {
    long  res = 0;

     /*
      * "cmpxchgq  r, [m]":
      *
      *     if (rax == [m]) {
      *         zf = 1;
      *         [m] = r;
      *     } else {
      *         zf = 0;
      *         rax = [m];
      *     }
      *
      * This instruction compares the accumulator with the destination-operand
      * (so the ZF-bit in rFLAGS gets assigned accordingly)
      *
      * The "r" is any register, %rax (%r0) - %r16.
      * The "=a" and "a" are the %rax register.
      * Although we can return result in any register, we use "a" because it is
      * used in cmpxchgq anyway.  The result is actually in %al but not in $rax,
      * however as the code is inlined gcc can test %al as well as %rax.
      *
      * The "cc" means that flags were changed.
      */

     /* Note: Keep in mind that the accumulator %eax will affect what happens!
      * So we need to consider this instruction within it's surrounding context
      */

        asm volatile (
         LOCK_PREFIX
         " cmpxchgq  %[set], %[count];   "
         " sete      %[res];       "
         : [res] "=a" (res)
         : [count]"m" (lock->counter), "a" (old), [set]"r" (set)
         : "cc", "memory"
         );

     return res;
    }

/** atomic_lock - spin on the atomic_t variable until its available and then set it
  *
  * Use lightweight looping on locked spinlock and set it atomicly.
  */
static inline void atomic_lock(atomic_t *val)
    {
    uint64_t tmp;

    asm volatile (
        "0:\n"
        "pause\n"
        "mov %[count], %[tmp]\n"
        "testq %[tmp], %[tmp]\n"
        "jnz 0b\n"       /* lightweight looping on locked spinlock */

        "incq %[tmp]\n"  /* now use the atomic operation */
        "xchgq %[count], %[tmp]\n"
        "testq %[tmp], %[tmp]\n"
        "jnz 0b\n"
        : [count] "+m" (val->counter), [tmp] "=&r" (tmp)
        );
    }
#endif

#endif /* __ASM__*/

/*********************CPUID cmd and bits (start)**************************/
#define CPUID_GETVENDORSTRING   0
#define CPUID_GETFEATURES       1
#define CPUID_GETTLB            2
#define CPUID_GETSERIAL         3

#define CPUID_INTELEXTENDED     0x80000000
#define CPUID_INTELFEATURES     0x80000001
#define CPUID_INTELBRANDSTRING  0x80000002
#define CPUID_INTELBRANDSTRINGMORE  0x80000003
#define CPUID_INTELBRANDSTRINGEND   0x80000004


/**
  * CPU Vendor ID String
  *
  * When called with EAX = 0, CPUID returns the vendor ID string in EBX, EDX and ECX.
  * Writing these to memory in this order results in a 12-character string. These can be
  * tested against known Vendor ID strings.
  *
  * Let us take an Intel processor. It should return "GenuineIntel". Look at the following
  * text to see how the string is placed in the registers:
  *
  * EBX = 2190756e6547h (* "Genu", with G in the low nibble of BL *)
  * EDX = 219049656e69h (* "ineI", with i in the low nibble of DL *)
  * ECX = 21906c65746eh (* "ntel", with n in the low nibble of CL *)
  *
  * Also, EAX is set to the maximum EAX value supported for CPUID calls, as not all
  * queries are supported on all processors.
  */
/* Vendor-strings. */
#define CPUID_VENDOR_OLDAMD       "AMDisbetter!" /*early samples of AMD K5 */
#define CPUID_VENDOR_AMD          "AuthenticAMD"
#define CPUID_VENDOR_INTEL        "GenuineIntel"
#define CPUID_VENDOR_VIA          "CentaurHauls"
#define CPUID_VENDOR_OLDTRANSMETA "TransmetaCPU"
#define CPUID_VENDOR_TRANSMETA    "GenuineTMx86"
#define CPUID_VENDOR_CYRIX        "CyrixInstead"
#define CPUID_VENDOR_CENTAUR      "CentaurHauls"
#define CPUID_VENDOR_NEXGEN       "NexGenDriven"
#define CPUID_VENDOR_UMC          "UMC UMC UMC "
#define CPUID_VENDOR_SIS          "SiS SiS SiS "
#define CPUID_VENDOR_NSC          "Geode by NSC"
#define CPUID_VENDOR_RISE         "RiseRiseRise"

/**
 * CPU Features
 *
 * When called with EAX = 1 (CPUID_GETFEATURES), CPUID returns a bit field in EDX
 * containing the following values. Note that different brands of CPUs may have given
 * different meanings to these. Recent processors also use ECX for features (which form
 * a different set), with which you should be very careful as some old CPUs return
 * bogus information in this register.
 */

#define CPUID_FEAT_ECX_SSE3          (1 << 0)
#define CPUID_FEAT_ECX_PCLMUL        (1 << 1)
#define CPUID_FEAT_ECX_DTES64        (1 << 2)
#define CPUID_FEAT_ECX_MONITOR       (1 << 3)
#define CPUID_FEAT_ECX_DS_CPL        (1 << 4)
#define CPUID_FEAT_ECX_VMX           (1 << 5)
#define CPUID_FEAT_ECX_SMX           (1 << 6)
#define CPUID_FEAT_ECX_EST           (1 << 7)
#define CPUID_FEAT_ECX_TM2           (1 << 8)
#define CPUID_FEAT_ECX_SSSE3         (1 << 9)
#define CPUID_FEAT_ECX_CID           (1 << 10)
#define CPUID_FEAT_ECX_FMA           (1 << 12)
#define CPUID_FEAT_ECX_CX16          (1 << 13)
#define CPUID_FEAT_ECX_ETPRD         (1 << 14)
#define CPUID_FEAT_ECX_PDCM          (1 << 15)
#define CPUID_FEAT_ECX_DCA           (1 << 18)
#define CPUID_FEAT_ECX_SSE4_1        (1 << 19)
#define CPUID_FEAT_ECX_SSE4_2        (1 << 20)
#define CPUID_FEAT_ECX_x2APIC        (1 << 21)
#define CPUID_FEAT_ECX_MOVBE         (1 << 22)
#define CPUID_FEAT_ECX_POPCNT        (1 << 23)
#define CPUID_FEAT_ECX_XSAVE         (1 << 26)
#define CPUID_FEAT_ECX_OSXSAVE       (1 << 27)
#define CPUID_FEAT_ECX_AVX           (1 << 28)

#define CPUID_FEAT_EDX_FPU           (1 << 0)  /* Floating Point Unit. */
#define CPUID_FEAT_EDX_VME           (1 << 1)  /* Virtual Mode Extensions. */
#define CPUID_FEAT_EDX_DE            (1 << 2)  /* Debugging Extensions. */
#define CPUID_FEAT_EDX_PSE           (1 << 3)  /* Page Size Extensions. */
#define CPUID_FEAT_EDX_TSC           (1 << 4)  /* Time Stamp Counter. */
#define CPUID_FEAT_EDX_MSR           (1 << 5)  /* Model-specific registers. */
#define CPUID_FEAT_EDX_PAE           (1 << 6)  /* Physical Address Extensions. */
#define CPUID_FEAT_EDX_MCE           (1 << 7)  /* Machine Check Exceptions. */
#define CPUID_FEAT_EDX_CX8           (1 << 8)  /* Compare and exchange 8-byte. */
#define CPUID_FEAT_EDX_APIC          (1 << 9)  /* On-chip LAPIC. */
#define CPUID_FEAT_EDX_SEP           (1 << 11) /* Fast System Calls. */
#define CPUID_FEAT_EDX_MTRR          (1 << 12) /* Memory Type Range Registers. */
#define CPUID_FEAT_EDX_PGE           (1 << 13) /* Page Global Enable. */
#define CPUID_FEAT_EDX_MCA           (1 << 14) /* Machine Check Architecture. */
#define CPUID_FEAT_EDX_CMOV          (1 << 15) /* Conditional move-instruction. */
#define CPUID_FEAT_EDX_PAT           (1 << 16) /* Page Attribute Table. */
#define CPUID_FEAT_EDX_PSE36         (1 << 17) /* 36-bit Page Size Extensions. */
#define CPUID_FEAT_EDX_PSN           (1 << 18) /* Processor Serial Number. */
#define CPUID_FEAT_EDX_CLF           (1 << 19) /* CLFLUSH */
#define CPUID_FEAT_EDX_DTES          (1 << 21) /* Debug Trace and EMON Store MSRs. */
#define CPUID_FEAT_EDX_ACPI          (1 << 22) /* Thermal Cotrol MSR. */
#define CPUID_FEAT_EDX_MMX           (1 << 23) /* MMX instruction set. */
#define CPUID_FEAT_EDX_FXSR          (1 << 24) /* Fast floating point save/restore. */
#define CPUID_FEAT_EDX_SSE           (1 << 25) /* SSE (Streaming SIMD Extensions) */
#define CPUID_FEAT_EDX_SSE2          (1 << 26) /* SSE2 (Streaming SIMD Extensions - #2) */
#define CPUID_FEAT_EDX_SS            (1 << 27) /* Selfsnoop. */
#define CPUID_FEAT_EDX_HTT           (1 << 28) /* Hyper-Threading Technology. */
#define CPUID_FEAT_EDX_TM1           (1 << 29) /* Thermal Interrupts, Status MSRs. */
#define CPUID_FEAT_EDX_I64           (1 << 30) /* Intel-64 (64-bit Intel CPU) */
#define CPUID_FEAT_EDX_PBE           (1 << 31)  /* Pending Break Event. */

#ifndef __ASM__
/**
  * CPUID can be invoked with various request codes (in eax) and will return values
  * in general registers (much as a built-in service interrupt).
  *
  * Provides processor identification information in registers EAX, EBX, ECX, and EDX.
  * This information identifies Intel as the vendor, gives the family, model, and stepping
  * of processor, feature information, and cache information. An input value loaded into
  * the EAX register determines what information is returned.
  *
  * The information returned with the CPUID instruction is divided into two groups:
  * basic information and extended function information. Basic information is returned
  * by entering an input value of from 0 to 3 in the EAX register depending on the
  * IA-32 processor type; extended function information is returned by entering an
  * input value of from 80000000H to 80000004H. The extended function CPUID
  * information was introduced in the Pentium(R) 4 processor and is not available in
  * earlier IA-32 processors.
  *
  * The CPUID instruction can be executed at any privilege level to serialize instruction
  * execution. Serializing instruction execution guarantees that any modifications to
  * flags, registers, and memory for previous instructions are completed before the
  * next instruction is fetched and executed.
  */
typedef struct _cpuid_info_t
    {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    } cpuid_info_t;

typedef struct _cpu_arch_t
    {
    uint32_t vendor;
    uint32_t family;
    uint32_t model;
    uint32_t stepping;
    } cpu_arch_t;


/** has_cpuid - check if the CPU supports CPUID instrcution
 *
 * Note that prior to use the CPUID instruction, you should also make sure
 * the processor supports it by testing the 'ID' bit (0x200000) in eflags
 * (modifiable only when the CPUID instruction is supported. For systems
 * that don't support CPUID, writing a '1' at that place will have no effect).
 *
 * @return true if the CPU supports CPUID instrcution
 */
static inline long has_cpuid(void)
    {
    long has_CPUID = 0;

    /* Technically, if this is being compiled under __x86_64__ then it has
         * CPUid by definition.  But it's nice to be able to prove it.  :)
         */
    /*
     * Software can 'toggle' the ID-bit (bit #21) in the 32-bit EFLAGS register
     * if the processor is capable of executing the 'cpuid' instruction
     */

    asm volatile (
    "        pushfq                      # Get original EFLAGS             \n"
    "        popq    %%rax                                                 \n"
    "        movq    %%rax,%%rcx                                           \n"
    "        xorl    $0x200000,%%eax     # Flip ID bit in EFLAGS           \n"
    "        pushq   %%rax               # Save new EFLAGS value on stack  \n"
    "        popfq                       # Replace current EFLAGS value    \n"
    "        pushfq                      # Get new EFLAGS                  \n"
    "        popq    %%rax               # Store new EFLAGS in EAX         \n"
    "        xorl    %%ecx,%%eax         # Can not toggle ID bit,          \n"
    "        jz      1f                  # Processor=80486                 \n"
    "        movl    $1,%[has_CPUID]     # We have CPUID support           \n"
    "1:                                                                    \n"
        : [has_CPUID]"=m" (has_CPUID)
        :
        : "%rax", "%rcx"
        );

    return has_CPUID;
    }

/** cpuid - run CPUID instrcution with the request in cmd and return values in cpu_info
  *
  * The CPUID instruction needs the request to be set in EAX and when run the return
  * values are saved in EAX,EBX,ECX,and EDX according to the cmd. The cpuid_info is
  * used to save these outputs.
  *
  *@return true if the CPU supports CPUID instrcution
  */
static inline void cpuid(uint32_t req, cpuid_info_t *cpuid_info)
    {
    asm volatile(
        "cpuid"
        : "=a" (cpuid_info->eax),
          "=b" (cpuid_info->ebx),
          "=c" (cpuid_info->ecx),
          "=d" (cpuid_info->edx)
        : "a" (req));
    }
#endif /* __ASM__ */
/*********************CPUID cmd and bits (end)***************************/

#ifndef __ASM__
/** 2.6 Extended Control Registers (including the XFEATURE_ENABLED_MASK register) */

/** If CPUID.01H:ECX.XSAVE[bit 26] is 1, the processor supports one or more extended
  * control registers (XCRs). Currently, the only such register defined is XCR0, the
  * XFEATURE_ENABLED_MASK register. This register specifies the set of processor
  * states that the operating system enables on that processor, e.g. x87 FPU States,
  * SSE states, and other processor extended states that Intel 64 architecture may
  * introduce in the future. The OS programs XCR0 to reflect the features it supports.
  */

/* Definitions for the eXtended Control Register instructions */

#define XCR_XFEATURE_ENABLED_MASK   0x00000000 /* XCR0 */

/*
  * Currently, the XFEATURE_ENABLED_MASK register (XCR0) has two processor states
  * defined, with up to 61 bits reserved for future processor extended states:
  * - XCR0.X87 (bit 0): If 1, indicates x87 FPU state (including MMX register states) is
  *    supported in the processor. Bit 0 must be 1. An attempt to write 0 causes a #GP
  *    exception.
  * - XCR0.SSE (bit 1): If 1, indicates MXCSR and XMM registers (XMM0-XMM15 in 64-bit
  *    mode, otherwise XMM0-XMM7) are supported by XSAVE/XRESTOR in the processor.
  */

#define XCR0_X87 (1 << 0)
#define XCR0_SSE (1 << 1)

/** has_xcr_registers - check if the CPU has the XCR registers
  *
  *@return true if the CPU has XCR registers
  */
static inline bool has_xcr_registers(void)
    {
    cpuid_info_t cpuid_info;

    cpuid(CPUID_GETFEATURES, &cpuid_info);

    if (cpuid_info.ecx & CPUID_FEAT_ECX_XSAVE)
        return 1;
    else
        return 0;
    }

/* XGETBV (Get Value of Extended Control Register) */
static inline uint64_t xgetbv(uint32_t index)
    {
    uint32_t eax, edx;

    asm volatile(".byte 0x0f,0x01,0xd0" /* xgetbv */
             : "=a" (eax), "=d" (edx)
             : "c" (index));
    return eax + ((uint64_t)edx << 32);
    }

/* XSETBV (Set Value of Extended Control Register) */
static inline void xsetbv(uint32_t index, uint64_t value)
    {
    uint32_t eax = value;
    uint32_t edx = value >> 32;

    asm volatile(".byte 0x0f,0x01,0xd1" /* xsetbv */
             : : "a" (eax), "d" (edx), "c" (index));
    }
#endif /* __ASM__ */

#ifndef __ASM__

/** has_apic - check if the CPU has on chip LAPIC
  *
  *@return true if the CPU has on chip LAPIC
  */

static inline bool has_apic(void)
    {
    cpuid_info_t cpuid_info;
    /*
     * Presence of the Local LAPIC
     *
     * Beginning with the P6 family processors, the presence or absence of an
     * on-chip local APIC can be detected using the CPUID instruction. When
     * the CPUID instruction is executed with a source operand of 1 in the
     * EAX register, bit 9 of the CPUID feature flags returned in the EDX
     * register indicates the presence (set) or absence (clear) of a local APIC.
     */

    cpuid(CPUID_GETFEATURES, &cpuid_info);

    if (cpuid_info.edx & CPUID_FEAT_EDX_APIC)
        return 1;
    else
        return 0;
    }

/** has_x2apic - check if the CPU has on chip x2APIC
  *
  *@return true if the CPU has on chip x2APIC
  */

static inline bool has_x2apic(void)
    {
    cpuid_info_t cpuid_info;

    /*
     * DETECTING AND ENABLING x2APIC
     *
     * Processor support for x2APIC mode can be detected by executing CPUID
     * with EAX=1 and then checking ECX, bit 21 ECX. If CPUID.(EAX=1):ECX.21
     * is set , the processor supports the x2APIC capability and can be placed
     * into the x2APIC mode.
     */

    cpuid(CPUID_GETFEATURES, &cpuid_info);

    if (cpuid_info.ecx & CPUID_FEAT_ECX_x2APIC)
        return 1;
    else
        return 0;
    }

#endif /* __ASM__ */
/*********************System registers and bits (start)**********************/

#define EFER_MSR_NUM    0xc0000080
#define AMD_SCE_FLAG    0
#define AMD_LME_FLAG    8
#define AMD_LMA_FLAG    10
#define AMD_FFXSR_FLAG  14
#define AMD_NXE_FLAG    11

/* MSR registers */
#define AMD_MSR_STAR    0xc0000081
#define AMD_MSR_LSTAR   0xc0000082
#define AMD_MSR_SFMASK  0xc0000084
#define AMD_MSR_FS      0xc0000100
#define AMD_MSR_GS      0xc0000101


/** The VME, PVI, TSD, DE, PSE, PAE, MCE, PGE, PCE, OSFXSR, and OSXMMEXCPT flags
  * in control register CR4 are model specific. All of these flags (except the PCE flag) can
  * be qualified with the CPUID instruction to determine if they are implemented on the
  * processor before they are used.
  *The CR8 register is available on processors that support Intel 64 architecture.
  */

#define CR0_PG (1<<31) /* PG Paging (bit 31 of CR0) */
#define CR0_CD (1<<30) /* CD Cache Disable (bit 30 of CR0) */
#define CR0_NW (1<<29) /* NW Not Write-through (bit 29 of CR0) */
#define CR0_AM (1<<18) /* AM Alignment Mask (bit 18 of CR0) */
#define CR0_WP (1<<16) /* WP Write Protect (bit 16 of CR0) */
#define CR0_NE (1<<5 ) /* NE Numeric Error (bit 5 of CR0) */
#define CR0_ET (1<<4 ) /* ET Extension Type (bit 4 of CR0) */
#define CR0_TS (1<<3 ) /* TS Task Switched (bit 3 of CR0) */
#define CR0_EM (1<<2 ) /* EM Emulation (bit 2 of CR0) */
#define CR0_MP (1<<1 ) /* MP Monitor Coprocessor (bit 1 of CR0) */
#define CR0_PE (1<<0 ) /* PE Protection Enable (bit 0 of CR0) */

#define CR3_PCD (1<<4 ) /* PCD Page-level Cache Disable (bit 4 of CR3) */
#define CR3_PWT (1<<3 ) /* PWT Page-level Write-Through (bit 3 of CR3) */
#define CR3_BASE_MASK ~(0xFFF) /* PWT Page-level Write-Through (bit 3 of CR3) */

#define CR4_VME  (1<<0 ) /* VME Virtual-8086 Mode Extensions (bit 0 of CR4) */
#define CR4_PVI  (1<<1 ) /* PVI Protected-Mode Virtual Interrupts (bit 1 of CR4) */
#define CR4_TSD  (1<<2 ) /* TSD Time Stamp Disable (bit 2 of CR4) */
#define CR4_DE   (1<<3 ) /* DE Debugging Extensions (bit 3 of CR4) */
#define CR4_PSE  (1<<4 ) /* PSE Page Size Extensions (bit 4 of CR4) */
#define CR4_PAE  (1<<5 ) /* PAE Physical Address Extension (bit 5 of CR4)  */
#define CR4_MCE  (1<<6 ) /* MCE Machine-Check Enable (bit 6 of CR4)  */
#define CR4_PGE  (1<<7 ) /* PGE Page Global Enable (bit 7 of CR4) */
#define CR4_PCE  (1<<8 ) /* PCE Performance-Monitoring Counter Enable (bit 8 of CR4) */
#define CR4_OSFXSR  (1<<9 ) /* OSFXSR
                                                   * Operating System Support for FXSAVE and FXRSTOR
                                                   * instructions (bit 9 of CR4) */
#define CR4_OSXMMEXCPT  (1<<10 ) /* OSXMMEXCPT
                                                            * Operating System Support for Unmasked SIMD
                                                            * Floating-Point Exceptions (bit 10 of CR4) */
#define CR4_VMXE  (1<<13 ) /* VMXE VMX-Enable Bit (bit 13 of CR4) */
#define CR4_SMXE  (1<<14 ) /* SMXE SMX-Enable Bit (bit 14 of CR4) */
#define CR4_OSXSAVE  (1<<18 ) /* OSXSAVE
                                                       * XSAVE and Processor Extended States-Enable
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

/*********************System registers and bits (end)***********************/

#endif
