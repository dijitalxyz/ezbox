# ezcnc - Copyright (C) 2011 ezbox project

set(SRC
  Fl_Native_File_Chooser.cxx
)

file(GLOB_RECURSE HDR RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.H)
append_ezcnc_src(contrib/NativeFileChooser "${SRC};${HDR}")
