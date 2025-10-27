#include "../../src/dataMatrix/dataMatrixADT.h"
#include "../../src/sortedMatrix/sortedMatrixADT.h"
#include "simpleTreeADT.h"
#include <stdio.h>

int main(int argc, char**argv){
	Matrix firstA;

	printf("Tests contained in simpleTreeADT:\n\n");

	if (argc==2){
		firstA = create_from_file(argv[1]);
	} else {printf("no file??\n"); return 1;}

	splitMatrix sm = testTrainSplit(firstA, 0.8, 5);
        Matrix testM = getTestMatrix(sm);
        Matrix a = getTrainMatrix(sm);
	SortedMatrix s = create_from_matrix(a);

	printf("\nTests contained in streeClient.c\n\n");

	for (int i=0; i<get_num_feats(a); i++){
		point*parr = get_sorted_col(s, i);
		printf("Feature %d: \n", i);
		for (int j=0; j<get_num_obs(a); j++){
			printf(" Obs %d Data %lf Label %d\n", parr[j].obs_number, parr[j].datum, get_label(a, parr[j].obs_number));
		}
	}

	split_location sl = find_split(a, s, 0l, 0); //last two arguments have to do with the node to split; 
						     //that is not implemented yet, but we are just splitting at the topmost node rn
	printf("\nSplit found at\nFeature:  %d\n", sl.feature);
	printf("bound: %lf\n", sl.bound);

	update_all_tree_pos(a, sl, 0l, 0);

	printf("Observations placed in right node:  ");
	for (int i=0; i<get_num_obs(a); i++) if (get_tree_pos(a, i)==(1l<<(MAXDEPTH-1))) printf("%d ", i);
	printf("\nObservations placed in left node:  ");
	for (int i=0; i<get_num_obs(a); i++) if (get_tree_pos(a, i)==(0l)) printf("%d ", i);
	printf("\n\n");

	printf("Let's split the right side again.  \n");
	sl=find_split(a, s, (1l<<(MAXDEPTH-1)), 1); 
	
	printf("\nSplit found at\nFeature:  %d\n", sl.feature);
	printf("bound: %lf\n", sl.bound);

	update_all_tree_pos(a, sl, (1l<<(MAXDEPTH-1)), 1);
	//here I'm using a lot of terms like 0b11l<<(MAXDEPTH-2).  This is good for testing, but I'd like to make a cleaner way 
	//to do it while building the tree itself.  
	printf("Observations placed in right->right node:  ");
	for (int i=0; i<get_num_obs(a); i++) if (get_tree_pos(a, i)==(0b11l<<(MAXDEPTH-2))) printf("%d ", i);
	printf("\nObservations placed in right->left node:  ");
	for (int i=0; i<get_num_obs(a); i++) if (get_tree_pos(a, i)==(0b10l<<(MAXDEPTH-2))) printf("%d ", i);
	printf("\n\n");


	printf("Now let's split the left side from before as well.  \n");
	sl=find_split(a, s, (0), 1); 
	
	printf("\nSplit found at\nFeature:  %d\n", sl.feature);
	printf("bound: %lf\n", sl.bound);

	update_all_tree_pos(a, sl, 0, 1);

	printf("Observations placed in right->right node:  ");
	for (int i=0; i<get_num_obs(a); i++) if (get_tree_pos(a, i)==(0b01l<<(MAXDEPTH-2))) printf("%d ", i);
	printf("\nObservations placed in right->left node:  ");
	for (int i=0; i<get_num_obs(a); i++) if (get_tree_pos(a, i)==(0b00l<<(MAXDEPTH-2))) printf("%d ", i);
	printf("\n\n");

	//notice that if there is no gini gain (such as when all items in a node are already the same class), 
	//no split is found, and all of the items are placed in the left node.  However, placing something in the
	//left node doesn't really do anything.  So there
	
	//now it's time to actually make a tree.  
	for (int i=0; i<get_num_obs(a); i++) set_tree_pos(a, i, 0); //clear treepos from previous testing
	
	Tree t = create_tree(a, s);
	print_tree(t);//this doesn't really check that the tree is working well, just that it has the right structure
	for (int i=0; i<get_num_obs(testM); i++) {
	printf("Predicted label for observation %d: %d vs. Actual label: %d\n", i, predictTree(t, a, testM, i), get_label(testM, i));}
	return 0;
}
