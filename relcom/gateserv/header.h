/*
 * Описания переменных и функций чтения заголовка письма.
 *
 * Copyright (C) 1992-1995 Cronyx Ltd.
 * Автор: Сергей Вакуленко, vak@cronyx.ru
 * Wed Feb  8 18:29:31 MSK 1995
 */
struct headertable {
	char    *name;
	char    **value;
};

extern char *h_approved;
extern char *h_date;
extern char *h_distribution;
extern char *h_expires;
extern char *h_followup_to;
extern char *h_from;
extern char *h_keywords;
extern char *h_message_id;
extern char *h_newsgroups;
extern char *h_organization;
extern char *h_references;
extern char *h_reply_to;
extern char *h_resent_from;
extern char *h_sender;
extern char *h_subject;
extern char *h_summary;
extern char *h_supersed;
extern char *h_mime_version;
extern char *h_content_type;
extern char *h_content_transfer_encoding;
extern char *h_content_length;
extern char *h_to;
extern char *h_cc;
extern char *h_from_;

extern void scanheader (FILE *fd);
extern void freeheader (void);
