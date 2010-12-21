#include <world.h>

#include <ezus.h>

/*
 * entry point for user appliactions
 * which can only view world layer API
 */
void init(void)
{
	while (1){
		if (W_P1_0 == 0) {
			W_P1_0 = 1;
		}
		else {
			W_P1_0 = 0;
		}
	};
}
