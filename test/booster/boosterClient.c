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
	printf("hH\n");
	train_booster(b);

}
