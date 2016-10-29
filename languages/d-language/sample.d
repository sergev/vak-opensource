//
// Sample program.
//
// Copyright (C) 2007 Serge Vakulenko, <vak@cronyx.ru>
//
// This file is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.
//
// You can redistribute this file and/or modify it under the terms of the GNU
// General Public License (GPL) as published by the Free Software Foundation;
// either version 2 of the License, or (at your discretion) any later version.
// See the accompanying file "COPYING.txt" for more details.
///
import std.stdio;
//import std.stream;
import std.conv;
import getopt;

const char[] VERSION = "1.0";
const char[] COPYRIGHT = "Copyright (C) 2007 Serge Vakulenko";

char[] progname;
int verbose;
int trace;

void usage ()
{
	writefln ("Sample program, Version ", VERSION, ", ", COPYRIGHT);
	writefln ("Usage:");
	writefln ("\t%s [-h] [-o filename] [--help] [--output filename]",
		progname);
	writefln ("Options:");
	writefln ("\t-v\tverbose mode");
	writefln ("\t-t\ttrace mode");
	writefln ("\t-r #\trepeat count");
	//std.stdlib.exit (1);
}

void main (char[][] args)
{
	progname = args [0];
	args = args [1..args.length];

	getopt_t[] options;
	try {
		options = getopt.getopt (args, "hvtr:",
			[ "help", "verbose", "trace", "repeat=" ]);
	}
	catch (GetOptException e) {
		writefln ("Error: ", e.msg);
		writefln ();
		usage ();
	}

	int count = 1;
	foreach (item; options) {
		if (item.option == "-h" || item.option == "--help") {
			usage ();
		} else if (item.option == "-v" || item.option == "--verbose") {
			verbose = 1;
		} else if (item.option == "-t" || item.option == "--trace") {
			trace = 1;
		} else if (item.option == "-r" || item.option == "--repeat") {
			count = toInt (item.value);
		}
	}
	if (verbose)
		writefln ("Verbose");
	if (trace)
		writefln ("Trace");
	writefln ("Count: ", count);

	foreach (arg; args) {
		auto f = new BufferedFile (arg);
		int lnum;

		while (! f.eof ()) {
			char[] buf = f.readLine();

			++lnum;
			writef ("Line ", lnum, ": ", buf.length, " bytes: ");
			writefln ("%s", buf);
		}
	}
}
