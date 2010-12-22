#include <stdlib.h>
#include <8051.h>

extern void world(void);

/*
 * universe layer is an extension of the hardware resource
 * and implemented by system programmer.
 * EdSim51 porting exports hardware resource to world layer fully
 * and provides services to world layer.
 */
void universe(void)
{
	world();
}

void u_start_world(void *e)
{
	if (e == NULL)
		return;
}
