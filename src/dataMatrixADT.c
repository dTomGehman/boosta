//This ADT to contain the unsorted data matrix type and methods for reading in data and accessing it.  
//I will probably reformat this at some point and replace each tab with two spaces.  until I change my vimrc

#include <stdio.h>
#include <stdlib.h>
#include "dataMatrixADT.h"


void read_word(FILE*fp, char*str);

struct matrix_type {
  int n_observations, n_features; //dimensions of the matrix, first line of input file.  
				  //Not sure yet if n_features will ultmately include the labels, but for now it does.  
  char**names; //the name of each feature (column).  This is the second line of the input file
  int*labels; //the labels/categories of each observation.  this is the first column of the input file
  double**data; //the rest of the data matrix (not including labels or names)
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
	m->names = (char**)malloc(sizeof(char*) * m->n_features);
	if (!(m->names)) {printf("fail."); exit(1);}

	for (int i=0; i<m->n_features; i++) {
		m->names[i]=(char*)malloc(sizeof(char)*(MAX_NAME_LENGTH+1));
		if (!(m->names[i])) {printf("fail."); exit(1);}
		read_word(fp, m->names[i]);	
	}
	//verify that names were read correctly - remove this
	for (int i=0; i<m->n_features; i++) {
		printf("%s\n", m->names[i]);
	}
	//______________________________________________________________________________
	//next part:  read in actual data 
	//...
	
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
    if (feature < -1 || feature > m->n_features){
		printf("Invalid feature number\n");
		exit(1);
    }
    if (feature==-1){
	return m->names[0];
    }
    return m->names[feature];
}

//other (getter) functions to be implemented here are in dataMatrixADT.h

