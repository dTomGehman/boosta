#include <stdio.h>
#include <stdlib.h>
#include "dataMatrixADT.h"
#include "sortedMatrixADT.h"
#include "pointPriorityADT.h"

/*
typedef struct {        //struct to hold a point in the sorted array.  Might move this to the header file depending on how we use it.  
		        //This point contains its (double) value as well as other fields to refer back to which observation its from, 
		        //as the order of each feature colunm will be sorted.  
	double datum;   //the value of the data, corresponding to some point (obs, feat) in the unsorted matrix
	int obs_number; //which observation the datum is from, as a number. obs as above^
	
	//the above two fields are essential.  Anything below is to hopefully save time as we build the tree to avoid searches and lookups
	
	//int label;      //(might not be necessary) equivalent to the observation's label.  
		        //This might save a small amount of time to copy it here instead of constantly referring back to the original matrix.  
	//long tree_pos;  //binary representing the datum's position in the tree (instance set) as the tree is being created
} point;*/

struct sorted_type{ 
	Matrix m; //parent matrix
	int n_observations, n_features; //might delete
	point**matrix;  //2d array of points.  This is equivalent to transposing the unsorted matrix and sorting each row (feature)
};


SortedMatrix create_from_matrix(Matrix m){
	SortedMatrix sm = malloc(sizeof(struct sorted_type));
	if (!sm) {printf("fail. "); exit(1);}

	sm->m = m;
	
	sm->matrix = malloc(sizeof(point*) * get_num_feats(m));
	if (!sm->matrix) {printf("fail. "); exit(1);}

	for (int i=0; i<get_num_feats(m); i++){
		sm->matrix[i] = malloc(sizeof(point) * get_num_obs(m));
		if (!sm->matrix[i]) {printf("fail. "); exit(1);}
	}


	return sm;
}
