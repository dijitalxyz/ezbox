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
void find2_cb(Fl_Widget* dw, void* v);

int check_save(mainWindow*v) {
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

void load_file(mainWindow*v, const char *newfile, int ipos) {
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

void save_file(mainWindow*v, const char *newfile) {
  if (v->textbuf->savefile(newfile))
    fl_alert("Error writing to file \'%s\':\n%s.", newfile, strerror(errno));
  else
    strcpy(v->filename, newfile);
  v->changed = 0;
  v->textbuf->call_modify_callbacks();
}

void changed_cb(int, int nInserted, int nDeleted, int, const char*, void* v) {
  mainWindow* w = (mainWindow*)v;
  if ((nInserted || nDeleted) && !w->loading) w->changed = 1;
  w->set_title();
  if (w->loading) w->editor->show_insert_position();
}


void new_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;

  if (!check_save(w)) return;

  w->filename[0] = '\0';
  w->textbuf->select(0, w->textbuf->length());
  w->textbuf->remove_selection();
  w->changed = 0;
  w->textbuf->call_modify_callbacks();
}

void open_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;
  Fl_Native_File_Chooser fnfc;

  if (!check_save(w)) return;

  fnfc.title("Open file");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
  if ( fnfc.show() ) return;
  load_file(w, fnfc.filename(), -1);
}

void insert_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;
  Fl_Native_File_Chooser fnfc;

  fnfc.title("Insert file");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
  if ( fnfc.show() ) return;
  load_file(w, fnfc.filename(), w->editor->insert_position());
}

void save_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;

  if (w->filename[0] == '\0') {
    // No filename - get one!
    saveas_cb(NULL, v);
    return;
  }
  else save_file(w, w->filename);
}

void saveas_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;
  Fl_Native_File_Chooser fnfc;

  fnfc.title("Save File As?");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  if ( fnfc.show() ) return;
  save_file(w, fnfc.filename());
}

void quit_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;

  if (w->changed && !check_save(w))
    return;

  exit(0);
}

void cut_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;
  Fl_Text_Editor::kf_cut(0, w->editor);
}

void copy_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;
  Fl_Text_Editor::kf_copy(0, w->editor);
}

void paste_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;
  Fl_Text_Editor::kf_paste(0, w->editor);
}

void delete_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;
  w->textbuf->remove_selection();
}

void find_cb(Fl_Widget* dw, void* v) {
  mainWindow* w = (mainWindow*)v;
  const char *val;

  val = fl_input("Search String:", w->search);
  if (val != NULL) {
    // User entered a string - go find it!
    strcpy(w->search, val);
    find2_cb(dw, v);
  }
}

void find2_cb(Fl_Widget* dw, void* v) {
  mainWindow* w = (mainWindow*)v;
  if (w->search[0] == '\0') {
    // Search string is blank; get a new one...
    find_cb(dw, v);
    return;
  }

  int pos = w->editor->insert_position();
  int found = w->textbuf->search_forward(pos, w->search, &pos);
  if (found) {
    // Found a match; select and update the position...
    w->textbuf->select(pos, pos+strlen(w->search));
    w->editor->insert_position(pos+strlen(w->search));
    w->editor->show_insert_position();
  }
  else fl_alert("No occurrences of \'%s\' found!", w->search);
}

void replace_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;
  w->replace_dlg->show();
}

void replace2_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;
  const char *find = w->replace_find->value();
  const char *replace = w->replace_with->value();

  if (find[0] == '\0') {
    // Search string is blank; get a new one...
    w->replace_dlg->show();
    return;
  }

  w->replace_dlg->hide();

  int pos = w->editor->insert_position();
  int found = w->textbuf->search_forward(pos, find, &pos);

  if (found) {
    // Found a match; update the position and replace text...
    w->textbuf->select(pos, pos+strlen(find));
    w->textbuf->remove_selection();
    w->textbuf->insert(pos, replace);
    w->textbuf->select(pos, pos+strlen(replace));
    w->editor->insert_position(pos+strlen(replace));
    w->editor->show_insert_position();
  }
  else fl_alert("No occurrences of \'%s\' found!", find);
}

void replall_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;
  const char *find = w->replace_find->value();
  const char *replace = w->replace_with->value();

  find = w->replace_find->value();
  if (find[0] == '\0') {
    // Search string is blank; get a new one...
    w->replace_dlg->show();
    return;
  }

  w->replace_dlg->hide();

  w->editor->insert_position(0);
  int times = 0;

  // Loop through the whole string
  for (int found = 1; found;) {
    int pos = w->editor->insert_position();
    found = w->textbuf->search_forward(pos, find, &pos);

    if (found) {
      // Found a match; update the position and replace text...
      w->textbuf->select(pos, pos+strlen(find));
      w->textbuf->remove_selection();
      w->textbuf->insert(pos, replace);
      w->editor->insert_position(pos+strlen(replace));
      w->editor->show_insert_position();
      times++;
    }
  }

  if (times) fl_message("Replaced %d occurrences.", times);
  else fl_alert("No occurrences of \'%s\' found!", find);
}

void replcan_cb(Fl_Widget*, void* v) {
  mainWindow* w = (mainWindow*)v;
  w->replace_dlg->hide();
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

mainWindow::mainWindow()
{
  _title = "Untitled";
  *filename = (char)0;
  *search = (char)0;
  changed = 0;
  loading = 0;
  textbuf = new Fl_Text_Buffer;

  // replace dialog window
  replace_dlg = new Fl_Window(300, 105, "Replace");
  replace_dlg->begin();
    replace_find = new Fl_Input(80, 10, 210, 25, "Find:");
    replace_find->align(FL_ALIGN_LEFT);

    replace_with = new Fl_Input(80, 40, 210, 25, "Replace:");
    replace_with->align(FL_ALIGN_LEFT);

    replace_all = new Fl_Button(10, 70, 90, 25, "Replace All");
    replace_all->callback((Fl_Callback *)replall_cb, this);

    replace_next = new Fl_Return_Button(105, 70, 120, 25, "Replace Next");
    replace_next->callback((Fl_Callback *)replace2_cb, this);

    replace_cancel = new Fl_Button(230, 70, 60, 25, "Cancel");
    replace_cancel->callback((Fl_Callback *)replcan_cb, this);
  replace_dlg->end();
  replace_dlg->set_non_modal();

  // main operation window
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

mainWindow::~mainWindow()
{
  win->clear();
  Fl::delete_widget(win);
}

void mainWindow::show()
{
  win->show();
}
