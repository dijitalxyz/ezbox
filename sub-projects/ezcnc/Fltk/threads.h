#ifndef Threads_H
#  define Threads_H

#  if HAVE_PTHREAD_H
// Use POSIX threading...

#    include <pthread.h>

typedef pthread_t Fl_Thread;

static int fl_create_thread(Fl_Thread& t, void *(*f) (void *), void* p) {
  return pthread_create((pthread_t*)&t, 0, f, p);
}

#  elif defined(WIN32) && !defined(__WATCOMC__) // Use Windows threading...

#    include <windows.h>
#    include <process.h>

typedef unsigned long Fl_Thread;

static int fl_create_thread(Fl_Thread& t, void *(*f) (void *), void* p) {
  return t = (Fl_Thread)_beginthread((void( __cdecl * )( void * ))f, 0, p);
}

#  elif defined(__WATCOMC__)
#    include <process.h>

typedef unsigned long Fl_Thread;

static int fl_create_thread(Fl_Thread& t, void *(*f) (void *), void* p) {
  return t = (Fl_Thread)_beginthread((void(* )( void * ))f, 32000, p);
}
#  endif // !HAVE_PTHREAD_H
#endif // !Threads_h
