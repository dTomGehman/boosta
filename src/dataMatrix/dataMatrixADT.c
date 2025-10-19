//This ADT to contain the unsorted data matrix type and methods for reading in data and accessing it.  
//I will probably reformat this at some point and replace each tab with two spaces.  until I change my vimrc

#include <stdio.h>
#include <stdlib.h>
#include "dataMatrixADT.h"


void read_word(FILE*fp, char*str);

struct matrix_type {
  int n_observations, n_features; //dimensions of the matrix, first line of input file.  
  				  //n_features now does not include the label.  
  char**names; //the name of each feature (column).  This is the second line of the input file
  int*labels; //the labels/categories of each observation.  this is the first column of the input file
  double**data; //the rest of the data matrix (not including labels or names)
  long*tree_positions; //binary representing the nodes (location) of an observation in a tree
};

//create a matrix from data file.  I'm pretty sure we won't have to create a matrix otherwise.  
Matrix create_from_file(char*filename){
	Matrix m = malloc(sizeof(struct matrix_type));
	if (!m) {printf("fail."); exit(1);}

	FILE *fp = fopen(filename, "r");
	if (!fp) {printf("failed to open file %s.", filename); exit(1);}
	
	fscanf(fp, "%d %d", &(m->n_observations), &(m->n_features));
	//printf("obs %d, feats %d", m->n_observations, m->n_features);
	
	// read in feature names -------------------------------------------------------
	m->names = (char**)malloc(sizeof(char*) * (m->n_features+1));
	if (!(m->names)) {printf("fail."); exit(1);}

	for (int i=0; i<m->n_features+1; i++) {
		m->names[i]=(char*)malloc(sizeof(char)*(MAX_NAME_LENGTH+1));
		if (!(m->names[i])) {printf("fail."); exit(1);}
		read_word(fp, m->names[i]);	
	}
	//verify that names were read correctly - remove this
	//for (int i=0; i<m->n_features+1; i++) {
	//	printf("%s\n", m->names[i]);
	//}
	
	//next part:  read in actual data-----------------------------------------------
	m->labels = malloc(sizeof(int) * m->n_observations);
	if (!(m->labels)) {printf("fail."); exit(1);}
	m->data = malloc(sizeof(double*) * m->n_observations);
	if (!(m->data)) {printf("fail."); exit(1);}
	
	for (int i=0; i<m->n_observations; i++){
		fscanf(fp, " %d", m->labels+i);
		m->data[i] = malloc(sizeof(double) * (m->n_features));
		if (!(m->data)) {printf("fail."); exit(1);}

		double d;
		for (int j=0; j<m->n_features; j++){
			fscanf(fp, " %lf", &m->data[i][j]);
			//printf(" %f\n", m->data[i][j]);
		}
	}
	
	//test
	//for (int i=0; i<m->n_observations; i++) printf("%d ", m->labels[i]);
	//for (int i=0; i<m->n_observations; i++) printf("\nfirst %f last %f", m->data[i][0], m->data[i][m->n_features-1]);
	//printf("\n");
	//
	
	m->tree_positions = malloc(sizeof(long) * m->n_observations);
	if (!(m->tree_positions)) {printf("fail."); exit(1);}
	for (int i=0; i<m->n_observations; i++) set_tree_pos(m, i, 0l);

	//for (int i=0; i<m->n_observations; i++) printf("%ld ", get_tree_pos(m, i));
	return m;
}

// read in a word from file
void read_word(FILE*fp, char*str){
	char c=' '; int i=0;
	while (c==' ' || c=='\n') fscanf(fp, "%c", &c);//skip leading whitespace
	while (c!=' ' && c!='\n' && i<30){//read until a space or newline is hit.  
		str[i]=c;
		i++;
		fscanf(fp, "%c", &c);
	}
	str[i]='\0';
	
}

int get_num_obs(Matrix m) {
	return m->n_observations;

}

int get_num_feats(Matrix m) {
        return m->n_features;

}

char* get_name(Matrix m, int feature){
    if (feature < -1 || feature >= m->n_features){
		printf("Invalid feature number\n");
		exit(1);
    }
    //if (feature==-1){
    //   return m->names[0];
    //}
    return m->names[feature+1];
}

int get_label(Matrix m, int observation) {
	return m->labels[observation];

}

double get_data(Matrix m, int observation, int feature) {
	return m->data[observation][feature];

}

//This sets the treepos of an observation
//This probably won't ultimately be public if only update_tree_pos uses it, but I'll leave it in the header for now.  
void set_tree_pos(Matrix m, int observation, pos_t treepos){
	m->tree_positions[observation] = treepos;
}

//Preconditions:  
//rightleft is either 0 or 1.  0 represents adding this observation to the left child of its current node, and 1 represents adding this observation to the right child of its current node
//depth is the depth of the new node, or one plus the current depth
//depth is no more than MAXDEPTH (63).  I don't think this will be a problem as xgb requires weak learners, but it is something to keep in mind.  
//
//for example, if we have just calculated the first split and intend to place a particular observation obs
//in the right node below the root, we call 
//	update_tree_pos(m, obs, 1, 1);
//if we're in the 5th layer of nodes (not including root) and want to place an observation to the left, call
//	update_tree_pos(m, obs, 0, 6);
//
//Notice that calling the function with rightleft=0 doesn't actually do anything
//that implies a few things
// - an observation that is repeatedly placed to the left will have a pos of 0.  
// - comparing the position of two observations to see if they are in the same node requires us to know the depth of the node, which I think should be the responsibility of the tree to store and not the matrix
// - placing a node to the right takes slightly more time than placing one to the left; we could use this for a tiny optimization later
void update_tree_pos(Matrix m, int observation, int rightleft, int depth){
	if (rightleft)
		set_tree_pos(m, observation, 
			((get_tree_pos(m, observation)>>(MAXDEPTH-depth))|rightleft)<<(MAXDEPTH-depth)
			);
}

pos_t get_tree_pos(Matrix m, int observation){
	return m->tree_positions[observation];
}

//compare two positions at a depth
//at depth zero, this will always return true (1): every observation is in the root node
//the first few (depth number of) bits are compared
//if we pass depth as MAXDEPTH, this function always returns if the two positions are in the same leaf node, as all bits are compared.  
//we can also compare at any other depth
//
//example:  
//observation 1 is placed left, right, right
//	its treepos is 011, or 01100000... truncated at a depth of 3
//observation 2 is placed left, right, left
//	its treepos is 010, or 01000000... truncated at a depth of 3
//let a = get_tree_pos(m, 1) and b = get_tree_pos(m, 2)
//a call is_same_node(MAXDEPTH, a, b) returns 0, as they are not in the same leaf node
//is_same_nodei(3, a, b) also returns 0, as they are not in the same 3rd layer node (which happens to be the leaf) 
//is_same_node(2, a, b) returns 1, as they are in the same 2nd layer node.
//This implies that is_same_node(n, a, b) will return 1 for any n<=2;
//calling is_same_node(0, a, b) returns 1, as they are both in the root node.  No bits are actually compared
//
//Note:  I originally had MAXDEPTH set to 64.  Even though pos_t is defined as an unsigned long, this implementation of c used an arithmetic shift, causing error.  Easiest thing to do was set it to 63, so the first bit is always 0.  whatever.  
int is_same_node(int depth, pos_t treepos1, pos_t treepos2){
	//if (!depth) return 1;
	//return ( (treepos1>>(0)) == (treepos2>>(0)) ); 
	return ( (treepos1>>(MAXDEPTH-depth)) == (treepos2>>(MAXDEPTH-depth)) ); 
}
