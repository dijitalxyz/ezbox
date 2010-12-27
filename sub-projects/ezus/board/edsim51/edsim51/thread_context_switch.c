#include <stdlib.h>
#include <stdint.h>
#include <8051.h>

#include "world.h"
#include "init.h"

__code void w_thread_context_switch(__data world_data_t *dp) __using 2
{
	if (dp->next_thread_id == 0) {
		init();
	}
	else if (dp->next_thread_id == 1) {
		thread1();
	}
	dp->cur_thread_id = dp->next_thread_id;
	dp->next_thread_id = 0;
}
