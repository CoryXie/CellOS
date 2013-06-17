#ifndef _ARCH_X86_APIC_H
#define _ARCH_X86_APIC_H

#define    LAPIC_DEFAULT_PHYS_BASE       0xfee00000

#define    LAPIC_ID                      0x20

#define    LAPIC_LVR                     0x30
#define    LAPIC_LVR_MASK                0xFF00FF
#define    GET_LAPIC_VERSION(x)          ((x) & 0xFFu)
#define    GET_LAPIC_MAXLVT(x)           (((x) >> 16) & 0xFFu)
#define    LAPIC_INTEGRATED(x)           (1)
#define    LAPIC_XAPIC(x)                ((x) >= 0x14)
#define    LAPIC_EXT_SPACE(x)            ((x) & 0x80000000)
#define    LAPIC_TASKPRI                 0x80
#define    LAPIC_TPRI_MASK               0xFFu
#define    LAPIC_ARBPRI                  0x90
#define    LAPIC_ARBPRI_MASK             0xFFu
#define    LAPIC_PROCPRI                 0xA0
#define    LAPIC_EOI                     0xB0
#define    LAPIC_EIO_ACK                 0x0
#define    LAPIC_RRR                     0xC0
#define    LAPIC_LDR                     0xD0
#define    LAPIC_LDR_MASK                (0xFFu << 24)
#define    GET_LAPIC_LOGICAL_ID(x)       (((x) >> 24) & 0xFFu)
#define    SET_LAPIC_LOGICAL_ID(x)       (((x) << 24))
#define    LAPIC_ALL_CPUS                0xFFu
#define    LAPIC_DFR                     0xE0
#define    LAPIC_DFR_CLUSTER             0x0FFFFFFFul
#define    LAPIC_DFR_FLAT                0xFFFFFFFFul
#define    LAPIC_SPURIOUS                0xF0 /* Spurious Interrupt Vector Register */
#define    LAPIC_SPURIOUS_FOCUS_DISABLED (1 << 9)
#define    LAPIC_SPURIOUS_LAPIC_ENABLED   (1 << 8)
#define    LAPIC_ISR                     0x100
#define    LAPIC_ISR_NR                  0x8 /* Number of 32 bit ISR registers. */
#define    LAPIC_TMR                     0x180
#define    LAPIC_IRR                     0x200
#define    LAPIC_ESR                     0x280
#define    LAPIC_ESR_SEND_CS             0x00001
#define    LAPIC_ESR_RECV_CS             0x00002
#define    LAPIC_ESR_SEND_ACC            0x00004
#define    LAPIC_ESR_RECV_ACC            0x00008
#define    LAPIC_ESR_SENDILL             0x00020
#define    LAPIC_ESR_RECVILL             0x00040
#define    LAPIC_ESR_ILLREGA             0x00080
#define    LAPIC_LVTCMCI                 0x2f0
#define    LAPIC_ICR_LOW                 0x300
#define    LAPIC_DEST_SELF               0x40000
#define    LAPIC_DEST_ALLINC             0x80000
#define    LAPIC_DEST_ALLBUT             0xC0000
#define    LAPIC_ICR_RR_MASK             0x30000
#define    LAPIC_ICR_RR_INVALID          0x00000
#define    LAPIC_ICR_RR_INPROG           0x10000
#define    LAPIC_ICR_RR_VALID            0x20000
#define    LAPIC_INT_LEVELTRIG           0x08000
#define    LAPIC_INT_ASSERT              0x04000
#define    LAPIC_ICR_BUSY                0x01000
#define    LAPIC_DEST_LOGICAL            0x00800
#define    LAPIC_DEST_PHYSICAL           0x00000
#define    LAPIC_DM_FIXED                0x00000
#define    LAPIC_DM_LOWEST               0x00100
#define    LAPIC_DM_SMI                  0x00200
#define    LAPIC_DM_REMRD                0x00300
#define    LAPIC_DM_NMI                  0x00400
#define    LAPIC_DM_INIT                 0x00500
#define    LAPIC_DM_STARTUP              0x00600
#define    LAPIC_DM_EXTINT               0x00700
#define    LAPIC_VECTOR_MASK             0x000FF
#define    LAPIC_ICR_HIGH                0x310
#define    GET_LAPIC_DEST_FIELD(x)       (((x) >> 24) & 0xFF)
#define    SET_LAPIC_DEST_FIELD(x)       ((x) << 24)
#define    LAPIC_LVT_TIMER               0x320 /* LVT Timer Register */
#define    LAPIC_LVT_THERMAL             0x330 /* LVT Thermal Sensor Register */
#define    LAPIC_LVTPC                   0x340
#define    LAPIC_LVT0                    0x350
#define    LAPIC_LVT_TIMER_BASE_MASK     (0x3 << 18)
#define    GET_LAPIC_TIMER_BASE(x)       (((x) >> 18) & 0x3)
#define    SET_LAPIC_TIMER_BASE(x)       (((x) << 18))
#define    LAPIC_TIMER_BASE_CLKIN        0x0
#define    LAPIC_TIMER_BASE_TMBASE       0x1
#define    LAPIC_TIMER_BASE_DIV          0x2
#define    LAPIC_LVT_TIMER_PERIODIC      (1 << 17) /* MODE: 0=one-shot 1=periodic */
#define    LAPIC_LVT_MASKED              (1 << 16) /* MASK: 0=unmasked 1=masked */
#define    LAPIC_LVT_BUSY                (1 << 12) /* BUSY: 0=not busy 1=busy */
#define    LAPIC_LVT_INTR_ID_MASK        (0xFF) /* Interrupt ID-number */
#define    LAPIC_LVT_LEVEL_TRIGGER       (1 << 15)
#define    LAPIC_LVT_REMOTE_IRR          (1 << 14)
#define    LAPIC_INPUT_POLARITY          (1 << 13)
#define    LAPIC_SEND_PENDING            (1 << 12)
#define    LAPIC_MODE_MASK               0x700
#define    GET_LAPIC_DELIVERY_MODE(x)    (((x) >> 8) & 0x7)
#define    SET_LAPIC_DELIVERY_MODE(x, y) (((x) & ~0x700) | ((y) << 8))
#define    LAPIC_MODE_FIXED              0x0
#define    LAPIC_MODE_NMI                0x4
#define    LAPIC_MODE_EXTINT             0x7
#define    LAPIC_LVT1                    0x360
#define    LAPIC_LVTERR                  0x370
#define    LAPIC_TICR                    0x380 /* Initial Count Register (read/write) */
#define    LAPIC_TCCR                    0x390 /* Current Count Register (read-only) */
#define    LAPIC_TDCR                    0x3E0 /* Timer's 'Divide-Configuration' */
#define    LAPIC_SELF_IPI                0x3F0
#define    LAPIC_TDIV_TMBASE             (1 << 2) /* Divider-Value field (bits 3, 1, and 0) */
#define    LAPIC_TDIV_1                  0xB /* 111 = divide by 1 */
#define    LAPIC_TDIV_2                  0x0 /* 000 = divide by 2 */
#define    LAPIC_TDIV_4                  0x1 /* 001 = divide by 4 */
#define    LAPIC_TDIV_8                  0x2 /* 010 = divide by 8 */
#define    LAPIC_TDIV_16                 0x3 /* 011 = divide by 16 */
#define    LAPIC_TDIV_32                 0x8 /* 100 = divide by 32 */
#define    LAPIC_TDIV_64                 0x9 /* 101 = divide by 64 */
#define    LAPIC_TDIV_128                0xA /* 110 = divide by 128 */
#define    LAPIC_TDIV_MSK                0xB 
#define    LAPIC_EFEAT                   0x400
#define    LAPIC_ECTRL                   0x410
#define    LAPIC_EILVTn(n)               (0x500 + 0x10 * n)
#define    LAPIC_EILVT_NR_AMD_K8         1    /* # of extended interrupts */
#define    LAPIC_EILVT_NR_AMD_10H        4
#define    LAPIC_EILVT_LVTOFF(x)         (((x) >> 4) & 0xF)
#define    LAPIC_EILVT_MSG_FIX           0x0
#define    LAPIC_EILVT_MSG_SMI           0x2
#define    LAPIC_EILVT_MSG_NMI           0x4
#define    LAPIC_EILVT_MSG_EXT           0x7
#define    LAPIC_EILVT_MASKED            (1 << 16)

