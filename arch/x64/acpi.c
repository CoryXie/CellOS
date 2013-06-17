#include <sys.h>
#include <arch.h>
#include <os.h>
#include <os/acpi.h>

/*
 * Refering http://wiki.osdev.org/ACPI:
 *
 * ACPI (Advanced Configuration and Power Interface) is a Power Management 
 * and configuration standard for the PC, developed by Intel, Microsoft 
 * and Toshiba. ACPI allows the operating system to control the amount of
 * power each device is given (allowing it to put certain devices on standby
 * or power-off for example). It is also used to control and/or check thermal
 * zones (temperature sensors, fan speeds, etc), battery levels, PCI IRQ 
 * routing, CPUs, NUMA domains and many other things. 
 *
 * Implementing ACPI
 *
 * Information about ACPI is stored in the BIOS's memory (for those systems 
 * that support ACPI of course).
 *
 * There are 2 main parts to ACPI. The first part is the tables used by the 
 * OS for configuration during boot (these include things like how many CPUs, 
 * APIC details, NUMA memory ranges, etc). The second part is the run time 
 * ACPI environment, which consists of AML code (a platform independent OOP 
 * language that comes from the BIOS and devices) and the ACPI SMM (System 
 * Management Mode) code.
 *
 * To begin using ACPI, the operating system must look for the RSDP (Root 
 * System Description Pointer). See RSDP section below.
 *
 * If the RSDP is found and the verification is valid, it contains a pointer 
 * to the RSDT (Root System Description Table) and for newer versions of ACPI 
 * (ACPI 2.0 and later) there is an additional XSDT (eXtended System 
 * Description Table). Both the RSDT and the XSDT contain pointers to other 
 * tables. The only real difference between the RSDT and the XSDT is that 
 * the XSDT contains 64 bit pointer instead of 32 bit pointers.
 *
 * For the run time part of ACPI the main table to detect is the FADT (Fixed 
 * ACPI Description Table) as this contains information needed to enable ACPI.
 *
 * You have two possibilities of using the ACPI. You can write your own ACPI 
 * table reader and AML interpreter. Or you can integrate ACPICA in your OS.
 * Switching to ACPI Mode
 *
 * Switching to ACPI Mode 
 *
 * On some PCs, this is already done for you if....
 *
 * - the smi command field in the FADT is 0
 * - the ACPI enable and ACPI disable fields in the FADT are both 0 
 *
 * Otherwise, write the value of the ACPI Enable field into the register number
 * pointed to by the smi command field, like so:
 *
 * outb(fadt->smi_cmd,fadt->acpi_enable);
 *
 * Linux waits 3 seconds for the hardware to change modes. 
 */
 
/*
 * Root System Description Pointer (RSDP)
 *
 * During OS initialization, OSPM must obtain the Root System Description 
 * Pointer (RSDP) structure from the platform. When OSPM locates the Root 
 * System Description Pointer (RSDP) structure, it then locates the Root 
 * System Description Table (RSDT) or the Extended Root System Description 
 * Table (XSDT) using the physical system address supplied in the RSDP.
 *
 * Finding the RSDP on IA-PC Systems
 *
 * OSPM finds the Root System Description Pointer (RSDP) structure by 
 * searching physical memory ranges on 16-byte boundaries for a valid 
 * Root System Description Pointer structure signature and checksum match
 * as follows:
 * - The first 1 KB of the Extended BIOS Data Area (EBDA). For EISA or 
 *   MCA systems, the EBDA can be found in the two-byte location 40:0Eh on 
 *   the BIOS data area.
 * - The BIOS read-only memory space between 0E0000h and 0FFFFFh.
 */
 
acpi_rsdp_t * acpi_locate_rsdp (void)
    {
    acpi_rsdp_t *   rsdp;
    char *          acpi_sig;
    cpu_addr_t      start_address;
    cpu_addr_t      end_address;
    cpu_addr_t      address;
    uint8_t *       ptr;

    /*
     * We only search the BIOS read-only memory space between 
     * 0E0000h and 0FFFFFh 
     */
     
    start_address = PA2KA(0xE0000);
    end_address = PA2KA(0xFFFFF);
    acpi_sig = ACPI_SIGNATURE;

    for (address = start_address; address < end_address; address += 16)
        {
        ptr = (uint8_t *) address;
        if (ptr[0] == acpi_sig[0] && ptr[1]==acpi_sig[1] &&
            ptr[2] == acpi_sig[2] && ptr[3]==acpi_sig[3] &&
            ptr[4] == acpi_sig[4] && ptr[5]==acpi_sig[5] &&
            ptr[6] == acpi_sig[6] && ptr[7]==acpi_sig[7])
            {
            rsdp = (acpi_rsdp_t *)ptr;

            printk("acpi_locate_rsdp: found ACPI at %p, revision %i.0\n",
                ptr,
                rsdp->revision+1);

            return rsdp;
            }
        }

    return NULL;
    }

