void reverseArray(int *a, int n);
void printArray(int *a, int n);
void copyArray(int *a, int *b, int n);

/* basic array operations */

// reverseArray
// reverse the order of an array
void reverseArray(int *a, int n)
{
  int steps = n/2;
  int i,tmp;
  for (i=0; i<steps; i++){
    tmp = a[i];
    a[i] = a[n-1-i];
    a[n-1-i] = tmp;
  }
}

// printArray
// prints elements of an array
void printArray(int *a, int n)
{
  int i;
  for (i = 0; i<n; i++){
    printf("%d ", a[i]);
  }
  printf("\n");
}

// copyArray
// Given two arrays, copy the first one to the second one.
// The two arrays must have the same size.
void copyArray(int *a, int *b, int n)
{
  int i;
  for (i=0; i<n; i++){
    b[i] = a[i];
  }
}

// max
// returns the max element of an array
int max(int *a, int n){
  int *array;
  array = malloc(n * sizeof(int));
  //copy
  copyArray(a, array, n);
  // compute max
  int i;
  int current = array[0];
  for (i=1; i<n; i++){
    if (array[i] > current){
      current = array[i];
    }
  }
  return current;

  free(array);
}
