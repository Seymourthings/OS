#include <pagetable.h>
#include <types.h>
#include <lib.h>
page_entry *page_table;

page_entry * pagetable_init(void){
	int index;

	page_table = NULL;

	/*initialize page_table node fields */
	page_table = kmalloc(sizeof(page_entry));
	page_table->vpn = 102;
	page_table->pas = 201;
	page_table->metadata[PERMISSION] = NONE;
	page_table->metadata[STATE] = MEM;
	page_table->metadata[VALID_BIT] = VALID;
	page_table->metadata[REFERENCED] = NO_RECENT_WR;
	page_table->next = NULL;

	/* Per TA - only need around 18 pages per process
	 * Don't want to give each process access to all 256 entires
	 * Allocate space for 18 page entries and add them to list
	 */
	
	index = 0;
	while(index < 17){
		push(&page_table);
		index++;
	}
	return page_table;
}

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

void push(page_entry **page_table){
	page_entry *new_page_entry;
	new_page_entry = kmalloc(sizeof(page_entry));
	
	new_page_entry->vpn = 101;
	new_page_entry->pas = 201;
	new_page_entry->metadata[PERMISSION] = NONE;
	new_page_entry->metadata[STATE] = MEM;
	new_page_entry->metadata[VALID_BIT] = VALID;
	new_page_entry->metadata[REFERENCED] = NO_RECENT_WR;

	
	new_page_entry->next = *page_table;
	*page_table = new_page_entry;
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
