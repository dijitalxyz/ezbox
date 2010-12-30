#include "ezus.h"

void thread3(void) __using 3
{
	while (1){
		if (W_P1_3 == 0) {
			W_P1_3 = 1;
		}
		else {
			W_P1_3 = 0;
		}
		w_thread_wait(10);
	};
}

