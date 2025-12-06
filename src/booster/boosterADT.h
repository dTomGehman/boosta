#ifndef BOOSTER
#define BOOSTER

#include "../dataMatrix/dataMatrixADT.h"

typedef struct booster_type *Booster;

typedef struct { //struct to hold parameters
	int max_depth_param; //max depth of a tree
	double lambda; //lambda param
	
	int max_learners; //number of trees to generate
	double gamma; 
       	double eta;	
} param_t;

Booster create_booster(Matrix m, param_t params);//Create a booster from a training matrix and parameter struct

void train_booster(Booster b); //train the model

double get_predicted_weight(Booster b, Matrix testM, int obs); //get a predicted weight for a test observation

int get_predicted_label(Booster b, Matrix testM, int obs); //get a predicted label for a test observation (same as rounding get_predicted_weight to 0 or 1)

#endif
