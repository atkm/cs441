//
// cycle.c
//
// A sample MPI program.
//
// This program has its processors act like a ring
// network.  They act in concert to permute/send an
// array of values around that ring.
//

#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>

#define BPRINTF_MSG 123
#define SHIFT_MSG   234

// 
// Some (*gasp*) global variables for MPI values---the 
// number of processors; the processor ID---used throughout
// the code.
int Pp,Ip;

// I use this everwhere an MPI call requires a status return.
MPI_Status status;

int shift(int value) {
  int recv_value;
  int to, from;

  to = (Ip + 1) % Pp;
  from = (Ip - 1 + Pp) % Pp;

  MPI_Send(&value,1,MPI_INT,to,SHIFT_MSG,MPI_COMM_WORLD);
  MPI_Recv(&recv_value,1,MPI_INT,from,SHIFT_MSG,MPI_COMM_WORLD,
	   &status);

  return recv_value;    
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

int main(int argc, char **argv) {

  int v;
  int round;
  char round_str[100];

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&Ip);
  MPI_Comm_size(MPI_COMM_WORLD,&Pp);

  v = Ip;

  mprintf("--------------\n");
  mprintf("Cycle program!\n");
  mprintf("--------------\n");
  bprintf("Initially:\n","%d\n",v);

  for (round=1; round < Pp; round++) {
    v = shift(v);
    sprintf(round_str,"After communication round %d:\n",round); 
    bprintf(round_str,"%d\n",v);
  }

  MPI_Finalize();
}

