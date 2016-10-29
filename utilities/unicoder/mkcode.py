#
# Copyright (C) 2007 Serge Vakulenko, <vak@cronyx.ru>
#
# This file is part of Unicoder utility, which is distributed
# under the terms of the GNU General Public License (GPL).
# See the accompanying file "COPYING" for more details.
#
import sys, string

id = string.lower (sys.argv[1])
print "#include <Unicoder.h>"
print "#include <" + id + ".h>\n"

id = string.translate (id, string.maketrans ("-", "_"))
if id[0] in string.digits:
	id = "iso" + id

map_to = {}
for i in range(256):
	map_to [i] = {}

m = map_to [0]
for i in range(256):
	m [i] = i

# Map 0x2028 to 0x0a (line feed)
m = map_to [0x20]
for i in range(256):
	m [i] = 0
m [0x28] = 0x0a

map_from = {}
for i in range(256):
	map_from [i] = i

for line in sys.stdin.readlines():
	try:
		line = string.split (line)
		c = string.atoi (line[0], 0)
		u = string.atoi (line[1], 0)
		map_from [c] = u
		m = map_to [u>>8]
		if not m:
			for i in range(256):
				m [i] = 0
		if map_to[0][c] == c:
			map_to[0][c] = 0
		m [u & 0xff] = c
	except:
		continue

print "static unsigned short", id + "_to_unicode_table [256] = {",
for i in range(256):
	if i % 8 == 0:
		print "\n\t",
	if map_from[i] < 0x100:
		print "0x%02x,  " % map_from[i],
	else:
		print "0x%04x," % map_from[i],
print "\n};\n"

print "static char unicode_to_"+id, "(unsigned short val)\n{"
print "\tstatic char tab0 [256] = {",
m = map_to[0]
for i in range(256):
	if i % 8 == 0:
		print "\n\t\t",
	if m[i] > 0:
		print "0x%02x, " % m[i],
	else:
		print "0,    ",
print "\n\t};"

print "\tswitch (val >> 8) {"
print "\tcase 0x00:\n\t\treturn tab0 [val];"
for i in range(1,256):
	m = map_to[i]
	if not m:
		continue
	print "\tcase 0x%02x:" % i
	print "\t\tswitch ((unsigned char) val) {"
	for k in range(256):
		if m[k]:
			print "\t\tcase 0x%02x: return 0x%02x;" % (k, m[k])
	print "\t\t}\n\t\tbreak;"
print "\t}\n\treturn 0;\n}\n"
print "int", id+"_engine::get_char (FILE *fd)\n{\n\tint c;\n"
print "\tc = getc (fd);\n\tif (c < 0)\n\t\treturn c;"
print "\treturn", id+"_to_unicode_table [c];\n}\n"
print "void", id+"_engine::put_char (unsigned short c, FILE *fd)\n{"
print "\tputc (unicode_to_"+id, "(c), fd);\n}"
