#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

#-----------------------------------
# List topics on a phpbb forum.
#
bb = mdb.connect (user = "retrobb",
                    db = "retrobb",
                passwd = "hiheyho")
select = bb.cursor()
select.execute ("""
    SELECT
        post_id,
        topic_id,
        forum_id,
        poster_id,
        post_time,
        post_subject,
        post_text
    FROM phpbb_posts
    ORDER BY post_id
    """)
active_topics = select.fetchall()

for row in active_topics:
    print row
