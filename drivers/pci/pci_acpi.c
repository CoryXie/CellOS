#include <sys.h>
#include <arch.h>
#include <os.h>

#include <acpi.h>
#include <accommon.h>
#include <amlcode.h>
#include <acparser.h>
#include <acdebug.h>
#include <acnamesp.h>

#undef PCI_ACPI_DETAL
#define PCI_MAX_DEVICES	32
#define PCI_MAX_PINS	4

#define IRQ_TABLE_ENTRIES	(PCI_MAX_DEVICES * PCI_MAX_PINS)

struct pci_bridge
    {
    ACPI_HANDLE		    handle;
    int			        irqtable[IRQ_TABLE_ENTRIES];
    int			        primary_bus;
    int			        secondary_bus;
    unsigned		    device;
    struct pci_bridge 	* parent;
    struct pci_bridge	* children[PCI_MAX_DEVICES];
    };

struct pci_bridge pci_root_bridge;

struct irq_resource
    {
    struct pci_bridge	* bridge;
    ACPI_PCI_ROUTING_TABLE	* tbl;
    };

struct pci_bridge * find_bridge(struct pci_bridge * root,
                                int pbnr,
                                int dev,
                                int sbnr)
    {
    if (!root)
        return NULL;

    if (sbnr == -1)
        {
        if (root->secondary_bus == pbnr)
            return root->children[dev];
        else
            {
            /* serach all children */
            unsigned d;
            for (d = 0; d < PCI_MAX_DEVICES; d++)
                {
                struct pci_bridge * b;
                b = find_bridge(root->children[d],
                                pbnr, dev, sbnr);
                if (b)
                    return b;
                }
            }
        }
    else
        {
        if (root->secondary_bus == sbnr)
            return root;
        else
            {
            /* check all children */
            unsigned d;
            for (d = 0; d < PCI_MAX_DEVICES; d++)
                {
                struct pci_bridge * b;
                b = find_bridge(root->children[d],
                                pbnr, dev, sbnr);
                if (b)
                    return b;
                }
            }
        }

    return NULL;
    }

int pci_map_bridge(unsigned dev, unsigned pbnr, unsigned sbnr)
    {
    int err = OK;
    struct pci_bridge * bridge;

    bridge = find_bridge(&pci_root_bridge, pbnr, dev, -1);

    if (!bridge)
        {
        err = ENODEV;
        goto map_error;
        }

    bridge->primary_bus = pbnr;
    bridge->secondary_bus = sbnr;

map_error:
    return err;
    }

ACPI_STATUS device_get_int(ACPI_HANDLE handle,
                           char * name,
                           ACPI_INTEGER * val)
    {
    ACPI_STATUS status;
    char buff[sizeof(ACPI_OBJECT)];
    ACPI_BUFFER abuff;

    abuff.Length = sizeof(buff);
    abuff.Pointer = buff;

    status =  AcpiEvaluateObjectTyped(handle, name, NULL,
                                      &abuff, ACPI_TYPE_INTEGER);
    if (ACPI_SUCCESS(status))
        {
        *val = ((ACPI_OBJECT *)abuff.Pointer)->Integer.Value;
        }

    return status;
    }

int pci_get_irq(unsigned bus, unsigned dev, unsigned pin)
    {
    struct pci_bridge * bridge;
    int irq;

    ASSERT(dev < PCI_MAX_DEVICES && pin < PCI_MAX_PINS);

    bridge = find_bridge(&pci_root_bridge, -1, -1, bus);

    if (!bridge)
        irq = -1;
    else
        irq = bridge->irqtable[dev * PCI_MAX_PINS + pin];

    return irq;
    }

void add_irq(struct pci_bridge * bridge,
             unsigned dev,
             unsigned pin,
             uint8_t irq)
    {
    ASSERT(dev < PCI_MAX_DEVICES && pin < PCI_MAX_PINS);
    
 #ifdef PCI_ACPI_DETAL   
    printk("add_irq bridge %p dev %p pin %d irq %d\n",
        bridge, dev, pin, irq);
 #endif
 
    bridge->irqtable[dev * PCI_MAX_PINS + pin] = irq;
    }

ACPI_STATUS get_irq_resource(ACPI_RESOURCE *res, void *context)
    {
    struct irq_resource * ires = (struct irq_resource *) context;

    if (res->Type == ACPI_RESOURCE_TYPE_IRQ)
        {
        ACPI_RESOURCE_IRQ *irq;

        irq = &res->Data.Irq;
                
        add_irq(ires->bridge, ires->tbl->Address >> 16, ires->tbl->Pin,
                irq->Interrupts[ires->tbl->SourceIndex]);
        }
    else if (res->Type == ACPI_RESOURCE_TYPE_EXTENDED_IRQ)
        {
        ACPI_RESOURCE_EXTENDED_IRQ *irq;

        irq = &res->Data.ExtendedIrq;

        add_irq(ires->bridge, ires->tbl->Address >> 16, ires->tbl->Pin,
                irq->Interrupts[ires->tbl->SourceIndex]);
        }

    return AE_OK;
    }

