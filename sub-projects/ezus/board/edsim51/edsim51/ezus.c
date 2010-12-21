#include <world.h>

#include <ezus.h>

/*
 * entry point for user appliactions
 * which can only view world layer API
 */
void init(void)
{
	/* global interrupt enable */
	EA = 1;

	while(1) {
		w_schedule_threads();
	}
}
