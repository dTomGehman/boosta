#include "../src/dataMatrixADT.h"
#include "../src/sortedMatrixADT.h"
#include <stdio.h>

int main(int argc, char**argv){
	Matrix a;
	if (argc==2){
		a = create_from_file(argv[1]);
	} else {printf("no file??\n"); return 1;}


	SortedMatrix s = create_from_matrix(a);

	return 0;
}
