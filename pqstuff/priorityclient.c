//priorityclient.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priorityADT.h"

int main(){
	
	Item a = newn(10, "ten", 3);
	/*Item a = malloc(sizeof(Item));
	a->value=10;
	a->info = malloc(sizeof(char)*10);
	strncpy(a->info, "bruh", 10);*/
	Item b;
	PQ q = create();

	insert(q, a);
	insert(q, new(11, "eleven"));
	insert(q, new(7, "seven"));
	insert(q, new(4, "four"));
	insert(q, new(2, "two"));
	insert(q, new(17, "seventeen"));
	insert(q, new(13, "thirteen"));
	insert(q, new(5, "five"));
	insert(q, new(5, "five"));
	insert(q, new(5, "five"));
	
	print_PQ(q);
	
	while (!is_empty(q)){
		b = extract(q);
		printf("%s\n", getname(b));
	}
	return 0;
}