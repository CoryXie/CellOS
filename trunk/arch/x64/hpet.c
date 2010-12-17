/*
  HPET
  
  The High-Precision Event Timer is a recent addition to the architecture,
  specified over 2001-2004 and present in ICH8-based chipsets. It was motivated
  by troubles with the TSC, the inadequacy of the PMT and the desire
  to replace the legacy PIT. The main components are a master 32 or 64-bit
  counter running at a frequency of at least 10 MHz (usually the 14.318 MHz
  source) and multiple comparator registers. Access is via memory-mapped
  I/O (at an address indicated by the ACPI HPET table), which is very convenient
  and also fast, taking 0.9 \u03bcs.
*/

#include <sys.h>
#include <arch.h>
#include <os.h>

#include <acpi.h>
#include <accommon.h>
#include <amlcode.h>
#include <acparser.h>
#include <acdebug.h>
#include <acnamesp.h>

static char *hpet_ids[] = { "PNP0103", NULL };

static ACPI_STATUS hpet_find
    (
    ACPI_HANDLE handle, 
    UINT32 level, 
    void *context,
    void **status
    )
    {
    char 		**ids;
    uint32_t	id = (uint32_t)(uintptr_t)context;
    uint32_t	uid = 0;

    for (ids = hpet_ids; *ids != NULL; ids++) 
        {
    	if (acpi_MatchHid(handle, *ids))
            break;
        }
    
    if (*ids == NULL)
    	return (AE_OK);
    
    if (ACPI_FAILURE(acpi_GetInteger(handle, "_UID", &uid)) || id == uid)
    	*((int *)status) = 1;
    
    return (AE_OK);
    }

/* Discover the HPET via the ACPI table of the same name. */
void hpet_identify(void)
    {
    ACPI_TABLE_HPET *hpet;
    ACPI_STATUS	status;
    int 		i, found;

    for (i = 1; ; i++) 
        {
        /* Search for HPET table. */
        status = AcpiGetTable(ACPI_SIG_HPET, i, (ACPI_TABLE_HEADER **)&hpet);

        if (ACPI_FAILURE(status))
            {
            printk("AcpiGetTable ACPI_SIG_HPET faile\n");
        	return;
            }

        /* Search for HPET device with same ID. */
        found = 0;
        
        AcpiWalkNamespace(ACPI_TYPE_DEVICE, 
                          ACPI_ROOT_OBJECT,
                          100,
                          hpet_find,
                          NULL, 
                          (void *)(uintptr_t)hpet->Sequence, 
                          (void *)&found);

        if (found)
            {
            printk("Found HPET at %d\n", i);
            }
        }
    }

int do_hpet (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
    {
    hpet_identify();
    
    return 0;
    }

CELL_OS_CMD(
    hpet,   1,        1,    do_hpet,
    "show hpet information",
    "show hpet information\n"
    );

