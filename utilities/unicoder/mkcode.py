#
# Copyright (C) 2007 Serge Vakulenko, <vak@cronyx.ru>
#
# This file is part of Unicoder utility, which is distributed
# under the terms of the GNU General Public License (GPL).
# See the accompanying file "COPYING" for more details.
#
import sys

encoding_id = sys.argv[1].lower()
print("#include <Unicoder.h>")
print(f"#include <{encoding_id}.h>\n")

encoding_id = encoding_id.translate(str.maketrans("-", "_"))
if encoding_id[0].isdigit():
	encoding_id = "iso" + encoding_id

map_to = {}
for i in range(256):
	map_to[i] = {}

m = map_to[0]
for i in range(256):
	m[i] = i

# Map 0x2028 to 0x0a (line feed)
m = map_to[0x20]
for i in range(256):
	m[i] = 0
m[0x28] = 0x0a

map_from = {}
for i in range(256):
	map_from[i] = i

for line in sys.stdin.readlines():
	try:
		line = line.split()
		c = int(line[0], 0)
		u = int(line[1], 0)
		map_from[c] = u
		m = map_to[u >> 8]
		if not m:
			for i in range(256):
				m[i] = 0
		if map_to[0][c] == c:
			map_to[0][c] = 0
		m[u & 0xff] = c
	except Exception:
		continue

print(f"static unsigned short {encoding_id}_to_unicode_table [256] = {{", end=" ")
for i in range(256):
	if i % 8 == 0:
		print("\n\t", end="")
	if map_from[i] < 0x100:
		print("0x%02x,  " % map_from[i], end="")
	else:
		print("0x%04x," % map_from[i], end="")
print("\n};\n")

print(f"static unsigned char unicode_to_{encoding_id} (unsigned short val)\n{{")
print("\tstatic unsigned char tab0 [256] = {", end=" ")
m = map_to[0]
for i in range(256):
	if i % 8 == 0:
		print("\n\t\t", end="")
	if m[i] > 0:
		print("0x%02x, " % m[i], end="")
	else:
		print("0,    ", end="")
print("\n\t};")

print("\tswitch (val >> 8) {")
print("\tcase 0x00:\n\t\treturn tab0 [val];")
for i in range(1,256):
	m = map_to[i]
	if not m:
		continue
	print(f"\tcase 0x{i:02x}:")
	print("\t\tswitch ((unsigned char) val) {")
	for k in range(256):
		if m[k]:
			print(f"\t\tcase 0x{k:02x}: return 0x{m[k]:02x};")
	print("\t\t}\n\t\tbreak;")
print("\t}\n\treturn 0;\n}\n")
print(f"int {encoding_id}_engine::get_char (FILE *fd)\n{{\n\tint c;\n")
print("\tc = getc (fd);\n\tif (c < 0)\n\t\treturn c;")
print(f"\treturn {encoding_id}_to_unicode_table [c];\n}}\n")
print(f"void {encoding_id}_engine::put_char (unsigned short c, FILE *fd)\n{{")
print(f"\tputc (unicode_to_{encoding_id} (c), fd);\n}}")
