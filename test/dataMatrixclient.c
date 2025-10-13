#include <stdio.h>
#include "../src/dataMatrix/dataMatrixADT.h"

int main(int argc, char**argv){
	Matrix a;
	printf("Following tests are printed in dataMatrixADT.c and should eventually be removed:  \n\n");

	if (argc==2){
		a = create_from_file(argv[1]);
	} else {printf("no file??\n"); return 1;}

	printf("Following tests are printed in dataMatrixClient.c and can stay:  \n\n");

	printf("Number of Observations: %d\n", get_num_obs(a));
        printf("Number of Features: %d\n", get_num_feats(a));
	for (int i=0; i<get_num_obs(a); i++){printf("%ld ", get_tree_pos(a, i));}
	printf("\n");

	printf("\nLabels\n");
        for (int i=0; i<get_num_obs(a); i++) {printf("Observation %d: has label %d\n", i, get_label(a,i));}

	printf("\nData\n");
	for (int i=0; i<get_num_obs(a);i++){
		for (int j=0; j<get_num_feats(a);j++){
			printf("%f ", get_data(a,i,j));
		}
		printf("\n");
	}

	printf("\nNames\n");
	for (int i=-1; i<=get_num_feats(a); i++){
		printf("Feature %d:  %s\n", i, get_name(a, i));
	} //invalid feature number expected for i=get_num_feats(a).  
	return 0;
}

