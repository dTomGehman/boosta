#include "../../src/dataMatrix/dataMatrixADT.h"
#include "../../src/sortedMatrix/sortedMatrixADT.h"
#include "../../src/gradientTree/gradientTreeADT.h"
#include "../../src/booster/boosterADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argc, char**argv){
	Matrix firstA;
	double lambda = 1;
	int max_depth_param = 6;
	struct timespec startT, endT;

	printf("Tests contained in boosterADT.c:\n\n");

	if (argc>=2){
		firstA = create_from_file(argv[1]);
	} else {printf("no file?? \n"); return 1;}
	
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
	my_parameters.gamma=1;
	my_parameters.eta=.5;

	Booster b = create_booster(trainM, my_parameters); //load the booster
	clock_gettime(CLOCK_MONOTONIC, &startT);
	train_booster(b); //train it--make the trees.  This could be wrapped in the create_booster if we would like to do that
	clock_gettime(CLOCK_MONOTONIC, &endT);

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
	int tn = 0;
	int tp = 0;
	int fn = 0;
	int fp = 0;
	for (int i=0; i<get_num_obs(testM); i++){
		printf("Test obs %d.  Weight %lf Pred label %d True label %d\n", 
			i,
			get_predicted_weight(b, testM, i),
			pred=get_predicted_label(b, testM, i),
			actual=get_label(testM, i)
		      );
		if (pred==actual) {
			total_correct++;
			if (pred==0) tn++;
			if (pred==1) tp++;
		} else {
			if (pred==0) fn++;
			if (pred==1) fp++;
		}
	}
	printf("\nTest Accuracy:  %d/%d = %lf\n", total_correct, get_num_obs(testM), (double)total_correct/get_num_obs(testM));
	printf("\nTest Sensitivity: %d/(%d+%d) = %lf\n", tp, tp, fn, (double)tp/(tp+fn));
	printf("\nTest Specificity: %d/(%d+%d) = %lf\n", tn, tn, fp, (double)tn/(tn+fp));

	double totalTime = (endT.tv_sec-startT.tv_sec)+((endT.tv_nsec-startT.tv_nsec)/1000000000.0);
        printf("Training Time: %f\n", totalTime);
}