#define    LAPIC_BASE        PA2VA(LAPIC_DEFAULT_PHYS_BASE)
#define    LAPIC_BASE_MSR    0x800
#define    X2LAPIC_ENABLE    (1UL << 10)

#define    MAX_IO_APICS     128
#define    MAX_LOCAL_APIC   32768

/*
 * All x86-64 systems are xAPIC compatible.
 * In the following, "apicid" is a physical LAPIC ID.
 */

#define XAPIC_DEST_CPUS_SHIFT       4
#define XAPIC_DEST_CPUS_MASK        ((1u << XAPIC_DEST_CPUS_SHIFT) - 1)
#define XAPIC_DEST_CLUSTER_MASK     (XAPIC_DEST_CPUS_MASK << XAPIC_DEST_CPUS_SHIFT)
#define APIC_CLUSTER(apicid)        ((apicid) & XAPIC_DEST_CLUSTER_MASK)
#define APIC_CLUSTERID(apicid)      (APIC_CLUSTER(apicid) >> XAPIC_DEST_CPUS_SHIFT)
#define APIC_CPUID(apicid)          ((apicid) & XAPIC_DEST_CPUS_MASK)
#define NUM_APIC_CLUSTERS           ((BAD_APICID + 1) >> XAPIC_DEST_CPUS_SHIFT)

#define BAD_APICID                  0xFFFFu

/* IMCR related bits */

#define IMCR_ADRS                       0x22    /* IMCR addr reg */
#define IMCR_DATA                       0x23    /* IMCR data reg */
#define IMCR_REG_SEL                    0x70    /* IMCR reg select */
#define IMCR_IOAPIC_ON                  0x01    /* IMCR IOAPIC route enable */
#define IMCR_IOAPIC_OFF                 0x00    /* IMCR IOAPIC route disable */

#ifndef __ASM__
status_t lapic_init(void);
uint8_t lapic_id(void);
void lapic_ipi(uint32_t dest, uint32_t type, uint8_t vec);
void lapic_write(uint32_t offset, uint32_t value);
uint32_t lapic_read(uint32_t offset);
#define this_cpu() lapic_id()
#endif /* __ASM__ */

#endif /* _ARCH_X86_APIC_H */

