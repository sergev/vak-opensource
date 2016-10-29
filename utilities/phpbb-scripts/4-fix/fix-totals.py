#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

#-----------------------------------
# Update total counters on a phpbb forum.
#
bb = mdb.connect (user = "retrobb",
                    db = "retrobb",
                passwd = "hiheyho")
action = bb.cursor()

#
# Count the number of topics.
#
action.execute ("""
    SELECT COUNT(*)
      FROM phpbb_topics
    """)
num_topics = int (action.fetchall()[0][0])

#
# Count the number of posts.
#
action.execute ("""
    SELECT COUNT(*)
      FROM phpbb_posts
    """)
num_posts = int (action.fetchall()[0][0])

print "Total posts", num_posts
print "Total topics", num_topics

#
# Update values in database.
#
action.execute ("""
    UPDATE phpbb_config
       SET config_value = %d
     WHERE config_name = 'num_topics'
    """ % num_topics)
action.execute ("""
    UPDATE phpbb_config
       SET config_value = %d
     WHERE config_name = 'num_posts'
    """ % num_posts)
bb.commit()
