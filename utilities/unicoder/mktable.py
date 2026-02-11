#
# Copyright (C) 2007 Serge Vakulenko, <vak@cronyx.ru>
#
# This file is part of Unicoder utility, which is distributed
# under the terms of the GNU General Public License (GPL).
# See the accompanying file "COPYING" for more details.
#
import sys

for source in sys.argv[1:]:
	encoding_name = source[:-4].lower()
	print(f"\tif (strcasecmp (fmt, \"{encoding_name}\") == 0) {{")

	engine_id = encoding_name.translate(str.maketrans("-", "_"))
	if engine_id[0].isdigit():
		engine_id = "iso" + engine_id
	print(f"\t\tengine = new {engine_id}_engine;\n\t\treturn 1;\n\t}}")
