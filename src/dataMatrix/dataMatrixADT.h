#ifndef DATAMATRIX
#define DATAMATRIX

#define MAX_NAME_LENGTH 30  //max number of characters to have in a feature name (for now)
#define MAXDEPTH 63

typedef struct matrix_type *Matrix;
typedef struct split_type *splitMatrix;

Matrix create_from_file(char*filename);
int get_num_obs(Matrix m); //get number of observations (rows)
int get_num_feats(Matrix m); //get number of features (cols)
char* get_name(Matrix m, int feature); //get the name of a feature.  I'm thinking feature=-1 could return the response name.  
int get_label(Matrix m, int observation); //get the category of an observation (row).  This will be in a separate array from the main data.  
double get_data(Matrix m, int observation, int feature); //get the data point at row, col
void destroy(Matrix m); //might not be necessary, so we can hold off on this for now. 
splitMatrix testTrainSplit(Matrix m, float splitPercentage, int seed);
Matrix getTestMatrix(splitMatrix sm);
Matrix getTrainMatrix(splitMatrix sm);

typedef unsigned long pos_t;
pos_t get_tree_pos(Matrix m, int observation); //get the treepos of an observation
void set_tree_pos(Matrix m, int observation, pos_t treepos); //set the treepos of an observation
void update_tree_pos(Matrix m, int observation, int rightleft, int depth); //update the treepos of a node at a new depth.  see comments in dataMatrixADT.c
int is_same_node(int depth, pos_t treepos1, pos_t treepos2); //compare two treepos to a certain depth.  see comments in dataMatrixADT.c
//we'll see what other functionality we need to add later.  
#endif
