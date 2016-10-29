#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys
import subprocess

p = subprocess.Popen ("./html-to-bbcode", shell = True,
    bufsize = 8000, stdin = subprocess.PIPE, stdout = subprocess.PIPE)
p.stdin.write ('<a href="Ref">Link</a>')
p.stdin.close()
row = p.stdout.read()
print row
