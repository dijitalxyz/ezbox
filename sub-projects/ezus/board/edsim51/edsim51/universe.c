#include <stdint.h>
#include <8051.h>

#include "universe.h"

/*
 * universe layer is an extension of the hardware resource
 * and implemented by system programmer.
 * EdSim51 porting exports hardware resource to world layer fully
 * and provides services to world layer.
 */
void universe(void) __using 1
{
#if 0
	universe_t u;
#endif
#if 1
	__code world_rules_t wr = {
		w_space_init,
		w_time_init,
		w_thread_init,
		w_thread_schedule,
		w_startup
	};

	__data world_data_t wd;
#endif

	/* put universe running rules here */
	/* worlds[0] is the default world, it must be there */
#if 0
	wp = &u.worlds[0];
#else
#endif
#if 0
	wp->space_init = w_space_init;
	wp->time_init = w_time_init;
	wp->thread_init = w_thread_init;
	wp->thread_schedule = w_thread_schedule;
	wp->startup = w_startup;
#endif

	/* ... */
	EA = 1;

	wr.startup(&wr, &wd);
}
