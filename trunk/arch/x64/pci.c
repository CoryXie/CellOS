#include <sys.h>
#include <arch.h>
#include <os.h>
#include <os/list.h>
#include <drivers/bus/pci/pci.h>

static int has_pci_init = 0;
static list_t rootbus;
static spinlock_t pci_lock;

uint32_t pci_read32(struct pci_config_addr_t *addr)
    {
    uint32_t r;

    spinlock_lock(&pci_lock);
    
    if (addr->reg_num & 0x3)
        panic("offset is not 0-padded!");
    
    ioport_out32(CONFIG1_ADDRESS, *(uint32_t *)addr);
    
    r = ioport_in32(CONFIG1_DATA);
    
    spinlock_unlock(&pci_lock);
    
    return r;
    }

uint32_t pci_init(void)
    {
    uint32_t ret, tmp;

    spinlock_init(&pci_lock);
    
    ioport_out32(CONFIG1_ADDRESS, 0);
    ioport_out32(CONFIG2_ADDRESS, 0);
    
    if (ioport_in32(CONFIG1_ADDRESS) == 0 && 
        ioport_in32(CONFIG2_ADDRESS) == 0)
        {
        printk("PCI type-2 is not supported!\n");
        return 1;
        }
    
    tmp = ioport_in32(CONFIG1_ADDRESS);
    ioport_out32(CONFIG1_ADDRESS, 0x80000000);
    
    ret = ioport_in32(CONFIG1_ADDRESS);
    ioport_out32(CONFIG1_ADDRESS, tmp);
    
    if (ret == 0x80000000)
        has_pci_init = 1;
    
    return ret != 0x80000000;
    }

static void dump_dev(struct pci_dev_t *dev)
    {
    struct pci_config_addr_t addr;
    union pci_config_space_t *cspace;
    uint32_t reg = 0x10, r;

    printk("Bus = %d, Device = %d\n", dev->busn, dev->devn);
    cspace = dev->cspace;
    if (cspace->type0.header_type == 0x1)
        printk("\tPCI-to-PCI bridge\n");
    printk("\tvendor_id = 0x%04lx, device_id = 0x%04lx\n",
           cspace->type0.vendor_id, cspace->type0.device_id);
    printk("\tstatus = 0x%04lx, command = 0x%04lx\n",
           cspace->type0.status, cspace->type0.command);
    printk("\tclass = 0x%02lx, subclass = 0x%02lx\n",
           cspace->type0.class, cspace->type0.subclass);
    printk("\tIRQ = %u\n", cspace->type0.irq);

    printk("Bus = %d, Device = %d\n", dev->busn, dev->devn);
    cspace = dev->cspace;
    if (cspace->type0.header_type == 0x1)
        printk("\tPCI-to-PCI bridge\n");
    printk("\tvendor_id = 0x%04lx, device_id = 0x%04lx\n",
                cspace->type0.vendor_id, cspace->type0.device_id);
    printk("\tstatus = 0x%04lx, command = 0x%04lx\n",
                cspace->type0.status, cspace->type0.command);
    printk("\tclass = 0x%02lx, subclass = 0x%02lx\n",
                cspace->type0.class, cspace->type0.subclass);
    printk("\tIRQ = %u\n", cspace->type0.irq);

    addr.enable_bit = 1;
    addr.bus_num = dev->busn;
    addr.dev_num = dev->devn;
    if (cspace->type0.header_type)
        {
        if (cspace->type0.header_type & 0x80)
            printk("\tmultiple functions are available\n");
        addr.reg_num = 0x18;
        r = pci_read32(&addr);
        printk("\tprimary bus = %u, secondary bus = %u\n",
               r & 0xff, (r >> 8) & 0xff);
        }
    for (; reg <= (cspace->type0.header_type == 0x1 ? 0x14 : 0x24);
            reg += 4)
        {
        addr.reg_num = reg;
        r = pci_read32(&addr);
        if (r != 0 && r != 0xffffffff)
            printk("\tbar%d = 0x%08lx\n",
                   (reg - 0x10) / 4, r);
        }
    }

void lspci(void)
    {
    struct pci_dev_t *dev;
    struct pci_bus_t *bus;

    if (!has_pci_init)
        {
        printk("there is no PCI bus!\n");
        return;
        }

    LIST_FOREACH(&rootbus, iter_bus)
        {
        bus = LIST_ENTRY(iter_bus, struct pci_bus_t, pci_bus_list);
        if (bus != NULL)
            {
            LIST_FOREACH(&bus->pci_dev_list, iter_dev)
                {
                dev = LIST_ENTRY(iter_dev, struct pci_dev_t, list);
                dump_dev(dev);
                }
            }
        }
    }

