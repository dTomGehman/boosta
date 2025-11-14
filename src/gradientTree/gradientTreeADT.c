#include "gradientTreeADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define LAMBDA 1
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

struct node*create_node(Matrix m, SortedMatrix s, pos_t node, int node_depth, double*gradients, double*hessians);


split_location find_split(Matrix m, SortedMatrix s, pos_t node, int node_depth, double*gradients, double*hessians){ //seems to work for first level split, but more testing needed, and I'll work on extending it to moar levels shortly.  
	split_location sl; //location of split: sl.feature (int) and sl.bound (double)
	sl.feature=-1; //-1 indicates that a split has not been found.  This might be useful for a return value

	double best_gain=0; 
	double total_G=0, total_H=0;
	int ct=0, ctlab=0; //count of observations in the node, count of how many of those have label '1'
	for (int j=0; j<get_num_obs(m); j++) if (get_tree_pos(m, j)==node) {
	       total_G+=gradients[j]; total_H+=hessians[j];
	       ct++; ctlab+=get_label(m, j);
	}
	if (ctlab==0 || ctlab==ct) return sl; //pure node.  don't split

		
	for (int i=0; i<get_num_feats(m); i++){ //for each feature
		point*parr=get_sorted_col(s, i);//return the already sorted feature column to traverse
		double total_Gr=0, total_Hr=0;
		double this, next; 

		for (int j=0; j<get_num_obs(m); j++) if (get_tree_pos(m, parr[j].obs_number)==node) { //for each two adjacent observations in feature column (obs-1)
			this=get_data(m, parr[j].obs_number, i);
			//if (this==last) continue; //this line ensures that there is actually a change at the prospective split
			//last=this; 
			//int left=0, right=0; //number of observations with a '1' label on each side of the split
			//int total_left=0, total_right=0; //number observations total on each side of the split

			total_Gr+=gradients[parr[j].obs_number];
			total_Hr+=hessians[parr[j].obs_number];

			double total_Gl = total_G-total_Gr, total_Hl = total_H-total_Hr;

			next=this;
			//for (int k=j+1; k<get_num_obs(m); k++) if (get_tree_pos(m, parr[k].obs_number)==node)
			for (int k=j+1; k<get_num_obs(m); k++) if (get_tree_pos(m, parr[k].obs_number)==node)
				if (this!= (next=get_data(m, parr[k].obs_number, i))) break;
			if (this==next) continue;

			double gain = (total_Gl * total_Gl) / (total_Hl + LAMBDA)
				    + (total_Gr * total_Gr) / (total_Hr + LAMBDA)
				    - (total_G  * total_G ) / (total_H  + LAMBDA);

			//printf("Feat %d, gain=%lf\n", i, gain);
			//printf("  Gl=%lf Gr=%lf G=%lf\n", total_Gl, total_Gr, total_G);
			//printf("  Hl=%lf Hr=%lf H=%lf\n\n", total_Hl, total_Hr, total_H);

			if (gain>best_gain) { //could put other conditions here to ensure the split is good; e.g. minimum number of observations on each side of the split, etc.
				best_gain=gain;
				sl.feature=i;
				sl.bound = (this+next)/2;
			}
		}
	}
	//printf("Best gain:  %lf, feat: %d, bnd: %lf\n", best_gain, sl.feature, sl.bound);
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

Tree create_tree(Matrix m, SortedMatrix s, double*gradients, double*hessians){
	Tree t = malloc(sizeof(struct tree_type));
	if (!t) {printf("fail."); exit(1);}
	t->root = create_node(m, s, 0, 0, gradients, hessians);
	return t;
}

struct node*create_node(Matrix m, SortedMatrix s, pos_t node, int node_depth, double*gradients, double*hessians){
	struct node*n = malloc(sizeof(struct node));
	if (!n) {printf("fail."); exit(1);}

	n->sl.feature=-2;//arbitrarily set this to -2 to avoid undefined behavior.  sl.feature will not equal -2 by the end; it will either be -1 or a valid feature number
	//if this node is already at the max depth, it is forced to be a leaf
	int max_depth_param = 6;
	if (node_depth>=max_depth_param){
		n->sl.feature=-1;//-1 indicates this is a leaf node
		//printf("maxdepthreached");
	}
	
	//*****this would be a good spot to put other conditions that prevent overfitting.  e.g., check the node size. for the simple tree, this doesn't matter
	

	n->depth = node_depth;
	//printf("d%d f%d \n", node_depth, n->sl.feature);
	n->id = node;
	if (n->sl.feature!=-1) n->sl = find_split(m, s, node, node_depth, gradients, hessians); //as long as this is not already marked as a leaf, find a split
	if (n->sl.feature!=-1){//check sl.feature again (find_split can set it to -1 if the node is pure, for example.  
		update_all_tree_pos(m, n->sl, node, node_depth);//mark the new split
		//recursively find the child nodes
		n->right = create_node(m, s, (((node>>(MAXDEPTH-node_depth-1)) )|1 )<<(MAXDEPTH-node_depth-1), node_depth+1, gradients, hessians);//bitwise tomfoolery
		n->left = create_node(m, s, node, node_depth+1, gradients, hessians);
	} else { n->right=n->left=NULL; } //if it's a leaf node, set its children to null
	return n;
}

void calc_weight(struct node*n, Matrix m, double*gradients, double*hessians){      //Matrix m is training and matrix b is testing
	if (n->sl.feature==-1) {
		double tot_G=0, tot_H=0;
		for (int i=0; i<get_num_obs(m); i++) {
			if (get_tree_pos(m, i)==n->id){
				tot_G+=gradients[i];
				tot_H+=hessians[i];
			}
		}
		//printf("%lf %lf\n", tot_G, tot_H);
		n->sl.bound = -tot_G/(tot_H+LAMBDA); //sl.bound has no other use in a leaf node, so why not use it to store the weight value?  to save memory
	} else {
	calc_weight(n->right, m, gradients, hessians);
		calc_weight(n->left, m, gradients, hessians);
	}

}
void fix_weights(Tree t, Matrix m, double*gradients, double*hessians){
	calc_weight(t->root, m, gradients, hessians);
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
      //return the weight at a leaf node
	if (n->sl.feature==-1) { 
		return n->sl.bound; //this is the weight that's stored in the leaf node
		//int numObs=0;
		//int label=0;
		//for (int i=0; i<get_num_obs(m); i++) {
		//	if (get_tree_pos(m, i)==n->id){
		//		label+=get_label(m, i);
		//		numObs++;
		//	}
		//}
		//return (int)(round(label/numObs));             //Returns the majority label (0 or 1) for cases where the tree isn't fully expanded
	} else {
		if (get_data(b, obs, n->sl.feature) > n->sl.bound) {
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

