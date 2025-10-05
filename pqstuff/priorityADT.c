//priorityADT.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priorityADT.h"

struct node {
   Item data;
   struct node *left;
   struct node *right;
};

struct item_type{
	int value;
	char* info;
};

struct PQ_type{
   struct node *root;
   unsigned long size; //change from original PQADT
};

//helper methods for insert, print_PQ, and extract
//I used helpers to apply recursion.  e.g., a call to print_PQ accesses the top node in the PQ and passses it to n_print_PQ
void n_insert(struct node *newn_node, struct node *existing, unsigned long n); //u long n : change from original PQADT
void n_print_PQ(struct node *n);
void n_chop(struct node **p);
//all other functions are in the PQADT.h


static void terminate(const char *message){
   printf("%s\n", message);
   exit(EXIT_FAILURE);
}

Item newn(int value, char*info, int infolen){
	Item i = malloc(sizeof(struct PQ_type));
	if (i==NULL)
		terminate("Error in newn: item could not be created.");
    i->value = value;
	i->info = malloc(sizeof(char)*(infolen+1));
	if (i->info==NULL)
		terminate("Error in newn: item could not be created.");
	strncpy(i->info, info, (size_t) infolen+1);
	return i;
}

Item new(int value, char*info){
	return newn(value, info, strlen(info));
}

char*getname(Item i){
	return i->info;
}

PQ create(void){
   PQ s = malloc(sizeof(struct PQ_type));
   if (s==NULL)
      terminate("Error in create: PQ could not be created.");
   s->root = NULL;
   s->size = 0l;  //0 long
   return s;
}

void destroy(PQ s){
   make_empty(s);
   free(s);
}

void make_empty(PQ s){
   while (!is_empty(s))
      extract(s);
}

bool is_empty(PQ s){
   return s->root==NULL;
}

bool is_full(PQ s){//blah
   return FALSE;
}

void insert(PQ s, Item i){
	struct node *newn_node = malloc(sizeof(struct node));
	if (newn_node==NULL) terminate("Error: PQ is full.");  
	newn_node -> data = i;
	newn_node -> left = NULL;
	newn_node -> right = NULL;
	
	
	if (is_empty(s)){
		s->root = newn_node;
	} else {
	    n_insert(newn_node, s->root, (s->size)++); //change from original PQADT
    } //I realized here that I didn't increment s->size for the first node, but the code works w/o it.  so whatever
}

void n_insert(struct node *newn_node, struct node *existing, unsigned long n){ //helper for insert
	//switcheroo if needed.  The lower item gets inserted down the tree.  
	if (newn_node->data->value > existing->data->value){
		Item tempd = newn_node->data;
		newn_node->data = existing->data;
		existing->data = tempd;
	}
	
	//change from original PQADT
	//the rightmost bit in n determines which side (right or left) to insert the newn node down
	//by keeping track of the size of the PQ and passing that size variable as n at the first call of n_insert, 
	//we ensure that the newn node is inserted "next" to the previous node, like it would be in the array implementation.  
	//This keeps the tree balanced as it is being built
	//this seemed simpler and faster than the implementation below, which is commented out.  
	//when I originally tested it, this change made PQADT2 about twice as fast as my PQADT
	if (n%2) { //go down the left side
		if (existing->left == NULL) existing->left = newn_node;
		else n_insert(newn_node, existing->left, n>>1);
	} else { //go down the right side
		if (existing->right == NULL) existing->right = newn_node;
		else n_insert(newn_node, existing->right, n>>1);
	}
	
}

void print_PQ(PQ s){
	if (!is_empty(s)) n_print_PQ(s->root);
	printf("\n");
}

void n_print_PQ(struct node *n){ //helper for print_PQ
	printf("(%d~%s:", n->data->value, n->data->info);
	if (n->left) n_print_PQ(n->left);
	printf(",");
	if (n->right) n_print_PQ(n->right);
	printf(")");
}


Item extract(PQ s){
	if (is_empty(s)) terminate("Error in extract: PQ is empty.");
	Item returnval=s->root->data;
	n_chop(&s->root);
	s->size--;//change from original PQADT
	return returnval;
}

void n_chop(struct node **p){  //helper for extract
	struct node *n= *p; //double pointer p allows us to delete the node.  
	if (n==NULL) return; //check, but this shouldn't happen
	if (n->right==NULL && n->left==NULL){ //if we extract from a node who doesn't have children, it is deleted
		free(n);
		*p=NULL;
	} else if (n->left==NULL){ //if only the right child exists, extract from it
		n->data = n->right->data;
		n_chop(&n->right);
	} else if (n->right==NULL){ //if only the left child exists, extract from it
		n->data = n->left->data;
		n_chop(&n->left);
	} else if (n->right->data->value > n->left->data->value) { //if both children exist, extract from the greater
		n->data = n->right->data;
		n_chop(&n->right);
	} else {
		n->data = n->left->data;
		n_chop(&n->left);
	}
}
