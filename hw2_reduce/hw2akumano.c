#include <stdio.h>
#include <stdlib.h>
#include "array_operations.h"
#include "prefixsum_helpers.h"
#include "radixsort_helpers.h"

/* compile using command: gcc -o hw2akumano hw2akumano.c
run using: ./hw2akumano 7 8 3 1 9 2 0 6
The number of inputs must be a power of 2.
*/

void leftBroadcast(int *a, int *b, int v, int n);
void permute(int *a, int *b, int *p, int n);
void prefixSum(int *a, int *b, int n);
void parallelRadixSort(int *a, int *b, int n);
int * bit_sort(int *a, int n);
int not_power_of_two(int n);

// 1. leftBroadcast
// fills a[0..n-1] with the value v
void leftBroadcast(int *a, int *b, int v, int n)
{
  int i;
  //intermediate array
  int *array;
  array = malloc(n * sizeof(int));
  for (i=n-1; i>-1; i--){
    array[i] = v;
  }
  // copy the result
  copyArray(array, b, n);

  free(array);
}

// 2. permute
// puts a[0..n-1] into b[0..n-1] where a[i] goes to position p[i] 
void permute(int *a, int *b, int *p, int n)
{
  int i;
  // intermediate array
  int *array;
  array = malloc(n * sizeof(int));
  for (i=0; i<n; i++){
    // tell the i-th element of 'a' to go to p[i] in the new array
    array[p[i]] = a[i];
  }
  
  // copy the result
  copyArray(array, b, n);
  
  // free the memory
  free(array);
}

// 3. prefixSum -- prefixSum with calc. mimicking parallel prefix.
// The implementation is the one described in Blelloch.
// Assume that the length of an input vector is a power of 2.
// Also assume that there is an unlimited number of processors (so we can skip processors sums).
// The algorithm starts with up-sweep to compute partial sums.
// Then down-sweep to (essentially) compute the prefix sum.
// After down-sweep we just need to make minor modifications
// to obtain the full prefix sum.
void prefixSum(int *a, int *b, int n)
{
  int i;
  // intermediate array
  int *array;
  array = malloc(n * sizeof(int));
  
  // copy the original
  copyArray(a, array, n);

  // call up-sweep
  // Note that 'array' is modified in this procedure
  up_sweep(array, n);

  // (Keep the grand sum. We'll need this after down-sweep.)
  int sum = array[0];

  // call down-sweep
  // This procedure also modify the array
  down_sweep(array, n);

  // shift elements to the right.
  for (i=n-1; i>0; i--){
    array[i] = array[i-1];
  }
  // The shifting doesn't work for the 0th element!
  // but that's just the grand sum that we already computed.
  array[0] = sum;

  // copy the result
  copyArray(array, b, n);
 
  free(array);
}


// 4. parallelRadixSort
// recursive stable sort on 1s bit, 2s bit, 2^2s bit, ...
// The greatest int comes to the head and the least int goes to the end.
void parallelRadixSort(int *a, int *b, int n)
{
  int *array;
  array = malloc(n * sizeof(int));
  copyArray(a, array, n);

  // array to hold information about the ints' binary representations
  int *bits;
  bits = malloc(n * sizeof(int));

  // compute the binary length of the max int
  int maxlen = binlength(max(array, n));

  // tmp array to copy the array before permutation
  int *tmp;
  tmp = malloc(n * sizeof(int));

  int i,j;
  // sorting. begin with the 0th bit. 
  for (i=0; i<maxlen; i++){
    for (j=0; j<n; j++){
      // get the i-th bit of each element
      bits[j] = nth_bit(i,array[j]);
    }
    copyArray(array, tmp, n);
    // permute the array using a permutation array obtained via bit_sort
    permute(tmp, array, bit_sort(bits,n), n);
 }

  // copy the result
  copyArray(array, b, n);

  // clean-up
  free(array);
  free(bits);
}

