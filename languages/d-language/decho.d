import std.stdio;

void main (char[][] args)
{
        if (args.length < 2) {
                writefln ("No args.");
                return;
        }

        char[] s;
	foreach (argv; args [1..args.length]) {
		if (s.length > 0)
                        s ~= " ";
                s ~= argv;
        }
	writefln (s);
}
