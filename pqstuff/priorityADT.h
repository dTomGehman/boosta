//priorityADT.h

#ifndef PRIORITY_H
#define PRIORITY_H

typedef enum{FALSE, TRUE} bool;     //create boolean type to use for function return types
typedef struct item_type *Item;   //define a type for items to be held by the queue
typedef struct PQ_type *PQ;   //define pointer to abstract data type

Item newn(int value, char*info, int infolen); 
Item new(int value, char*info);
char*getname(Item i);
PQ create(void);                 //create newn queue and return its address
void destroy(PQ s);              //destroy the queue
void make_empty(PQ s);           //clear the contents of the queue
bool is_empty(PQ s);             //check to see if the queue is empty
bool is_full(PQ s);              //check to see if the queue is full
void insert(PQ s, Item i);       //enqueue an item onto the queue (if possible)
Item extract(PQ s);              //dequeue an item off of the queue (if possible)
void print_PQ(PQ s);


#endif