// Routines used in sequential Gram-Schmidt process

double inner_product(double *x, double *y, int n);
void orthogonalize(double **x, int n);

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

void orthogonalize(double **matrix, int n){
  int i,j;
  double scaler;
  // tmp, before, and projection are vectors.
  double *tmp, *before, *projection;
  tmp = malloc(n * sizeof(double));
  before = malloc(n * sizeof(double));
  projection = malloc(n * sizeof(double));
  for (i=0; i<n; i++){
    copyArray(matrix[i],tmp,n);  
    copyArray(matrix[i],before,n);  
    // to orthogonalize v_n, subtract projections of v_n
    // onto v_(n-1), ... , v_0
    for (j=0; j<i; j++){ 
      scaler = inner_product(before, matrix[j], n)/inner_product(matrix[j],matrix[j], n);
      //printf("%d, %d, scaler: %f\n", i, j, scaler);
      copyArray(matrix[j],projection,n);
      //printArray(projection,n);
      scaler_mult(scaler,projection,n);
      //printArray(projection,n);
      vector_diff(tmp, projection, n);
    }
    copyArray(tmp,matrix[i],n);
  }

  free(tmp);
  free(before);
  free(projection);
}


