#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

if len(sys.argv) != 3:
        print "Usage: replace-user oldID newID"
        sys.exit (1)

old_user_id = int (sys.argv[1])
new_user_id = int (sys.argv[2])

#
# Connect to database.
#
bb = mdb.connect (user = "retrobb",
                    db = "retrobb",
                passwd = "hiheyho")
action = bb.cursor()

#
# Retrieve the user names.
#
action.execute ("""
    SELECT username
      FROM phpbb_users
     WHERE user_id = %d
    """ % old_user_id)
old_username = action.fetchall()[0][0]
action.execute ("""
    SELECT username
      FROM phpbb_users
     WHERE user_id = %d
    """ % new_user_id)
new_username = action.fetchall()[0][0]

#
# Get a confirmation from user.
#
while True:
    print "Do you really want to replace a user %d '%s'" % (old_user_id, old_username)
    reply = raw_input ("by a user %d '%s'? " % (new_user_id, new_username))
    if reply == 'no':
        print "Canceled."
        sys.exit(0)
    if reply == 'yes':
        break
    print "\nPlease, reply 'yes' or 'no'.\n"

#
# Update forums.
#
action.execute ("""
     UPDATE phpbb_forums
        SET forum_last_poster_id = %d,
            forum_last_poster_name = '%s'
      WHERE forum_last_poster_id = %d
    """ %  (new_user_id,
            new_username,
            old_user_id))

#
# Update topics.
#
action.execute ("""
     UPDATE phpbb_topics
        SET topic_poster = %d,
            topic_first_poster_name = '%s'
      WHERE topic_poster = %d
    """ %  (new_user_id,
            new_username,
            old_user_id))
action.execute ("""
     UPDATE phpbb_topics
        SET topic_last_poster_id = %d,
            topic_last_poster_name = '%s'
      WHERE topic_last_poster_id = %d
    """ %  (new_user_id,
            new_username,
            old_user_id))

#
# Update posts.
#
action.execute ("""
     UPDATE phpbb_posts
        SET poster_id = %d,
            post_username = '%s'
      WHERE poster_id = %d
    """ %  (new_user_id,
            new_username,
            old_user_id))

#
# Count the number of posts.
#
action.execute ("""
    SELECT COUNT(*)
      FROM phpbb_posts
     WHERE poster_id = %d
    """ % new_user_id)
num_posts = int (action.fetchall()[0][0])
print "User %d '%s'" % (new_user_id, new_username), "posts", num_posts

#
# Update users.
#
action.execute ("""
     UPDATE phpbb_users
        SET user_posts = %d
      WHERE user_id = %d
    """ %  (num_posts,
            new_user_id))

bb.commit()
