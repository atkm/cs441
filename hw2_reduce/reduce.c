#include <stdio.h>
#include <stdlib.h>
#include <memory.h>



// reduceA
//
// Computes the sum of the values src[0..n-1] 
//
void reduceA(int *src, int *result, int n) {
  int i;
  int sum;

  sum = 0;

  // add each source value to the running sum
  for (i = 0; i < n; i = i + 1) {
    sum = sum + src[i];
  }

  // report the result
  (*result) = sum;
}

// reduceB
//
// Same as the above, but recursive.
//
int reduceB(int *src, int n) {
  if (n == 0) {
    return 0;
  } else {
    return reduceB(src,n-1) + src[n-1];
  }
}


// reduceC
//
// Computes the sum of the values src[0..n-1] mimicking
// the sum operations of a parallel reduce whose circuit
// is shaped like a binary tree.
//
// Assumes that n is power of two.
//
void reduceC(int *src, int *result, int n) {

  int i;
  int stride;
  int *wrk;
  
  // build a work array to store intermediate calculations
  wrk = (int *)malloc(n * sizeof(int));

  // Copy the source array into the work array.
  for (i = 0; i < n; i = i + 1) {
    wrk[i] = src[i];
  }

  // Perform pairwise sums, rewriting the work array
  // with partial sums.
  //
  // If n = 2^k, then we have k phases correspond to
  // a level in the binary tree reduce circuit.
  //
  for (stride = 1; stride < n; stride = stride * 2) {

    // Perform one phase, performing pairwise sums of values,
    // replacing one of the two values in the work array.
    for (i = 0; i < n; i = i + 2*stride) {
      wrk[i] = wrk[i] + wrk[i+stride];
    }
  }

  // report the result
  (*result) = wrk[0];

  // free the working array's memory
  free(wrk);
}


//
// main 
//
// Defines the main procedure of the program that runs the three
// reduce procedures above.
//
// usage:
//   ./reduce 6 7 9 1 2 3 8 6
//
int main(int argc, char **argv) {
  int i,N;
  int sumA, sumB, sumC;
  int *V;
  
  // build the array of values from the argument vector
  N = argc-1;
  V = (int *)malloc(N*sizeof(int));
  for (i=1; i<=N; i++) {
    V[i-1] = atoi(argv[i]);
  }

  // test the first reduce code
  reduceA(V,&sumA,N);
  printf("The sum of the values\n");
  for (i=0; i<N; i++) {
    printf("\t%d",V[i]);
  }
  printf("\nis %d\n\n",sumA);

  // test the second reduce code
  sumB = reduceB(V,N);
  printf("The sum of the values\n");
  for (i=0; i<N; i++) {
    printf("\t%d",V[i]);
  }
  printf("\nis %d\n\n",sumB);

  // test the third reduce code
  reduceC(V,&sumC,N);
  printf("The sum of the values\n");
  for (i=0; i<N; i++) {
    printf("\t%d",V[i]);
  }
  printf("\nis %d\n\n",sumC);

  free(V);
}
  

