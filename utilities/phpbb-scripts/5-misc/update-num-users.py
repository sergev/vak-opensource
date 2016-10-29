#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

print "Disabled."
sys.exit(0)

num_users = 5

bb = mdb.connect (user = "retrobb",
                     db = "retrobb",
                 passwd = "hiheyho")

action = bb.cursor()
action.execute ("""
    UPDATE phpbb_config
       SET config_value = %d
     WHERE config_name = 'num_users'
    """ % num_users)
bb.commit()
