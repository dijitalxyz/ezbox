# ezcnc - Copyright (C) 2011 ezbox project
set(SRC )
file(GLOB HDR RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)
append_ezcnc_src(Common "${SRC};${HDR}")
