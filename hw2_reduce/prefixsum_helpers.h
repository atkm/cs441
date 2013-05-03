void up_sweep(int *a, int n);
void down_sweep(int *a, int n);

/* The two routines used in computing prefix sum */

// up-sweep 
// Computes partial sums of elements of an array.
// (using ideas from reduceC)
// The total sum comes to the 0th element.
// This operation directly modify the input array.
void up_sweep(int *a, int n)
{
  int i, stride;
  for (stride=1; stride<n; stride = stride * 2){
    for (i=0; i<n; i = i + 2*stride){
      a[i] = a[i] + a[i+stride];
    }
  }
}

// down-sweep
// Given an array of ints compute the prerequisite for 
// prefix sum. (To be a complete prefix sum, the 'shift'
// operation is required after this. See 'prefixSum'.)
// This operation directly modify the input array.
void down_sweep(int *a, int n)
{
  int i, stride;
  // Set the head to be the identity element.
  a[0] = 0;
  // Similar loop to the one in up-sweep
  // but this time start with a large stride
  // and reduce the width of a stride for each loop.
  // For each stride, values of left and right children are set.
  for (stride=n; stride>1; stride = stride/2){
    for (i=0; i<n; i = i + stride){
      // temporary int to hold the current value
      int tmp = a[i];
      // add its value to the right child's value and give it to left child
      a[i] = tmp + a[i + stride/2];
      // copy its value to the right child 
      a[i + stride/2] = tmp;
    }
  }
}
