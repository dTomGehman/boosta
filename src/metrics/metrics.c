#include "metrics.h"
#include <stdio.h>

double ROC_AUC(int*actual_labels, double*predicted_weights, int num_observations, int iterations){
    double threshold=1, fp, tp, fn, tn, lastfp=0, lasttp=0, area=0;
    int pred;
    for (int i=0; i<=iterations; i++){
        threshold-=(double)1/iterations; //gradually increase threshold
        fp=tp=fn=tn=0; //these store counts of false positive, true, etc
        for (int j=0; j<num_observations; j++){
            pred = predicted_weights[j]>=threshold; //compare each value to threshold
            if (pred==actual_labels[j]){
                if (pred) tp++;
                else tn++;
            } else {
                if (pred) fp++;
                else fn++;
            }
        }
        //printf("tp %lf fn %lf fp %lf tn %lf\n", tp, fn, fp, tn);
        tp /= (tp+fn); //convert from raw tp to proportion
        fp /= (fp+tn); //ditto
        area += (fp-lastfp)*(tp+lasttp)/2; //add to riemann sum
        lastfp=fp; lasttp=tp;
        //printf("tp %lf fp %lf area %lf\n\n", tp, fp, area); 
    }
    return area;
}
