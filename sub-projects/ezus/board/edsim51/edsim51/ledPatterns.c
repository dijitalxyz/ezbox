#include <8051.h>
void main()
{
	while(1) {
_asm
		dec	_P0
_endasm;
	}
}
