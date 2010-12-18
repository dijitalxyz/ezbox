#include <stdlib.h>
#include <8051.h>
#include <ezus.h>

/*
 * world layer is defined by user's requirement
 * and implemented by system programmer.
 * EdSim51 porting maps world layer onto universe layer directly
 * and provides services to user applications.
 */
void world(void)
{
	init();
}

void w_start_world(void *e)
{
	if (e == NULL)
		return;
}
