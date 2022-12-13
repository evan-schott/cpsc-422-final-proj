#include <lib/x86.h>
#include <lib/debug.h>

#include "import.h"

#define PAGESIZE      4096
#define PDIRSIZE      (PAGESIZE * 1024)
#define VM_USERLO     0x40000000
#define VM_USERHI     0xF0000000
#define VM_USERLO_PDE (VM_USERLO / PDIRSIZE)
#define VM_USERHI_PDE (VM_USERHI / PDIRSIZE)

/**
 * Sets the entire page map for process 0 as the identity map.
 * Note that part of the task is already completed by pdir_init.
 */
void pdir_init_kern(unsigned int mbi_addr)
{
    unsigned int pde_index;

    pdir_init(mbi_addr);

    // Set identity map for user PDEs
    for (pde_index = VM_USERLO_PDE; pde_index < VM_USERHI_PDE; pde_index++) {
        set_pdir_entry_identity(0, pde_index);
    }
}

/**
 * Maps the physical page # [page_index] for the given virtual address with the given permission.
 * In the case when the page table for the page directory entry is not set up,
 * you need to allocate the page table first.
 * In the case of error, it returns the constant MagicNumber defined in lib/x86.h,
 * otherwise, it returns the physical page index registered in the page directory,
 * (the return value of get_pdir_entry_by_va or alloc_ptbl).
 */
unsigned int map_page(unsigned int proc_index, unsigned int vaddr,
                      unsigned int page_index, unsigned int perm)
{
    unsigned int pde_entry = get_pdir_entry_by_va(proc_index, vaddr);
    unsigned int pde_page_index = pde_entry >> 12;

    if (pde_entry == 0) {
        pde_page_index = alloc_ptbl(proc_index, vaddr);
        if (pde_page_index == 0) {
            return MagicNumber;
        }
    }

    set_ptbl_entry_by_va(proc_index, vaddr, page_index, perm);
    return pde_page_index;
}

//TODO map_multipage
unsigned int map_page_multi(unsigned int proc_index, unsigned int vaddr,
                      unsigned int page_index, unsigned int perm, unsigned int size) 
{
    unsigned int pde_entry;
    unsigned int pde_page_index;
    unsigned int ret_page_index;
    unsigned int curr_vaddr;
    bool first = TRUE;

    for (curr_vaddr = vaddr; curr_vaddr < vaddr + size*PAGESIZE; curr_vaddr++) {
        
        pde_entry = get_pdir_entry_by_va(proc_index, vaddr);
        pde_page_index = pde_entry >> 12;
        if (pde_entry == 0) {
            pde_page_index = alloc_ptbl(proc_index, vaddr);
            if (pde_page_index == 0) {
                return MagicNumber;
            }
        }
        if (first) {
            ret_page_index = pde_page_index;
            first = FALSE;
        }
    }

    return ret_page_index;

}

//TODO map_superpage
unsigned int map_page_super(unsigned int proc_index, unsigned int vaddr,
                      unsigned int page_index, unsigned int perm) 
{
    unsigned int pde_entry = get_pdir_entry_by_va(proc_index, vaddr);
    unsigned int pde_page_index = pde_entry >> 12;

    if (pde_entry == 0) {
        // No need to deal with page tables, just make directory
        set_pdir_entry_by_va_super(proc_index, vaddr, page_index);
    }
    else { 
        return MagicNumber // Need to assume going in from brk/pagefault or whatever that input vaddr corresponds to pdir entry
    }
    // This shouldn't be necessary actually
    // set_ptbl_entry_by_va_super(proc_index, vaddr, page_index, perm);
    return pde_page_index;    
}

/**
 * Remove the mapping for the given virtual address (with rmv_ptbl_entry_by_va).
 * You need to first make sure that the mapping is still valid,
 * e.g., by reading the page table entry for the virtual address.
 * Nothing should be done if the mapping no longer exists.
 * You do not need to unmap the page table from the page directory.
 * It should return the corresponding page table entry.
 */
unsigned int unmap_page(unsigned int proc_index, unsigned int vaddr)
{
    unsigned int pte_entry = get_ptbl_entry_by_va(proc_index, vaddr);
    if (pte_entry != 0) {
        rmv_ptbl_entry_by_va(proc_index, vaddr);
    }
    return pte_entry;
}

// Return void or return value of first segment? 
void unmap_page_super(unsigned int proc_index, unsigned int vaddr)
{
    rmv_pg_entry_by_va_super(proc_index, vaddr);
    return;
}








