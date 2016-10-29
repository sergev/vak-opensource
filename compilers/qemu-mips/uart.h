/*
 * Initialize UART registers.
 */
void uart_init (void);

/*
 * Send a byte to UART.
 */
void uart_putchar (int c);

/*
 * Send a string to UART.
 */
void uart_puts (const char *s);
