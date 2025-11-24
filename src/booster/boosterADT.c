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

//gradient and hessian calculatged based on squared loss.  We'll add in log loss and others soon
double sqGradient(double predicted, double actual);
double sqHessian(double predicted, double actual);

//initialize and return a booster ADT
//this method automatically creates the necessary sortedMatrix but doesn't create any trees yet
//the dataMatrix is passed by reference, so it should not be modified
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

//create the booster's trees
//One note:  create_tree has a side effect--it sets the tree positions inside the dataMatrix on each round.  
//thus, the dataMatrix shouldn't be modified while a booster is training, and two boosters shouldn't be trained on the same matrix at the same time
//this shouldn't be a problem
void train_booster(Booster b){
	//create arrays to hold the gradients and hessians for each boosting round
	double*gradients = malloc(sizeof(double) * get_num_obs(b->m));
	double*hessians = malloc(sizeof(double) * get_num_obs(b->m));
	
	//set the current predictions to zero to start 
	//this represents the best prediction for each training observation after each boosting round.
	double*predictions = calloc(get_num_obs(b->m), sizeof(double));
	if (!(gradients&&hessians&&predictions)){printf("fail."); exit(1);}

	//Main boosting loop: create all trees
	for (int i=0; i<b->params.max_learners; i++){
		printf("Tree %d:  \n", i);

		//get the gradients, hessians based on loss of current predictions
		for (int j=0; j<get_num_obs(b->m); j++){
			set_tree_pos(b->m, j, 0); //also clear treeposes in the matrix.  create_tree modifies these
			gradients[j] = sqGradient(predictions[j], get_label(b->m, j));
			hessians[j] = sqHessian(predictions[j], get_label(b->m, j));
		}

		//train a tree based on the gradients and hessians
		b->learners[i] = create_tree(b->m, b->sm, gradients, hessians, b->params.lambda, b->params.max_depth_param, b->params.gamma);
		//fix weights on the tree.  This function could be combined with the previous
		fix_weights(b->learners[i], b->m, gradients, hessians, b->params.lambda);
		//We can remove this and separate it to speed things up a little
		print_tree(b->learners[i]);

		//add the results (predictions or weights) of this tree to the predictions
		for (int j=0; j<get_num_obs(b->m); j++){
			predictions[j]+=predictTree(b->learners[i], b->m, b->m, j);
			//reminder for later:  can remove one parameter from predictTree now
		}
	}
}

//For this observation, sum the prediction (leaf weight) from each tree.  
double get_predicted_weight(Booster b, Matrix testM, int obs){
	double output=0;
	for (int i=0; i<b->params.max_learners; i++) output += predictTree(b->learners[i], b->m, testM, obs);
	return output;
}

//Round the predicted observation
int get_predicted_label(Booster b, Matrix testM, int obs){
	return get_predicted_weight(b, testM, obs)<0.5?0:1;
}

//loss calculations.  Right now, we're just doing squared loss, but we can add more easily
double sqGradient(double predicted, double actual){
	return (predicted - actual) * 2.0;
}
double sqHessian(double predicted, double actual){
	return 2;
}


