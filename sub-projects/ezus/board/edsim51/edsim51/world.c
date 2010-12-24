#include <stdlib.h>
#include <stdint.h>
#include <8051.h>

#include "world.h"
#include "init.h"

/*
 * world layer is defined by user's requirement
 * and implemented by system programmer.
 * EdSim51 porting maps world layer onto universe layer directly
 * and provides services to user applications.
 */

__code void w_space_init(__data world_data_t *dp) __using 2
{
	dp->wid = 0;
}

__code void w_time_init(__data world_data_t *dp) __using 2
{
#if 0
	/* use timer 0 as time tick source */
	TMOD &= 0xf0;
	/* Mode 1 : 16-bit mode */
	TMOD |= 0x01;
	/* put timer interval */
	TL0 = (TIMER0_COUNT & 0x00ff);
	TH0 = (TIMER0_COUNT >> 8);
	/* low priority */
	PT0 = 0;
	/* enable timer 0 interrupt */
	ET0 = 1;
	/* start timer 0 */
	TR0 = 1;
#endif

	dp->time_ticks = 0;
	dp->uptime_seconds = 0;
}

__code void (* w_threads[W_THREAD_NUM])(void) = {
	init,
	thread1
};

__code void w_thread_init(__data world_data_t *dp) __using 2
{
	uint8_t i;

	dp->wid = 1;

	dp->cur_thread_id = 0;

	dp->thread_quantum[0] = 10;	/* 10 ticks for init() */

	i = W_THREAD_NUM - 1;
	while (i > 0) {
		dp->thread_quantum[i] = 10;	/* 10 ticks for a thread running */
		i--;
	}
}

__code void w_thread_context_switch(
	__code const world_rules_t *rp,
	__data world_data_t *dp,
	__data uint8_t i) __using 2
{
	dp->wid = 0;
	rp->threads[i]();
}

__code void w_thread_schedule(__code const world_rules_t *rp,
                              __data world_data_t *dp) __using 2
{
	__data uint8_t i;

	dp->wid = 0;

	i = W_THREAD_NUM - 1;
	while (i > 0) {
		if ((dp->thread_quantum)[i] > 0) {
			(rp->thread_context_switch)(rp, dp, i);
			/* after context switch, SP point to i-th thread */
			return;
		}
		i--;
	}
}

__code void w_startup(__code const world_rules_t *rp,
                      __data world_data_t *dp) __using 2
{
	dp->wid = 0;

	/* setup the space of this world */
	rp->space_init(dp);

	/* setup the time of this world */
	rp->time_init(dp);

	/* setup threads in this world */
	rp->thread_init(dp);

	/* start up the user's applications */
#if 1
	rp->thread_schedule(rp, dp);
#endif

#if 0
	/* init() is an ever-lasting loop routine */
	init(wid);
#endif

	/* should not reach here in a normal running */
}

#if 0
void world(uint8_t wid) __using 2
{
	/* setup the space of this world */
	w_space_init(wid);

	/* setup the time of this world */
	w_time_init(wid);

	/* setup threads in this world */
	w_thread_init(wid);

	/* start up the user's applications */
	/* init() is an ever-lasting loop routine */
	init();

	/* should not reach here in a normal running */
}
#endif
