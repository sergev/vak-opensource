#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

wordpress = mdb.connect (user = "retrouserwp",
                     db = "retrowordpress",
                 passwd = "skomorox")
select = wordpress.cursor()
select.execute ("""
    SELECT *
    FROM wp_posts
    WHERE post_type = 'topic'
    ORDER BY ID
    """)

for row in select.fetchall():
    print row
    #print "%-5d %-23s %-31s %-7s %s" % row
