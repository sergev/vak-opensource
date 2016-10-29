#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

#--------------------------------------------------
# Get the number of users.
#
bb = mdb.connect (user = "retrobb",
                    db = "retrobb",
                passwd = "hiheyho")
select = bb.cursor()
select.execute ("""
    SELECT config_value
      FROM phpbb_config
     WHERE config_name = 'num_users'
    """)
num_users = int (select.fetchall()[0][0])

print num_users
