/**
 * \file boot.S
 * \brief takes over control from the Multiboot bootloader,
 *  and jumps into the kernel proper.
 */

#include <arch/x86/x64/multiboot.h>
#include <arch/x86/x64/vm.h>
#include <arch/x86/x64/msr.h>

.section .text
.align 8
.code32

/* Multiboot header for GRUB */
_multiboot:
    .long MB_HEADER_MAGIC
    .long (MB_HFLAG_MODALIGN|MB_HFLAG_MEMINFO|MB_HFLAG_KLUDGE)
    .long -(MB_HEADER_MAGIC + (MB_HFLAG_MODALIGN|MB_HFLAG_MEMINFO|MB_HFLAG_KLUDGE))
    .long KA2PA(_multiboot)
    .long KA2PA(_code)
    .long KA2PA(_data_end)
    .long KA2PA(_end)
    .long KA2PA(_cell64_32_entry)


.globl _cell64_32_entry    /* making entry point visible to linker */
_cell64_32_entry:

    /* Disable interrupts */
    cli

    /* Clear EFLAGS. */
    push    $0
    popf

    /* save multiboot stuff grub passes us */
    movl %eax, KA2PA(_grub_magic)
    movl %ebx, KA2PA(_grub_mboot)

 /*
  * In compatibility mode and 64-bit mode, four levels of system data
  * structures are used. These include:
  *
  * The page map level 4 (PML4) - An entry in a PML4 table contains
  * the physical address of the base of a page directory pointer table,
  * access rights, and memory management information. The base physical
  * address of the PML4 is stored in CR3.
  *
  * A set of page directory pointer tables - An entry in a page directory
  * pointer table contains the physical address of the base of a page
  * directory table, access rights, and memory management information.
  *
  * Sets of page directories - An entry in a page directory table contains
  * the physical address of the base of a page table, access rights, and
  * memory management information.
  *
  * Sets of page tables - An entry in a page table contains the physical
  * address of a page frame, access rights, and memory management
  * information.
  */

 /* Note that the following code to add 0x7 is to set the following bits:
  *
  * bit0, P,        Present.
  * bit1, R/W,      Read/write; if 0, writes may not be allowed to the region
  *                 controlled by this entry.
  * bit2, U/S,      User/supervisor; if 0, accesses with CPL=3 are not allowed
  *                 to the region controlled by this entry
  * bit7, PS,       Page Size; apply for PDE only, if set, the PDE is pointing
  *                 to a 2MB big page and there is no page table associated 
  *                 with this PDE; otherwise th PDE is pointing to a page table 
  *                 which contains PTEs.
  */

    /* map the pdpt(page directory pointer table) into the pml4 */

    movl $KA2PA(_boot_pdpt) + (PG_PRESENT | PG_WRITE), KA2PA(_boot_pml4)

    /* map pd0 into pdpt (identity mapping) */

    movl $KA2PA(_boot_pd0)+ (PG_PRESENT | PG_WRITE), KA2PA(_boot_pdpt)

    /* map the same pdpt to the end of the pml4 for our higher half */
    /*0xFFFF,FF80,0000,0000 512 GB */
    movl $KA2PA(_boot_pdpt) + (PG_PRESENT | PG_WRITE), KA2PA(_boot_pml4)+0xFF8

    /* map pd0 high in the address space (for higher half) */
    /*0xFFFF,FFFF,8000,0000 1GB */
    movl $KA2PA(_boot_pd0) + (PG_PRESENT | PG_WRITE), KA2PA(_boot_pdpt)+0xFF0


    /* identity map the first 6 x 2MB pages - PS bit is set */

    movl $KA2PA(_boot_pd0), %ebx
    movl $(PG_PRESENT | PG_WRITE | PG_LARGE | PG_GLOBAL), %eax    /* 2MB starting at 0x0 */
    movl %eax, (%ebx)
    movl $0, 4(%ebx)

    add $8, %ebx
    addl $0x200000, %eax /* 2MB starting at 0x200000 */
    movl %eax, (%ebx)
    movl $0, 4(%ebx)

    add $8, %ebx
    addl $0x200000, %eax /* 2MB starting at 0x400000 */
    movl %eax, (%ebx)
    movl $0, 4(%ebx)

    add $8, %ebx
    addl $0x200000, %eax /* 2MB starting at 0x600000 */
    movl %eax, (%ebx)
    movl $0, 4(%ebx)

    add $8, %ebx
    addl $0x200000, %eax /* 2MB starting at 0x800000 */
    movl %eax, (%ebx)
    movl $0, 4(%ebx)

    add $8, %ebx
    addl $0x200000, %eax /* 2MB starting at 0xA00000 */
    movl %eax, (%ebx)
    movl $0, 4(%ebx)

