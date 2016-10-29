/*
 * $Header: lang.h,v 2.0 92/08/05 08:43:25 vak Exp $
 * $Log:	lang.h,v $
 * Revision 2.0  92/08/05  08:43:25  vak
 * 
 * 
 * Revision 1.3  92/06/19  22:20:09  usenet
 * Fix errors. Rudnev
 * 
 * Revision 1.2  92/06/19  22:12:07  usenet
 * 
 */
/*
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

/* server.c */

extern char *permission_denied;
extern char *server_reply;
extern char *news_server;
extern char *to_be_continued;

/* reader.c */

extern char *command_not_implemented;
extern char *too_long_line;
extern char *too_many_arguments;

/* commands.c */

extern char *no_commands_entered;
extern char *no_help_available;
extern char *group_not_found;
extern char *no_group_selected;
extern char *no_such_group;
extern char *you_are_not_subscribed;
extern char *you_are_not_subscribed_to_group;
extern char *no_such_article;
extern char *user_unsubscribed_from_group;
extern char *user_subscribed_to_group;
extern char *in_feed_mode;
extern char *in_notify_mode;
extern char *in_rfeed_mode;
extern char *pack_turned_on;
extern char *pack_turned_off;
extern char *aging_turned_on;
extern char *aging_turned_off;
extern char *no_such_package;
extern char *user_removed_from_list;
extern char *you_are_not_registered;
extern char *you_are_unknown;
extern char *short_list_of_news_groups;

/* submit.c */

extern char *cannot_post_article;
extern char *unsent_message_follows;
extern char *groups_cannot_post;
extern char *dont_exist;
extern char *no_moderator;
extern char *closed_for_posting;
extern char *bad_newsgroups_field;

/* notify.c */

extern char *new_groups_created;
extern char *groups_deleted;
extern char *list_of_articles;
extern char *info_from_server;
extern char *uncomment_this;
extern char *uncomment_article_commands;
extern char *and_send_list_back;
extern char *you_send_nothing;
extern char *rest_in_pease;

/* newnews.c */
extern char *batch_of_articles;
extern char *group_dont_allow;
