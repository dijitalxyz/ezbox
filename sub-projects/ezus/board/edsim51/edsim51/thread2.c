#include "ezus.h"

void thread2(void) __using 3
{
	while (1){
		if (W_P1_2 == 0) {
			W_P1_2 = 1;
		}
		else {
			W_P1_2 = 0;
		}
		w_thread_wait(10);
	};
}

