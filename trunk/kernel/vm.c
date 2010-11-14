/* vm.c - kernel virtual memory management interfaces */

#include <sys.h>
#include <arch.h>
#include <os.h>

/* 
 * vm_virt2phys - translate virtual address of current task to physical address
 *
 * Translate virtual address of current task to physical address.
 * Returns physical address on success, or NULL if no mapped memory.
 */

cpu_addr_t vm_virt2phys
    (
    cpu_addr_t vaddr, 
    size_t size
    )
    {

    }
