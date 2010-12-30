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

/* serial port is used to communicate with up PC */
void serial_interrupt_handler(void) __interrupt 4 __using 0
{

}

void big_bang(void) __using 0
{
	/* setup universe space stack pointer */
	/* We can do it here since no acall/lcall has been happenned before */
	SP = U_SP_BOTTOM - 1;

	/* setup universe time tick */
        /* use timer 0 as time tick source */
	TMOD &= 0xf0;

	/* Mode 1 : 16-bit timer mode */
	TMOD |= 0x01;

	/* put timer interval in TL/TH */
	TL0 = (U_TIMER0_COUNT & 0x00ff);
	TH0 = (U_TIMER0_COUNT >> 8);

	/* low priority */
	PT0 = 0;

	/* enable timer 0 interrupt */
	ET0 = 1;

	/* start timer 0 */
	TR0 = 1;

	/* initialize universe time */

	/* ready to start universe */
	universe();
}
