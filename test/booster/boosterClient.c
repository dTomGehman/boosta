#include "../../src/dataMatrix/dataMatrixADT.h"
#include "../../src/sortedMatrix/sortedMatrixADT.h"
#include "../../src/gradientTree/gradientTreeADT.h"
#include "../../src/booster/boosterADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char**argv){
	Matrix firstA;
	double lambda = 1;
	int max_depth_param = 6;

	printf("Tests contained in boosterADT.c:\n\n");

	if (argc>=2){
		firstA = create_from_file(argv[1]);
	} else {printf("no file??\n"); return 1;}
	
	double train_size=0.8;
	if (argc==3){
		train_size=atof(argv[2]);
	}

	//split the whole data matrix into testing and training data--this is not random, just a split
	splitMatrix sm = testTrainSplit(firstA, train_size, 5);
        Matrix testM = getTestMatrix(sm);
        Matrix trainM = getTrainMatrix(sm);

	//This struct defines the parameters to pass to the booster.  More to be added soon.
	param_t my_parameters;
	my_parameters.max_depth_param=6; //maximum depth of a tree
	my_parameters.lambda=1;
	my_parameters.max_learners=5;    //number of learners to generate
	my_parameters.gamma=1;		 //this one has not been implemented yet

	Booster b = create_booster(trainM, my_parameters); //load the booster
	train_booster(b); //train it--make the trees.  This could be wrapped in the create_booster if we would like to do that
	
	//verify the training dataset accuracy.  
	printf("Check the training observations\n");
	int total_correct=0;
	int pred, actual;
	for (int i=0; i<get_num_obs(trainM); i++){
		printf("Train obs %d.  Weight %lf Pred label %d True label %d\n", 
			i,
			get_predicted_weight(b, trainM, i),
			pred=get_predicted_label(b, trainM, i),
			actual=get_label(trainM, i)
		      );
		if (pred==actual) total_correct++;
	}
	printf("\nTrain Accuracy:  %d/%d = %lf\n", total_correct, get_num_obs(trainM), (double)total_correct/get_num_obs(trainM));
	
	//verify the testing dataset accuracy
	printf("\n\nCheck the testing observations\n");
	total_correct=0;
	for (int i=0; i<get_num_obs(testM); i++){
		printf("Test obs %d.  Weight %lf Pred label %d True label %d\n", 
			i,
			get_predicted_weight(b, testM, i),
			pred=get_predicted_label(b, testM, i),
			actual=get_label(testM, i)
		      );
		if (pred==actual) total_correct++;
	}
	printf("\nTest Accuracy:  %d/%d = %lf\n", total_correct, get_num_obs(testM), (double)total_correct/get_num_obs(testM));
}
