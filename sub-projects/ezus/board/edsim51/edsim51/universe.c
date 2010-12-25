#include <stdint.h>
#include <8051.h>

#include "universe.h"
#include "init.h"

/*
 * universe layer is an extension of the hardware resource
 * and implemented by system programmer.
 * EdSim51 porting exports hardware resource to world layer fully
 * and provides services to world layer.
 */
void universe(void) __using 1
{
	__code const world_rules_t wr = {
		w_space_init,
		w_time_init,
		{
			init, /* default thread */
			thread1
		},
		w_thread_init,
		w_thread_context_switch,
		w_thread_schedule,
		w_startup
	};

	__data world_data_t wd;

	/*
	 * put universe running rules here
	 * worlds[0] is the default world, it must be there
	 */

	/* ... */

	wr.startup(&wr, &wd);
}
