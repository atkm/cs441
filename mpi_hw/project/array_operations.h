void printArray(double *a, int n);
void copyArray(double *a, double *b, int n);
void printMatrix(double **m, int n);

/* array_operations.h:
    basic array operations */

// printArray
// prints elements of an array
void printArray(double *a, int n)
{
  int i;
  for (i = 0; i<n; i++){
    printf("%e ", a[i]);
  }
  printf("\n");
}

// copyArray
// Given two arrays, copy the first one to the second one.
// The two arrays must have the same size.
void copyArray(double *a, double *b, int n)
{
  int i;
  for (i=0; i<n; i++){
    b[i] = a[i];
  }
}

// printMatrix
// prints out a square matrix
void printMatrix(double **m, int n){
  int i;
  for (i=0; i<n; i++){
    printArray(m[i],n);
  }
}

