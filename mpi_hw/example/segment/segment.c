//
// segment.c
//
// Contains an implementation of a segmented parallel
// prefix operation, written using MPI.
//
// A segmented parallel prefix operation is 

#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>

#define BPRINTF_MSG 123
#define SPP_MSG 234

// 
// Some (*gasp*) global variables for MPI values---the 
// number of processors; the processor ID---used throughout
// the code.
int Pp,Ip;

// I use this everwhere an MPI call requires a status return.
MPI_Status status;


//
// segment_record_t
//
// Defines a (value,status) pair for a segmented parallel prefix
// operation.
//
// For example, a status of 1 denotes an "active" participant 
// in the parallel prefix, meaning that the processor is at
// the left end of a prefix segment.  A status of 0 ("passive")
// denotes a processor that is within a prefix segment.
//
// The other portion of the record stores the value held by that
// processor, the one to be acted upon by the prefix operation.
//
typedef struct _segment_record_t {
  int value;
  int active;
} segment_record_t;

//
// segment_op
//
// Defines a segmented parallel prefix binary operation
// given by
//
//    (L,a) @ (R,1) := (R,1)
//    (L,a) @ (R,0) := (L o R,a)
//
// where @ is the segment associative operation segment_op
// and o is the given associative operation op.
//
segment_record_t segment_op(segment_record_t left, 
			    segment_record_t right,
			    int (*op)(int,int)) {
  if (right.active) {
    return right;
  } else {
    segment_record_t result;
    result.active = left.active;
    result.value = op(left.value,right.value);
    return result;
  }
}

// segmented_prefix(value,active,op)
//
// This performs a segmented parallel prefix operation using the
// standard non-optimal (in terms of total work) parallel prefix 
// algorithm.  
//
// It is given a value for this processor, along with the segment
// status (active == TRUE; passive == FALSE) of this processor.
// 
// The result returned is the integer resulting from the segmented
// prefix operation.
//
int segmented_prefix(int value, int active,
		     int (*op)(int, int)) {
  int stride;
  segment_record_t vs,vr;
  vs.active = active;
  vs.value  = value;
  for (stride = 1; stride < Pp; stride *=2) {
    int to = Ip + stride;
    int from = Ip - stride;
    if (to < Pp) {
      MPI_Send(&vs,2,MPI_INT,to,SPP_MSG,MPI_COMM_WORLD);
    }
    if (from >= 0) {
      MPI_Recv(&vr,2,MPI_INT,from,SPP_MSG,MPI_COMM_WORLD,&status);
      vs = segment_op(vr,vs,op);
    }
  }
  return vs.value;
}

// mprintf
//
// Stands for "master printf".  The effect of this call is
// for the processor with ID of 0 (running on the front end)
// to print the given string.  The other processors do nothing.
//
void mprintf(char *string) {
  if (Ip == 0) {
    printf(string);
  }
}

// bprintf
//
// Stands for "broadcast printf".  Using this, each processor
// reports a value v according to a format string (with an 
// embedded %d) provided.  The collection of reported values
// is preceded by a header string.  Each processor's value
// is reported along with that processor's ID.
//
void bprintf(char *header, const char *format, int v) {
  int i;
  if (Ip != 0) {
    MPI_Send(&v,1,MPI_INT,0,BPRINTF_MSG,MPI_COMM_WORLD);
  } else {
    printf(header);
    printf("\t%02d:",0);
    printf(format,v);
    for (i=1; i<Pp; i++) {
      MPI_Recv(&v,1,MPI_INT,i,BPRINTF_MSG,MPI_COMM_WORLD,&status);
      printf("\t%02d:",i);
      printf(format,v);
    }
    printf("\n");
  }
}


int plus_op(int left, int right) {
  return left + right;
}

// This main function tests the segmented parallel prefix code
// written above.  It takes two arguments: a string of decimal
// digits and a string of bits (t or f).  The first serve as the
// value ensemble being acted upon, the second define the segments
// in the parallel prefix computation.  The length of each string
// should be equal to the number of processes.
//
// usage:
//    segment <digits> <bits>
// 
// E.g.
//    segment 16571894 tffttfff
//
int main(int argc, char **argv) {

  int Xs,As,Ws;

  MPI_Init(&argc,&argv);
  // initialize global values
  MPI_Comm_rank(MPI_COMM_WORLD,&Ip);  // processor id
  MPI_Comm_size(MPI_COMM_WORLD,&Pp);  // number of processors

  mprintf(argv[1]);
  // initialize the segment record for this processor
  if (argv[1][Ip] == 't') {
    As = 1;
  } else {
    As = 0;
  }
  Xs = atoi(argv[2+Ip]);

  // report the input
  bprintf("Before values  :\n","%d\n",Xs);
  bprintf("Before segments:\n","%d\n",As);

  // compute a segmented sum
  Ws = segmented_prefix(Xs,As,plus_op);

  // report the result
  bprintf("After          :\n","%d\n",Ws);

  MPI_Finalize();
}

