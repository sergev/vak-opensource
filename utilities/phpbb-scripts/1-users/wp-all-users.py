#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

con = mdb.connect (user = "retrouserwp",
                     db = "retrowordpress",
                 passwd = "skomorox")

cur = con.cursor()
cur.execute ("""
    SELECT
        ID,
        user_login,
        user_email,
        display_name
    FROM wp_users
    ORDER BY ID
    """)

for row in cur.fetchall():
    #print row
    print "%-5d %-23s %-31s %s" % row
