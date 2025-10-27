#include "simpleTreeADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct node{
	int depth; //the depth of this node.  root is 0
	pos_t id; //the position of this node.  root is 0l
	split_location sl; //the split location, containing the feature and bound.
			   //root's sl is the value returned by find_split(m, s, 0, 0); 
			   //to generalize, sl is found by find_split(m, s, id, depth);
	struct node*right; //the right child.  
				 //observations "contained" in this node whose feature sl.feature is greater than sl.bound are placed to the right.  
				 //the depth of the right node is this node's depth+1
				 //the id of the right node is this node's id with a 1 appended
	struct node*left; //the left child
				//observations "contained" in this node whose feature sl.feature is leq than sl.bound are placed to the left
				//the depth of the left node is this node's depth+1 (same as right)
				//the id of the left node is this node's id with a 0 appended (so it actually doesn't change
	
	//the observations "in" this node are not tracked here but marked in the dataMatrix by update_all_tree_pos.  
	//an observation is known to be in this node if is_same_node returns true with the observation's id and this node's id at this node's depth
};

struct tree_type {
	struct node *root;
	//not sure if we need anything else in here or not
};

struct node*create_node(Matrix m, SortedMatrix s, pos_t node, int node_depth);

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
		double last=get_data(m, parr[0].obs_number, i), this; //as the possible split locations of a particular feature i are traversed, 
								      //`last' represents the data at feature i of an observation that is on the left side of the prospective split
								      //it starts as the lowest observation in the sorted data and trails after `this'
								      //`this' is the data at feature i of observation j.  
								      //if the data has no duplicates and we are splitting the root node, `this' is always located at j-1
								      //however, we have to ignore values that are not in the same node as we move further down the tree
								      //and we have to ignore values of `last' that are equal to `this'
								      //the prospective split is located halfway between `last' and `this'
		for (int j=1; j<get_num_obs(m); j++) if (get_tree_pos(m, parr[j].obs_number)==node) { //for each two adjacent observations in feature column (obs-1)
			this=get_data(m, parr[j].obs_number, i);
			if (this==last) continue; //this line ensures that there is actually a change at the prospective split
			last=this; 
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
				sl.bound = (this+last)/2;
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

Tree create_tree(Matrix m, SortedMatrix s){
	Tree t = malloc(sizeof(struct tree_type));
	if (!t) {printf("fail."); exit(1);}

	t->root = create_node(m, s, 0, 0);
	return t;
}

struct node*create_node(Matrix m, SortedMatrix s, pos_t node, int node_depth){
	struct node*n = malloc(sizeof(struct node));
	if (!n) {printf("fail."); exit(1);}

	n->sl.feature=-2;//arbitrarily set this to -2 to avoid undefined behavior.  sl.feature will not equal -2 by the end; it will either be -1 or a valid feature number

	//if this node is already at the max depth, it is forced to be a leaf
	if (node_depth>=MAXDEPTH) n->sl.feature==-1;//-1 indicates this is a leaf node
	
	//*****this would be a good spot to put other conditions that prevent overfitting.  e.g., check the node size. for the simple tree, this doesn't matter
	

	n->depth = node_depth;
	n->id = node;

	if (n->sl.feature!=-1) n->sl = find_split(m, s, node, node_depth); //as long as this is not already marked as a leaf, find a split
	if (n->sl.feature!=-1){//check sl.feature again (find_split can set it to -1 if the node is pure, for example.  
		update_all_tree_pos(m, n->sl, node, node_depth);//mark the new split
		//recursively find the child nodes
		n->right = create_node(m, s, (((node>>(MAXDEPTH-node_depth-1)) )|1 )<<(MAXDEPTH-node_depth-1), node_depth+1);//bitwise tomfoolery
		n->left = create_node(m, s, node, node_depth+1);
	} else { n->right=n->left=NULL; } //if it's a leaf node, set its children to null
	return n;
}

void print_bits(pos_t pos, int depth){ //print the bits of the id of a node.  Replace trailing zeros with *
//	pos<<=1;//delete leading zero
	for (int i=0; i<=MAXDEPTH; i++){
		printf(i<=depth? (pos&(1l<<MAXDEPTH)?"1":"0") : "*"); //this is not efficient but for testing purposes whatever
		pos<<=1;
	}
}
void print_node(struct node*n){ //print out the details of a node recursively.  Leaves are the base case.  
	if (n->sl.feature==-1) {
		printf("leaf@depth %2d id ", n->depth);
		print_bits(n->id, n->depth);
		printf("\n");
	} else {
		printf("node@depth %2d id ", n->depth);
		print_bits(n->id, n->depth);
		printf(" sfeat %d sbound %lf\n", n->sl.feature, n->sl.bound);
		print_node(n->left);
		print_node(n->right);
	}
}
void print_tree(Tree t){
	print_node(t->root);
}
int predict(struct node*n, Matrix m, Matrix b, int obs){      //Matrix m is training and matrix b is testing
	if (n->sl.feature==-1) {
		int numObs=0;
		int label=0;
		for (int i=0; i<get_num_obs(m); i++) {
			if (get_tree_pos(m, i)==n->id){
				label+=get_label(m, i);
				numObs++;
			}
		}
		return (int)(round(label/numObs));             //Returns the majority label (0 or 1) for cases where the tree isn't fully expanded
	} else {
		if (get_data(b, obs, n->sl.feature) > n->sl.bound) {
			predict(n->right, m, b, obs);
		} else {
			predict(n->left, m, b, obs);
		}
	}

}
int predictTree(Tree t, Matrix m, Matrix b, int obs){
	return predict(t->root, m, b, obs);
}
//there is no method to destroy the tree, much like the data/sorted matrices, because I expect the tree to persist until the end of the program.  