// bit_sort
// Given an array of 0 and 1 do a stable sort (0 comes first)
// then return the result as a list of indices (permutation).
// This operation directly writes the result to the array.
// Permutations for 0s and 1s are computed separately.
int * bit_sort(int *a, int n)
{
  int *bits, *oneperm, *zeroperm;
  bits = malloc(n * sizeof(int));
  copyArray(a, bits, n);
  
  // 1. Compute the permutation indices for 1s
  oneperm = malloc(n * sizeof(int));
  // reverse bits
  reverseArray(bits, n);
  prefixSum_without_shift(bits,oneperm,n);
  // need to reverse the permutation array because it is the
  // prefix sum of a reverse-order bits array.
  reverseArray(oneperm,n);

  // bring back the elements to the original order
  copyArray(a, bits, n);

  // 2. Compute the permutation indices for 0s
  // First invert the bits
  invert_bits(bits,n);
 
  int i;
  // then do prefix sum without shift to get
  // the number of zeros in the array
  zeroperm = malloc(n * sizeof(int));
  prefixSum(bits,zeroperm,n);
  // finally subtract the current indices from the length
  // of the array to make it a stable sort
  // (we want the largest int at the 0th position)
  for (i=0; i<n; i++){
    zeroperm[i] = n - zeroperm[i];
  }

  // flip back the bits since they were inverted
  copyArray(a, bits, n);

  // Copy the result into the given array
  for (i=0; i<n; i++){
    // if the bit is 1
    if (bits[i]){
      // (using bits array for holding the result)
      bits[i] = oneperm[i];
      // if the bit is 0
    } else {
      // Take the corresponding entry from the 'up' array
      bits[i] = zeroperm[i];
    }
  }

  return bits;

  // clean-up
  free(bits);
  free(oneperm);
  free(zeroperm);
}


// Tests 
void main(int argc, char **argv)
{
  if (argc == 1)
  {
    printf("No array elements given.");
  } else if (not_power_of_two(argc-1)) {
    printf("The number of arguments is not a power of 2.");
  }
  else
  {
  // number of arguments, excluding the program name
  int n = argc - 1;
  int *array;
  array = malloc(n * sizeof(int));
  int i;
  // correspond the values of the array to command-line arguments
  for (i=0; i<n; i++){
    array[i] = atoi(argv[i+1]);
  }
  
  // initialize another array to carry results
  int *brray;
  brray = malloc(n * sizeof(int));

  // prepare permutation vector
  // in this case, just reverse the order
  int *perm;
  perm = malloc(n * sizeof(int));
  for (i=0; i<n; i++){
    perm[i] = -i + n - 1;
  }
   
  // arbitrary value to be broadcasted
  int v = 10;
  // tests
  // printArray
  printf("array = ");
  printArray(array, n);
  printf("\n");

  // leftBroadcast
  printf("Executing leftBroadcast(array, brray, v, n) with v=%d... ",v);
  leftBroadcast(array, brray, v, n);
  printf("Done. Result:\n");
  printArray(brray, n);
  printf("\n");

  // permute
  printf("Executing permute(array, brray, perm, n) with\n");
  printf("permutation vector:\n");
  printArray(perm, n);
  permute(array, brray, perm, n);
  printf("Done. Result:\n");
  printArray(brray, n);
  printf("\n");

  // prefixSum
  printf("Executing prefixSum(array, brray, n)... ");
  prefixSum(array, brray, n);
  printf("Done. Result:\n");
  printArray(brray, n);
  printf("\n");
  
  // parallelRadixSort
  printf("Executing parallelRadixSort(array, brray, n)... ");
  parallelRadixSort(array, brray, n);
  printf("Done. Result:\n");
  printArray(brray, n);
  printf("\n");

  free(array);
  free(brray);
  free(perm);
  }
 
}

// not_power_of_two
// checks if the given int is a power of two
// returns 1 if it's not a power of two. returns 0 otherwise.
int not_power_of_two(int n)
{
  int m = 1;
  while (n >= m) {
    if (n == m){
      return 0;
    }
    m = m * 2;
  }
  return 1;
}
