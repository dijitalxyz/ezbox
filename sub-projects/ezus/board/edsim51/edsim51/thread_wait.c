#include "ezus.h"

__code void w_thread_wait(uint8_t ticks) __using 2
{
	/* set waiting ticks for current thread */
	(wd.thread_tts)[wd.cur_thread_id] = ticks;

	/* do thread schedule */
	/* the wd.next_thread_id must be 0 now */
	w_thread_schedule();
}
