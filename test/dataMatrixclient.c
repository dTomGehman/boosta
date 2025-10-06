#include <stdio.h>
#include "../src/dataMatrixADT.h"

int main(int argc, char**argv){
	if (argc==2){
		Matrix a = create_from_file(argv[1]);
	} else printf("no file??\n");
	return 0;
}

