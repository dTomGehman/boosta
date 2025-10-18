#include "simpleTreeADT.h"
#include <stdio.h>

double gini(int numclass, int numtotal){
	return 1 - (
		((double)numclass / numtotal) * ((double)numclass / numtotal) 
		+ (1-(double)numclass/numtotal) * (1-(double)numclass/numtotal));
}

split_location find_split(Matrix m, SortedMatrix s, long node, int node_depth){ //seems to work for first level split, but more testing needed, and I'll work on extending it to moar levels shortly.  
	split_location sl; //location of split: sl.feature (int) and sl.bound (double)
	sl.feature=-1; //-1 indicates that a split has not been found.  This might be useful for a return value
	double best_gini_gain=0; //keep track of the highest gini gain so far.  
				 //the split with the highest gini gain will be selected
				 //if no gini gain is better than 0 then no split is selected, and sl.feature will remain -1
	for (int i=0; i<get_num_feats(m); i++){ //for each feature
		point*parr=get_sorted_col(s, i);//return the already sorted feature column to traverse
		for (int j=1; j<get_num_obs(m); j++){ //for each two adjacent observations in feature column (obs-1)
			int left=0, right=0; //number of observations with a '1' label on each side of the split
			int total_left=0, total_right=0; //number observations total on each side of the split
			for (int k=0; k<j; k++){ //count how many observations are to the right, and how many of them have a '1' label
				right+=get_label(m, parr[k].obs_number);
				total_right++;
			}
			for (int k=j; k<get_num_obs(m); k++){ //left side
				left+=get_label(m, parr[k].obs_number);
				total_left++;
			}
			double gini_gain = gini(left+right, total_left+total_right) //don't really need to do this first call to gini() repeatedly
			        	- gini(left, total_left)
		       			- gini(right, total_right);//gini gain: gini of the parent node minus the gini of each child.  We greedily maximize this  
			if (gini_gain>best_gini_gain) {
				best_gini_gain=gini_gain;
				sl.feature=i;
				sl.bound = (get_data(m, parr[j].obs_number, i)
					   +get_data(m, parr[j-1].obs_number, i)) /2;
			}
		}
	}

	return sl;
}
