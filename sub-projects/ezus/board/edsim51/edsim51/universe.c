#include <stdint.h>
#include <8051.h>

#include "ezus.h"

/*
 * universe layer is an extension of the hardware resource
 * and implemented by system programmer.
 * EdSim51 porting exports hardware resource to world layer fully
 * and provides services to world layer.
 */
void universe(void) __using 1
{
	/*
	 * put universe running rules here
	 * worlds[0] is the default world, it must be there
	 */

	/* ... */

	w_startup();
}
