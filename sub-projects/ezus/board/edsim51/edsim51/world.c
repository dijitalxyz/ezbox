#include <stdlib.h>
#include <stdint.h>
#include <8051.h>

#include "world.h"
#include "init.h"

/* world global variables */
/*-------------------------*/
uint32_t world_uptime_seconds;	/* seconds since world starts */
uint32_t world_time_ticks;	/* time ticks since last update of world_uptime_seconds */
uint8_t  world_time_flags;	/* bit 0: whether do w_schedule_threads() within one tick */

uint8_t world_cur_thread_id;	/* current running thread ID */

uint8_t world_thread_quantum[W_THREAD_NUM];	/* thread running quantum of ticks */

/*-------------------------*/

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

void (* const world_thread_list[W_THREAD_NUM])(void) = {
	init,
	thread1
};

void w_thread_context_switch(uint8_t tid) __using 2
{
	if (world_cur_thread_id == tid) {
		return ;
	}
}

#if 0
void w_time_int_service(void) __using 2
{
	uint8_t i;

	TF0 = 0;
	TL0 = TL0 + (TIMER0_COUNT & 0x00ff);
	TH0 = TH0 + (TIMER0_COUNT >> 8);

	world_time_ticks++;
	world_time_ticks %= HZ;

	/* ticks has been accumulated to one second */
	if (world_time_ticks == 0) {
		world_uptime_seconds++;
	}

	/* decrease threads quantum */
	i = W_THREAD_NUM - 1;
	while (i > 0) {
		if (world_thread_quantum[i] > 0) {
			world_thread_quantum[i]--;
		}
		i--;
	}

	/* signal scheduler */
	/* w_schedule_threads(); */
}
#endif

__code void w_thread_init(__data world_data_t *dp) __using 2
{
	uint8_t i;

	dp->wid = 1;

	world_cur_thread_id = 0;

	world_thread_quantum[0] = 10;	/* 10 ticks for init() */

	i = W_THREAD_NUM - 1;
	while (i > 0) {
		world_thread_quantum[i] = 10;	/* 10 ticks for a thread running */
		i--;
	}
}

__code void w_thread_schedule(__data world_data_t *dp) __using 2
{
	uint8_t i;

	dp->wid = 0;

	i = W_THREAD_NUM - 1;
	while (i > 0) {
		if (world_thread_quantum[i] > 0) {
			w_thread_context_switch(i);
			/* after context switch, SP point to i-th thread */
			return;
		}
		i--;
	}
}

__code void w_startup(__code world_rules_t *rp,
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
	rp->thread_schedule(dp);
#if 0
	/* init() is an ever-lasting loop routine */
	init(wid);
#endif

	/* should not reach here in a normal running */
}

/*
 * world layer is defined by user's requirement
 * and implemented by system programmer.
 * EdSim51 porting maps world layer onto universe layer directly
 * and provides services to user applications.
 */
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
