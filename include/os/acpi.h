#ifndef _OS_ACPI_H
#define _OS_ACPI_H

#include <acpi.h>
#include <accommon.h>
#include <amlcode.h>
#include <acparser.h>
#include <acdebug.h>
#include <acnamesp.h>

typedef struct acpi_rsdp
    {
    uint8_t  sig[8];
    uint8_t checksum;
    uint8_t  oemid[6];
    uint8_t revision;
    uint32_t rsdt_address;

    //uint32_t length;
    //uint64_t xsdt_address;
    //uint8_t extended_checksum;
    //uint8_t reserved[3];

    } acpi_rsdp_t;

typedef struct acpi_rsdt_header
    {
    uint8_t sig[4];
    uint32_t length;

    uint8_t revision;
    uint8_t checksum;
    uint8_t oemid[6];
    uint8_t oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_rev;
    } acpi_rsdt_header_t;

#define ACPI_SIGNATURE "RSD PTR "

void acpi_init(void);

#endif /* _OS_ACPI_H */

