#include "uart.h"

int main ()
{
	uart_init ();
	uart_puts ("Hello, World!\n");
	return 0;
}
