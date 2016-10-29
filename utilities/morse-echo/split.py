#!/usr/bin/python

import sys, string, re

suffix = re.compile ("/.*", re.IGNORECASE | re.DOTALL)

filename = ""
while 1:
	line = sys.stdin.readline ()
	if not line: break

	line = string.strip (line)
	if line == "":
		filename = ""
		continue

	if filename == "":
		filename = string.split (line) [0]
		filename = string.lower (filename)
		filename = suffix.sub ("", filename)
		sys.stdout = open ("qso/" + filename + ".txt", "a")
		print

	print line

sys.exit (0)
