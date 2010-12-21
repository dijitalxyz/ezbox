#include <8051.h>

void thread1(void)
{
	while (1){
		if (P1_1 == 0) {
			P1_1 = 1;
		}
		else {
			P1_1 = 0;
		}
	};
}

