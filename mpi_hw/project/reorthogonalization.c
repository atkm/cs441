/* 

reorthogonalization.c

Given k vectors of length n (k<n), which form an orthogonal basis Q, 
and another vector v of length n, compute a vector q that is
orthogonal to all vectors in Q (so that Q U {q} is an orthogonal basis).

Usage: 
  Compile with mpicc -o p_orthogonalize reorthogonalization.c
  
  Use the ruby script run.rb to run the program:
*/

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<float.h>
#include<math.h>
#include "vector_ops.h"
#include "array_operations.h"

#define BPRINTF_MSG 123
#define ORTH_MSG   234

void mprintf(char *string, int rank);
void bprintf(char *header, const char *format, int v, int rank, int size);
int check_orthogonal(double **basis, int m, int n);

// For MPI calls that require a status return
MPI_Status status;

void main(int argc, char **argv) {

  MPI_Init(&argc, &argv);

  // argv[1] ~ dimension of the basis
  int m = atoi(argv[1]);
  // argv[2] ~ length of a vector
  int n = atoi(argv[2]);
  
  // Create a matrix (for basis) from the input
  double **basis;
  // allocate memory for the matrix (m rows)
  basis = malloc(m * sizeof(double *));

  int i,j;
  // allocate memory for each vector in the matrix (row vectors)
  for (i=0; i<m; i++){
    basis[i] = malloc(n * sizeof(double));
  }
  // Fill the matrix with the basis vectors
  for (i=0; i<m; i++){
    for (j=0; j<n; j++){
      basis[i][j] = atof(argv[i*n + j + 3]);
    }
  }

  // Create the last vector
  double *new_vec;
  new_vec = malloc(n * sizeof(double));
  for (i=0; i<n; i++){
    new_vec[i] = atof(argv[m*n + 3 + i]);
  }


  int rank, world_size;
  // get processor rank
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  // get the total number of processors
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  

  // The master processesor does the following
  if (rank==0){
  // Declare what we're working with.
    printf("Parallel (Gram-Schmidt) re-orthogonalization\n", rank);
    printf("of vector:\n");
    printArray(new_vec,n);
    printf("with basis:\n");
    for (i=0; i<m; i++){
      printArray(basis[i],n);
    }
    printf("\n",rank);

    // Check if we can carry out the computation
    if (m > world_size){
      printf("The dimension of the basis is too high.\n");
      printf("The dimension can't exceed the number of processors.\n");
      printf("We only have %d processors available. Aborting.\n", world_size);
      // Finalize mpi then exit
      MPI_Finalize();
      exit(0);
    }

    // check for orthogonality of the basis
    printf("Testing the orthogonality of the basis...");
    if (!check_orthogonal(basis, m, n)) {
      printf("The basis is not orthogonal. Aborting.\n");
      // Finalize mpi then exit
      MPI_Finalize();
      exit(0);
    } else {
      printf("pass.\n");
    }
  }

  // According to the formula, the orthogonalized vector q_(m+1) is
  // q_(m+1) = v_(m+1) - (Sum over i = 0..m) <v_(m+1), q_i>/norm(q_i) * q_i
  // Processor i computes the i-th term of the sum.
  // Do it only for the processors that are necessary.
  
  if (rank < m){
    double *my_vec;
    my_vec = malloc(n * sizeof(double));
    copyArray(basis[rank], my_vec, n);

    printf("I'm process %d. My vector is ", rank);
    printArray(my_vec,n);

    double inprd = inner_product(my_vec, new_vec, n); // inner product
    double nrm = vector_norm(my_vec, n); // norm

    // compute (inner product)/(norm) * my_vec 
    scaler_mult(inprd/nrm, my_vec, n);

    // broadcast it to the master processor
    if (rank != 0 && rank < m){ // Each processor with 0<rank<m sends the result.
      printf("I'm process %d. I'm sending ", rank);
      printArray(my_vec,n);
      printf("to the master processor.\n");
      MPI_Send(my_vec, n, MPI_DOUBLE, 0, ORTH_MSG, MPI_COMM_WORLD);

    } else if (rank == 0) {
      printf("I'm the master processor. My vector after the computation is ");
      printArray(my_vec,n);
      printf(".\n");
      // Master collects all intermediate vectors and sum them up
      double *recv_vec; // just receive the first component for now
      recv_vec = malloc(n * sizeof(double));
      double *store;
      store = malloc(n * sizeof(double));
      copyArray(my_vec, store, n);
      for (i=1; i<m; i++){ // Receive result from processors 0<rank<m.
        // receive from processor i
        // ORTH_MSG + j corresponds to the jth entry of
        // of the vector from the processor
        printf("Master processor: receiving a vector from processor %d.\n", i);
        MPI_Recv(recv_vec, n, MPI_DOUBLE, i, ORTH_MSG, MPI_COMM_WORLD, &status);
        for (j=0; j<n; j++){
          store[j] += recv_vec[j];
        }
      }

      // Finally...
      vector_diff(new_vec,store,n);
      printf("Result:\n");
      printArray(new_vec,n);

      free(recv_vec);
      free(store);
    }

    free(my_vec);
  }

  free(new_vec);
  // Free matrix
  for (i=0; i<m; i++){
    free(basis[i]);
  }
  free(basis);

  MPI_Finalize();
}

// check_orthogonal
// Given m vectors of length n, check if they are orthogonal to each other

int check_orthogonal(double **basis, int m, int n){
  int i,j;
  double tmp;
  for (i=0; i<m; i++){
    for (j=0; j<i; j++){
      tmp = inner_product(basis[i],basis[j],n);
      // return 0 if a pair of non-orthogonal vectors is found.
      if (fabs(tmp) >= DBL_EPSILON){
        return 0;
      }
    }
  }
  // if all goes well, return 1.
  return 1;
}

// mprintf
//
// Stands for "master printf".  The effect of this call is
// for the processor with ID (or rank) of 0 (running on the front end)
// to print the given string.  The other processors do nothing.
//
void mprintf(char *string, int rank) {
  if (rank == 0) {
    printf("%s\n", string);
  }
}

// bprintf
//
// Stands for "broadcast printf".  Using this, each processor
// reports a value v according to a format string (with an 
// embedded %d) provided.  The collection of reported values
// is preceded by a header string.  Each processor's value
// is reported along with that processor's ID (=rank).
//
void bprintf(char *header, const char *format, int v, int rank, int size) {
  int i;
  if (rank != 0) {
    MPI_Send(&v,1,MPI_INT,0,BPRINTF_MSG,MPI_COMM_WORLD);
  } else {
    printf("%s\n",header);
    printf("\t%02d:",0);
    printf(format,v);
    for (i=1; i<size; i++) {
      MPI_Recv(&v,1,MPI_INT,i,BPRINTF_MSG,MPI_COMM_WORLD,&status);
      printf("\t%02d:",i);
      printf(format,v);
    }
    printf("\n");
  }
}
