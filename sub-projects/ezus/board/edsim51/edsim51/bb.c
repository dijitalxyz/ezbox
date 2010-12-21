#include <8051.h>

extern void universe(void);

void big_bang(void)
{
	/* setup universe space stack pointer */
	/* We can do it here since no acall/lcall has been happenned before */
	SP = 0x30 - 1;

	/* ready to start universe */
	universe();
}
