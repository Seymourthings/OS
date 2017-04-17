#include <pagetable.h>
#include <types.h>
#include <lib.h>
page_entry *page_table;
/*
page_entry * pagetable_init(void){
	int index;

	page_table = NULL;

	initialize page_table node fields 
	page_table = kmalloc(sizeof(page_entry));
	page_table->vpn = 0;
	page_table->pas = 0;
	page_table->permissions = 000;
	page_table->metadata[PERMISSION] = NONE;
	page_table->metadata[STATE] = MEM;
	page_table->metadata[VALID_BIT] = VALID;
	page_table->metadata[REFERENCED] = NO_RECENT_WR;
	page_table->next = NULL;

	* Per TA - only need around 18 pages per process
	 * Don't want to give each process access to all 256 entires
	 * Allocate space for 18 page entries and add them to list
	 /
	
	index = 0;
	while(index < 17){
		push(&page_table);
		index++;
	}
	return page_table;
}
*/
page_entry *destroy_pagetable(page_entry *page_table){
	while(page_table != NULL){
		page_table = pop(&page_table);
	}
	return page_table;
}

page_entry * pop(page_entry **page_table){
	page_entry *temp = NULL;
	if(*page_table == NULL){
		kprintf("Page_table is NULL");	
	}
	temp = (*page_table)->next;

	kfree(*page_table);

	*page_table = temp;
	return *page_table;
}

void print_list(page_entry *page_table){
	page_entry *current = page_table;
	if(current == NULL){
		kprintf("The list is empty good sir");
	}
	while(current != NULL){
		kprintf("%d\n", current->vpn);
		current = current->next;
	}
}

/*
void push(page_entry **page_table, vaddr_t vpn){
	page_entry *new_page_entry;
	new_page_entry = kmalloc(sizeof(page_entry)); //should be derefernced
	
	new_page_entry->vpn = vpn;
	new_page_entry->pas = 0;
	*new_page_entry->metadata[PERMISSION] = NONE;
	new_page_entry->metadata[STATE] = MEM;
	new_page_entry->metadata[VALID_BIT] = VALID;
	new_page_entry->metadata[REFERENCED] = NO_RECENT_WR;

	
	new_page_entry->next = *page_table;
	*page_table = new_page_entry;
}

*/
int push_pte(struct page_entry **pt, vaddr_t vpn){
	/* Need to do a check on the head to see if NULL */
	if(*pt == NULL){
		(*pt)->vpn = vpn;
		(*pt)->pas = alloc_upages(1);
		return 0;

	} else {
		struct page_entry *new_pte;
		new_pte = kmalloc(sizeof(*new_pte));

		if(new_pte == NULL){
			kprintf("No mem for new pte");
			return -1;
		}
	
		new_pte->vpn = vpn;
		new_pte->pas = alloc_upages(1);
		
		new_pte->next = *pt;
		*pt = new_pte;
		return 0;
	}
	kprintf("something went wrong with the push");
	return -1;
}

page_entry * find_vaddr(page_entry *page_table, vaddr_t va){
	page_entry *current = page_table;

	while(current != NULL){
		/* To be fixed when page entry gets set up with addr */
		if(current->next->vpn == va){
			return current->next;	
		}
}
	return NULL;
}

void remove(page_entry **page_table, vaddr_t va){
	page_entry *current = *page_table;
	page_entry *temp = NULL;

	while(current != NULL){
		/* To be fixed when page entry gets set up with addr */
		if(current->next->vpn == va){
			temp = current->next;
			current->next = temp->next;
			break;
		}
		current = current->next;
	}

	kfree(temp);
}

/* Return the permissions all in one int 
 * e.g read - 4, write - 2, exex - 1
 * return int 421
 */
int concat_permissions(int read, int write, int exec){
	int array[3];
	array[0] = read;
	array[1] = write;
	array[2] = exec;
	int concat = array[0];
	int index = 1;
	while(index < 3){
		concat = concat * 10;
		concat+=array[index];
		index++;
	}
	return concat;
}

int * get_permissions(int concat){
	int *array;
	array = NULL;
	int index = 0;
	while(index < 3){
		array[index] = concat % 10;
		concat = concat/10;
	}
	return array;	
}	
