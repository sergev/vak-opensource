#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys
import subprocess

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
        post_content,
        post_type
    FROM wp_posts
    WHERE post_type = 'reply'
    OR    post_type = 'topic'
    ORDER BY post_parent
    """)
active_posts = select.fetchall()

#for row in active_posts:
#    print row
#sys.exit(0)

# Build a parent map.
parent = {}
for row in active_posts:
    topic_id = row[0]
    post_parent = row[1]
    parent [topic_id] = post_parent

# Find a forum id for a given post id.
def get_forum (id):
    while True:
        if   id == 96:  return 1  # News and announcements
        elif id == 100: return 4  # Development tools
        elif id == 102: return 5  # Kernel and drivers
        elif id == 104: return 6  # Application software
        elif id == 106: return 3  # Boards and hardware
        elif id == 305: return 7  # Bug reports
        id = parent [id]

# Convert html to bbcode using external script
def html_to_bbcode (html):
    p = subprocess.Popen ("./html-to-bbcode", shell = True,
        bufsize = 8000, stdin = subprocess.PIPE, stdout = subprocess.PIPE)
    p.stdin.write (html)
    p.stdin.close()
    bbcode = p.stdout.read()
    del p
    return bbcode

#-----------------------------------
# Create new posts on a phpbb forum.
#
bb = mdb.connect (user = "retrobb",
                    db = "retrobb",
                passwd = "hiheyho")
insert = bb.cursor()

count = 0
for row in active_posts:
    # Convert WordPress data to phpbb"
    post_id      = row[0]
    post_type    = row[6]
    if post_type == 'topic':
        topic_id = post_id
    else:
        topic_id = row[1]
    forum_id     = get_forum (row[1])
    poster_id    = row[2] + 70000
    post_time    = row[3]
    post_subject = row[4]
    post_text    = html_to_bbcode (row[5])

    print "%-5d %-5d %-5d %-5d %s" % (post_id, topic_id, forum_id, poster_id, post_subject)

    count = count + 1
    insert.execute ("""
        INSERT INTO phpbb_posts(
            post_id,
            topic_id,
            forum_id,
            poster_id,
            post_time,
            post_subject,
            post_text
        ) VALUES (%d, %d, %d, %d, %d, '%s', '%s')
        """ % (
            post_id,
            topic_id,
            forum_id,
            poster_id,
            post_time,
            mdb.escape_string (post_subject),
            mdb.escape_string (post_text)
        ))

bb.commit()
print "Moved", count, "posts."
