#include "ezus.h"

__code void w_thread_schedule(void) __using 2
{
	uint8_t i;
	i = W_THREAD_NUM - 1;
	while (i > 0) {
		if ((wd.thread_tts)[i] == 0) {
			if (wd.cur_thread_id != i) {
				/* enter critical area */
				W_LOCK_CRITICAL();

				wd.next_thread_id = i;

				/*
				 * the priority of current running thread is lower
				 * than target thread, switch thread context
				 */
				w_thread_context_switch();
				/*
				 * after context switch, SP point to i-th thread
				 * tag i-th thread as current running thread
				 */

				/* exit critical area */
				W_UNLOCK_CRITICAL();
			}
			return;
		}
		i--;
	}
	/* running default thread */
#if 0
	wd.next_thread_id = 0;
	w_thread_context_switch();
#endif
}
