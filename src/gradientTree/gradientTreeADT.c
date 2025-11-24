#include "gradientTreeADT.h"
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

struct node*create_node(Matrix m, SortedMatrix s, pos_t node, int node_depth, double*gradients, double*hessians, double lambda, int max_depth_param, double gamma);


split_location find_split(Matrix m, SortedMatrix s, pos_t node, int node_depth, double*gradients, double*hessians, double lambda, double gamma){ 
	//the split_location sl indicates whether a split has been found or not and where that split is.  
	//if a split is not found, sl.feature is kept as -1 by this function.  The tree will later mark sl.bound with the weight value.  
	//if a split is found, sl.feature is a nonnegative integer representing the feature where the split is found, and sl.bound represents the border of that split
	split_location sl; 
	sl.feature=-1;

	double best_gain=gamma; //the best gain found across all possible splits.  minimum is gamma
	double total_G=0, total_H=0; //total Gradient and total Hessian 
	int ct=0, ctlab=0; //count of observations in the node, count of how many of those have label '1'.  to test for purity
	
	//sum all of the gradients and hessians of the observations in this node, and also check the labels to see if the node is pure
	for (int j=0; j<get_num_obs(m); j++) if (get_tree_pos(m, j)==node) {
	       total_G+=gradients[j]; total_H+=hessians[j];
	       ct++; ctlab+=get_label(m, j);
	}
	if (ctlab==0 || ctlab==ct) return sl; //pure node.  don't split

	//exact greedy algorithm for splitting
	for (int i=0; i<get_num_feats(m); i++){ //for each feature
		point*parr=get_sorted_col(s, i);//return the already sorted feature column to traverse
						//parr contains points
		double total_Gr=0, total_Hr=0;
		double this, next; 

		for (int j=0; j<get_num_obs(m); j++) if (get_tree_pos(m, parr[j].obs_number)==node) { //for each two adjacent observations in feature column (obs-1)
			this=get_data(m, parr[j].obs_number, i);

			total_Gr+=gradients[parr[j].obs_number];
			total_Hr+=hessians[parr[j].obs_number];

			//the left-side gradients and hessians are simply the total minus the right side for each
			double total_Gl = total_G-total_Gr, total_Hl = total_H-total_Hr;

			next=this;

			for (int k=j+1; k<get_num_obs(m); k++) if (get_tree_pos(m, parr[k].obs_number)==node)
				if (this!= (next=get_data(m, parr[k].obs_number, i))) break;
			if (this==next) continue;

			double gain = (total_Gl * total_Gl) / (total_Hl + lambda)
				    + (total_Gr * total_Gr) / (total_Hr + lambda)
				    - (total_G  * total_G ) / (total_H  + lambda);

			if (gain>best_gain) { //could put other conditions here to ensure the split is good; e.g. minimum number of observations on each side of the split, etc.
				best_gain=gain;
				sl.feature=i;
				sl.bound = (this+next)/2;
			}
		}
	}
	return sl;
}


void update_all_tree_pos(Matrix m, split_location sl, pos_t node, int node_depth){ //iterate through all observations in a node and update their positions
	for (int i=0; i<get_num_obs(m); i++){
		//if an observation is in this node and is to the right of the bound, update it to be in the node's right child
		if (get_tree_pos(m, i)==node && get_data(m, i, sl.feature)>sl.bound) {//we are only concerned with a leaf, so a direct == can be used instead of is_same_node
			update_tree_pos(m, i, 1, node_depth+1);
		}
	}
}

Tree create_tree(Matrix m, SortedMatrix s, double*gradients, double*hessians, double lambda, int max_depth_param, double gamma){
	Tree t = malloc(sizeof(struct tree_type));
	if (!t) {printf("fail."); exit(1);}
	
	//create_node starts at the root node and works down until every path is terminated by a leaf
	t->root = create_node(m, s, 0, 0, gradients, hessians, lambda, max_depth_param, gamma);
	return t;
}

struct node*create_node(Matrix m, SortedMatrix s, pos_t node, int node_depth, double*gradients, double*hessians, double lambda, int max_depth_param, double gamma){
	struct node*n = malloc(sizeof(struct node));
	if (!n) {printf("fail."); exit(1);}

