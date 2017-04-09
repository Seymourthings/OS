#include <pagetable.h>
#include <types.h>
#include <kern/errno.h>
#include <spl.h>
#include <proc.h>
#include <current.h>
#include <addrspace.h>
#include <vnode.h>
#include <kern/unistd.h>
#include <proc_syscall.h>
#include <kern/wait.h>
#include <lib.h>
#include <copyinout.h>
#include <kern/fcntl.h>
#include <vfs.h>
#include <syscall.h>

pagetable * pagetable_init(void){
	pagetable *head = NULL;
	head = kmalloc(sizeof(pagetable));
	head->val = 1;
	head->next = NULL;
	return head;
}


void print_list(pagetable *head){
	pagetable *current = head;
	
	while(current != NULL){
		kprintf("%d\n", current->val);
		current = current->next;
		
	}
	
}

void push(pagetable **head, int val){
	pagetable *new_pagetable;
	new_pagetable = kmalloc(sizeof(pagetable));


	new_pagetable->val = val;
	new_pagetable->next = *head;
	*head = new_pagetable;
}

int pop(pagetable **head){
	int retval = -1;
	pagetable *next = NULL;

	if(*head == NULL){
		return -1;
	}

	next = (*head)->next;
	retval = (*head)->val;
	kfree(*head);
	*head = next;
	
	return retval;
}

void remove(pagetable **head, int n){

	pagetable *current = *head;
	pagetable *temp = NULL;

	while(current != NULL){
		if(current->next->val == n){
			temp = current->next;
			current->next = temp->next;
			break;
		}
		current = current->next;
	}
	
	kfree(temp);

}
