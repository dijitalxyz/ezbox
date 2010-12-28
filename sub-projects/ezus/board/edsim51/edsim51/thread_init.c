#include "ezus.h"

__code void w_thread_init(void) __using 2
{
	uint8_t i;
	i = 0;
	while(i < W_THREAD_NUM) {
		/* 0 tick for time to start, running immediately */
		wd.thread_tts[i] = 0;
		i++ ;
	}
	wd.cur_thread_id = 0;
	wd.next_thread_id = 0;
}
