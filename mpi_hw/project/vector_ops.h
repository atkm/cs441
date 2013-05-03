/* vector_ops.h:
     Basic vector operations
 */
double inner_product(double *x, double *y, int n);
double vector_norm(double *x, int n);
void scaler_mult(double c, double *x, int n);
void vector_diff(double *x, double *y, int n);
void vector_add(double *x, double *y, int n);

// inner_product
// Compute the dot product of two vectors
// This is embarrasingly parallel.
double inner_product(double *x, double *y, int n){
  int i;
  double sum = 0.0;
  // get the sum of component-wise products
  for (i=0; i<n; i++){
    sum += x[i] * y[i];
  }
  // return the sum
  return sum;
}

// vector_norm
// Compute the norm of a vector
double vector_norm(double *v, int n){
  int i;
  double sum = 0;
  for (i=0; i<n; i++){
    sum += v[i] * v[i];
  }
  return sum;
}

// scaler_mult
// Perform a scaler multiplication of vector x by c
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

// vector_add
// Perform vector addition
void vector_add(double *x, double *y, int n){
  int i;
  for (i=0; i<n; i++){
    x[i] += y[i];
  }
}