void x64_acpi_debug
    (
    acpi_rsdp_t *p
    )
    {
    struct acpi_rsdt_header *header;

    printk("RSDT is at 0x%x\n",p->rsdt_address);
    header = (struct acpi_rsdt_header *) (PA2VA(p->rsdt_address));
    printk("header=%p\n",header);
    printk("length=%x\n",header->length);
    }

void acpi_init(void)
    {
    struct acpi_rsdp *rsdp;

    rsdp = (struct acpi_rsdp*)acpi_locate_rsdp();

    x64_acpi_debug(rsdp);
    }

/*
 * Match a HID string against a handle
 */
BOOLEAN
acpi_MatchHid(ACPI_HANDLE h, const char *hid) 
{
    ACPI_DEVICE_INFO    *devinfo;
    BOOLEAN     ret;
    int         i;

    if (hid == NULL || h == NULL ||
    ACPI_FAILURE(AcpiGetObjectInfo(h, &devinfo)))
    return (FALSE);

    ret = FALSE;
    if ((devinfo->Valid & ACPI_VALID_HID) != 0 &&
    strcmp(hid, devinfo->HardwareId.String) == 0)
        ret = TRUE;
    else if ((devinfo->Valid & ACPI_VALID_CID) != 0)
    for (i = 0; i < devinfo->CompatibleIdList.Count; i++) {
        if (strcmp(hid, devinfo->CompatibleIdList.Ids[i].String) == 0) {
        ret = TRUE;
        break;
        }
    }

    AcpiOsFree(devinfo);
    return (ret);
}

ACPI_STATUS
acpi_ConvertBufferToInteger(ACPI_BUFFER *bufp, UINT32 *number)
{
    ACPI_OBJECT    *p;
    UINT8    *val;
    int        i;

    p = (ACPI_OBJECT *)bufp->Pointer;
    if (p->Type == ACPI_TYPE_INTEGER) {
    *number = p->Integer.Value;
    return (AE_OK);
    }
    if (p->Type != ACPI_TYPE_BUFFER)
    return (AE_TYPE);
    if (p->Buffer.Length > sizeof(int))
    return (AE_BAD_DATA);

    *number = 0;
    val = p->Buffer.Pointer;
    for (i = 0; i < p->Buffer.Length; i++)
    *number += val[i] << (i * 8);
    return (AE_OK);
}

/*
 * Evaluate a path that should return an integer.
 */
ACPI_STATUS
acpi_GetInteger(ACPI_HANDLE handle, char *path, UINT32 *number)
{
    ACPI_STATUS    status;
    ACPI_BUFFER    buf;
    ACPI_OBJECT    param;

    if (handle == NULL)
    handle = ACPI_ROOT_OBJECT;

    /*
     * Assume that what we've been pointed at is an Integer object, or
     * a method that will return an Integer.
     */
    buf.Pointer = &param;
    buf.Length = sizeof(param);
    status = AcpiEvaluateObject(handle, path, NULL, &buf);
    if (ACPI_SUCCESS(status)) {
    if (param.Type == ACPI_TYPE_INTEGER)
        *number = param.Integer.Value;
    else
        status = AE_TYPE;
    }

    /* 
     * In some applications, a method that's expected to return an Integer
     * may instead return a Buffer (probably to simplify some internal
     * arithmetic).  We'll try to fetch whatever it is, and if it's a Buffer,
     * convert it into an Integer as best we can.
     *
     * This is a hack.
     */
    if (status == AE_BUFFER_OVERFLOW) {
    if ((buf.Pointer = AcpiOsAllocate(buf.Length)) == NULL) {
        status = AE_NO_MEMORY;
    } else {
        status = AcpiEvaluateObject(handle, path, NULL, &buf);
        if (ACPI_SUCCESS(status))
        status = acpi_ConvertBufferToInteger(&buf, number);
        AcpiOsFree(buf.Pointer);
    }
    }
    return (status);
}

