#include "EzcncConfig.h"

#if (defined(HAVE_PTHREAD) || defined(WIN32))
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Value_Output.H>
#include <FL/fl_ask.H>
#include "EzcncThreads.h"
#include <stdio.h>
#include <math.h>
#include "fltkGui.h"

int main(int argc, char **argv)
{
  // create the GUI
  fltkGui::instance(argc, argv);

  // display GUI immediately for quick launch time
  fltkGui::instance()->check();

  // loop
  return fltkGui::instance()->run();
}
#else
#include <FL/fl_ask.H>

int main(int argc, char **argv) {
  fl_alert("Sorry, threading not supported on this platform!");
}
#endif // HAVE_PTHREAD || WIN32
