// ezcnc - Copyright (C) 2011 ezbox project.

#include <string.h>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/math.h>
#include "fltkGui.h"
#include "mainWindow.h"

#define TS 14 // default editor textsize

void new_cb(Fl_Widget*, void*) {
}

void open_cb(Fl_Widget*, void*) {
}

void insert_cb(Fl_Widget*, void*) {
}

void save_cb(Fl_Widget*, void*) {
}

void saveas_cb(Fl_Widget*, void*) {
}

void view_cb(Fl_Widget*, void*) {
}

void close_cb(Fl_Widget*, void*) {
}

void quit_cb(Fl_Widget*, void*) {
}

void cut_cb(Fl_Widget*, void*) {
}

void copy_cb(Fl_Widget*, void*) {
}

void paste_cb(Fl_Widget*, void*) {
}

void delete_cb(Fl_Widget*, void*) {
}

void find_cb(Fl_Widget*, void*) {
}

void find2_cb(Fl_Widget*, void*) {
}

void replace_cb(Fl_Widget*, void*) {
}

void replace2_cb(Fl_Widget*, void*) {
}

void simulate_cb(Fl_Widget*, void*) {
  fltkGui::instance()->show_simulate_window();
}

Fl_Menu_Item menuitems[] = {
  { "&File",              0, 0, 0, FL_SUBMENU },
    { "&New File",        0, (Fl_Callback *)new_cb },
    { "&Open File...",    FL_COMMAND + 'o', (Fl_Callback *)open_cb },
    { "&Insert File...",  FL_COMMAND + 'i', (Fl_Callback *)insert_cb, 0, FL_MENU_DIVIDER },
    { "&Save File",       FL_COMMAND + 's', (Fl_Callback *)save_cb },
    { "Save File &As...", FL_COMMAND + FL_SHIFT + 's', (Fl_Callback *)saveas_cb, 0, FL_MENU_DIVIDER },
    { "New &View",        FL_ALT
#ifdef __APPLE__
      + FL_COMMAND
#endif
      + 'v', (Fl_Callback *)view_cb, 0 },
    { "&Close View",      FL_COMMAND + 'w', (Fl_Callback *)close_cb, 0, FL_MENU_DIVIDER },
    { "E&xit",            FL_COMMAND + 'q', (Fl_Callback *)quit_cb, 0 },
    { 0 },

  { "&Edit", 0, 0, 0, FL_SUBMENU },
    { "Cu&t",             FL_COMMAND + 'x', (Fl_Callback *)cut_cb },
    { "&Copy",            FL_COMMAND + 'c', (Fl_Callback *)copy_cb },
    { "&Paste",           FL_COMMAND + 'v', (Fl_Callback *)paste_cb },
    { "&Delete",          0, (Fl_Callback *)delete_cb },
    { 0 },

  { "&Search", 0, 0, 0, FL_SUBMENU },
    { "&Find...",         FL_COMMAND + 'f', (Fl_Callback *)find_cb },
    { "F&ind Again",      FL_COMMAND + 'g', (Fl_Callback *)find2_cb },
    { "&Replace...",      FL_COMMAND + 'r', (Fl_Callback *)replace_cb },
    { "Re&place Again",   FL_COMMAND + 't', (Fl_Callback *)replace2_cb },
    { 0 },

  { "&Tools", 0, 0, 0, FL_SUBMENU },
    { "Si&mulate",      FL_COMMAND + 'g', (Fl_Callback *)simulate_cb },
    { 0 },

  { 0 }
};

mainWindow::mainWindow()
{
  textbuf = new Fl_Text_Buffer;
  _title = "Untitled";
  win = new Fl_Double_Window(660, 400, _title.c_str());
  win->begin();
    menubar = new Fl_Menu_Bar(0, 0, 660, 30);
    menubar->copy(menuitems, win);
    editor = new Fl_Text_Editor(0, 30, 660, 370);
    editor->textfont(FL_COURIER);
    editor->textsize(TS);
    editor->buffer(textbuf);
    textbuf->text();
  win->end();
  win->resizable(editor);
}

mainWindow::~mainWindow()
{
  win->clear();
  Fl::delete_widget(win);
}

void mainWindow::show()
{
  win->show();
}
