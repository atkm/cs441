#include "draw_forest.h"

void set_color_label(cairo_t *cr){
  cairo_set_source_rgb (cr, 0, 0, 0);
}
void set_color_rank(cairo_t *cr){
  cairo_set_source_rgb (cr, 0.7, 0, 0);
}
void set_color_arc(cairo_t *cr){
  cairo_set_source_rgb(cr, 0.2, 0.5, 0.4); 
}
void set_color_circle(cairo_t *cr){
  cairo_set_source_rgb(cr, 0.9, 0.98, 0.98);
}

void set_background(cairo_t *cr, int width, int height){
  cairo_set_source_rgb(cr,0.95,0.96,0.95);
  cairo_rectangle(cr,0,0,width,height);
  cairo_fill(cr);
}

//
// Draw a circle, put the name inside it.
// Write down the rank above it.
//
void draw_circle(cairo_t *cr, double x, double y, int num, int rank){
  // Draw circumference
  cairo_set_line_width(cr, 5);  
  set_color_arc(cr);
  cairo_translate(cr, x, y);
  cairo_arc(cr, 0, 0, 20, 0, 2 * M_PI);
  cairo_stroke_preserve(cr);

  // Fill
  set_color_circle(cr);
  cairo_fill(cr);

  // Label it.
  set_color_label(cr);
  char label[5];
  snprintf(label, 10, "%d", num);
  cairo_move_to(cr, -12, 8);
  cairo_show_text(cr, label);
  cairo_fill(cr);

  // Write a rank
  set_color_rank(cr);
  char depth[5];
  snprintf(depth, 10, "%d", rank);
  cairo_move_to(cr, 1, -28);
  cairo_show_text(cr, depth);
  cairo_rel_move_to(cr, 12, -8);
  cairo_rel_move_to(cr, -1, 28);
  cairo_fill(cr);
}

// Draw n circles
void n_circles(cairo_t *cr, gpointer data, double interval){
  int *rank_list = data;
  int rank = rank_list[1];
  // Start off with the first node
  draw_circle(cr, interval*1.5, interval, 0, rank);
  // Retrieve the number of nodes
  int n = rank_list[0];
  // Draw the rest
  int i;
  for (i=1; i<n; i++){
    rank = rank_list[i+1];
    if (i%8 == 0){
      cairo_translate(cr, (-1)*interval*(8 + ((i/8)%2)) , interval);
      draw_circle(cr, interval*1.5, 0, i, rank);
    }
    else
      draw_circle(cr, interval, 0, i, rank);
  }
}

