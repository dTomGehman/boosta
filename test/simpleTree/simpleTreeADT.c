#include "simpleTreeADT.h"
#include <stdio.h>

double gini(int numclass, int numtotal){ //gini calculation
	return 1 - (
		((double)numclass / numtotal) * ((double)numclass / numtotal) 
		+ (1-(double)numclass/numtotal) * (1-(double)numclass/numtotal));
}

split_location find_split(Matrix m, SortedMatrix s, pos_t node, int node_depth){ //seems to work for first level split, but more testing needed, and I'll work on extending it to moar levels shortly.  
	split_location sl; //location of split: sl.feature (int) and sl.bound (double)
	sl.feature=-1; //-1 indicates that a split has not been found.  This might be useful for a return value
	double best_gini_gain=0; //keep track of the highest gini gain so far.  
				 //the split with the highest gini gain will be selected
				 //if no gini gain is better than 0, then no split is selected, and sl.feature will remain -1
				 //this value could be parameterized to adjust for overfitting
	for (int i=0; i<get_num_feats(m); i++){ //for each feature
		point*parr=get_sorted_col(s, i);//return the already sorted feature column to traverse
		int last=0, this=0; //last keeps track of each previous value of j (observation) that is also contained in the same node.  It is used in the sl.bound calculation
		for (int j=1; j<get_num_obs(m); j++) if (get_tree_pos(m, parr[j].obs_number)==node) { //for each two adjacent observations in feature column (obs-1)
			last=this; this=j;
			int left=0, right=0; //number of observations with a '1' label on each side of the split
			int total_left=0, total_right=0; //number observations total on each side of the split
			for (int k=0; k<j; k++) if (get_tree_pos(m, parr[k].obs_number)==node){ //count how many observations are to the right, and how many of them have a '1' label
				right+=get_label(m, parr[k].obs_number);
				total_right++;
			}
			for (int k=j; k<get_num_obs(m); k++) if  (get_tree_pos(m, parr[k].obs_number)==node) {//left side
				left+=get_label(m, parr[k].obs_number);
				total_left++;
			}
			double gini_gain = (total_left+total_right) * gini(left+right, total_left+total_right) //don't really need to do this first call to gini() repeatedly
			        	- (total_left) * gini(left, total_left)
		       			- (total_right) * gini(right, total_right);//weighted gini gain: gini of the parent node minus the gini of each child.  We greedily maximize this  
										   //this calculation could be simplified to run faster, but it reads more clearly
			//printf("Feat %d, gain=%lf\n", i, gini_gain);
			if (gini_gain>best_gini_gain) { //could put other conditions here to ensure the split is good; e.g. minimum number of nodes on each side of the split, etc.
				best_gini_gain=gini_gain;
				sl.feature=i;
				sl.bound = (get_data(m, parr[j].obs_number, i)  //set the bound at the mean of the current observation and previous observation in this node
					   +get_data(m, parr[last].obs_number, i)) /2; //previously this line used j-1 instead of last, which works but becomes less ideal as we
										       //get further down from the root node.  It took the average of the current observation and 
										       //the adjacent one in the root node; now it takes the average of the current observation
										       //and the adjacent one in the current node.  
			}
		}
	}

	return sl;
}


void update_all_tree_pos(Matrix m, split_location sl, pos_t node, int node_depth){ //iterate through all observations in a node and update their positions
	for (int i=0; i<get_num_obs(m); i++){
		if (get_tree_pos(m, i)==node && get_data(m, i, sl.feature)>sl.bound) {//we are only concerned with a leaf, so a direct == can be used instead of is_same_node
										      //is_same_node will really come in handy with the tree's predict function
			update_tree_pos(m, i, 1, node_depth+1);
		}
	}
}

