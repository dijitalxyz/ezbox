#include "world.h"

#include "init.h"

/*
 * entry point for user appliactions
 * which can only view world layer API
 */
void init(void) __using 3
{
	/* global interrupt enable */
	EA = 1;

#if 0
	while(1) {
		w_schedule_threads();
	}
#endif

}
