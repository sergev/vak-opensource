#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

print "Disabled."
sys.exit(0)

bb = mdb.connect (user = "retrobb",
                     db = "retrobb",
                 passwd = "hiheyho")

delete = bb.cursor()
delete.execute ("""
    DELETE
    FROM phpbb_users
    WHERE username LIKE '[%%]'
    """)
bb.commit()
