#include <8051.h>
void big_bang(void)
{
	while(1) {
		P1 -= 1;
	}
}