	n->sl.feature=-2;//arbitrarily set this to -2 to avoid undefined behavior.  sl.feature will not equal -2 by the end; it will either be -1 or a valid (nonnegative) feature number
	
	if (node_depth>=max_depth_param){//if this node is already at the max depth, it is forced to be a leaf
		n->sl.feature=-1;//-1 indicates this is a leaf node
	}
	
	//*****this would be a good spot to put other conditions that prevent overfitting.  e.g., check the node size. for the simple tree, this doesn't matter
	

	n->depth = node_depth;
	n->id = node;

	if (n->sl.feature!=-1) n->sl = find_split(m, s, node, node_depth, gradients, hessians, lambda, gamma); //as long as this is not already marked as a leaf, find a split

	if (n->sl.feature!=-1){//check sl.feature again (find_split can set it to -1 if the node is pure, for example.)
		update_all_tree_pos(m, n->sl, node, node_depth);//mark the new split

		//recursively find the child nodes
		n->right = create_node(m, s, (((node>>(MAXDEPTH-node_depth-1)) )|1 )<<(MAXDEPTH-node_depth-1), node_depth+1, gradients, hessians, lambda, max_depth_param, gamma);
		n->left = create_node(m, s, node, node_depth+1, gradients, hessians, lambda, max_depth_param, gamma);

	} else { n->right=n->left=NULL; } //if it's a leaf node, set its children to null
	return n;
}

//recursively calculate all the weights below this node
void calc_weight(struct node*n, Matrix m, double*gradients, double*hessians, double lambda){
	if (n->sl.feature==-1) { //if this is a leaf node
		double tot_G=0, tot_H=0;
		for (int i=0; i<get_num_obs(m); i++) {
			if (get_tree_pos(m, i)==n->id){
				tot_G+=gradients[i];
				tot_H+=hessians[i];
			}
		}
		//sl.bound stores the boundary of a split in a non-leaf node
		//in a leaf node, we just store the optimal weight value
		n->sl.bound = -tot_G/(tot_H+lambda); 
	} else { //recursively mark all leaves on the tree
		calc_weight(n->right, m, gradients, hessians, lambda);
		calc_weight(n->left, m, gradients, hessians, lambda);
	}

}
void fix_weights(Tree t, Matrix m, double*gradients, double*hessians, double lambda){
	calc_weight(t->root, m, gradients, hessians, lambda);
}

void print_bits(pos_t pos, int depth){ //print the bits of the id of a node.  Replace trailing zeros with *
//	pos<<=1;//delete leading zero if desired
	for (int i=0; i<=MAXDEPTH; i++){
		printf(i<=depth? (pos&(1l<<MAXDEPTH)?"1":"0") : "*"); //this is not efficient but for testing purposes whatever
		pos<<=1;
	}
}
void print_node(struct node*n){ //print out the details of a node recursively.  Leaves are the base case.  
	if (n->sl.feature==-1) {
		printf("leaf@depth %2d id ", n->depth);
		print_bits(n->id, n->depth);
		printf("                            weight %lf\n", n->sl.bound);
	} else {
		printf("node@depth %2d id ", n->depth);
		print_bits(n->id, n->depth);
		printf(" sfeat %4d sbound %lf\n", n->sl.feature, n->sl.bound);
		print_node(n->left);
		print_node(n->right);
	}
}
void print_tree(Tree t){
	print_node(t->root);
}

double predict(struct node*n, Matrix m, Matrix b, int obs){      //Matrix m is training and matrix b is testing
	//place the observation in a leaf node and return the weight at the node
	if (n->sl.feature==-1) { //if this is a leaf node
		return n->sl.bound; //return weight that's stored in the leaf node
	} else {  //otherwise, this is not a leaf node
		if (get_data(b, obs, n->sl.feature) > n->sl.bound) { //use the split bound to continue the search to either the right or left
			predict(n->right, m, b, obs);
		} else {
			predict(n->left, m, b, obs);
		}
	}

}
double  predictTree(Tree t, Matrix m, Matrix b, int obs){
	return predict(t->root, m, b, obs);
}

//there is no method to destroy the tree, much like the data/sorted matrices, because I expect the tree to persist until the end of the program.  
//we can add something to this end shortly
