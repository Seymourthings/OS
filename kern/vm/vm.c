#include <types.h>
#include <lib.h>
#include <vm.h>
#include <spinlock.h>
#include <mainbus.h>
#include <pagetable.h>
#include <proc.h>
#include <kern/errno.h>
#include <current.h>
#include <addrspace.h>
#include <mips/tlb.h>
#include <spl.h>

struct coremap_entry *coremap;
int NUM_ENTRIES;
struct spinlock coremap_spinlock = SPINLOCK_INITIALIZER;
int bytes_left;

/* VM functions */

void vm_bootstrap()
{
    /* implement*/
}

/* We need to make alloc_upages and free_upages functions */

/* Allocate/free some kernel-space virtual pages */
paddr_t
get_ppages(unsigned npages){
	
	paddr_t pa;
	// Critical section. Protect the coremap
	spinlock_acquire(&coremap_spinlock);
	
	if((int)(npages*PAGE_SIZE) > bytes_left || npages==0){
        	pa = 0;
		spinlock_release(&coremap_spinlock);
        	return pa;
	}
	for(int i=0; i<NUM_ENTRIES; i++) {
		if(coremap[i].pg_state == PAGE_FREE && coremap[i].use_state == REUSE){
		unsigned cont;
		for(cont = 0; cont<npages; cont++){
			if(coremap[i+cont].pg_state != PAGE_FREE || coremap[i+cont].use_state != REUSE){
			i+=cont;
			break;
			}
		}
		if(cont==npages){
			pa = coremap[i].pas;
			for(unsigned n=0; n<npages; n++){
				if(n==0){
					coremap[i+n].blk_state = BLOCK_PARENT;
				}
				else{
					coremap[i+n].blk_state = BLOCK_CHILD;
				}
				coremap[i+n].block_size = npages;
				coremap[i+n].pg_state = PAGE_FIXED;
				coremap[i+n].use_state = REUSE;
				}
			// Update bytes_left
			bytes_left -= (npages*PAGE_SIZE);
			spinlock_release(&coremap_spinlock);
			return pa;
			}
		}
	}
	pa = 0;
	spinlock_release(&coremap_spinlock);
	return pa;

}

vaddr_t
alloc_kpages(unsigned npages)
{
	paddr_t pa;
	pa = get_ppages(npages);
	return PADDR_TO_KVADDR(pa);
}

paddr_t 
alloc_upages(unsigned npages){
	paddr_t pa = get_ppages(npages);	
	return pa;
}

void free_pages(unsigned int addr, int index){	
	int n = coremap[index].block_size;
	int npages = coremap[index].block_size;
	while(n>0){
		coremap[index].as = NULL;
		coremap[index].block_size = 0;
		coremap[index].pg_state = PAGE_FREE;
		coremap[index].blk_state = BLOCK_CHILD;
		n--;
		index++;
	}
	if(n==0){
		bytes_left += (npages*PAGE_SIZE);
	}
	(void)addr;
}

void
free_kpages(vaddr_t addr)
{

	// Critical section. Protect the coremap
	spinlock_acquire(&coremap_spinlock);
   
	//NEW STUFF
	int index = 0;
	for(; index<NUM_ENTRIES; index++){
		if(addr == coremap[index].vas){
			if(coremap[index].blk_state == BLOCK_CHILD || 
				coremap[index].use_state == NO_REUSE){
        			break;
			}

			free_pages(addr,index);
		}
	}
	spinlock_release(&coremap_spinlock);
	(void)addr;
}

void
free_upages(paddr_t addr)
{

	// Critical section. Protect the coremap
	spinlock_acquire(&coremap_spinlock);
   
	//NEW STUFF
	int index = 0;
	for(; index<NUM_ENTRIES; index++){
		if(addr == coremap[index].pas){
			if(coremap[index].blk_state == BLOCK_CHILD || 
				coremap[index].use_state == NO_REUSE){
        			break;
			}
			free_pages(addr,index);
		}
	}
	spinlock_release(&coremap_spinlock);
	(void)addr;
}


unsigned
int
coremap_used_bytes() {
    size_t ramsize;
    ramsize = mainbus_ramsize();
    return ramsize - bytes_left;
}

void
vm_tlbshootdown(const struct tlbshootdown *ts)
{
	(void)ts;
	/* Implement */
}

/* Acts as a boolean */
bool region_check(vaddr_t faultaddress, struct region *region){
	struct region *temp;
	temp = region;
	while(temp != NULL){
		/* is the address within range */
		if(faultaddress >= temp->as_vbase && faultaddress <= temp->as_vend){
			return true;
		}
		temp = temp->next;
	}
	temp = NULL;
	return false;
}


int valid_address(vaddr_t faultaddress, struct addrspace *as){
	/* Checks it falls in code and text addr */
	bool valid = false;
	valid = region_check(faultaddress, as->region_table);
	if(!valid){
		/* If not in code or text check the stack */
		valid = region_check(faultaddress, as->stack_region);
		if(!valid){
			/* If not in stack, check the heap */
			valid = region_check(faultaddress, as->heap_region);
			/* If it gets here, it's not valid*/
			if(!valid){
				//kprintf("Address could not be found in any region");
				return EFAULT;	
			}
			return 0;
		}
		return 0;
	}
	return 0;
}


