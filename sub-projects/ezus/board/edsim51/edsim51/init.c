#include "ezus.h"

#include "init.h"

/*
 * default thread for user appliactions
 * which can only view world layer API
 */
void init(void) __using 3
{
	/* init thread, do nothing */
	while (1) {
#if 0
		if (W_P1_0 == 0) {
			W_P1_0 = 1;
		}
		else {
			W_P1_0 = 0;
		}
#endif
		w_thread_schedule();
	}
}
