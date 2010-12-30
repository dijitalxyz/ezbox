#include <stdint.h>
#include <8051.h>

#include "ezus.h"

/* assuming system clock frequency of 12MHz, 1 step for 1us */
/* timer0 set to mode 1, 50ms interrupt */

/* timer0 interrupt will cause thread context switch */
void timer0_interrupt_handler(void) __interrupt 1 __using 1
{
	uint8_t i;

	/* enter interrupt handler */
	W_ENTER_INTERRUPT();

	/* clear timer overflow flag */
	/* TF0 = 0; */

	/* reset TL0/TH0 */
	TL0 = TL0 + (W_TIMER0_COUNT & 0x00ff);
	TH0 = TH0 + (W_TIMER0_COUNT >> 8);

	/* increase world tick counter */
	wd.time_ticks++;
	wd.time_ticks %= W_HZ;

	/* ticks has been accumulated to one second */
	if (wd.time_ticks == 0) {
		wd.uptime_seconds++;
	}

	/* thread time to start(tts) decrease 1 ticks */
	for (i = 0; i < W_THREAD_NUM; i++) {
		if (wd.thread_tts[i] != 0) {
			wd.thread_tts[i]--;
		}
	}

	/* exit interrupt handler */
	W_EXIT_INTERRUPT();
}
