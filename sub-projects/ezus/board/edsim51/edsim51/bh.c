#include <8051.h>
void black_hole(void)
{
	while(1) {
		P1 -= 1;
	}
}
