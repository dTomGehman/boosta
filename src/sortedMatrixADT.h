#ifndef SORTEDMATRIX
#define SORTEDMATRIX

#include "dataMatrixADT.h"

typedef struct sorted_type *SortedMatrix;

typedef struct {
	double datum;
	int obs_number;
} point;

SortedMatrix create_from_matrix(Matrix m); //take matrix m and copy/sort it into a new SortedMatrix.  We can use heaps and threading here.  
double linear_scan_col(SortedMatrix s); //might put this in the ADT instead of the header and replace with scan matrix.  Each col will need to be scanned for splits

#endif
