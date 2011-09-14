# ezcnc - Copyright (C) 2011 ezbox project
set(SRC
  fltkGui.cxx
  mainWindow.cxx
  simulateWindow.cxx
)
file(GLOB HDR RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)
append_ezcnc_src(Fltk "${SRC};${HDR}")
