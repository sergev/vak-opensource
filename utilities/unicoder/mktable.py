#
# Copyright (C) 2007 Serge Vakulenko, <vak@cronyx.ru>
#
# This file is part of Unicoder utility, which is distributed
# under the terms of the GNU General Public License (GPL).
# See the accompanying file "COPYING" for more details.
#
import sys, string

for id in sys.argv[1:]:
	id = string.lower (id[:-4])
	print "\tif (strcasecmp (fmt, \"" + id + "\") == 0) {"

	id = string.translate (id, string.maketrans ("-", "_"))
	if id[0] in string.digits:
		id = "iso" + id
	print "\t\tengine = new", id + "_engine;\n\t\treturn 1;\n\t}"
