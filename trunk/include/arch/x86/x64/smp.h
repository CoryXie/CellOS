#ifndef __INCLUDE_ARCH_X64_SMP_H
#define __INCLUDE_ARCH_X64_SMP_H

#include <sys.h>

/* Table 4-3. Base MP Configuration Table Entry Types */

#define MP_CONFIG_ENTRY_TYPE_PROCESSOR              0 /* One entry per processor.*/
#define MP_CONFIG_ENTRY_TYPE_BUS                    1 /* One entry per bus. */
#define MP_CONFIG_ENTRY_TYPE_IO_APIC                2 /* One entry per I/O LAPIC.*/
#define MP_CONFIG_ENTRY_TYPE_IO_INTR_ASSIGNMENT     3 /* One entry per bus interrupt source.*/
#define MP_CONFIG_ENTRY_TYPE_LOCAL_INTR_ASSIGNMENT  4 /* One entry per system interrupt source.*/

#define MP_CONFIG_ENTRY_PROCESSOR_LEN               20
#define MP_CONFIG_ENTRY_BUS_LEN                     8
#define MP_CONFIG_ENTRY_IOAPIC_LEN                  8
#define MP_CONFIG_ENTRY_IO_INTR_ASSIGNMENT_LEN      8
#define MP_CONFIG_ENTRY_LOCAL_INTR_ASSIGNMENT_LEN   8

#define MP_CFG_CPU_FLAGS_EN                         1
#define MP_CFG_CPU_FLAGS_BSP                        2

typedef struct x64_smp_cpu_info
    {
    uint8_t cpu_no;
    uint8_t apic_id;
    uint8_t cpu_info;
    } smp_cpu_info_t;


typedef struct smp_floating_pointer
    {
    uint8_t smp_signature[4]; /* The ASCII string represented by "_MP_" which
                               * serves as a search key for locating the pointer
                               * structure.*/

    uint32_t smp_config_table; /* The address of the beginning of the MP
                                * configuration table. All zeros if the MP
                                * configuration table does not exist.*/
    uint8_t smp_length; /* The length of the floating pointer structure table
                         * in paragraph (16-byte) units. The structure is
                         * 16 bytes or 1 paragraph long; so this field
                         * contains 01h.*/

    uint8_t smp_version; /* The version number of the MP specification
                          * supported. A value of 01h indicates Version 1.1.
                          * A value of 04h indicates Version 1.4.*/
    uint8_t smp_checksum; /* A checksum of the complete pointer structure.
                           * All bytes specified by the length field, including
                           * CHECKSUM and reserved bytes, must add up to
                           * zero.*/
    uint8_t smp_features1; /* Bits 0-7: MP System Configuration Type.
                            * When these bits are all zeros, the MP
                            * configuration table is present. When nonzero,
                            * the value indicates which default configuration
                            * (as defined in Chapter 5) is implemented by the
                            * system.*/
    uint8_t smp_features2; /* Bits 0-6: Reserved for future MP definitions.
                            * Bit 7: IMCRP. When the IMCR presence bit is
                            * set, the IMCR is present and PIC Mode is
                            * implemented; otherwise, Virtual Wire Mode is
                            * implemented.*/
    uint8_t smp_features3; /* Reserved for future MP definitions. Must be 0.*/
    uint8_t smp_features4; /* Reserved for future MP definitions. Must be 0.*/
    uint8_t smp_features5; /* Reserved for future MP definitions. Must be 0.*/
    } __attribute__((packed)) smp_floating_pointer_t;

typedef struct smp_config_table
    {
    uint8_t cfg_signature[4]; /* The ASCII string representation of "PCMP",
                               * which confirms the presence of the table.*/
    uint16_t cfg_base_cfg_len; /* The length of the base configuration table in bytes,
                                * including the header, starting from offset 0. This
                                * field aids in calculation of the checksum. */

    uint8_t cfg_revision; /* The version number of the MP specification.
                           * A value of 01h indicates Version 1.1. A value
                           * of 04h indicates Version 1.4.*/
    uint8_t cfg_checksum; /* A checksum of the entire base configuration table.
                           * All bytes, including CHECKSUM and reserved bytes,
                           * must add up to zero. */

    uint8_t cfg_oem_id[8];  /* A string that identifies the manufacturer of the system
                             * hardware.*/
    uint8_t cfg_product_id[12]; /* A string that identifies the product family. */
    uint32_t cfg_oem_table_addr; /* A physical-address pointer to an OEM-defined
                                 * configuration table. This table is optional.
                                 * If not present, this field is zero. */
    uint16_t cfg_oem_table_size; /* The size of the base OEM table in bytes. If the
                                  * table is not present, this field is zero. */

    uint16_t cfg_entry_count;    /* The number of entries in the variable
                                  * portion of the base table. This field
                                  * helps the software identify the end of
                                  * the table when stepping through the
                                  * entries. */

    uint32_t cfg_local_apic_base; /* The base address by which each processor
                                   * accesses its local LAPIC. */

    uint16_t cfg_extended_len;   /* Length in bytes of the extended entries that
                                  * are located in memory at the end of the base
                                  * configuration table. A zero value in this
                                  * field indicates that no extended entries
                                  * are present. */
    uint8_t cfg_extended_checksum;   /* Checksum for the set of extended table
                                      * entries, including only extended entries
                                      * starting from the end of the base
                                      * configuration table. When no extended
                                      * table is present, this field is zero. */

    uint8_t cfg_reserved; /* Not used */
    } __attribute__((packed)) smp_config_table_t;

