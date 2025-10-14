#include "../../src/dataMatrix/dataMatrixADT.h"
#include "../../src/sortedMatrix/sortedMatrixADT.h"
#include <stdio.h>

int main(int argc, char**argv){
	Matrix a;

	printf("Tests contained in sortedMatrixADT:\n\n");

	if (argc==2){
		a = create_from_file(argv[1]);
	} else {printf("no file??\n"); return 1;}


	SortedMatrix s = create_from_matrix(a);

	printf("\nTests contained in sortedClient\n\n");

	for (int i=0; i<get_num_feats(a); i++){
		point*parr = get_sorted_col(s, i);
		printf("Feature %d: \n", i);
		for (int j=0; j<get_num_obs(a); j++){
			printf(" Obs %d Data %lf\n", parr[j].obs_number, parr[j].datum);
		}
	}

	return 0;
}
