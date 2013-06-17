#ifndef _DRIVERS_BUS_PCI_PCI_H
#define _DRIVERS_BUS_PCI_PCI_H

#define PCI_CFG_TYPE_NONE 0
#define PCI_CFG_TYPE_1    1
#define PCI_CFG_TYPE_2    2

/*
 * Under PCI, each device has 256 bytes of configuration address space,
 * of which the first 64 bytes are standardized as follows:
 */
#define PCI_VENDOR_ID		0x00	/* 16 bits */
#define PCI_DEVICE_ID		0x02	/* 16 bits */
#define PCI_COMMAND		    0x04	/* 16 bits */
#define PCI_COMMAND_IO		0x1	/* Enable response in I/O space */
#define PCI_COMMAND_MEMORY	0x2	/* Enable response in Memory space */
#define PCI_COMMAND_MASTER	0x4	/* Enable bus mastering */
#define PCI_COMMAND_SPECIAL	0x8	/* Enable response to special cycles */
#define PCI_COMMAND_INVALIDATE	0x10	/* Use memory write and invalidate */
#define PCI_COMMAND_VGA_PALETTE 0x20	/* Enable palette snooping */
#define PCI_COMMAND_PARITY	0x40	/* Enable parity checking */
#define PCI_COMMAND_WAIT 	0x80	/* Enable address/data stepping */
#define PCI_COMMAND_SERR	0x100	/* Enable SERR */
#define PCI_COMMAND_FAST_BACK	0x200	/* Enable back-to-back writes */

#define PCI_STATUS		    0x06	/* 16 bits */
#define PCI_STATUS_CAP_LIST	0x10	/* Support Capability List */
#define PCI_STATUS_66MHZ	0x20	/* Support 66 Mhz PCI 2.1 bus */
#define PCI_STATUS_UDF		0x40	/* Support User Definable Features [obsolete] */
#define PCI_STATUS_FAST_BACK	0x80	/* Accept fast-back to back */
#define PCI_STATUS_PARITY	    0x100	/* Detected parity error */
#define PCI_STATUS_DEVSEL_MASK	0x600	/* DEVSEL timing */
#define PCI_STATUS_DEVSEL_FAST	0x000	
#define PCI_STATUS_DEVSEL_MEDIUM    0x200
#define PCI_STATUS_DEVSEL_SLOW      0x400
#define PCI_STATUS_SIG_TARGET_ABORT 0x800  /* Set on target abort */
#define PCI_STATUS_REC_TARGET_ABORT 0x1000 /* Master ack of " */
#define PCI_STATUS_REC_MASTER_ABORT 0x2000 /* Set on master abort */
#define PCI_STATUS_SIG_SYSTEM_ERROR 0x4000 /* Set when we drive SERR */
#define PCI_STATUS_DETECTED_PARITY  0x8000 /* Set on parity error */

#define PCI_CLASS_REVISION	    0x08	/* High 24 bits class, low 8 revision */
#define PCI_REVISION_ID         0x08    /* Revision ID */
#define PCI_CLASS_PROG          0x09    /* Reg. Level Programming Interface */
#define PCI_CLASS_DEVICE        0x0a    /* Device class */

#define PCI_CACHE_LINE_SIZE	0x0c	/* 8 bits */
#define PCI_LATENCY_TIMER	0x0d	/* 8 bits */
#define PCI_HEADER_TYPE		0x0e	/* 8 bits */
#define PCI_HEADER_TYPE_NORMAL	0
#define PCI_HEADER_TYPE_BRIDGE  1
#define PCI_HEADER_TYPE_CARDBUS 2

#define PCI_BIST		    0x0f	/* 8 bits */
#define PCI_BIST_CODE_MASK	0x0f	/* Return result */
#define PCI_BIST_START		0x40	/* 1 to start BIST, 2 secs or less */
#define PCI_BIST_CAPABLE	0x80	/* 1 if BIST capable */

/*
 * Base addresses specify locations in memory or I/O space.
 * Decoded size can be determined by writing a value of 
 * 0xffffffff to the register, and reading it back.  Only 
 * 1 bits are decoded.
 */
