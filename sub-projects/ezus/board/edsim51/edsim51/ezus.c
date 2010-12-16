#include <ezus.h>

void main(void)
{
	if ( is_universe_state_ready() == false ) {
		black_hole();
	}
	else {
		big_bang();
	}
}
