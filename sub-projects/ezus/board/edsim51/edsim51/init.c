#include "ezus.h"

#include "init.h"

/*
 * default thread for user appliactions
 * which can only view world layer API
 */
void init(void) __using 3
{
	/* enable interrupts */
	EA = 1;

	/* init thread, do scheduling */
	while (1) {
		w_thread_schedule();
	}
}