/*
 * The steps for enabling long mode are:
 *
 *  - Have paging disabled (this is true when GRUB booted us)
 *  - Set the PAE enable bit in CR4 (bit 5)
 *  - Set the PCE enable bit in CR4 (bit 8)
 *  - Load CR3 with the physical address of the PML4
 *  - Enable long mode by setting the EFER.LME flag (bit 8) in MSR 0xC0000080
 *  - Enable paging (PE, bit 31 in CR0)
 *
 * Now the CPU will be in compatibility mode, and instructions are still 32-bit.
 * To enter long mode, the D/B bit (bit 22, 2nd dword) of the GDT code segment
 * must be clear (as it would be for a 16-bit code segment), and the L bit
 * (bit 21, 2nd dword) of the GDT code segment must be set. Once that is done,
 * the CPU is in 64-bit long mode.
 */
    /* enable PAE */

    movl %cr4, %eax
    bts $5, %eax /* CR4_PAE */
    bts $8, %eax /* CR4_PCE */
    movl %eax, %cr4

    /* load our PML4 */

    movl $KA2PA(_boot_pml4),%eax
    movl %eax, %cr3

    /* enable long mode */

    movl $(MSR_EFER), %ecx
    rdmsr
    bts $8, %eax     /* set EFER.LME=1 */
    wrmsr

    /* activate long mode by enabling paging */

    movl %cr0, %eax
    bts  $31, %eax
    movl %eax, %cr0

    /* fixup our GDT address to a low address */

    movl $KA2PA(_lmode_gdt_start), KA2PA(_boot_gdtr) + 0x2
    lgdt KA2PA(_boot_gdtr)

    /* long mode here we come!!! */

    ljmp $0x08, $KA2PA(long_mode)

/* ##################### X86-64 Long mode ###################### */
.code64
.align 8
long_mode:

    movq $go_high, %rax
    jmp *%rax
go_high:

    cld

    /* move into a temporary boot stack */

    #movq $_boot_stack + 4092, %rsp

    movq $PA2KA(0x2000), %rax
    movq %rax, %rsp
    movq $(KERNEL_VIRT_MAP_BASE), %rbp

    /* fixup our GDT address to a high address */

    movq $_lmode_gdt_start, (_boot_gdtr) + 0x2
    lgdt (_boot_gdtr)

    /* pass multiboot info to main */

    movq (_grub_magic), %rdi
    movq (_grub_mboot), %rsi
    call main
1:
    /* die */
    cli
    hlt
    jmp 1b

################## DATA Section ############################
.section .data

_lmode_gdt_start:
    .quad 0
    .quad 0x002F98000000FFFF /* L set (long mode); S set (code) */
.L_lmode_gdt_end:

_boot_gdtr:
    .word .L_lmode_gdt_end - _lmode_gdt_start - 1
    .long 0

################## BSS Section ###############################
.section .bss

.globl _grub_magic
_grub_magic:
    .long _grub_magic

.globl _grub_mboot
_grub_mboot:
    .long _grub_mboot

.align 4096
.globl _boot_stack
_boot_stack:
    .fill 4096

/*
 * A 4-KByte naturally aligned PML4 table is located at the physical address
 * specified in bits 51:12 of CR3. A PML4 table comprises 512 64-bit entries
 * (PML4Es). A PML4E is selected using the physical address defined as follows:
 * - Bits 51:12 are from CR3.
 * - Bits 11:3 are bits 47:39 of the linear address.
 * - Bits 2:0 are all 0.
 * Because a PML4E is identified using bits 47:39 of the linear address, it
 * controls access to a 512-GByte region of the linear-address space.
 */

.align 4096
.globl _boot_pml4
_boot_pml4:
    .fill 4096

/*
 * A 4-KByte naturally aligned page-directory-pointer table is located
 * at the physical address specified in bits 51:12 of the PML4E. A page-
 * directory-pointer table comprises 512 64-bit entries (PDPTEs). A PDPTE is
 * selected using the physical address defined as follows:
 * - Bits 51:12 are from the PML4E.
 * - Bits 11:3 are bits 38:30 of the linear address.
 * - Bits 2:0 are all 0.
 *
 * Because a PDPTE is identified using bits 47:30 of the linear address,
 * it controls access to a 1-GByte region of the linear-address space.
 */

.align 4096
.globl _boot_pdpt
_boot_pdpt:
    .fill 4096

/*
 * A 4-KByte naturally aligned page directory is located at the physical
 * address specified in bits 51:12 of the PDPTE. A page directory comprises
 * 512 64-bit entries (PDEs). A PDE is selected using the physical address
 * defined as follows:
 * - Bits 51:12 are from the PDPTE.
 * - Bits 11:3 are bits 29:21 of the linear address.
 * - Bits 2:0 are all 0.
 *
 * Because a PDE is identified using bits 47:21 of the linear address, it
 * controls access to a 2-MByte region of the linear-address space. Use
 * of the PDE depends on its PS flag (bit 7):
 * - If the PDE's PS flag is 1, the PDE maps a 2-MByte page.
 * - If the PDE's PS flag is 0, a 4-KByte naturally aligned page table
 * is located at the physical address specified in bits 51:12 of the PDE.
 * A page table comprises 512 64-bit entries (PTEs).
 */

.align 4096
.globl _boot_pd0
_boot_pd0:
    .fill 4096

/*
 * A PTE is selected using the physical address defined as follows:
 *
 * - Bits 51:12 are from the PDE.
 * - Bits 11:3 are bits20:12 of the linear address.
 * - Bits 2:0 are all 0.
 *
 * Because a PTE is identified using bits47:12 of the linear address, every PTE
 * maps a 4-KByte page (see Table4-17). The final physical address is computed
 * as follows:
 * - Bits 51:12 are from the PTE.
 * - Bits 11:0 are from the original linear address.
 */

