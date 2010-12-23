#include <8051.h>

void black_hole(void) __using 0
{
	while(1) {
		P1 -= 1;
	}
}
