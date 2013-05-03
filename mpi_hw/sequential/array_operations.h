void printArray(double *a, int n);
void copyArray(double *a, double *b, int n);
void scaler_mult(double c, double *x, int n);
void vector_diff(double *x, double *y, int n);
void printMatrix(double **m, int n);

/* basic array operations */

// printArray
// prints elements of an array
void printArray(double *a, int n)
{
  int i;
  for (i = 0; i<n; i++){
    printf("%f ", a[i]);
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

// scaler_mult
// Multiply a given vector by a scaler
void scaler_mult(double c, double *x, int n){
  int i;
  for (i=0; i<n; i++){
    x[i] = c * x[i];
  }
}

// vector_diff
// Perform vector subtraction
// Given two vectors, subtract vector2 from vector1,
// and overwrite vector1.
void vector_diff(double *x, double *y, int n){
  int i;
  for (i=0; i<n; i++){
    x[i] -= y[i];
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

