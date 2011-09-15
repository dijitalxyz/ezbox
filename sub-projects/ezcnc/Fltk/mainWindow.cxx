// ezcnc - Copyright (C) 2011 ezbox project.

#include <string.h>
#include <errno.h>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/math.h>
#include "fltkGui.h"
#include "mainWindow.h"

#define TS 14 // default editor textsize

void save_cb(Fl_Widget*, void* v);
void saveas_cb(Fl_Widget*, void* v);

int check_save(mainWindow *v) {
  if (!v->changed) return 1;

  int r = fl_choice("The current file has not been saved.\n"
                    "Would you like to save it now?",
                    "Cancel", "Save", "Don't Save");

  if (r == 1) {
    save_cb(NULL, v); // Save the file...
    return !v->changed;
  }

  return (r == 2) ? 1 : 0;
}

void load_file(mainWindow *v, const char *newfile, int ipos) {
  int insert = (ipos != -1);
  int r;

  v->loading = 1;
  v->changed = insert;
  if (!insert) strcpy(v->filename, "");
  if (!insert) r = v->textbuf->loadfile(newfile);
  else r = v->textbuf->insertfile(newfile, ipos);
#if (FL_MAJOR_VERSION == 1) && (FL_MINOR_VERSION == 3)
  v->changed = v->changed || v->textbuf->input_file_was_transcoded;
#endif
  if (r)
    fl_alert("Error reading from file \'%s\':\n%s.", newfile, strerror(errno));
  else
    if (!insert) strcpy(v->filename, newfile);
  v->loading = 0;
  v->textbuf->call_modify_callbacks();
}

void save_file(mainWindow *v, const char *newfile) {
  if (v->textbuf->savefile(newfile))
    fl_alert("Error writing to file \'%s\':\n%s.", newfile, strerror(errno));
  else
    strcpy(v->filename, newfile);
  v->changed = 0;
  v->textbuf->call_modify_callbacks();
}

void changed_cb(int, int nInserted, int nDeleted, int, const char*, void* v) {
  mainWindow *w = (mainWindow *)v;
  if ((nInserted || nDeleted) && !w->loading) w->changed = 1;
  w->set_title();
  if (w->loading) w->editor->show_insert_position();
}


void new_cb(Fl_Widget*, void* v) {
  mainWindow *w = (mainWindow *)v;

  if (!check_save(w)) return;

  w->filename[0] = '\0';
  w->textbuf->select(0, w->textbuf->length());
  w->textbuf->remove_selection();
  w->changed = 0;
  w->textbuf->call_modify_callbacks();
}

void open_cb(Fl_Widget*, void* v) {
  mainWindow *w = (mainWindow *)v;
  Fl_Native_File_Chooser fnfc;

  if (!check_save(w)) return;

  fnfc.title("Open file");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
  if ( fnfc.show() ) return;
  load_file(w, fnfc.filename(), -1);
}

void insert_cb(Fl_Widget*, void*) {
}

void save_cb(Fl_Widget*, void* v) {
  mainWindow *m = (mainWindow *)v;

  if (m->filename[0] == '\0') {
    // No filename - get one!
    saveas_cb(NULL, v);
    return;
  }
  else save_file(m, m->filename);
}

void saveas_cb(Fl_Widget*, void* v) {
  Fl_Native_File_Chooser fnfc;
  fnfc.title("Save File As?");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  if ( fnfc.show() ) return;
  save_file((mainWindow *)v, fnfc.filename());
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
  _title = "Untitled";
  changed = 0;
  loading = 0;
  textbuf = new Fl_Text_Buffer;

  win = new Fl_Double_Window(660, 400, _title.c_str());
  win->begin();
    menubar = new Fl_Menu_Bar(0, 0, 660, 30);
    menubar->copy(menuitems, this);
    editor = new Fl_Text_Editor(0, 30, 660, 370);
    editor->textfont(FL_COURIER);
    editor->textsize(TS);
    editor->buffer(textbuf);
    textbuf->text();
  win->end();
  win->resizable(editor);

  textbuf->add_modify_callback(changed_cb, this);
  textbuf->call_modify_callbacks();
}

void mainWindow::set_title() {
  if (filename[0] == '\0') {
    _title = "Untitled";
  }
  else {
    char *slash;
    slash = strrchr(filename, '/');
#ifdef WIN32
    if (slash == NULL) slash = strrchr(filename, '\\');
#endif
    if (slash != NULL) _title = (slash + 1);
    else _title = filename;
  }

  if (changed) _title += " (modified)";

  win->label(_title.c_str());
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
