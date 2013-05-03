#include "p_msort.h"

//
// parallel_msort
// 
// The main process for parallel mergesort.
// It calls the helper function to complete the process.
//
void parallel_msort(int *next_array, int n){ 
  // The master proc is responsible for starting the process.
  if (Id==0){
    int root_height = 0;
    int node_count = 1;

    //printf("Next array content: ");
    //int i;
    //for (i=0; i<n; i++){
    //  printf("%d",next_array[i]);
    //}
    //printf(".\n");

    // The max height is limited by the number of procs:
    // 2^height < #ofProcs
    while (node_count < world_size){
      node_count += node_count;
      root_height++;
    }

    printf("Initializing merge sort of [", root_height);
    int i;
    for (i=0; i<n; i++){
      printf("%d", next_array[i]);
      if (i!=n-1)
        printf(" ");
    }
    printf("]\n");

    // Kick off.
    p_mergesort_helper(next_array, n, root_height);
    printf("Sorting Done.\n");

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
    p_mergesort_helper(array, size, height);

    // Free memory when the process is over.
    free(array);
  }
}

//
// p_mergesort_helper
// A sub-routine for parallel mergesort
//
void p_mergesort_helper(int *array, int size, int height){
  int parent;
  int next, right;

  // Shift bit and do bit-wise comparison
  parent = Id & ~(1 << height);
  next = height - 1;
  right = Id | (1 << next);

  if (height > 0){
    // No right child. Move down one level.
    if (right >= world_size){
      p_mergesort_helper(array, size, next);
    }
    
    else {
      // Size of the array that goes to the left child.
      int left_size  = size / 2;
      // Size of the other array.
      int right_size = size - left_size;
      int *leftArray  = malloc(left_size * sizeof(int));
      int *rightArray = malloc(right_size * sizeof(int));

      // Split the array into two.
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
      MPI_Send(array_info, 2, MPI_INT, right, START_MSG, MPI_COMM_WORLD);
      MPI_Send(rightArray, right_size, MPI_INT, right, ARRAY_MSG, MPI_COMM_WORLD);

      // Tell the left child (the same proc) the same thing.
      p_mergesort_helper(leftArray, left_size, next);

      // Get the results (leftArray should have been sorted in the previous line).
      MPI_Recv(rightArray, right_size, MPI_INT, right, SORTED_MSG, MPI_COMM_WORLD, &status);

      //Merge the two results back into array
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
      free(leftArray); free(rightArray);
    }
  }
  // Height = 0 case i.e. the leaves. Sort them using the quicksort from stdlib.
  else
    // The cast before 'compare' suppresses warnings.
    // There are cases where size=0. In that case, qsort does nothing.
    qsort(array, size, sizeof(int), (int(*)(const void *, const void *))compare);

  // Send the result to its parent (unless the parent is itself)
  if (parent != Id)
    MPI_Send(array, size, MPI_INT, parent, SORTED_MSG, MPI_COMM_WORLD);
}

int compare(const int *a, const int *b){
  if (*a >= *b)
    return 1;
  else
    return 0;
}
