#include "ezus.h"
#include "init.h"

/* this is an atomic operation */
__code void w_thread_context_switch(void) __using 2
{
	uint8_t i, j;
	__idata volatile uint8_t *dst, *src;

	if (wd.cur_thread_id < wd.next_thread_id) {
		i = wd.cur_thread_id + 1;
		j = wd.next_thread_id + 1;
		/* move wd.thread_spb[i] ~ wd.thread_spb[j] to SP+1 */
		src = wd.thread_spb[i];
		dst =  (__idata volatile uint8_t *)SP + 1;
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
		/* move wd.thread_spb[j] ~ SP+1 to wd.thread_spb[i]-1 */
		src = (__idata volatile uint8_t *)SP + 1;
		dst = wd.thread_spb[i] - 1;
		while(src != wd.thread_spb[j]) {
			*dst-- = *src--;
		}
		/* adjust wd.thread_spb[i] ~ wd.thread_spb[j] */
		while(j < i) {
			wd.thread_spb[j] += (dst - src);
			j++;
		}
	}

	wd.cur_thread_id = wd.next_thread_id;
	wd.next_thread_id = 0;
	SP = (__sfr volatile)(wd.thread_spb[wd.cur_thread_id]) - 1;
}
