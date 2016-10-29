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
extern char *h_from_;

void scanheader (FILE *fd);
void freeheader (void);
