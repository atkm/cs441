#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "array_operations.h"
#include "orthogonalize_sequential.h"

int sqrt_of(int n);

int main(int argc, char **argv)
{
  int n;
  n = argc - 1;
 
  if (n==0){
    printf("No vector given.");
  }
  else {
    int dim;
    dim = sqrt_of(n);
    if (dim == 0){
      printf("The matrix isn't a square matrix.\n");
      exit(0);
    } else {
      printf("The matrix is a %dx%d square matrix.\n",dim,dim);
      // Create a matrix from the input
      double **matrix;
      matrix = malloc(dim * sizeof(double *));
      int i,j;
      // allocate memory for the matrix
      for (i=0; i<dim; i++){
        matrix[i] = malloc(dim * sizeof(double));
      }
      // allocate memory for each vector
      for (i=0; i<dim; i++){
        for (j=0; j<dim; j++){
          matrix[i][j] = (double) atof(argv[i*dim +j+1]);
        }
      }
      printMatrix(matrix,dim);
      printf("Orthogonalizing it...\n");
      orthogonalize(matrix,dim);
      printf("Done:\n");
      printMatrix(matrix,dim);
    }
  }

}

// sqrt_of
// returns sqrt(n) if n is a square.
// returns 0 otherwise.
int sqrt_of(int n){
  int m = 1;
  while (m*m <= n) {
    if (m*m==n) {
      return m;
    }
    m++;
  }
  return 0;
}
