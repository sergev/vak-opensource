/*
 * $Header: header.h,v 2.0 92/08/05 08:43:24 vak Exp $
 * $Log:	header.h,v $
 * Revision 2.0  92/08/05  08:43:24  vak
 * 
 * 
 * Revision 1.3  92/06/19  22:20:04  usenet
 * Fix errors. Rudnev
 * 
 * Revision 1.2  92/06/19  22:12:01  usenet
 * 
 * Revision 1.2  92/06/19  22:08:47  usenet
 * News Server
 * 
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
extern char *h_from_;
