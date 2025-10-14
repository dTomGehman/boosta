#include "simpleTreeADT.h"
#include <stdio.h>

double gini(int numclass, int numtotal){
	return 1 - (
		((double)numclass / numtotal) * ((double)numclass / numtotal) 
		+ (1-(double)numclass/numtotal) * (1-(double)numclass/numtotal));
}

split_location find_split(Matrix m, SortedMatrix s, long node, int node_depth){
	split_location sl;
	double best_matrix_gini=1; //worst gini is 1.  Keep track of the best gini.  
	for (int i=0; i<get_num_feats(m); i++){ //for each feature
		double best_feat_gini=1;
		int best_feat_split;
		point*parr=get_sorted_col(s, i);
		for (int j=1; j<get_num_obs(m); j++){ //for each two adjacent observations in feature column (obs-1)
			int left=0, right=0; //number of '1' labels on each side of the split
			int total_left=0, total_right=0; //number total on each side of the split
			for (int k=0; k<j; k++){
				right+=get_label(m, parr[k].obs_number);
				total_right++;
			}
			for (int k=j; k<get_num_obs(m); k++){
				left+=get_label(m, parr[k].obs_number);
				total_left++;
			}
			double gini_gain = gini(left+right, total_left+total_right) 
			        	- gini(left, total_left)
		       			- gini(right, total_right);	       
		}
	}

	//not done yet
	return sl;
}
