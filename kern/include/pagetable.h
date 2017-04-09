
/* Page Table (Linked List) */

typedef struct pagetable{
	int val;
//	struct pte *pte;
	struct pagetable *next;

}pagetable;

pagetable * pagetable_init(void); 

/* Print Linked List */
void print_list(pagetable *head);

/* Add To Beginning of Linked List */
void push(pagetable **head, int val);

/*Remove First Item from Linked List */
int pop(pagetable **head);

/* Remove Specific Item From Linked List */
void remove(pagetable **head, int n);
