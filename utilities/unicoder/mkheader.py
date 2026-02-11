#
# Copyright (C) 2007 Serge Vakulenko, <vak@cronyx.ru>
#
# This file is part of Unicoder utility, which is distributed
# under the terms of the GNU General Public License (GPL).
# See the accompanying file "COPYING" for more details.
#
import sys

encoding_id = sys.argv[1].translate(str.maketrans("-", "_")).lower()
if encoding_id[0].isdigit():
	encoding_id = "iso" + encoding_id

print(f"class {encoding_id}_engine : public Unicode_engine {{")
print("\tint get_char (FILE *fd);")
print("\tvoid put_char (unsigned short c, FILE *fd);")
print(f"\tconst char *name (void) {{ return \"{encoding_id}\"; }}")
print("\n\tfriend class Unicoder;\n};")
