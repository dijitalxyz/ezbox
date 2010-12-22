#include <stdlib.h>
#include <stdint.h>
#include <8051.h>

#include "world.h"
#include "ezus.h"

/* world global variables */
/*-------------------------*/
uint32_t world_uptime_seconds;	/* seconds since world starts */
uint32_t world_time_ticks;	/* time ticks since last update of world_uptime_seconds */
uint8_t  world_time_flags;	/* bit 0: whether do w_schedule_threads() within one tick */
/*-------------------------*/

uint8_t w_space_init(void)
{
	return 0;
}

#if 0
#define HZ	20	/* 20HZ for time interrupt */
#define TIMER0_COUNT 0x3cb0 /* 15536 (= 65536 - 50000) , 50ms */
#else
#define HZ	100	/* 100HZ for time interrupt */
#define TIMER0_COUNT 0xd8f0 /* 55536 (= 65536 - 10000) , 10ms */
#endif
uint8_t w_time_init(void)
{
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
	/* global interrupt enable */
	/* FIXME: enable at w_world_start()/init() */
	/* EA = 1; */
	/* start timer 0 */
	TR0 = 1;

	world_time_ticks = 0;
	world_time_flags = 0;
	world_uptime_seconds = 0;

	return 0;
}

/* current running thread ID */
uint8_t w_cur_thread_id = 0;

/* thread running state */
/* 0 : wait; 1 : ready */
uint8_t w_thread_state = 0x01;

void (* const w_thread_list[W_THREAD_NUM])(void) = {
	init,
	thread1
};

void w_thread_context_switch(void)
{
	return ;
}

void w_schedule_threads(void)
{
	uint8_t i;
	uint8_t mask;

	i = W_THREAD_NUM - 1;
	mask = (0x01 << i);
	while (i > 0) {
		if ((mask & w_thread_state) == 1) {
			w_thread_context_switch();
			w_thread_list[i]();
		}
		i--;
	}
}

#if 0
void w_world_start()
{
	/* global interrupt enable */
	EA = 1;

	while(1) {
		w_schedule_threads();
	}
}
#endif


void w_time_int_service(void)
{
	TF0 = 0;
	TL0 = TL0 + (TIMER0_COUNT & 0x00ff);
	TH0 = TH0 + (TIMER0_COUNT >> 8);

	world_time_ticks++;
	world_time_ticks %= HZ;

	/* ticks has been accumulated to one second */
	if (world_time_ticks == 0) {
		world_uptime_seconds++;
	}

	/* do the schedule */
	if (world_time_flags & 0x01) {
		/* we have do w_schedule_threads() within one tick */
		/* so do nothingh right now. */
	}
	else {
		w_schedule_threads();
	}
}

/*
 * world layer is defined by user's requirement
 * and implemented by system programmer.
 * EdSim51 porting maps world layer onto universe layer directly
 * and provides services to user applications.
 */
void world(void)
{
	/* setup the space of this world */
	w_space_init();

	/* setup the time of this world */
	w_time_init();

	/* start up the user's applications */
#if 0
	/* w_start_world() is an ever-last loop routine */
	w_world_start();
#endif
	/* init() is an ever-last loop routine */
	init();

	/* should not reach here in a normal running */
}
