#include <stdio.h>
#include "../src/dataMatrixADT.h"

int main(int argc, char**argv){
	Matrix a;
	if (argc==2){
		a = create_from_file(argv[1]);
	} else printf("no file??\n");
	printf("Number of Observations: %d\n", get_num_obs(a));
        printf("Number of Features: %d\n", get_num_feats(a));
	printf("Feature Name: %s\n", get_name(a, 2));
	printf("%s", get_name(a, 50));
	return 0;
}

