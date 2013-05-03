#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "array_operations.h"
#include "orthogonalize_sequential.h"

int main(int argc, char **argv)
{
  printf("Gram-Schmidt orthogonalization.\n");
  // n ~ total number of entries
  int n = 3;

  // test vectors
  int i;
  double *a, *b, *c;
  a = malloc(n * sizeof(double));
  b = malloc(n * sizeof(double));
  c = malloc(n * sizeof(double));
  // Three simple vectors
  for (i=0; i<n; i++){
    a[i] = i+1;
  }
  for (i=0; i<n; i++){
    b[i] = pow(i,2);
  }
  for (i=0; i<n; i++){
    c[i] = 1;
  }
  // matrix A 
  // := [a]
  //    [b]
  //    [c]
  // = 
  // [1.0, 2.0, 3.0]
  // [0.0, 1.0, 4.0]
  // [1.0, 1.0, 1.0]
  // should become
  // [ 1.0,  2.0, 3.0]
  // [-1.0, -1.0, 1.0]
  // [5/21 ~= 0.238095, -4/21 ~= -0.190476,  1/21 ~= 0.047619]

  // more complicated vectors for testing
  //    for (i=0; i<n; i++){
  //      a[i] = cos(i) * 3;
  //    }
  //    for (i=0; i<n; i++){
  //      b[i] = sin(exp(i)) * 5;
  //    }
  printf("Vector 1:\n");
  printArray(a,n);
  printf("Vector 2:\n");
  printArray(b,n);
  printf("Vector 3:\n");
  printArray(c,n);

  // test vector_diff
  //printf("Diff of vector 1 and 2:\n");
  //double *diff;
  //diff = malloc(n * sizeof(double));
  //copyArray(a, diff, n);
  //vector_diff(diff, b, n);
  //printArray(diff, n);
  //free(diff);

  //test scaler_mult
  //printf("Scaler multiplication of vector 1 by 5:\n");
  //double *sclr;
  //sclr = malloc(n * sizeof(double));
  //copyArray(a, sclr, n);
  //scaler_mult(5, sclr, n);
  //printArray(sclr, n);
  //free(sclr);

  // test inner_product
  //printf("Inner product: %f\n", inner_product(a, c, n));

  printf("Matrix:\n");
  double **matrix;
  matrix[0] = a;
  matrix[1] = b;
  matrix[2] = c;
  printMatrix(matrix,n);
  printf("\n");
  printf("Orthogonalizing the matrix...!\n");
  orthogonalize(matrix,n);
  printf("Done. Result:\n");
  printMatrix(matrix,n);

  // free the vectors
  free(a);
  free(b);
  free(c);

}
