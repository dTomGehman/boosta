#include <stdio.h>
#include <stdlib.h>
#include "../dataMatrix/dataMatrixADT.h"
#include "sortedMatrixADT.h"
#include "pointPriorityADT.h"
#include <pthread.h>
#include <time.h>

//I moved the following commented-out struct to sortedMatrixADT.h.  
/*
typedef struct {        //struct to hold a point in the sorted array.  Might move this to the header file depending on how we use it.  
		        //This point contains its (double) value as well as other fields to refer back to which observation its from, 
		        //as the order of each feature colunm will be sorted.  
	double datum;   //the value of the data, corresponding to some point (obs, feat) in the unsorted matrix
	int obs_number; //which observation the datum is from, as a number. obs as above^
	
	//the above two fields are essential.  Anything below is to hopefully save time as we build the tree to avoid searches and lookups
	
	//int label;      //(might not be necessary) equivalent to the observation's label.  
		        //This might save a small amount of time to copy it here instead of constantly referring back to the original matrix.  
	//long tree_pos;  //binary representing the datum's position in the tree (instance set) as the tree is being created
} point;*/

struct sorted_type{ 
	Matrix m; //parent matrix
	int n_observations, n_features; //might delete
	point**matrix;  //2d array of points.  This is equivalent to transposing the unsorted matrix and sorting each row (feature)
};


void sortnshove(point*array, Matrix m, int feature);

SortedMatrix create_from_matrix(Matrix m){
	struct timespec startTime, endTime;
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	SortedMatrix sm = malloc(sizeof(struct sorted_type));
	if (!sm) {printf("fail. "); exit(1);}
	sm->m = m;
	
	sm->matrix = malloc(sizeof(point*) * get_num_feats(m));
	if (!sm->matrix) {printf("fail. "); exit(1);}

	typedef struct __arg {       //arg for threads
		int start;
		int end;

	} arg_t;

	void *mythread(void *arg) {
		arg_t *ar = (arg_t *) arg;
		arg_t arr = *ar;
		int startCol = arr.start;
		int endCol = arr.end;
		for (int i=startCol; i<endCol; i++){
                	sortnshove(sm->matrix[i], m, i);
		}
        }



	for (int i=0; i<get_num_feats(m); i++){
		sm->matrix[i] = malloc(sizeof(point)*get_num_obs(m));
		if (!sm->matrix[i]) {printf("fail. "); exit(1);}
	}

	if (get_num_feats(m) >= 3) {                   //I only made three threads for now; I'll probably add more later
		pthread_t p1, p2, p3;
		int partition = (int) get_num_feats(m)/3;
		arg_t arg1 = {0, partition};
		arg_t arg2 = {partition, partition*2};
		arg_t arg3 = {partition*2, get_num_feats(m)};

		pthread_create(&p1, NULL, mythread, &arg1);
		pthread_create(&p2, NULL, mythread, &arg2);
		pthread_create(&p3, NULL, mythread, &arg3);

		pthread_join(p1, NULL);
		pthread_join(p2, NULL);
		pthread_join(p3, NULL);
	} else {                                        //Doesn't use threads if less than three features
		for (int i=0; i<get_num_feats(m); i++){       
			sortnshove(sm->matrix[i], m, i);
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &endTime);


	double totalTime = (endTime.tv_sec-startTime.tv_sec)+((endTime.tv_nsec-startTime.tv_nsec)/1000000000.0);
	printf("Time to create sorted Matrix %f\n", totalTime);

	return sm;
}

//sort a certain feature column of Matrix m and place it in the array
void sortnshove(point*array, Matrix m, int feature){ //this might be inefficient due to copying, so maybe I'll improve it, or maybe I won't.  
	PQ q = create(); //use a priority queue to quickly sort the columns
	for (int i=0; i<get_num_obs(m); i++){
		point*p = malloc(sizeof(point));
		if (!p) {printf("fail. "); exit(1);}
		p->obs_number = i;
		p->datum = get_data(m, i, feature);
		insert(q, p);
	}
	for (int i=0; !is_empty(q); i++){
		point*p = extract(q);
		array[i].datum = p->datum;
		array[i].obs_number = p->obs_number;
		free(p); //most of the dynamically-allocated memory is supposed to persist until the end of the program
			 //this one does not.  I'll have to check through the rest of the code at some point for other cases like this.  
	}
	destroy_q(q);
		
}

//return an address to a sorted matrix column
point*get_sorted_col(SortedMatrix s, int feature){
	return s->matrix[feature];
}
