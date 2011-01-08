#include <sys.h>
#include <arch.h>
#include <os.h>

x64_desc_ptr_64_t gdtr;
x64_tss_t tss;

/* The standard GDT */
static x64_seg_descriptor_t gdt [] =
    {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },           /* NULL descriptor  */
    { 0xFFFF, 0, 0, 0x9A, 0xF, 0, 1, 0, 1, 0 }, /* KERNEL CS 0x8    */
    { 0xFFFF, 0, 0, 0x92, 0xF, 0, 0, 0, 1, 0 }, /* KERNEL DS 0x10   */
    { 0xFFFF, 0, 0, 0xF8, 0xF, 0, 1, 0, 1, 0 }, /* USER CS 0x18     */
    { 0xFFFF, 0, 0, 0xF2, 0xF, 0, 0, 1, 1, 0 }, /* USER DS 0x20     */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },           /* TSS (part1) 0x28 */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },           /* TSS (part2) 0x30 */
    };

void x64_gdt_init(void)
    {
    /* setup our GDT pointer */

    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (uint64_t)&gdt;

    /* load the GDT */

    asm volatile ("lgdt %0" :: "m"(gdtr));

    /* reload the segment registers */

    asm volatile ("mov $0x0, %ax");
    asm volatile ("mov %ax, %fs");
    asm volatile ("mov %ax, %gs");
    asm volatile ("mov $0x10, %ax");
    asm volatile ("mov %ax, %ss");
    asm volatile ("mov %ax, %ds");
    asm volatile ("mov %ax, %es");

    }

void x64_gdt_ap_init(void)
    {
    /* load the GDT */

    asm volatile ("lgdt %0" :: "m"(gdtr));    
    }