static int load_config_space(struct pci_dev_t *dev, struct pci_config_addr_t addr)
    {
    uint32_t r, reg, *p;
    union pci_config_space_t *cspace;

    cspace = kmalloc(sizeof(*cspace));
    if (!cspace)
        return ENOMEM;
    
    memset(cspace, 0, sizeof(cspace));
    p = (uint32_t *)cspace;
    addr.enable_bit = 1;
    for (reg = 0; reg <= 0x3c; reg += 4)
        {
        addr.reg_num = reg;
        r = pci_read32(&addr);
        if (r != 0)
            *p = r;
        ++p;
        }
    dev->cspace = cspace;
    
    return OK;
    }

static void cleanup(void)
    {
    struct pci_dev_t *dev;
    struct pci_bus_t *bus;

    LIST_FOREACH_SAFE(&rootbus, iter_bus)
        {
        bus = LIST_ENTRY(iter_bus, struct pci_bus_t, pci_bus_list);
        LIST_FOREACH_SAFE(&bus->pci_dev_list, iter_dev)
            {
            dev = LIST_ENTRY(iter_dev, struct pci_dev_t, list);
            list_remove(iter_dev);
            kfree(dev->cspace);
            kfree(dev);
            }
        list_remove(iter_bus);
        kfree(bus);
        }
    }

static struct pci_bus_t * create_bus(void)
    {
    struct pci_bus_t *bus;

    bus = kmalloc(sizeof(*bus));
    
    if (!bus)
        {
        cleanup();
        
        return bus;
        }
    
    list_init(&bus->pci_dev_list);
    
    return bus;
    }

static int attach_dev(struct pci_bus_t *bus, struct pci_config_addr_t addr,
                      int busn, int devn)
    {
    int ret;
    struct pci_dev_t *dev;

    dev = kmalloc(sizeof(*dev));
    if (!dev)
        {
        cleanup();
        return ENOMEM;
        }
    memset(dev, 0, sizeof(*dev));
    dev->cspace = NULL;
    dev->bus = bus;
    dev->busn = busn;
    dev->devn = devn;
    ret = load_config_space(dev, addr);
    if (ret < 0)
        {
        ASSERT(!dev->cspace);
        kfree(dev);
        cleanup();
        return -1;
        }
    
    list_append(&bus->pci_dev_list, &dev->list);
    
    return 0;
    }

int pci_enumerate(void)
    {
    struct pci_bus_t *busp = NULL;
    struct pci_config_addr_t addr;
    int dev, bus, ret, f;
    uint32_t r;

    list_init(&rootbus);
    memset(&addr, 0, sizeof(addr));
    addr.enable_bit = 1;
    for (bus = 0; bus < 256; ++bus)
        {
        addr.bus_num = bus;
        for (dev = 0, f = 0; dev < 32; ++dev)
            {
            addr.dev_num = dev;
            if ((r = pci_read32(&addr)) == PCI_NO_DEV)
                continue;
            if (!f)
                {
                busp = create_bus();
                if (!busp)
                    return ENOMEM;
                list_append(&rootbus, &busp->pci_bus_list);
                f = 1;
                }
            ret = attach_dev(busp, addr, bus, dev);
            if (ret < 0)
                {
                return ret;
                }
            }
        }
    return 0;
    }

struct pci_dev_t * get_pci_dev(uint16_t vendor_id, uint16_t device_id)
    {
    struct pci_dev_t *dev;
    struct pci_bus_t *bus;

    if (!has_pci_init)
        {
        printk("there is no PCI bus!\n");
        return NULL;
        }

    LIST_FOREACH(&rootbus, iter_bus)
        {
        bus = LIST_ENTRY(iter_bus, struct pci_bus_t, pci_bus_list);
        LIST_FOREACH(&bus->pci_dev_list, iter_dev)
            {
            dev = LIST_ENTRY(iter_dev, struct pci_dev_t, list);
            if (!dev->cspace)
                continue;
            if (dev->cspace->type0.vendor_id == vendor_id
                    && dev->cspace->type0.device_id == device_id)
                return dev;
            }
        }
    return NULL;
    }

int do_lspci (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
    {
    pci_enumerate();
    lspci();
    return 0;
    }

CELL_OS_CMD(
    lspci,   1,        1,    do_lspci,
    "show pci bus device tree",
    "show the pci bus device tree!"
    );

