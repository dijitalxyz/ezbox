#include <stdlib.h>
#include <8051.h>
#include <ezus.h>

void w_space_init(void)
{
	/* setup stack pointer */
	SP = 0x30 - 1;
}

void w_time_init(void)
{
	
}

void w_start_world(void *e)
{
	if (e == NULL)
		return;
}

/*
 * world layer is defined by user's requirement
 * and implemented by system programmer.
 * EdSim51 porting maps world layer onto universe layer directly
 * and provides services to user applications.
 */
void world(void)
{
	/* setup the space of this world */
	w_space_init();

	/* setup the time of this world */
	w_time_init();

	/* start up the user's applications */
	/* init() is an ever-last loop routine */
	init();

	/* should not reach here in a normal running */
}
