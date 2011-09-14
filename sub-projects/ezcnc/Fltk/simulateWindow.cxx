// ezcnc - Copyright (C) 2011 ezbox project.

#include <string.h>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/math.h>
#include "fltkGui.h"
#include "simulateWindow.h"

shape_window::shape_window(int x,int y,int w,int h,const char *l) :
Fl_Gl_Window(x,y,w,h,l) {
  sides = overlay_sides = 3;
}

void shape_window::draw() {
// the valid() property may be used to avoid reinitializing your
// GL transformation for each redraw:
  if (!valid()) {
    valid(1);
    glLoadIdentity();
    glViewport(0,0,w(),h());
  }
// draw an amazing but slow graphic:
  glClear(GL_COLOR_BUFFER_BIT);
  //  for (int j=1; j<=1000; j++) {
    glBegin(GL_POLYGON);
    for (int j=0; j<sides; j++) {
      double ang = j*2*M_PI/sides;
      glColor3f(float(j)/sides,float(j)/sides,float(j)/sides);
      glVertex3f(cos(ang),sin(ang),0);
    }
    glEnd();
  // }
}

void shape_window::draw_overlay() {
// the valid() property may be used to avoid reinitializing your
// GL transformation for each redraw:
  if (!valid()) {
    valid(1);
    glLoadIdentity();
    glViewport(0,0,w(),h());
  }
// draw an amazing graphic:
  gl_color(FL_RED);
  glBegin(GL_LINE_LOOP);
  for (int j=0; j<overlay_sides; j++) {
    double ang = j*2*M_PI/overlay_sides;
    glVertex3f(cos(ang),sin(ang),0);
  }
  glEnd();
}

// when you change the data, as in this callback, you must call redraw():
void sides_cb(Fl_Widget *o, void *p) {
  shape_window *sw = (shape_window *)p;
  sw->sides = int(((Fl_Slider *)o)->value());
  sw->redraw();
}

void overlay_sides_cb(Fl_Widget *o, void *p) {
  shape_window *sw = (shape_window *)p;
  sw->overlay_sides = int(((Fl_Slider *)o)->value());
  sw->redraw_overlay();
}

simulateWindow::simulateWindow()
{
  _title = "Simulate";
  win = new Fl_Window(300,370);
  win->begin();
    swin = new shape_window(10, 75, win->w()-20, win->h()-90);
    win->resizable(swin);

    slider = new Fl_Hor_Slider(60, 5, win->w()-70, 30, "Sides:");
    slider->align(FL_ALIGN_LEFT);
    slider->callback(sides_cb,swin);
    slider->value(swin->sides);
    slider->step(1);
    slider->bounds(3,40);

    oslider = new Fl_Hor_Slider(60, 40, win->w()-70, 30, "Overlay:");
    oslider->align(FL_ALIGN_LEFT);
    oslider->callback(overlay_sides_cb,swin);
    oslider->value(swin->overlay_sides);
    oslider->step(1);
    oslider->bounds(3,40);
  win->end();
  win->label(_title.c_str());
}

simulateWindow::~simulateWindow()
{
  win->clear();
  Fl::delete_widget(win);
}

void simulateWindow::show()
{
  win->show();
  swin->show();
  swin->redraw_overlay();
}
