#ifndef METRICS
#define METRICS

double accuracy();

double sensitivity();

double specificity();

double ROC_AUC(int*actual_labels, double*predicted_weights, int num_observations, int iterations);



#endif
