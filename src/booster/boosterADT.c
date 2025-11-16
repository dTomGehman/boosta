#include "../../src/dataMatrix/dataMatrixADT.h"
#include "../../src/sortedMatrix/sortedMatrixADT.h"
#include "../../src/gradientTree/gradientTreeADT.h"
#include "boosterADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct booster_type {
	Matrix m; //training data
	SortedMatrix sm;  //sorted training data
	Tree*learners; //chain of all trees
	
	param_t params;
};

double sqGradient(double predicted, double actual);
double sqHessian(double predicted, double actual);

Booster create_booster(Matrix m, param_t params){
	Booster b = malloc(sizeof(struct booster_type));
	if (!b) {printf("fail."); exit(1);}

	b->m = m;
	printf("%d\n", get_num_obs(m));
	printf("%d\n", get_num_obs(b->m));

	b->params = params;
	b->sm = create_from_matrix(m);
	b->learners = malloc(sizeof(Tree) * params.max_learners);
	if (!b->learners) {printf("fail."); exit(1);}
	return b;
}


void train_booster(Booster b){
	double*gradients = malloc(sizeof(double) * get_num_obs(b->m));
	double*hessians = malloc(sizeof(double) * get_num_obs(b->m));
	//set the current predictions to zero to start
	double*predictions = calloc(get_num_obs(b->m), sizeof(double));
	if (!(gradients&&hessians&&predictions)){printf("fail."); exit(1);}

	for (int i=0; i<b->params.max_learners; i++){
		printf("Tree %d:  \n", i);
		//get the gradients, hessians based on loss of current predictions
		for (int j=0; j<get_num_obs(b->m); j++){
			set_tree_pos(b->m, j, 0); //also clear treeposes
			gradients[j] = sqGradient(predictions[0], get_label(b->m, j));
			hessians[j] = sqHessian(predictions[0], get_label(b->m, j));
		}
		//train a tree based on the gradients and hessians
		b->learners[i] = create_tree(b->m, b->sm, gradients, hessians, b->params.lambda, b->params.max_depth_param);
		fix_weights(b->learners[i], b->m, gradients, hessians, b->params.lambda);
		print_tree(b->learners[i]);

		//add the results (predictions or weights) of this tree to the predictiions
		for (int j=0; j<get_num_obs(b->m); j++){
			predictions[j]+=predictTree(b->learners[i], b->m, b->m, i);
			//reminder for later:  can remove one parameter from predictTree now
		}
	}
}

//loss calculations.  Right now, we're just doing squared loss, but we can add more easily
double sqGradient(double predicted, double actual){
	return (predicted - actual) * 2.0;
}
double sqHessian(double predicted, double actual){
	return 2;
}


