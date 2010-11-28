#include "configs/solos.h"

void serial_addr(unsigned int i)
{
	return;
}

void serial_setbrg(void)
{
	CHARMIO(UART_CTRL_REG) = 0;
	CHARMIO(UART_CTRL_REG) =  (CHARM_UART_CTRL_BAUD_38400 | CHARM_UART_CTRL_UARTEN);
}

void serial_putc(const char c)
{
  if(c == '\n') uart_tx('\r');
  uart_tx(c);
}

void serial_putc_raw(const char c)
{	
	serial_putc(c);
}

void serial_puts(const char *s)
{
	while (*s != '\0') {
		serial_putc (*s++);
	}
}
