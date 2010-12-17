#include <ezus.h>

void main(void)
{
	int i;
	for (i=0; i<100; i++) {
		if ( i%2 == 0 ) {
			black_hole();
		}
		else {
			big_bang();
		}
	}
}
