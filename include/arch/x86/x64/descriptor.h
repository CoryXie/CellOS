/* descriptor.h - X86-64 specific descriptor management */

#ifndef _ARCH_X86_X64_DESCRIPTOR_H
#define _ARCH_X86_X64_DESCRIPTOR_H

/* Segment selectors */
#define GDT_SEL_NULL        0x0
#define GDT_SEL_KERNEL_CS   0x8
#define GDT_SEL_KERNEL_DS   0x10
#define GDT_SEL_USER_CS     0x18
#define GDT_SEL_USER_DS     0x20
#define GDT_SEL_PART1_TSS   0x28 /* 64-bit mode TSS descriptor is 16 bytes */
#define GDT_SEL_PART2_TSS   0x30 /* Part2 of 16-byte, 64-bit mode TSS descriptor */
#define GDT_SEL_ENTRIES     7

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
#define TSS_IOMAP_SIZE  (8 * 1024)  

#define IO_PORTS        (64 * 1024)

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
 * Although hardware task-switching is not supported in 64-bit mode, 
 * a 64-bit task state segment (TSS) must exist.
 *
 * The operating system must create at least one 64-bit TSS after 
 * activating IA-32e mode. It must execute the LTR instruction (in 
 * 64-bit mode) to load the TR register with a pointer to the 64-bit
 * TSS responsible for both 64-bit-mode programs and compatibility-mode 
 * programs.
 */
typedef struct x64_tss
    {
    uint32_t reserve1;
    /* The full 64-bit canonical forms of the stack pointers (RSP)
     * for privilege levels 0-2. */
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserve2;
    /* The full 64-bit canonical forms of the interrupt stack table
     * (IST) pointers. */
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

#endif /* _ARCH_X86_X64_DESCRIPTOR_H */
