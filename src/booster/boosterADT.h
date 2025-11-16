#ifndef BOOSTER
#define BOOSTER

#include "../dataMatrix/dataMatrixADT.h"

typedef struct booster_type *Booster;

typedef struct {
	int max_depth_param;
	double lambda;
	
	int max_learners;
	double gamma;
} param_t;

Booster create_booster(Matrix m, param_t params);

void train_booster(Booster b);

#endif
