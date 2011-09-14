// ezcnc - Copyright (C) 2011 ezbox project.

#include <sstream>
#include <string.h>
#include <inttypes.h>
#include <FL/Fl.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_File_Icon.H>
#include <FL/x.H>
#include "EzcncThreads.h"
#include "fltkGui.h"
#include "mainWindow.h"
#include "simulateWindow.h"

fltkGui::fltkGui(int argc, char **argv)
{
  main = new mainWindow();
  main->show();
  sim = new simulateWindow();
}

fltkGui *fltkGui::_instance = NULL;

fltkGui *fltkGui::instance(int argc, char **argv)
{
  if(!_instance){
    _instance = new fltkGui(argc, argv);
  }
  return _instance;
}

int fltkGui::run()
{
  return Fl::run();
}

void fltkGui::show_simulate_window()
{
  sim->show();
}
