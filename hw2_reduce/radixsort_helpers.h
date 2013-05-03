void prefixSum_without_shift(int *a, int *b, int n);
int binlength(int n);
int nth_bit(int n, int m);
void invert_bits(int *bits, int n);

/* Helper functions/procedures for parallelRadixSort
   'bit_sort' should also be here, but it depends
   on 'prefixSum' so it's kept in the main file */

// prefixSum_without_shift
// The same as prefixSum except for the shifting at the end
void prefixSum_without_shift(int *a, int *b, int n)
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

  // copy the result
  copyArray(array, b, n);
 
  free(array);
}

// binlength
// Computes the length of the binary representation of a given int
// i.e. the position of the last 1 (counting from one).
int binlength(int n)
{
  int current = n;
  int length = 1;
  while (current > 1) {
    length += 1;
    current = current/2;
  }
  return length;
}

// nth_bit
// compute the nth bit of a given int and return it
// (count starts from 0)
int nth_bit(int n, int m)
{
  // j is an int whose nth digit in its binary representation. 
  // is 1, and all other digits are 0.
  // This is achieved by shifting 1 by n.
  int j = 1 << n;
  // &: bit-wise AND operation.
  // If the nth digit is NOT 1, (m & j) will return 0 (which acts as 'false')
  // if it is 1, then a non-zero value is returned.
  if (m & j) {
    return 1;
  }
  // otherwise the bit is 0
  return 0;
}

// invert_bits
// replace 1s with 0s and vice versa.
void invert_bits(int *bits, int n)
{
  int i;
  for (i=0; i<n; i++){
    if (bits[i]){
      bits[i] = 0;
    } else {
      bits[i] = 1;
    }
  }
}
