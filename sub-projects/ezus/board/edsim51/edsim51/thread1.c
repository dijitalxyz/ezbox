#include "ezus.h"

void thread1(void) __using 3
{
	while (1){
		if (W_P1_1 == 0) {
			W_P1_1 = 1;
		}
		else {
			W_P1_1 = 0;
		}
	};
}

