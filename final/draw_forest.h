#include <stdio.h>
#include <stdlib.h>
#include <cairo/cairo.h>
#include <gtk/gtk.h>
#include <math.h>

void set_color_label(cairo_t *cr);
void set_color_rank(cairo_t *cr);
void set_color_arc(cairo_t *cr);
void set_color_circle(cairo_t *cr);
void set_background(cairo_t *cr, int width, int height);
void draw_circle(cairo_t *cr, double x, double y, int num, int rank);
void n_circles(cairo_t *cr, gpointer data, double interval);
