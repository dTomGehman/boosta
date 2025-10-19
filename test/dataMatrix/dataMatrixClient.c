#include <stdio.h>
#include "../../src/dataMatrix/dataMatrixADT.h"

int main(int argc, char**argv){
	Matrix m;
	//printf("Following tests are printed in dataMatrixADT.c and should eventually be removed:  \n\n");
	if (argc==2){
		m = create_from_file(argv[1]);
	} else {printf("no file??\n"); return 1;}

	printf("Following tests are printed in dataMatrixClient.c and can stay:  \n\n");

	printf("Number of Observations: %d\n", get_num_obs(m));
        printf("Number of Features: %d\n", get_num_feats(m));
	for (int i=0; i<get_num_obs(m); i++){printf("%ld ", get_tree_pos(m, i));}
	printf("\n");

	printf("\nLabels\n");
        for (int i=0; i<get_num_obs(m); i++) {printf("Observation %d: has label %d\n", i, get_label(m,i));}

	printf("\nData\n");
	for (int i=0; i<get_num_obs(m);i++){
		for (int j=0; j<get_num_feats(m);j++){
			printf("%f ", get_data(m,i,j));
		}
		printf("\n");
	}

	printf("\nNames\n");
	for (int i=-1; i<get_num_feats(m); i++){
		printf("Feature %d:  %s\n", i, get_name(m, i));
	}

	//tests for assigning and comparing node positions.  Hopefully this describes the concept well
	pos_t a, b;

	int depth=0;
	a=get_tree_pos(m, 2); b=get_tree_pos(m, 3); //arbitrary observations.  pos should be 0 for both yet
	printf("\na is root, \nb is root.\n");
	for (int i=0; i<=depth; i++) printf("a=b at depth %d? %d.\n", i, is_same_node(i, a, b));
	printf("a=b at leaf node?  %d.\n", is_same_node(MAXDEPTH, a, b));

	depth++;
	update_tree_pos(m, 2, 1, depth);
	update_tree_pos(m, 3, 1, depth);
	a=get_tree_pos(m, 2); b=get_tree_pos(m, 3);
	printf("\na is root->right, \nb is root->right. \n");
	for (int i=0; i<=depth; i++) printf("a=b at depth %d? %d.\n", i, is_same_node(i, a, b));
	printf("a=b at leaf node?  %d.\n", is_same_node(MAXDEPTH, a, b));

	depth++;
	update_tree_pos(m, 2, 0, depth);
	update_tree_pos(m, 3, 0, depth);
	a=get_tree_pos(m, 2); b=get_tree_pos(m, 3);
	printf("\na is root->right->left, \nb is root->right->left.\n");
	for (int i=0; i<=depth; i++) printf("a=b at depth %d? %d.\n", i, is_same_node(i, a, b));
	printf("a=b at leaf node?  %d.\n", is_same_node(MAXDEPTH, a, b));
	
	depth++;
	update_tree_pos(m, 2, 1, depth);//now a and b are no longer in the same leaf
	update_tree_pos(m, 3, 0, depth);
	a=get_tree_pos(m, 2); b=get_tree_pos(m, 3);
	printf("\na is root->right->right->right, \nb is root->right->right->left.\n");
	for (int i=0; i<=depth; i++) printf("a=b at depth %d? %d.\n", i, is_same_node(i, a, b));
	printf("a=b at leaf node?  %d.\n", is_same_node(MAXDEPTH, a, b));

	depth++;
	update_tree_pos(m, 2, 0, depth);
	update_tree_pos(m, 3, 0, depth);
	a=get_tree_pos(m, 2); b=get_tree_pos(m, 3);
	printf("\na is root->right->right->right->left, \nb is root->right->right->left->left.\n");
	for (int i=0; i<=depth; i++) printf("a=b at depth %d? %d.\n", i, is_same_node(i, a, b));
	printf("a=b at leaf node?  %d.\n", is_same_node(MAXDEPTH, a, b));

	return 0;
}

