/* x64.h - Intel 64 and AMD64 common definitions and access routines */

#ifndef __INCLUDE_ARCH_X64_X64_H
#define __INCLUDE_ARCH_X64_X64_H

#include <sys.h>
#include <arch.h>

#ifndef __ASM__


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


/*********************System registers and bits (end)***********************/

#endif
