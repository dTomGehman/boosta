//This ADT to contain the unsorted data matrix type and methods for reading in data and accessing it.  
//I will probably reformat this at some point and replace each tab with two spaces.  until I change my vimrc

#include <stdio.h>
#include <stdlib.h>
#include "dataMatrixADT.h"


void read_word(FILE*fp, char*str);

struct matrix_type {
  int n_observations;
  int n_features;
  char**names;
  double**data;
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
		read_word(fp, m->names[i]);	
	}
	//verify that names were read correctly
	for (int i=0; i<m->n_features; i++) {
		printf("%s\n", m->names[i]);
	}
	//______________________________________________________________________________
	//next part:  read in actual data 
	//...
	
	return m;
}

// read in a word from stdin
void read_word(FILE*fp, char*str){
	char c=' '; int i=0;
	while (c==' ' || c=='\n') fscanf(fp, "%c", &c);
	while (c!=' ' && c!='\n' && c!=EOF && i<30){
		str[i]=(char)c;
		i++;
		fscanf(fp, "%c", &c);
	}
	str[i]='\0';
	
}

