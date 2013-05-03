#include <stdio.h>
#include <stdlib.h>
#include <cairo/cairo.h>
#include <gtk/gtk.h>
#include <math.h>

void set_color_black(cairo_t *cr){
  cairo_set_source_rgb (cr, 0, 0, 0);
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

void draw_circle(cairo_t *cr, double x, double y, int num){
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
  set_color_black(cr);
  char label[5];
  snprintf(label, 10, "%d", num);
  cairo_move_to(cr, -12, 8);
  cairo_show_text(cr, label);
  cairo_rel_move_to(cr, 12, -8);
  cairo_fill(cr);

}

void n_circles(cairo_t *cr, int n, double interval){
  // Draw n circles
  draw_circle(cr, interval*1.5, interval, 0);
  int i;
  for (i=1; i<n; i++){
    if (i%8 == 0){
      cairo_translate(cr, (-1)*interval*(8 + ((i/8)%2)) , interval);
      draw_circle(cr, interval*1.5, 0, i);
    }
    else
      draw_circle(cr, interval, 0, i);
  }
}


  static gboolean
on_draw_event(GtkWidget *widget, cairo_t *cr, 
    gpointer user_data)
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
  int n = 9;
  n_circles(cr, n, separation);

  // text
  //cairo_text_extents_t te;
  //const char *utf8 = "cairo";
  //set_color_black(cr);
  //cairo_text_extents (cr, utf8, &te);

  //cairo_move_to (cr, 0, 0);
  //cairo_show_text (cr, "CAIRO");

  // Done
  cairo_destroy(cr);

  return FALSE;
}



int main (int argc, char *argv[])
{

  GtkWidget *window;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  g_signal_connect(G_OBJECT(window), "draw", 
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(G_OBJECT(window), "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 200); 

  gtk_widget_set_app_paintable(window, TRUE);
  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
