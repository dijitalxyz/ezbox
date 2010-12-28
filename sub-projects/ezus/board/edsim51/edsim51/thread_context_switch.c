#include "ezus.h"
#include "init.h"

/* this is an atomic operation */
__code void w_thread_context_switch(void) __using 2
{
	if (wd.next_thread_id == 0) {
		init();
	}
	else if (wd.next_thread_id == 1) {
		thread1();
	}
	wd.cur_thread_id = wd.next_thread_id;
	wd.next_thread_id = 0;
}
