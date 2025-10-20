#ifndef SIMPLETREE
#define SIMPLETREE

#include "../../src/dataMatrix/dataMatrixADT.h"
#include "../../src/sortedMatrix/sortedMatrixADT.h"

typedef struct tree_type *Tree;

typedef struct {
	int feature; //feature to split on
	double bound; //value of split boundary:  all observations whose feature is greater than the boundary go to the right node (for example); the rest go to the left.  
} split_location;

split_location find_split(Matrix m, SortedMatrix s, pos_t node, int node_depth); //find the best split in the matrix at this node and return its location (struct containing which feature the split is on and the bound to split by)

void update_all_tree_pos(Matrix m, split_location sl, pos_t node, int node_depth); //repeatedly update_tree_pos for each observation found in this node depending on the new split location sl

//Next up:  we make a tree ADT and a node structure with functions to grow the tree and make predictions from it.  

#endif
