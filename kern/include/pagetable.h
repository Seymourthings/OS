#include <types.h>
#include <vm.h>

/* Page Table (Linked List) */
enum page_valid_bit{
	VALID,
	INVALID,
};
enum page_referenced{
	NO_RECENT_WR,
	RECENT_WR,
};

enum page_state{
	MEM,
	DISK,
};

enum page_permissions{
	NONE,		//0
	RD, 		//1
	WR,		//2
	EX,		//3
	RD_WR,		//4
	RD_EX,		//5
	WR_EX,		//6
	ALL,		//7
};


struct page_entry *page_table; //head of the page_table linked list

/* Page Table */
struct page_entry{
	vaddr_t vpn; 	//top 20 bits
	paddr_t pas; 	//physical page location on disk
	enum page_state state;
	enum page_valid_bit valid;
	enum page_referenced activity;
	enum page_permissions permission; 
};

typedef struct pagetable_node{
	struct page_entry *page_entry;
	struct pagetable_node *next;

}pagetable_node;

pagetable_node * pagetable_init(void); 

/* Print Linked List */
void print_list(pagetable_node *head);

/* Add To Beginning of Linked List */
void push(pagetable_node **head, struct page_entry *);

/* Remove Specific Item From Linked List */
void remove(pagetable_node **head, struct page_entry *);
