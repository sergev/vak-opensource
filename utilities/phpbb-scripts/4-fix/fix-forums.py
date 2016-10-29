#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

#-----------------------------------
# Update counters in forums table.
#
bb = mdb.connect (user = "retrobb",
                    db = "retrobb",
                passwd = "hiheyho")
action = bb.cursor()
action.execute ("""
      SELECT forum_id
        FROM phpbb_forums
    ORDER BY forum_id
    """)

# Loop through all the forums.
for row in action.fetchall():
    forum_id = row[0]
    #print forum_id

    # Find the last post.
    action.execute ("""
         SELECT post_id,
                poster_id,
                post_time,
                post_subject
           FROM phpbb_posts
          WHERE forum_id = %d
       ORDER BY post_time DESC
          LIMIT 1
        """ % forum_id)
    row = action.fetchall()[0]
    post_id = row[0]
    poster_id = row[1]
    post_time = row[2]
    post_subject = row[3]

    # Get the user name
    action.execute ("""
         SELECT username
           FROM phpbb_users
          WHERE user_id = %d
        """ % poster_id)
    poster_name = action.fetchall()[0][0]
    #print "Forum", forum_id, "post", post_id, poster_id, poster_name, post_time, post_subject

    # Count the number of topics.
    action.execute ("""
        SELECT COUNT(*)
          FROM phpbb_topics
         WHERE forum_id = %d
        """ % forum_id)
    num_topics = int (action.fetchall()[0][0])

    # Count the number of posts.
    action.execute ("""
        SELECT COUNT(*)
          FROM phpbb_posts
         WHERE forum_id = %d
        """ % forum_id)
    num_posts = int (action.fetchall()[0][0])

    print "Forum", forum_id, "posts", num_posts, "topics", num_topics

    # Update values in database.
    action.execute ("""
        UPDATE phpbb_forums
           SET forum_topics = %d,
               forum_topics_real = %d,
               forum_posts = %d,
               forum_last_post_id = %d,
               forum_last_poster_id = %d,
               forum_last_poster_name = '%s',
               forum_last_post_time = %d,
               forum_last_post_subject = '%s'
         WHERE forum_id = %d
        """ %  (num_topics,
                num_topics,
                num_posts,
                post_id,
                poster_id,
                poster_name,
                post_time,
                mdb.escape_string (post_subject),
                forum_id))

bb.commit()
