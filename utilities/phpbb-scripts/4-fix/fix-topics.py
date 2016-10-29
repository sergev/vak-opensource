#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

#-----------------------------------
# Update counters in topics table.
#
bb = mdb.connect (user = "retrobb",
                    db = "retrobb",
                passwd = "hiheyho")
action = bb.cursor()
action.execute ("""
      SELECT topic_id
        FROM phpbb_topics
    ORDER BY topic_id
    """)

# Loop through all the topics.
count = 0
for row in action.fetchall():
    topic_id = row[0]
    count = count + 1

    # Find the first post.
    action.execute ("""
         SELECT post_id,
                poster_id
           FROM phpbb_posts
          WHERE topic_id = %d
       ORDER BY post_time
          LIMIT 1
        """ % topic_id)
    row = action.fetchall()[0]
    first_post_id = row[0]
    first_poster_id = row[1]

    # Get the user name
    action.execute ("""
         SELECT username
           FROM phpbb_users
          WHERE user_id = %d
        """ % first_poster_id)
    first_poster_name = action.fetchall()[0][0]
    #print "Topic", topic_id, "first post", first_post_id, first_poster_id
    #continue

    # Find the last post.
    action.execute ("""
         SELECT post_id,
                poster_id,
                post_time,
                post_subject
           FROM phpbb_posts
          WHERE topic_id = %d
       ORDER BY post_time DESC
          LIMIT 1
        """ % topic_id)
    row = action.fetchall()[0]
    last_post_id = row[0]
    last_poster_id = row[1]
    last_post_time = row[2]
    last_post_subject = row[3]

    # Get the user name
    action.execute ("""
         SELECT username
           FROM phpbb_users
          WHERE user_id = %d
        """ % last_poster_id)
    last_poster_name = action.fetchall()[0][0]
    #print "Topic", topic_id, "last post", last_post_id, last_poster_id, last_poster_name, last_post_time, last_post_subject
    #continue

    # Count the number of replies.
    action.execute ("""
        SELECT COUNT(*)
          FROM phpbb_posts
         WHERE topic_id = %d
        """ % topic_id)
    num_replies = int (action.fetchall()[0][0]) - 1
    #print "Topic", topic_id, "replies", num_replies
    #continue

    # Update values in database.
    action.execute ("""
        UPDATE phpbb_topics
           SET topic_replies = %d,
               topic_replies_real = %d,
               topic_first_post_id = %d,
               topic_first_poster_name = '%s',
               topic_last_post_id = %d,
               topic_last_poster_id = %d,
               topic_last_poster_name = '%s',
               topic_last_post_time = %d,
               topic_last_post_subject = '%s'
         WHERE topic_id = %d
        """ %  (num_replies,
                num_replies,
                first_poster_id,
                first_poster_name,
                last_post_id,
                last_poster_id,
                last_poster_name,
                last_post_time,
                mdb.escape_string (last_post_subject),
                topic_id))

bb.commit()
print "Fixed", count, "topics."
