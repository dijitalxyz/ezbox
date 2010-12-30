#include "ezus.h"

#include "init.h"

/*
 * default thread for user appliactions
 * which can only view world layer API
 */
void init(void) __using 3
{
	/* enable interrupt */
	W_EA = 1;

	/* init thread, do nothing */
	while (1) {
		W_PCON |= W_PCON_IDL;
		/* do scheduling at once */
		w_thread_wait(0);
	}
}
