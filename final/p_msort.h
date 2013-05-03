#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>

#define MSORT_H 99
// MSGs for merge-sort
#define START_MSG 678
#define ARRAY_MSG 789
#define SORTED_MSG 891

int Id,world_size;
MPI_Status status;

void parallel_msort(int *next_array, int n);
void p_mergesort_helper(int *array, int n, int height);
int compare(const int *a, const int *b);
