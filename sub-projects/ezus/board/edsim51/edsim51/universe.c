#include <stdint.h>
#include <8051.h>

#include "universe.h"

/*
 * interrupt number defined in SDCC
 *
Interrupt #     Description     Vector Address
0               External 0      0x0003
1               Timer 0         0x000b
2               External 1      0x0013
3               Timer 1         0x001b
4               Serial 	        0x0023
5               Timer 2 (8052)  0x002b
...             ...
n                               0x0003 + 8*n
*/

void external0_interrupt_handler(void) __interrupt 0 __using 1
{

}

/* assuming system clock frequency of 12MHz, 1 step for 1us */
/* timer0 set to mode 1, 50ms interrupt */

void extenal1_interrupt_handler(void) __interrupt 2 __using 1
{

}

void timer1_interrupt_handler(void) __interrupt 3 __using 1
{

}

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

	/* ready to start world */
	world();
}
