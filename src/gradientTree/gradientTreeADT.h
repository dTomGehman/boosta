#ifndef GRADIENTTREE
#define GRADIENTTREE

#include "../../src/dataMatrix/dataMatrixADT.h"
#include "../../src/sortedMatrix/sortedMatrixADT.h"
#include "../../src/booster/boosterADT.h"

typedef struct tree_type *Tree;

typedef struct {
	int feature; //feature to split on
	double bound; //value of split boundary:  all observations whose feature is greater than the boundary go to the right node (for example); the rest go to the left.  
} split_location;

//find the best split in the matrix at this node and return its location (struct containing which feature the split is on and the bound to split by)
split_location find_split(Matrix m, SortedMatrix s, pos_t node, int node_depth, double*gradients, double*hessians, param_t*params); 

//repeatedly update_tree_pos for each observation found in this node depending on the new split location sl
void update_all_tree_pos(Matrix m, split_location sl, pos_t node, int node_depth); 

//create a tree trained on a dataMatrix, its respective sortedMatrix, and the gradients and hessians of each observation
Tree create_tree(Matrix m, SortedMatrix s, double*gradients, double*hessians, param_t*params);

//calculate the weight at each leaf node in the tree
void fix_weights(Tree t, Matrix m, double*gradients, double*hessians, param_t*params);

void print_tree(Tree t); //print out the tree structure with its split boundaries and leaf weights

//Make a prediction:  use the tree to place the observation in a leaf node, and return the leaf's weight
double predictTree(Tree t, Matrix m, Matrix b, int obs); //Matrix m is the training matrix; Matrix b is the testing matrix.  

#endif
