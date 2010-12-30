#include "ezus.h"

__code void w_thread_init(void) __using 2
{
	uint8_t i;
	__idata volatile uint8_t *dst;

	wd.thread_spb[W_THREAD_NUM] = (__idata volatile uint8_t *)W_SP_TOP;

	i = W_THREAD_NUM - 1;
	while(i > 0) {
		/* 0 tick for time to start, running immediately */
		wd.thread_tts[i] = 0;

		/* build init running stack for thread-i */
		dst = wd.thread_spb[i+1] - 1;
		*dst-- = (((uint16_t)(w_threads[i])) >> 8);
		*dst   = (((uint16_t)(w_threads[i])) & 0x00ff);
		wd.thread_spb[i] = (__idata volatile uint8_t *)dst;

		i-- ;
	}
	wd.cur_thread_id = 0;
	wd.next_thread_id = 0;
}
