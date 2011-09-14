// ezcnc - Copyright (C) 2011 ezbox project.

#ifndef _SIMULATE_WINDOW_H_
#define _SIMULATE_WINDOW_H_

#include <string>
#include <vector>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/gl.h>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Hor_Slider.H>

class shape_window : public Fl_Gl_Window {
  void draw();
  void draw_overlay();
public:
  int sides;
  int overlay_sides;
  shape_window(int x,int y,int w,int h,const char *l=0);
};

class simulateWindow{
 private:
  std::string _title;
 public:
  Fl_Window *win;
  shape_window *swin;
  Fl_Hor_Slider *slider;
  Fl_Hor_Slider *oslider;
 public:
  simulateWindow();
  ~simulateWindow();
  void show();
};

#endif
