#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

wordpress = mdb.connect (user = "retrouserwp",
                           db = "retrowordpress",
                       passwd = "skomorox")
select = wordpress.cursor()
select.execute ("""
    SELECT
        ID,
        post_parent,
        post_author,
        UNIX_TIMESTAMP(post_date_gmt),
        post_title,
        post_content
    FROM wp_posts
    WHERE post_type = 'topic'
    ORDER BY post_parent
    """)
active_topics = select.fetchall()

#for row in active_topics:
#    print row
#sys.exit(0)

#-----------------------------------
# Create new topics on a phpbb forum.
#
bb = mdb.connect (user = "retrobb",
                    db = "retrobb",
                passwd = "hiheyho")
insert = bb.cursor()

count = 0
for row in active_topics:
    # Convert WordPress data to phpbb"
    topic_id     = row[0]
    forum_id     = row[1]
    topic_poster = row[2] + 70000
    topic_time   = row[3]
    topic_title  = mdb.escape_string (row[4])

    #print "%-5d %-10lu %-23s %s" % (user_id, user_regdate, username, user_email)

    # Remap parent to forum id.
    if   forum_id == 96:  forum_id = 1  # News and announcements
    elif forum_id == 100: forum_id = 4  # Development tools
    elif forum_id == 102: forum_id = 5  # Kernel and drivers
    elif forum_id == 104: forum_id = 6  # Application software
    elif forum_id == 106: forum_id = 3  # Boards and hardware
    elif forum_id == 305: forum_id = 7  # Bug reports
    else:
        print "Unknown post parent =", forum_id
        sys.exit(0)

    count = count + 1
    insert.execute ("""
        INSERT INTO phpbb_topics(
            topic_id,
            forum_id,
            topic_title,
            topic_poster,
            topic_time,
            topic_last_poster_id,
            topic_last_post_subject,
            topic_last_post_time,
            topic_last_view_time
        ) VALUES (%d, %d, '%s', %d, %d, %d, '%s', %d, %d)
        """ % (
            topic_id,           # topic_id,
            forum_id,           # forum_id,
            topic_title,        # topic_title,
            topic_poster,       # topic_poster,
            topic_time,         # topic_time,
            topic_poster,       # topic_last_poster_id,
            topic_title,        # topic_last_post_subject,
            topic_time,         # topic_last_post_time,
            topic_time          # topic_last_view_time,
        ))

bb.commit()
print "Moved", count, "topics."
