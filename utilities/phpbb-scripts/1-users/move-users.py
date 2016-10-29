#!/usr/bin/python
# -*- coding: utf-8 -*-
import MySQLdb as mdb
import sys

#--------------------------------------------------
# Select all active users from old WordPress forum.
#
wordpress = mdb.connect (user = "retrouserwp",
                           db = "retrowordpress",
                       passwd = "skomorox")
select = wordpress.cursor()
select.execute ("""
    SELECT
        wp_users.ID,
        wp_users.user_login,
        wp_users.user_email,
        UNIX_TIMESTAMP(wp_users.user_registered),
        wp_users.display_name
    FROM wp_users, wp_posts
    WHERE wp_users.ID = wp_posts.post_author
    GROUP BY wp_users.ID
    ORDER BY wp_users.ID
    """)
active_users = select.fetchall()

#-----------------------------------
# Create new users on a phpbb forum.
#
bb = mdb.connect (user = "retrobb",
                    db = "retrobb",
                passwd = "hiheyho")
insert = bb.cursor()

# Inherit permissions from this user
retrobsd_user_id = 54

count = 0
for row in active_users:
    # Convert WordPress data to phpbb
    user_id      = int (row[0]) + 70000
    user_regdate = int (row[3])
    user_email   = row[2]
    if len(row[1]) < len(row[4]):
        username = row[1]
    else:
        username = row[4]
    print "%-5d %-10lu %-23s %s" % (user_id, user_regdate, username, user_email)

    count = count + 1
    insert.execute ("""
        INSERT INTO phpbb_users(
            user_id,
            user_type,
            group_id,
            user_perm_from,
            user_regdate,
            user_passchg,
            username,
            username_clean,
            user_email,
            user_lang,
            user_dateformat,
            user_style,
            user_full_folder,
            user_topic_sortby_type,
            user_topic_sortby_dir,
            user_post_sortby_type,
            user_post_sortby_dir,
            user_options,
            user_new
        ) VALUES (%d, %d, %d, %d, %u, %u, '%s', '%s', '%s', '%s', '%s',
                  %d, %d, '%s', '%s', '%s', '%s', %d, %d)
        """ % (
            user_id,                        # user_id
            1,                              # user_type = blocked
            2,                              # group_id
            retrobsd_user_id,               # user_perm_from
            user_regdate,                   # user_regdate
            user_regdate,                   # user_passchg
            '[' + username + ']',           # username
            '[' + username + ']',           # username_clean
            '[' + user_email + ']',         # user_email
            'en_us',                        # user_lang
            'd M Y, H:i',                   # user_dateformat
            2,                              # user_style
            -3L,                            # user_full_folder
            't',                            # user_topic_sortby_type
            'd',                            # user_topic_sortby_dir
            't',                            # user_post_sortby_type
            'a',                            # user_post_sortby_dir
            230271L,                        # user_options
            1,                              # user_new
        ))

bb.commit()
print "Moved", count, "users."

#
# Update the number of users.
#
#select = bb.cursor()
#select.execute ("""
#    SELECT config_value
#      FROM phpbb_config
#     WHERE config_name = 'num_users'
#    """)
#num_users = int (select.fetchall()[0][0])

#num_users = num_users + count
#action = bb.cursor()
#action.execute ("""
#    UPDATE phpbb_config
#       SET config_value = %d
#     WHERE config_name = 'num_users'
#    """ % num_users)
#bb.commit()

# TODO:
# user_form_salt              '495c8a07fecf5590',
# user_email_hash             43523174625L,
# user_ip                     '98.234.180.30',
# user_password               '$H$9C6/dc/3JSM1aCtjNcNOWR0lR6OzQe.',
