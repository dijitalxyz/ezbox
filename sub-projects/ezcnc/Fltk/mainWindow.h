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
  Fl_Double_Window* win;
  Fl_Text_Editor* editor;
  Fl_Text_Buffer* textbuf;
  Fl_Menu_Bar* menubar;
  Fl_Window* replace_dlg;
  Fl_Input* replace_find;
  Fl_Input* replace_with;
  Fl_Button* replace_all;
  Fl_Return_Button* replace_next;
  Fl_Button* replace_cancel;

  char filename[FL_PATH_MAX];
  char search[256];
  int changed;
  int loading;
 public:
  mainWindow();
  ~mainWindow();
  void show();
  void set_title();
};

#endif
