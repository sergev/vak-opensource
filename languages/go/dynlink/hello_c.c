#include "stream_c.h"

//extern void debug_printf (const char *fmt, ...);

int x = 12345;

void _start (Stream_t console)
{
//debug_printf ("Loaded at %08X\n", (unsigned) _start);
//debug_printf ("console.data = %08X\n", (unsigned) console.data);
//debug_printf ("console.call = %08X\n", (unsigned) console.call);
	console.call->Put_String (console.data, "Hello, World!\n");

	console.call->Put_String (console.data, "Loaded at ");
	console.call->Put_Unsigned (console.data, (unsigned long) &_start, 8, 16);
	console.call->Put_String (console.data, "\n");

	console.call->Put_String (console.data, "Global variable x at ");
	console.call->Put_Unsigned (console.data, (unsigned long) &x, 8, 16);
	console.call->Put_String (console.data, " = ");
	console.call->Put_Integer (console.data, x, 0, 10);
	console.call->Put_String (console.data, "\n");
	x += x;
}
