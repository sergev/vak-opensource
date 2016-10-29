#include "ada-compatible.h"

class Sequence : public Ada_Compatible {
public:
	int Value;

	virtual void Next () = 0;
};

class Arithmetic : public Sequence {
	ADA_COMPATIBLE_IMPLEMENTATION;
public:
	virtual void Next ()
	{
		Value += 1;
	}
};

asm ("__cxa_pure_virtual: ret");

void print_seq (Sequence *s)
{
	asm (";;;");
	s->Next ();
	asm (";;;");
}

int main ()
{
	Arithmetic a;

	asm (";;;");
	print_seq (&a);
	return 0;
}