struct page_entry * vpn_check(vaddr_t vpn, struct page_entry *pt){
	struct page_entry *temp;
	temp = pt;
	while(temp != NULL){
		/* is the address within range */
		if(temp->vpn == vpn){
			return temp;
		}
		temp = temp->next;
	}
	temp = NULL;
	return temp;
}


int
vm_fault(int faulttype, vaddr_t faultaddress){
	(void)faulttype;
	int err;
	
	if (curproc == NULL) {
		/*
		 * No process. This is probably a kernel fault early
		 * in boot. Return EFAULT so as to panic instead of
		 * getting into an infinite faulting loop.
		 */
		return EFAULT;
	}

	struct addrspace *as = proc_getas();
	if (as == NULL) {
		/*
		 * No address space set up. This is probably also a
		 * kernel fault early in boot.
		 */
		return EFAULT;
	}

	err = valid_address(faultaddress, as);
	if(err){
		return err; //check valid_addr return value
	}
	//kprintf("It does get here");	
	
	//Vaild address, mask vaddr w/ PAGE_FRAME to get vpn to check for PTE inpage_table linked list.
	
	vaddr_t vpn = faultaddress & PAGE_FRAME;
	struct page_entry *pg_entry;
	pg_entry = vpn_check(vpn, as->page_table);
	if(pg_entry == NULL){
		err = push_pte(&(as->page_table), vpn);
		if(err){
			return err;
		} 
		//else
		//write new pte to TLB
			
	}
	
        /* Disable interrupts on this CPU while frobbing the TLB. */

	uint32_t ehi, elo;
	int spl/*, tlb_return*/;
	paddr_t tlb_pas;

        spl = splhigh();
	faultaddress &= PAGE_FRAME;
	ehi = faultaddress;
	if(pg_entry == NULL){
		tlb_pas = as->page_table->pas; //what was pushed on	
	} else{
		tlb_pas = pg_entry->pas; //vpn check returned a page_entry
	}
	elo = tlb_pas | TLBLO_DIRTY | TLBLO_VALID;
/*	tlb_return = tlb_probe(ehi,elo);
	if(tlb_return < 0){
		kprintf("ELO : %d",elo);
	}*/
	tlb_random(ehi, elo);
	

	DEBUG(DB_VM, "dumbvm: 0x%x -> 0x%x\n", faultaddress,as->page_table->pas);

        splx(spl);
		
	return 0;
}

void 
init_coremap(size_t ramsize, paddr_t firstpaddr){
	bytes_left = ramsize;
	NUM_ENTRIES = ramsize / PAGE_SIZE; // 256 entries - pgs are 4K
	
	//How many pages do our coremap_spinlock & coremap take?
	size_t spinlock_pgs;
	spinlock_pgs = (sizeof(struct spinlock)+PAGE_SIZE-1) / PAGE_SIZE; // Round ^UP^
	size_t coremap_pgs;
	coremap_pgs = ((sizeof(struct coremap_entry)*NUM_ENTRIES)+PAGE_SIZE-1)/ PAGE_SIZE; // Round UP

	// firstpaddr should be moved up beyond coremap & spinlock
	firstpaddr += spinlock_pgs*PAGE_SIZE;
	coremap = (struct coremap_entry *)PADDR_TO_KVADDR(firstpaddr);
    
	// Mem addr for cmap entries (had to move beyond spinlock)
	paddr_t entrypaddr;
	entrypaddr = firstpaddr; // Both beyond kernel & spinlock

	// How many pages are already used?
	firstpaddr += coremap_pgs*PAGE_SIZE;//Next allocatable addr
	int pgs_used;
	pgs_used = firstpaddr/PAGE_SIZE; 

	//  THE pas AND vas MUST BE INCREMENTED BY PAGE_SIZE
	// Initialize all coremap entries
	paddr_t pg_paddr;
	pg_paddr = 0;

	for (int i = 0; i<NUM_ENTRIES; i++){
		coremap[i].entry_pas = entrypaddr;
		coremap[i].as = NULL;
		coremap[i].pas = pg_paddr;
		//kprintf("Coremap[index].pas: %d\n", coremap[i].pas);
		coremap[i].vas = PADDR_TO_KVADDR(pg_paddr);
		coremap[i].block_size = 0;
		coremap[i].pg_state = PAGE_FREE;
		coremap[i].blk_state = BLOCK_CHILD;
		coremap[i].use_state = REUSE;
		//These are pgs already used. Must not be recycled EVER
		if (i<pgs_used){
			coremap[i].pg_state = PAGE_FIXED;
			coremap[i].use_state = NO_REUSE;
		}
		pg_paddr += PAGE_SIZE;
		entrypaddr += sizeof(struct coremap_entry);
	}

	firstpaddr = coremap[pgs_used].pas;

    // Update the amount of available bytes left in coremap
	bytes_left -= firstpaddr;

	spinlock_release(&coremap_spinlock);
}
