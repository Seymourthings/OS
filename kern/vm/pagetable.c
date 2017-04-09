#include <pagetable.h>
#include <types.h>
#include <lib.h>

pagetable_node * pagetable_init(void){
	int index;

	pagetable_node *head = NULL;

	/*initialize head node fields */
	head = kmalloc(sizeof(pagetable_node));
	head->page_entry = kmalloc(sizeof(struct page_entry));	
	head->page_entry->vpn = 101;
	head->page_entry->pas = 201;
	head->page_entry->state = MEM;
	head->page_entry->permission = NONE;
	head->page_entry->activity = NO_RECENT_WR;
	head->next = NULL;

	/* 1 to 1 mapping of virtual addresses to physical
	 * NUM_ENTRIES = 256
	 * Allocate space for 256 page entries and add them to list
	 */
	
	index = 0;
	while(index < NUM_ENTRIES){
		struct page_entry *page_entry;
		page_entry = kmalloc(sizeof(*page_entry));
		push(&head, page_entry);
		index++;
	}
	return head;
}


void print_list(pagetable_node *head){
	pagetable_node *current = head;

	//getting stuck in this print for some reason	
	while(current != NULL){
		kprintf("%d\n", current->page_entry->vpn);
		current = current->next;
		
	}
}

void push(pagetable_node **head, struct page_entry *page_entry){
//	(void)page_entry;
	pagetable_node *new_pagetable_node;
	new_pagetable_node = kmalloc(sizeof(pagetable_node));
	
	new_pagetable_node->page_entry = kmalloc(sizeof(*page_entry));
	new_pagetable_node->page_entry = page_entry;
	new_pagetable_node->page_entry->vpn = 101;
	new_pagetable_node->page_entry->pas = 201;
	new_pagetable_node->page_entry->state = MEM;
	new_pagetable_node->page_entry->activity = NO_RECENT_WR;
	new_pagetable_node->page_entry->permission = NONE;
	
	new_pagetable_node->next = *head;
	*head = new_pagetable_node;
}

void remove(pagetable_node **head, struct page_entry *page_entry){

	pagetable_node *current = *head;
	pagetable_node *temp = NULL;

	while(current != NULL){
		/* To be fixed when page entry gets set up with addr */
		/*if(current->next->page_entry == n){
			temp = current->next;
			current->next = temp->next;
			break;
		}*/
		current = current->next;
	}
	(void)page_entry;	
	kfree(temp);
}
