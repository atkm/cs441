#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>

#define START_MSG 123
#define ARRAY_MSG 234
#define SORTED_MSG 345
int Id, world_size;
MPI_Status status;            

void p_mergesort(int *array, int n, int height);
int compare(const int *a, const int *b);
void printArray(int *array, int n);

/*
  Tree Structure (in {} are names of nodes):

Height 3  {0}_____________
           |              \
Height 2  {0}____         {4}____        
           |     \         |     \
Height 1  {0}     {2}     {4}     {6}
           | \     | \     | \     | \
Height 0  {0} {1} {2} {3} {4} {5} {6} {7}


*/

void main(int argc, char **argv){
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &Id);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // The master proc is responsible for starting the process.
  if (Id==0){
    int size = argc - 1;
    int *array;
    array = malloc(size * sizeof(int));
    int i;
    for (i=0; i<size; i++){
      array[i] = atoi(argv[i+1]);
    }
    int root_height = 0;
    int node_count = 1;

    printf("Array content: ");
    for (i=0; i<size; i++){
      printf("%d",array[i]);
      if (i!=size)
        printf(" ");
    }
    printf(".\n");

    // The max height is limited by the number of procs:
    // 2^height < #ofProcs
    while (node_count < world_size){
      node_count += node_count;
      root_height++;
    }

    printf("Merge sort using a tree of height %d.\n", root_height);

    // Kick off.
    p_mergesort(array, size, root_height);

    // When finished.
    printf("Result: ");
    for (i=0; i<size; i++){
      printf("%d", array[i]);
      if (i!=size)
        printf(" ");
    }
    printf("\n");
    free(array);
    printf("ID=0; Master array memory fred.\n", Id);
  }
  // Other procs wait until the master proc tells them to start.
  else {
    // Start upon receiving a message.
    int array_info[2];
    int size, height;
    MPI_Recv(array_info, 2, MPI_INT, MPI_ANY_SOURCE, START_MSG, MPI_COMM_WORLD, &status);
    size = array_info[0];
    height = array_info[1];
    int *array;
    array = malloc(size * sizeof(int));
    MPI_Recv(array, size, MPI_INT, MPI_ANY_SOURCE, ARRAY_MSG, MPI_COMM_WORLD, &status);

    // Recursive call.
    p_mergesort(array, size, height);

    // Free memory when the process is over.
    free(array);
    printf("ID=%d; Master array memory fred.\n", Id);
  }

  // When the recursion terminates, we're done.
  printf("ID=%d; Done!\n", Id);
  MPI_Finalize();
}

void p_mergesort(int *array, int size, int height){
  int parent;
  int next, right;

  // Shift bit and do bit-wise comparison
  parent = Id & ~(1 << height);
  printf("ID=%d; Parent = %d\n", Id, parent);
  printf("ID=%d; Height = %d\n", Id, height);
  next = height - 1;
  right = Id | (1 << next);

  if (height > 0){
    // No right child. Move down one level.
    if (right >= world_size){
      printf("ID=%d; No right child.\n", Id);
      p_mergesort(array, size, next);
    }
    
    else {
      // Size of the array that goes to the left child.
      int left_size  = size / 2;
      // Size of the other array.
      int right_size = size - left_size;
      int *leftArray  = malloc(left_size * sizeof(int));
      int *rightArray = malloc(right_size * sizeof(int));

      // Split the array into two.
      printf("ID=%d; size = %d, left_size = %d, right_size = %d.\n", Id, size, left_size, right_size);
      int i; 
      for (i=0; i<left_size; i++){
        leftArray[i] = array[i];
      }
      for (i=0; i<size; i++){
        rightArray[i] = array[i + left_size];
      }

      // Tell the right child what to sort.
      int array_info[2];
      array_info[0] = right_size;
      array_info[1] = next;
      printf("ID=%d; Send to %d\n", Id, right);
      MPI_Send(array_info, 2, MPI_INT, right, START_MSG, MPI_COMM_WORLD);
      MPI_Send(rightArray, right_size, MPI_INT, right, ARRAY_MSG, MPI_COMM_WORLD);

      // Tell the left child (itself) the same thing.
      p_mergesort(leftArray, left_size, next);

      // Get the sorted rightArray (leftArray should have been sorted in the previous line).
      MPI_Recv(rightArray, right_size, MPI_INT, right, SORTED_MSG, MPI_COMM_WORLD, &status);

      // Merge the two results back into array, the fancy way.
      int j, k; 
      i = j = k = 0;
      // Compare the heads of two arrays and take the smaller ones.
      // Do it until either of the two is empty.
      while (i < left_size && j < right_size){
         if (leftArray[i] > rightArray[j])
           array[k++] = rightArray[j++];
         else
           array[k++] = leftArray[i++];
      }
      // Fill up the rest with the remaining elements.
      while (i < left_size){
        array[k++] = leftArray[i++];
      }
      while (j < right_size){
        array[k++] = rightArray[j++];
      }

      printf("ID=%d; Merge success! Merged array:\n", Id);
      printArray(array, size);

      free(leftArray); 
      // Wow; The master proc error is happening right here.
      if (Id == 0){
        printf("ID=%d; size = %d, right = %d, left_size = %d, right_size = %d.\n", Id, size, right, left_size, right_size);
        printArray(rightArray, right_size);
      }
      
      free(rightArray);
      printf("ID=%d; height=%d; left and right arrays memory fred.\n", Id, height);

    }
  }
  // The leaves. Just sort them.
  else {  
    printf("ID=%d; Sort!\n", Id);
    qsort(array, size, sizeof(int), (int(*)(const void *, const void *))compare);
    printf("ID=%d; Sort success!\n", Id);
  }

  if (parent != Id){
    printf("ID=%d; Send back to %d.\n", Id, parent);
    MPI_Send(array, size, MPI_INT, parent, SORTED_MSG, MPI_COMM_WORLD);
  }
}

int compare(const int *a, const int *b){
  if (*a >= *b)
    return 1;
  else
    return 0;
}

void printArray(int *array, int n){
  printf("ID=%d; Array: ", Id);
  int i;
  for (i=0; i<n; i++){
    printf("%d", array[i]);
    if (i!=n)
      printf(" ");
  }
  printf("\n");
}
