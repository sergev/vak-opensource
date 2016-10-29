#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

con = mdb.connect (user = "retrobb",
                     db = "retrobb",
                 passwd = "hiheyho")

cur = con.cursor()
cur.execute ("""
    SELECT
        user_id,
        user_type,
        username,
        username_clean,
        user_email
    FROM phpbb_users
#    WHERE username LIKE '[%%]'
    ORDER BY user_id
    """)

for row in cur.fetchall():
    #print row
    print "%-5d %d %-23s %-23s %s" % row
