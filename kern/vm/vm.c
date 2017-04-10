#include <types.h>
#include <lib.h>
#include <vm.h>
#include <spinlock.h>
#include <mainbus.h>
#include <pagetable.h>
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
vaddr_t
alloc_kpages(unsigned npages)
{

	
    // Critical section. Protect the coremap
    spinlock_acquire(&coremap_spinlock); 

    vaddr_t va; // What were returning
    //loop through coremap and find n contiguous free pages

    // Theres not enough free pages to allocate
    if((int)(npages*PAGE_SIZE) > bytes_left || npages==0){
        va = 0;
        spinlock_release(&coremap_spinlock);
        return va;
    }

    for(int i=0; i<NUM_ENTRIES; i++) {
        if(coremap[i].pg_state == PAGE_FREE && coremap[i].use_state == REUSE){
            unsigned cont = 0;
            for(; cont<npages; cont++){
                if(coremap[i+cont].pg_state != PAGE_FREE || coremap[i+cont].use_state != REUSE){
                    i+=cont;
                    break;
                }
            }
            if(cont==npages){
                va = coremap[i].vas;
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
                return va;
            }
        }
    }
    va = 0;
    spinlock_release(&coremap_spinlock);
    return va;
}


void
free_kpages(vaddr_t addr)
{

    // Critical section. Protect the coremap
    spinlock_acquire(&coremap_spinlock);
   
    //NEW STUFF
    int i = 0;
    for(; i<NUM_ENTRIES; i++){
        if(addr == coremap[i].vas){
            if(coremap[i].blk_state==BLOCK_CHILD || coremap[i].use_state==NO_REUSE){
                break; //Fail
            }
            int n = coremap[i].block_size;
            int npages = coremap[i].block_size;
            while(n>0){
                coremap[i].as = NULL;
                coremap[i].block_size = 0;
                coremap[i].pg_state = PAGE_FREE;
                coremap[i].blk_state = BLOCK_CHILD;
                n--;
                i++;
            }
            if(n==0){
                bytes_left += (npages*PAGE_SIZE);
            }
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

int
vm_fault(int faulttype, vaddr_t faultaddress){
	(void)faulttype;
	(void)faultaddress;
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
