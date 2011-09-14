// ezcnc - Copyright (C) 2011 ezbox project.

#ifndef _EZCNC_THREADS_H
#define _EZCNC_THREADS_H

// Use POSIX threading...
#include <pthread.h>

typedef pthread_t Fl_Thread;

static int fl_create_thread(Fl_Thread& t, void *(*f) (void *), void* p) {
  return pthread_create((pthread_t*)&t, 0, f, p);
}

#endif