/*
 * Processor Entries
 *
 * The MP Configuration Table will contain a record for each logical processor.
 * The configuration table is filled in by the BIOS after it executes a CPU
 * identification procedure on each of the processors in the system. Whenever
 * possible, the complete 32-bit CPU signature should be filled with the values
 * returned by the CPUID instruction. The CPU signature includes but is not
 * limited to, the stepping, model, and family fields. If the processor does
 * not have a CPUID instruction, the BIOS must fill these and future reserved
 * fields with information returned by the processor in the EDX register after
 * a processor reset.
 */

typedef struct mp_cfg_entry_processor
    {
    uint8_t entry_type;         /* A value of 0 identifies a processor entry. */
    uint8_t lapic_id;           /* The local APIC ID number for the particular processor. */
    uint8_t lapic_version;      /* Bits 0-7 of the local APIC¡¯s version register. */
    uint8_t cpu_flags;          /* CPU Flags BP (bit 1), EN (bit 0) */
    uint8_t cpu_signature[4];   /* CPU signature (stepping, model, family) */
    uint8_t cpu_features_flags[4];    /* Feature Flags */
    } __attribute__((packed)) mp_cfg_entry_processor_t;

/*
 * Bus Entries
 *
 * Bus entries identify the kinds of buses in the system. Because there may
 * be more than one bus in a system, each bus is assigned a unique bus ID
 * number by the BIOS. The bus ID number is used by the operating system to
 * associate interrupt lines with specific buses.
 *
 * Each bus in a system must have a unique BUS ID if any one of the following
 * criteria are true:
 * 1. The bus does not share its memory address space with another bus.
 * 2. The bus does not share its I/O address space with another bus.
 * 3. The bus does not share interrupt lines with another bus.
 * 4. Any aspect of the bus as an independent entity is software visible
 * (such as PCI configuration space).
 */

typedef struct mp_cfg_entry_bus
    {
    uint8_t entry_type;         /* Entry type 1 identifies a bus entry. */
    uint8_t bus_id; /* An integer that identifies the bus entry. The BIOS
                     * assigns identifiers sequentially, starting at zero.*/
    uint8_t bus_type_string[6]; /* A string that identifies the type of bus.
                                 * Refer to Table 4-8 for values. These are
                                 * 6-character ASCII (blank-filled) strings
                                 * used by the MP specification.*/
    }__attribute__((packed)) mp_cfg_entry_bus_t;

/*
 * I/O APIC Entries
 *
 * The configuration table contains one or more entries for I/O APICs.
 */

typedef struct mp_cfg_entry_ioapic
    {
    uint8_t entry_type;         /* A value of 2 identifies an I/O APIC entry. */
    uint8_t ioapic_id; /* The ID of this I/O APIC. */
    uint8_t ioapic_version; /* Bits 0¨C7 of the I/O APIC¡¯s version register. */
    uint8_t ioapic_flags; /* Bit 0: EN; If zero, this I/O APIC is unusable,
                           * and the operating system should not attempt to
                           * access this I/O APIC.
                           * At least one I/O APIC must be enabled.*/
    uint32_t ioapic_base_addr; /* Base address for this I/O APIC. */
    }__attribute__((packed)) mp_cfg_entry_ioapic_t;

