#include <stdint.h>
#include <8051.h>

#include "ezus.h"

extern void universe(void);

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

void serial_interrupt_handler(void) __interrupt 4 __using 1
{

}

void big_bang(void) __using 0
{
	/* setup universe space stack pointer */
	/* We can do it here since no acall/lcall has been happenned before */
	SP = 0x30 + sizeof(world_data_t) + 0x20 - 1;

	/* setup universe time tick */
        /* use timer 0 as time tick source */
	TMOD &= 0xf0;

	/* Mode 1 : 16-bit timer mode */
	TMOD |= 0x01;

	/* put timer interval in TL/TH */
	TL0 = (W_TIMER0_COUNT & 0x00ff);
	TH0 = (W_TIMER0_COUNT >> 8);

	/* low priority */
	PT0 = 0;

	/* enable timer 0 interrupt */
	ET0 = 1;

	/* start timer 0 */
	TR0 = 1;

	/* initialize universe time */
	wd.time_ticks = 0;
	wd.uptime_seconds = 0;

	/* ready to start universe */
	universe();
}
