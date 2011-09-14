// ezcnc - Copyright (C) 2011 ezbox project.

#ifndef _FLTK_GUI_H_
#define _FLTK_GUI_H_

#include <string>
#include <vector>
#include <FL/Fl.H>

class mainWindow;
class simulateWindow;

class fltkGui{
 private:
  static fltkGui *_instance;
 public:
  mainWindow *main;
  simulateWindow *sim;
 public:
  fltkGui(int argc, char **argv);
  ~fltkGui(){}
  // return the single static instance of the GUI
  static fltkGui *instance(int argc=0, char **argv=NULL);
  // check if the GUI is available
  static bool available(){ return (_instance != NULL); }
  // check (now!) if there are any pending events, and process them
  static void check(){ Fl::check(); }
  // run the GUI until there's no window left
  static int run();
  // show simulate window
  void show_simulate_window();
};

#endif
