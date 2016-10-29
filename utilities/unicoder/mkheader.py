#
# Copyright (C) 2007 Serge Vakulenko, <vak@cronyx.ru>
#
# This file is part of Unicoder utility, which is distributed
# under the terms of the GNU General Public License (GPL).
# See the accompanying file "COPYING" for more details.
#
import sys, string

id = string.lower (string.translate (sys.argv[1], string.maketrans ("-", "_")))
if id[0] in string.digits:
	id = "iso" + id

print "class", id+"_engine : public Unicode_engine {"
print "\tint get_char (FILE *fd);"
print "\tvoid put_char (unsigned short c, FILE *fd);"
print "\tconst char *name (void) { return \"" + id + "\"; }"
print "\n\tfriend class Unicoder;\n};"
