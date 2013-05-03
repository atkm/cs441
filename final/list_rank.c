#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>
#include "draw_forest.h"
#include "p_msort.h"

#define BPRINTF_MSG 123
// MSGs for list rank
#define RANK_MSG 234
#define PRED_MSG 345
#define NEXT_MSG 456
#define DRAW_MSG 567

#if !defined(MSORT_H)
int Id, world_size;
MPI_Status status;
#endif

// Prototypes
int list_rank(int next, int pred, int rank, int n);
int in_list(int *list, int p, int n);
void mprintf(char *string);
void bprintf(char *header, const char *format, int v);

// Drawing procedures
// Invoked when a GTK pane is started in main
  static gboolean
on_draw_event(GtkWidget *widget, cairo_t *cr, 
    gpointer *user_data)
{
  cr = gdk_cairo_create(gtk_widget_get_window(widget));

  int width, height;
  gtk_window_get_size(GTK_WINDOW(widget), &width, &height);

  int separation = width/10;
  // Set background color
  set_background(cr, width, height);

  // Set font
  cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cr, 24.0);
  // draw circles
  // user_data[0] = the number of nodes
  // user_data[1..-1] = ranks
  n_circles(cr, user_data, separation);

  // Done
  cairo_destroy(cr);

  return FALSE;
}


