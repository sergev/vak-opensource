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

for row in select.fetchall():
    #print row
    print "%-5d %-23s %-31s %-7s %s" % row