/*
 * I/O Interrupt Assignment Entries
 *
 * These entries indicate which interrupt source is connected to each I/O APIC
 * interrupt input. There is one entry for each I/O APIC interrupt input that
 * is connected.
 *
 * When using I/O APICs, it is preferable that the buses do not share
 * interrupts with the other buses. Bus implementations that share interrupts,
 * such as the PCI and VL local buses, support their bus interrupts by
 * overloading them into another bus space. These buses can be supported in
 * one of the following two ways:
 *
 * 1. Interrupt Assignment Entries for each of the bus interrupts are listed
 * in the MP configuration table. Each interrupt destination matches the
 * destination of another interrupt source interrupt that this interrupt
 * shares. For example, if PCI-Device1/INTA# has the same vector as ISA-IRQ2,
 * then both Interrupt Assignment Entries for these vectors would refer to
 * the same destination I/O APIC and INTIN#.
 * 2. No Interrupt Assignment Entries are declared for any of the bus source
 * interrupts, and the operating system uses some other bus-specific knowledge
 * of bus interrupt schemes in order to support the bus. This operating system
 * bus-specific knowledge is beyond the scope of this specification.
 */

typedef struct mp_cfg_io_intr_assignment
    {
    uint8_t entry_type;  /* Entry type 3 identifies an I/O interrupt entry. */
    uint8_t intr_type;   /* See Table 4-11 for values. */
    uint16_t intr_flags; /* Bit 1:0 PO, Polarity of APIC I/O input signals */
                         /* Bit 3:2 EL, Trigger mode of APIC I/O input signals */
    uint8_t src_bus_id;  /* Identifies the bus from which the interrupt
                            * signal comes.*/
    uint8_t src_bus_irq;    /* Identifies the interrupt signal from the
                             * source bus. Values are mapped onto
                             * source bus signals, starting from zero. A
                             * value of 0, for example, would indicate
                             * IRQ0 of an ISA bus.*/
    uint8_t dest_ioapic_id; /* Identifies the I/O APIC to which the signal
                             * is connected. If the ID is 0FFh, the signal
                             * is connected to all I/O APICs.*/
    uint8_t dest_ioapic_intr_pin; /* Identifies the INTINn pin to which the
                             * signal is connected.*/
    }__attribute__((packed)) mp_cfg_io_intr_assignment_t;

/*
 * Local Interrupt Assignment Entries
 *
 * These configuration table entries tell what interrupt source is connected
 * to each local interrupt input of each local APIC.
 */

typedef struct mp_cfg_local_intr_assignment
    {
    uint8_t entry_type;  /* Entry type 4 identifies a local interrupt entry. */
    uint8_t intr_type;   /* See Table 4-11 for values. */
    uint16_t intr_flags; /* Bit 1:0 PO, Polarity of APIC I/O input signals */
                         /* Bit 3:2 EL, Trigger mode of APIC I/O input signals */
    uint8_t src_bus_id;  /* Identifies the bus from which the interrupt
                            * signal comes.*/
    uint8_t src_bus_irq;    /* Identifies the interrupt signal from the
                             * source bus. Values are mapped onto
                             * source bus signals, starting from zero. A
                             * value of 0, for example, would indicate
                             * IRQ0 of an ISA bus.*/
    uint8_t dest_lapic_id; /* Identifies the local APIC to which the signal
                             * is connected. If the ID is 0FFh, the signal
                             * is connected to all local APICs.*/
    uint8_t dest_lapic_intr_pin; /* Identifies the INTINn pin to which the
                             * signal is connected, where n = 0 or 1.*/
    }__attribute__((packed)) mp_cfg_local_intr_assignment_t;

/* Table 4-11. Interrupt Type Values */

#define INTR_TYPE_INT        0 /* Signal is a vectored interrupt; vector
                                   * is supplied by APIC redirection table. */
#define INTR_TYPE_NMI        1 /* Signal is a nonmaskable interrupt. */
#define INTR_TYPE_SMI        2 /* Signal is a system management interrupt */
#define INTR_TYPE_ExtINT     3 /* Signal is a vectored interrupt; vector
                                   * is supplied by external PIC. For example,
                                   * if an 8259 is used as the external PIC,
                                   * the source is the 8259 INTR output line,
                                   * and the vector is supplied by the 8259.*/



/* Polarity of APIC I/O input signals */

#define INTR_PO_BUS_SPECIFIC 0 /* Conforms to specifications of bus (for
                                   * example, EISA is activelow for
                                   * level-triggered interrupts) */
#define INTR_PO_ACTIVE_HIGH  1  /* Active high */
#define INTR_PO_RESERVED     2  /* RESERVED */
#define INTR_PO_ACTIVE_LOW   3  /* Active low */

/* Trigger mode of APIC I/O input signals */

#define INTR_EL_BUS_SPECIFIC 0 /* Conforms to specifications of bus (for
                                   * example, ISA is edge triggered)*/
#define INTR_EL_EDGE_TRIG    1  /* Edge-triggered */
#define INTR_EL_RESERVED     2  /* RESERVED */
#define INTR_EL_LEVEL_TRIG   3  /* Level-triggered */

void smp_init(void);

#endif
