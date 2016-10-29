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
#        forum_id,
#        forum_topics,
#        forum_posts,
#        forum_name
    FROM phpbb_forums
    ORDER BY forum_id
    """)

for row in cur.fetchall():
    print row
    #print "%-5d %-5d %-5d %s" % row
