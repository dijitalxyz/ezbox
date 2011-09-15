# ezcnc - Copyright (C) 2011 ezbox project

set(SRC
  Fl_Tree.cxx
  Fl_Tree_Item.cxx
  Fl_Tree_Item_Array.cxx
  Fl_Tree_Prefs.cxx
)

file(GLOB_RECURSE HDR RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.H)
append_ezcnc_src(contrib/Fl_Tree "${SRC};${HDR}")
