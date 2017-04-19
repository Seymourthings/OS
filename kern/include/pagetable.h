#include <types.h>
#include <vm.h>

/* Permissions */
#define NONE		'0'
#define RD		'1'
#define	WR		'2'
#define	EX		'3'
#define	RD_WR		'4'
#define	RD_EX		'5'
#define	WR_EX		'6'
#define	ALL		'7'

/* Page State */
#define	MEM		'0'
#define	DISK		'1'
	
/* Page Valid Bit */
#define	VALID		'0'
#define INVALID 	'1'

/* Page Referenced */
#define	NO_RECENT_WR	'0'
#define RECENT_WR	'1'

/* Indices into Metadata ARRAY */
#define PERMISSION	'0' 
#define STATE		'1'
#define	VALID_BIT	'2'
#define REFERENCED	'3'
	
typedef struct page_entry{
	vaddr_t vpn; 	//top 20 bits
	paddr_t pas; 	//physical page location on disk
	
	/*Order goes Permission, State, Valid, Referenced */
	int permissions;
	struct page_entry *next;

}page_entry;


//page_entry * pagetable_init(void); 

/* Print Linked List */
void print_list(page_entry *head);

/* Add To Beginning of Linked List */
int push_pte(page_entry **head, vaddr_t vpn);
struct page_entry * vpn_check(vaddr_t vpn, page_entry *pt);

page_entry * find_vaddr(page_entry *, vaddr_t va);

/* Remove Specific Page Entry From Linked List
 *  (tbd what value will be passed) 
 */
void remove(page_entry **head, vaddr_t va);

int concat_permissions(int read, int write, int exec);
int * get_permissions(int concat);

page_entry * destroy_pagetable(page_entry *);

/* Pop's the head off and updates it,
 * helper function for destroy_pagetable 
 */
page_entry * pop(page_entry **head);
