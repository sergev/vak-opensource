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
        topic_id,
        forum_id,
        topic_poster,
        topic_time,
        topic_title
    FROM phpbb_topics
    ORDER BY topic_id
    """)
active_topics = select.fetchall()

for row in active_topics:
    print row
