#include "ezus.h"
#include "init.h"

/* this is an atomic operation */
__code void w_thread_context_switch(void) __using 2
{
#if 0
	uint8_t i, j;
	volatile uint8_t *dst, *src;
	volatile char sp;

	if (wd.cur_thread_id < wd.next_thread_id) {
		i = wd.cur_thread_id + 1;
		j = wd.next_thread_id + 1;
		/* move wd.thread_spb[i] ~ wd.thread_spb[j] to SP */
		src = wd.thread_spb[i];
		sp = SP;
		/* dst = (volatile uint8_t *)(SP + 1); */
		dst = &sp;
		while(src != wd.thread_spb[j]) {
			*dst++ = *src++;
		}
		/* adjust wd.thread_spb[i] ~ wd.thread_spb[j] */
		while(i < j) {
			wd.thread_spb[i] -= (src - dst);
			i++;
		}
	}
	else {
		i = wd.cur_thread_id + 1;
		j = wd.next_thread_id + 1;
	}
#endif

	wd.cur_thread_id = wd.next_thread_id;
	wd.next_thread_id = 0;
}
