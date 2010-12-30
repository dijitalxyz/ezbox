#include "ezus.h"
#include "init.h"

static volatile uint8_t i, j;
static __idata volatile uint8_t *dst, *src;
static volatile __bit thread_loaded;

/* this is an atomic operation */
__code void w_thread_context_switch(void) __using 2
{

#if 0
	/*
	 * move it from stack to data area
	 * do not mangle SP and stack info
	 */
	volatile uint8_t i, j;
	__idata volatile uint8_t *dst, *src;
	volatile __bit thread_loaded;
#endif

	/* dynamic load thread */
	i = wd.next_thread_id;
	j = wd.next_thread_id + 1;
	if (wd.thread_spb[i] == wd.thread_spb[j]) {
		thread_loaded = 0;
	}
	else {
		thread_loaded = 1;
	}

	i = wd.cur_thread_id + 1;
	/* j = wd.next_thread_id + 1; now */

	if (i < j) {
		/* move wd.thread_spb[i] ~ wd.thread_spb[j] to W_SP+1 */
		src = wd.thread_spb[i];
		dst =  (__idata volatile uint8_t *)W_SP + 1;
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
		/* move wd.thread_spb[j] ~ W_SP+1 to wd.thread_spb[i]-1 */
		src = (__idata volatile uint8_t *)W_SP + 1;
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
	W_SP = (__sfr volatile)(wd.thread_spb[wd.cur_thread_id]) - 1;
}
