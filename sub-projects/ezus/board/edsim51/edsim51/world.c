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

/*
 * world global variables
 */
#if 0
__code const world_rules_t wr = {
	w_space_init,
	w_time_init,
	{
		init, /* default thread */
		thread1
	},
	w_thread_init,
	w_thread_context_switch,
	w_thread_schedule,
	w_startup
};

__code void (* w_threads[W_THREAD_NUM])(void) = {
	init,
	thread1
};
#endif

__data volatile world_data_t wd;


__code void w_space_init(void) __using 2
{
	wd.wid = 0;
}

__code void w_time_init(void) __using 2
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

	wd.time_ticks = 0;
	wd.uptime_seconds = 0;
}

__code void w_thread_init(void) __using 2
{
	wd.cur_thread_id = 0;
	while(wd.cur_thread_id < W_THREAD_NUM) {
		/* 10 ticks for a thread running */
		wd.thread_quantum[wd.cur_thread_id] = 10;
		wd.cur_thread_id++ ;
	}
	wd.cur_thread_id = 0;
	wd.next_thread_id = 0;
}

__code void w_thread_schedule(void) __using 2
{
	wd.next_thread_id = W_THREAD_NUM - 1;
	while (wd.next_thread_id > 0) {
		if ((wd.thread_quantum)[wd.next_thread_id] > 0) {
			if (wd.cur_thread_id < wd.next_thread_id) {
				/*
				 * the priority of current running thread is lower
				 * than target thread, switch thread context
				 */
				w_thread_context_switch();
				/*
				 * after context switch, SP point to i-th thread
				 * tag i-th thread as current running thread
				 */
			}
			return;
		}
		wd.next_thread_id--;
	}
	/* running default thread */
	/* wd.next_thread_id == 0 now */
	w_thread_context_switch();
}

__code void w_startup(void) __using 2
{
	/* setup the space of this world */
	w_space_init();

	/* setup the time of this world */
	w_time_init();

	/* setup threads in this world */
	w_thread_init();

	/* enable interrupts */
	EA = 1;

	/* start up the user's applications */
	init();
#if 0
	while (1) {
		w_thread_schedule();
	}
#endif
	
	/* should not reach here in a normal running */
}
