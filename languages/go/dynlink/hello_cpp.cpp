#include "stream_cpp.h"

int x = 12345;

extern "C" {
void _start (Interface_t iface)
{
	Stream_t *console = (Stream_t*) iface.object;

	console->Put_String ("Hello, World!\n");

	console->Put_String ("Loaded at ");
	console->Put_Unsigned ((unsigned long) &_start, 8, 16);
	console->Put_String ("\n");

//console->Put_String ("console = ");
//console->Put_Unsigned ((unsigned long) console, 8, 16);
//console->Put_String (", sizeof(*console) = ");
//console->Put_Unsigned (sizeof (*console), 0, 10);
//console->Put_String (", *console = ");
//console->Put_Unsigned (*(unsigned*) console, 8, 16);
//console->Put_String ("\n");

//unsigned *call = *(unsigned**) console;
//console->Put_String ("call[] = ");
//console->Put_Unsigned (call[0], 8, 16);
//console->Put_String (", ");
//console->Put_Unsigned (call[1], 8, 16);
//console->Put_String (", ");
//console->Put_Unsigned (call[2], 8, 16);
//console->Put_String (", ");
//console->Put_Unsigned (call[3], 8, 16);
//console->Put_String (", ");
//console->Put_Unsigned (call[4], 8, 16);
//console->Put_String ("\n");

	console->Put_String ("Global variable x at ");
	console->Put_Unsigned ((unsigned) &x, 8, 16);
	console->Put_String (" = ");
	console->Put_Integer (x);
	console->Put_String ("\n");
	x += x;
}
}
