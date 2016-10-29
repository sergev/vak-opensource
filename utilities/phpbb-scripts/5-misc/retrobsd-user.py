#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

con = mdb.connect (user = "retrobb",
                     db = "retrobb",
                 passwd = "hiheyho")

cur = con.cursor()
cur.execute ("""
    SELECT *
    FROM phpbb_users
    WHERE username = 'retrobsd'
    """)

for row in cur.fetchall():
    print row
    #print "%-5d %-23s %-23s %s" % row
