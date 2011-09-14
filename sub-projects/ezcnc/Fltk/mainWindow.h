// ezcnc - Copyright (C) 2011 ezbox project.

#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <string>
#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/filename.H>

class mainWindow{
 private:
  std::string _title;
 public:
  Fl_Double_Window *win;
  Fl_Text_Editor *editor;
  Fl_Text_Buffer *textbuf;
  Fl_Menu_Bar* menubar;
  char filename[FL_PATH_MAX];
 public:
  mainWindow();
  ~mainWindow();
  void show();
};

#endif
