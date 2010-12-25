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

#if 0
__code void (* w_threads[W_THREAD_NUM])(void) = {
	init,
	thread1
};
#endif

__code void w_thread_init(__data world_data_t *dp) __using 2
{
	uint8_t i;

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

	i = W_THREAD_NUM - 1;
	while (i > 0) {
		if ((dp->thread_quantum)[i] > 0) {
			if (dp->cur_thread_id < i) {
				/*
				 * the priority of current running thread is lower
				 * than target thread, switch thread context
				 */
				(rp->thread_context_switch)(rp, dp, i);
				/*
				 * after context switch, SP point to i-th thread
				 * tag i-th thread as current running thread
				 */
				dp->cur_thread_id = i;
			}
			return;
		}
		i--;
	}
	/* running default thread */
	(rp->thread_context_switch)(rp, dp, 0);
	dp->cur_thread_id = 0;
}

__code void w_startup(__code const world_rules_t *rp,
                      __data world_data_t *dp) __using 2
{
	/* setup the space of this world */
	rp->space_init(dp);

	/* setup the time of this world */
	rp->time_init(dp);

	/* setup threads in this world */
	rp->thread_init(dp);

	/* enable interrupts */
	EA = 1;

	/* start up the user's applications */
	while (1) {
		rp->thread_schedule(rp, dp);
	}
	
	/* should not reach here in a normal running */
}
