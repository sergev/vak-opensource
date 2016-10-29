#include "stream.h"

int x = 12345;

extern "C" {
void _start (Stream_t *console)
{
	console->Put_String ("Hello, World!\n");

	console->Put_String ("Loaded at ");
	console->Put_Unsigned ((unsigned long) &_start, 8, 16);
	console->Put_String ("\n");

	console->Put_String ("Global variable x = ");
	console->Put_Integer (x);
	console->Put_String ("\n");
}
}