ACPI_STATUS get_pci_irq_routing(struct pci_bridge * bridge)
    {
    ACPI_STATUS status;
    ACPI_BUFFER abuff;
    char * buff = kmalloc(4096);
    ACPI_PCI_ROUTING_TABLE *tbl;
    ACPI_DEVICE_INFO *info;
    int i;

    if (buff == NULL)
        {
        printk("ACPI: get_pci_irq_routing AE_NO_MEMORY\n");
        return AE_NO_MEMORY;
        }
    
    abuff.Length = 4096;
    abuff.Pointer = buff;
    
    status = AcpiGetIrqRoutingTable(bridge->handle, &abuff);
    
    if (ACPI_FAILURE(status))
        {
#ifdef PCI_ACPI_DETAL   
        printk("ACPI: get_pci_irq_routing AcpiGetIrqRoutingTable fail %s\n",
            AcpiFormatException(status));
#endif
        kfree(buff);
        return AE_OK;
        }
    
    info = abuff.Pointer;
    status = AcpiGetObjectInfo(bridge->handle, &info);
    if (ACPI_FAILURE(status))
        {
        printk("ACPI: get_pci_irq_routing AcpiGetObjectInfo fail %s\n",
            AcpiFormatException(status));
        kfree(buff);
        return status;
        }
    
    /*
     * Decode the device number (upper half of the address) and attach the
     * new bridge in the children list of its parent
     */
    bridge->device = info->Address >> 16;
    if (bridge != &pci_root_bridge)
        {
        bridge->parent->children[bridge->device] = bridge;
        bridge->primary_bus = bridge->secondary_bus = -1;
        }

    for (i = 0; i < PCI_MAX_DEVICES; i++)
        bridge->children[i] = NULL;
    for (i = 0; i < IRQ_TABLE_ENTRIES; i++)
        bridge->irqtable[i] = -1;

    for (tbl = (ACPI_PCI_ROUTING_TABLE *)abuff.Pointer; tbl->Length;
            tbl = (ACPI_PCI_ROUTING_TABLE *)
                  ((char *)tbl + tbl->Length))
        {
        ACPI_HANDLE src_handle;
        struct irq_resource ires;

        if (*(char*)tbl->Source == '\0')
            {            
            add_irq(bridge, tbl->Address >> 16,
                    tbl->Pin, tbl->SourceIndex);
            continue;
            }

        status = AcpiGetHandle(bridge->handle, tbl->Source, &src_handle);
        if (ACPI_FAILURE(status))
            {
            printk("Failed AcpiGetHandle\n");
            continue;
            }
                
        ires.bridge = bridge;
        ires.tbl = tbl;
        status = AcpiWalkResources(src_handle, METHOD_NAME__CRS,
                                   get_irq_resource, &ires);
        if (ACPI_FAILURE(status))
            {
            printk("Failed IRQ resource\n");
            continue;
            }
        }
            
    kfree(buff);

    return AE_OK;
    }

ACPI_STATUS add_pci_dev(ACPI_HANDLE handle,
                        UINT32 level,
                        void *context,
                        void **retval)
    {
    ACPI_STATUS status;
    ACPI_HANDLE parent_handle;
    struct pci_bridge * bridge;
    struct pci_bridge * parent_bridge = (struct pci_bridge *) context;
    
    /* skip pci root when we get to it again */
    if (handle == pci_root_bridge.handle)
        {
        return AE_OK;
        }

    status = AcpiGetParent(handle, &parent_handle);
        
    if (!ACPI_SUCCESS(status))
        {
        printk("ACPI: add_pci_dev - AcpiGetParent fail 0x%X\n", status);
        return status;
        }
        
    /* skip devices that have a different parent */
    if (parent_handle != parent_bridge->handle)
        {
        return AE_OK;
        }

    bridge = kmalloc(sizeof(struct pci_bridge));
    if (!bridge)
        {
        printk("ACPI: add_pci_dev - AE_NO_MEMORY for pci_bridge\n");
        return AE_NO_MEMORY;
        }

    bridge->handle = handle;
    bridge->parent = parent_bridge;
    status =  get_pci_irq_routing(bridge);
    if (!(ACPI_SUCCESS(status)))
        {
        printk("ACPI: add_pci_dev - get_pci_irq_routing fail\n");
        kfree(bridge);
        return status;
        }
    
    /* get the pci bridges */
    status = AcpiGetDevices(NULL, add_pci_dev, bridge, NULL);
    
    return status;
    }

ACPI_STATUS add_pci_root_dev(ACPI_HANDLE handle,
                             UINT32 level,
                             void *context,
                             void **retval)
    {
    static unsigned called;
    ACPI_STATUS status;
    
    if (++called > 1)
        {
        printk("ACPI: Warning! Multi rooted PCI is not supported!\n");
        return AE_OK;
        }

    pci_root_bridge.handle = handle;
    pci_root_bridge.primary_bus = -1; /* undefined */
    pci_root_bridge.secondary_bus = 0; /* root bus is 0 in a single root system */

    status = get_pci_irq_routing(&pci_root_bridge);
        
    if (!ACPI_SUCCESS(status))
        {
        printk("ACPI: add_pci_root_dev get_pci_irq_routing fail\n");
        return status;
        }

    /* get the pci bridges */
    status = AcpiGetDevices(NULL, add_pci_dev, &pci_root_bridge, NULL);
    return status;
    }

void pci_scan_devices(void)
    {
    ACPI_STATUS status;

    /* get the root first */
    status = AcpiGetDevices("PNP0A03", add_pci_root_dev, NULL, NULL);
    
    ASSERT(ACPI_SUCCESS(status));
    }

int do_scanpci (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
    {
    //AcpiDbgLevel = ACPI_DEBUG_ALL;
    pci_scan_devices();
    //AcpiDbgLevel = ACPI_DEBUG_DEFAULT;
    return 0;
    }

CELL_OS_CMD(
    scanpci,   1,        1,    do_scanpci,
    "show pci bus device tree",
    "show the pci bus device tree!"
    );

