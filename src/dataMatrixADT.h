#ifndef DATAMATRIX
#define DATAMATRIX

#define MAX_NAME_LENGTH 30  //max number of characters to have in a feature name (for now)

typedef struct matrix_type *Matrix;

Matrix create_from_file(char*filename);

#endif
