#ifndef SORTEDMATRIX
#define SORTEDMATRIX

#include "../dataMatrix/dataMatrixADT.h"

typedef struct sorted_type *SortedMatrix;

typedef struct {
	double datum;
	int obs_number;
} point;

SortedMatrix create_from_matrix(Matrix m); //take matrix m and copy/sort it into a new SortedMatrix.  We can use heaps and threading here.  
point*get_sorted_col(SortedMatrix s, int feature); //return pointer to the sorted column for one feature.  To be used by the tree.  
						   //this allows the tree to be closely connected with the sorted matrix, and it can even modify the matrix, but it shouldn't.  Maybe we can change that later but idk.  

//I think I will write the scanning functionality in the tree instead for more flexibility
//double linear_scan_col(SortedMatrix s); //might put this in the ADT instead of the header and replace with scan matrix.  Each col will need to be scanned for splits

#endif
