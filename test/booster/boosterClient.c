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

	splitMatrix sm = testTrainSplit(firstA, train_size, 5);
        Matrix testM = getTestMatrix(sm);
        Matrix trainM = getTrainMatrix(sm);

	param_t my_parameters;
	my_parameters.max_depth_param=6;
	my_parameters.lambda=1;
	my_parameters.max_learners=5;
	my_parameters.gamma=1;

	Booster b = create_booster(trainM, my_parameters);
	train_booster(b);
	
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
