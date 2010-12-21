#include <stdlib.h>
#include <stdint.h>
#include <8051.h>

#include <world.h>
#include <ezus.h>

uint8_t w_space_init(void)
{
	return 0;
}

#if 0
#define TIMER0_COUNT 0x3cb0 /* 15536 (= 65536 - 50000) , 50ms */
#else
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
	/* FIXME: enable at init() */
	/* EA = 1; */
	/* start timer 0 */
	TR0 = 1;

	return 0;
}

void w_time_int_service(void)
{
	TF0 = 0;
	TL0 = TL0 + (TIMER0_COUNT & 0x00ff);
	TH0 = TH0 + (TIMER0_COUNT >> 8);
	/* blink LED */
	P1 -= 1;
}

void (* const w_thread_list[W_MAX_THREADS])(void) = {
	init,
	thread1
};


void w_start_world(void (* const thread_list[W_MAX_THREADS])(void))
{
	int i;
	while(1) {
		i = W_MAX_THREADS - 1;
		while(i >= 0) {
			if (thread_list[i] != NULL) {
				thread_list[i]();
			}
			i--;
		}
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
	/* w_start_world() is an ever-last loop routine */
	/* thread_list define in ezus.c */
	w_start_world(w_thread_list);

	/* should not reach here in a normal running */
}