#define PCI_BASE_ADDRESS_0	0x10	/* 32 bits */
#define PCI_BASE_ADDRESS_1	0x14	/* 32 bits [htype 0,1 only] */
#define PCI_BASE_ADDRESS_2	0x18	/* 32 bits [htype 0 only] */
#define PCI_BASE_ADDRESS_3	0x1c	/* 32 bits */
#define PCI_BASE_ADDRESS_4	0x20	/* 32 bits */
#define PCI_BASE_ADDRESS_5	0x24	/* 32 bits */
#define PCI_BASE_ADDRESS_SPACE	0x01	/* 0 = memory, 1 = I/O */
#define PCI_BASE_ADDRESS_SPACE_IO 0x01
#define PCI_BASE_ADDRESS_SPACE_MEMORY   0x00
#define PCI_BASE_ADDRESS_MEM_TYPE_MASK  0x06
#define PCI_BASE_ADDRESS_MEM_TYPE_32	0x00	/* 32 bit address */
#define PCI_BASE_ADDRESS_MEM_TYPE_1M	0x02	/* Below 1M [obsolete] */
#define PCI_BASE_ADDRESS_MEM_TYPE_64	0x04	/* 64 bit address */
#define PCI_BASE_ADDRESS_MEM_PREFETCH	0x08	/* prefetchable? */
#define PCI_BASE_ADDRESS_MEM_MASK	    (~0x0fUL)
#define PCI_BASE_ADDRESS_IO_MASK	    (~0x03UL)
/* bit 1 is reserved if address_space = 1 */

/* Device classes and subclasses */
#define PCI_CLASS_NOT_DEFINED		0x0000
#define PCI_CLASS_NOT_DEFINED_VGA	0x0001

#define PCI_BASE_CLASS_BRIDGE		0x06
#define PCI_CLASS_BRIDGE_HOST		0x0600

enum 
    {
     CONFIG1_ADDRESS = 0xCF8,
     CONFIG1_DATA = 0xCFC,
     CONFIG2_ADDRESS = 0xCFA,
     CONFIG2_DATA = 0xCFB,
     PCI_NO_DEV = 0xffffffff
    };

struct pci_config_addr_t 
    {
     uint8_t reg_num : 8;
     uint8_t func_num : 3;
     uint8_t dev_num : 5;
     uint8_t bus_num : 8;
     uint8_t reserved : 7;
     uint8_t enable_bit : 1;
    } __attribute__ ((packed));

union pci_config_space_t 
    {
    struct {
          uint16_t vendor_id;
          uint16_t device_id;
          uint16_t command;
          uint16_t status;
          uint8_t rev_id;
          uint8_t prog_if;
          uint8_t subclass;
          uint8_t class;
          uint8_t cachelsz;
          uint8_t latency_timer;
          uint8_t header_type;
          uint8_t bist;
          uint32_t bar0, bar1, bar2, bar3, bar4, bar5;
          uint32_t cardbus_cis_reg;
          uint16_t sub_vendor_id;
          uint16_t sub_id;
          uint32_t exp_rom_addr;
          uint16_t caps;
          uint16_t reserved0;
          uint32_t reserved1;
          uint8_t irq;
          uint8_t int_pin;
          uint8_t min_grant;
          uint8_t max_latency;
     } __attribute__((packed)) type0;
    };

struct pci_dev_t;

struct pci_bus_t 
    {
    list_t pci_dev_list;
    list_t pci_bus_list;
    };

struct pci_dev_t 
    {
    union pci_config_space_t *cspace;
    struct pci_bus_t *bus;
    list_t list;
    int busn;
    int devn;
    };

extern uint32_t pci_read32(struct pci_config_addr_t *addr);
extern uint32_t pci_init(void);
extern int pci_enumerate(void);
extern void lspci(void);
extern struct pci_dev_t * get_pci_dev(uint16_t vendor_id, uint16_t device_id);

#endif /* _DRIVERS_BUS_PCI_PCI_H */

