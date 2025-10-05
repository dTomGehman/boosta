//This ADT to contain the unsorted data matrix type and methods for reading in data and accessing it.  

#include <stdio.h>
#include <stdlib.h>
#include "dataMatrixADT.h"

struct matrix_type {
  int n_observations;
  int n_features;
  char**names;
  double**data;
};

