#include <8051.h>

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
	if (P1_0 == 0)
		P1_0 = 1;
	else
		P1_0 = 0;
}

void timer0_interrupt_handler(void) __interrupt 1 __using 0
{
	if (P1_1 == 0)
		P1_1 = 1;
	else
		P1_1 = 0;
}

void extenal1_interrupt_handler(void) __interrupt 2 __using 1
{
	if (P1_2 == 0)
		P1_2 = 1;
	else
		P1_2 = 0;
}

void timer1_interrupt_handler(void) __interrupt 3 __using 1
{
	if (P1_3 == 0)
		P1_3 = 1;
	else
		P1_3 = 0;
}

void serial_interrupt_handler(void) __interrupt 4 __using 1
{
	if (P1_4 == 0)
		P1_4 = 1;
	else
		P1_4 = 0;
}

void big_bang(void) __using 0
{
	/* setup universe space stack pointer */
	/* We can do it here since no acall/lcall has been happenned before */
	SP = 0x30 - 1;

	/* ready to start universe */
	universe();
}