void main(int argc, char **argv){

  MPI_Init(&argc, &argv);
  // get processor rank
  MPI_Comm_rank(MPI_COMM_WORLD, &Id);
  // get the total number of processors
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  mprintf("--- LIST RANK ---\n");

  int n;
  n = argc - 1;
  // If there are more processors than required, shut down unnecessary ones.
  if (Id >= n){
    MPI_Finalize();
    exit(0);
  }

  // Get the arguments.
  int *next_array;
  next_array = malloc(n * sizeof(int));
  int i;
  for (i=0; i<n; i++){
    next_array[i] = atoi(argv[i+1]);
  }
  // Let each proc figure out its initial "next".
  int next;
  next = next_array[Id]; 
  bprintf("Successors (-1 means null)\n", "%d\n", next);

  /* Also let it figure out its predecessor.
     Unlike the pseudocode, each processor needs to know its predecessor to decide
     whether it should send rank and next at all, and if so, where.
     A simple look-up method (loop through the next array and see if ID is in there)
     would take O(n)-time. Instead, we use parallel merge-sort, which runs
     in O(log n)-time (O(nlog n)-work), to sort the next array,
     then employ sequential divide-and-conquer search method (also O(log n)-time) to determine membership. 
  */
  mprintf("Computing predecessors.... \n");

  /*
     The block is commented out because merge sort has a bug that
     makes it doesn't work for list of length 7 or larger.
     To use parallel mergesort to sort the list, 
   
  // MERGE SORT
  // Use parallel merge-sort to sort the 'next' array,
  parallel_msort(next_array, n);

  // After the sort, the master proc has the sorted array.
  // Broadcast the sorted array. This also takes O(log n)-time.
  MPI_Bcast(next_array, n, MPI_INT, 0, MPI_COMM_WORLD);

  */
  // Each proc sort the next_array the lame way
  qsort(next_array, n, sizeof(int), (int(*)(const void *, const void *))compare);

  // Talk to the successor if there is one.
  if (next!=-1)
    MPI_Send(&Id, 1, MPI_INT, next, PRED_MSG, MPI_COMM_WORLD);

  // If the processor ID is not in the next array, then it has no predecessor.
  int pred;
  if (!in_list(next_array, Id, n)){
    pred = -1;
  }
  // Else receive the predecessor ID from its predecessor
  // (MPI_ANY_SOURCE allows for receiving a message from any proc.)
  else {
    MPI_Recv(&pred, 1, MPI_INT, MPI_ANY_SOURCE, PRED_MSG, MPI_COMM_WORLD, &status);
  }
  bprintf("Predecessors (-1 means null)\n", "%d\n", pred);

  // Initialize "rank". If root, then rank = 0. Otherwise 1.
  int rank;
  if (next==-1)
    rank = 0;
  else
    rank = 1;

  // Compute ranks in parallel
  mprintf("Calculating ranks......... ");
  rank = list_rank(next, pred, rank, n);
  mprintf("Done!\n");
  bprintf("Ranks:\n", "%d\n", rank);

  // Collect ranks to master
  if (Id != 0) {
    MPI_Send(&rank,1,MPI_INT,0,DRAW_MSG,MPI_COMM_WORLD);
  }
  else {
    int other_rank;
    int *rank_list;
    // Make an array large enough to contain the number of nodes, ranks, and (initial) nexts
    rank_list = malloc((2 * world_size + 1) * sizeof(int));
    rank_list[0] = n;
    rank_list[1] = rank;
    for (i=1; i<world_size; i++) {
      MPI_Recv(&other_rank,1,MPI_INT,i,DRAW_MSG,MPI_COMM_WORLD,&status);
      rank_list[i+1] = other_rank;
    }
    for (i=0; i<world_size; i++) {
      rank_list[i + world_size + 1] = atoi(argv[i+1]);
    }

    printf("In rank list:");
    for (i=0; i<2*world_size+1; i++){
      printf("%d ", rank_list[i]);
    }
    printf("\n");

    /* Printing Phase */
    GtkWidget *window;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    // You can pass arguments to cairo using 'gpointer'.
    // The 0th element of the 'rank_list' is the length of the list.
    g_signal_connect(G_OBJECT(window), "draw", 
        G_CALLBACK(on_draw_event), (gpointer) rank_list);
    g_signal_connect(G_OBJECT(window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200); 

    gtk_widget_set_app_paintable(window, TRUE);
    gtk_widget_show_all(window);

    gtk_main();

    free(rank_list);
  }

  MPI_Finalize();
  return 0;
}

//
// list_rank
//
// In addition to the pseudocode, each proc has to know
// who the pred proc is in order to send its rank and next fields.
// Operations regarding preds add only constant time, so the running
// time and work are asymptotically unchanged.
// 
int list_rank(int next, int pred, int init_rank, int n){
  int rank = init_rank;
  int next_rank, next_next, pred_pred;

  // Loop until neither next nor pred exists.
  while (next!=-1 || pred!= -1){

    /* SEND PHASE */
    // Ensure all sends happen before receives

    // If the next pointer is not null, tell the next proc about pred proc.
    if (next!=-1){
      MPI_Send(&pred, 1, MPI_INT, next, PRED_MSG, MPI_COMM_WORLD);
    }

    // If the pred pointer is not null, tell the pred proc its rank and next proc
    // Note that during pointer jumping there is always only one "pred" node.
    if (pred!=-1){
      MPI_Send(&next, 1, MPI_INT, pred, NEXT_MSG, MPI_COMM_WORLD);
      MPI_Send(&rank, 1, MPI_INT, pred, RANK_MSG, MPI_COMM_WORLD);
    } 

    /* RECEIVE PHASE */
    // Receive next's rank and next, and set next to next's next; add next's rank to current rank.
    if (next!=-1){
      MPI_Recv(&next_rank, 1, MPI_INT, next, RANK_MSG, MPI_COMM_WORLD, &status);
      MPI_Recv(&next_next, 1, MPI_INT, next, NEXT_MSG, MPI_COMM_WORLD, &status);
      next = next_next;
      rank += next_rank;
    }
    // Receive pred of pred, then set pred to pred's pred.
    if (pred!=-1){
      MPI_Recv(&pred_pred, 1, MPI_INT, pred, PRED_MSG, MPI_COMM_WORLD, &status);
      pred = pred_pred;
    }

  }

  return rank;
}

//
// in_list
// Look at a sorted list, and determine whether an integer p is in it.
//
int in_list(int *array, int p, int n){
  // Do the comparison if the array has one element.
  if (n==1){
    if (p==array[0])
      return 1;
    else
      return 0;
  }
  // Determine the half-point.
  int half = n/2;
  // If p is the head of the given array, done.
  if (p == array[0])
    return 1;
  // Otherwise, determine which half p may belong to, 
  else 
    if (p >= array[half])
      return in_list(array + half, p, n - half);
    else
      return in_list(array, p, half);
}

//
// mprintf
//
// Stands for "master printf".  The effect of this call is
// for the processor with ID of 0 (running on the front end)
// to print the given string.  The other processors do nothing.
//
void mprintf(char *string) {
  if (Id == 0) {
    printf(string);
  }
}

//
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
  if (Id != 0) {
    MPI_Send(&v,1,MPI_INT,0,BPRINTF_MSG,MPI_COMM_WORLD);
  } else {
    printf(header);
    printf("\t%02d:",0);
    printf(format,v);
    for (i=1; i<world_size; i++) {
      MPI_Recv(&v,1,MPI_INT,i,BPRINTF_MSG,MPI_COMM_WORLD,&status);
      printf("\t%02d:",i);
      printf(format,v);
    }
    printf("\n");
  }
}
