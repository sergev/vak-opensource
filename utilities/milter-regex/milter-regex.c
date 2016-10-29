/* $Id: milter-regex.c,v 1.7 2004-11-11 22:25:32 vak Exp $ */

/*
 * Copyright (c) 2003-2004 Daniel Hartmeier
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

static const char rcsid[] = "$Id: milter-regex.c,v 1.7 2004-11-11 22:25:32 vak Exp $";

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <pthread.h>
#ifdef __linux__
#include <grp.h>
#endif
#include <libmilter/mfapi.h>

#include "eval.h"

extern int parse_ruleset(const char *, struct ruleset **, char *, size_t);

static const char	*rule_file_name = "/etc/milter-regex.conf";
static int		 debug = 0;

struct context {
	struct ruleset	*rs;
	int		*res;
	char		 buf[2048];	/* longer body lines are wrapped */
	unsigned	 pos;		/* write position within buf */
	char		 host[128];
	char		 addr[64];
	char		 hdr_from[128];
	char		 hdr_to[128];
	char		 hdr_subject[128];
};

static sfsistat		 setreply(SMFICTX *, struct context *,
			    const struct action *, char *, char *);
static struct ruleset	*get_ruleset(void);
static sfsistat		 cb_connect(SMFICTX *, char *, _SOCK_ADDR *);
static sfsistat		 cb_helo(SMFICTX *, char *);
static sfsistat		 cb_envfrom(SMFICTX *, char **);
static sfsistat		 cb_envrcpt(SMFICTX *, char **);
static sfsistat		 cb_header(SMFICTX *, char *, char *);
static sfsistat		 cb_eoh(SMFICTX *);
static sfsistat		 cb_body(SMFICTX *, u_char *, size_t);
static sfsistat		 cb_eom(SMFICTX *);
static sfsistat		 cb_abort(SMFICTX *);
static sfsistat		 cb_close(SMFICTX *);
static void		 usage(const char *);
static void		 msg(int, struct context *, const char *, ...);

#define USER		"_milter-regex"
#define OCONN		"unix:/var/spool/milter-regex/sock"
#define RCODE_REJECT	"554"
#define RCODE_TEMPFAIL	"451"
#define XCODE_REJECT	"5.7.1"
#define XCODE_TEMPFAIL	"4.7.1"
#define	MAXRS		16

static const char	*user = USER;

#if defined(__FreeBSD__) && __FreeBSD__ <= 30
/* no sockaddr_in6 in FreeBSD 3.4 */
#undef AF_INET6
#endif

#if __linux__ || __sun__
#define	ST_MTIME st_mtime
extern size_t	 strlcpy(char *, const char *, size_t);
#else
#define	ST_MTIME st_mtimespec
#endif

#ifdef __sun__
int
daemon(int nochdir, int noclose)
{
	pid_t pid;
	int fd;

	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork: %s\n", strerror(errno));
		return (1);
	} else if (pid > 0)
		_exit(0);
	if ((pid = setsid()) == -1) {
		fprintf(stderr, "setsid: %s\n", strerror(errno));
		return (1);
	}
	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork: %s\n", strerror(errno));
		return (1);
	} else if (pid > 0)
		_exit(0);
	if (!nochdir && chdir("/")) {
		fprintf(stderr, "chdir: %s\n", strerror(errno));
		return (1);
	}
	if (!noclose) {
		dup2(fd, fileno(stdout));
		dup2(fd, fileno(stderr));
		dup2(open("/dev/null", O_RDONLY, 0), fileno(stdin));
	}
	return (0);
}
#endif

void
drop_privileges ()
{
	struct passwd *pw;

	if (getuid() != 0)
		return;
	pw = getpwnam(user);
	if (! pw) {
		fprintf(stderr, "getpwnam: %s: %s\n", user,
		    strerror(errno));
		exit (1);
	}
	setgroups(1, &pw->pw_gid);
	if (setegid(pw->pw_gid) || setgid(pw->pw_gid)) {
		fprintf(stderr, "setgid: %s\n", strerror(errno));
		exit (1);
	}
	if (
#if ! ( __linux__ || __sun__ )
	    seteuid(pw->pw_uid) ||
#endif
	    setuid(pw->pw_uid)) {
		fprintf(stderr, "setuid: %s\n", strerror(errno));
		exit (1);
	}
}

static sfsistat
setreply(SMFICTX *ctx, struct context *context, const struct action *action,
	char *command, char *arg)
{
	int priority;
	char *reply;

	switch (action->type) {
	case SMFIS_REJECT:
		priority = LOG_NOTICE;
		reply = "REJECT";
		break;
	case SMFIS_TEMPFAIL:
		priority = LOG_NOTICE;
		reply = "TEMPFAIL";
		break;
	case SMFIS_DISCARD:
		priority = LOG_NOTICE;
		reply = "DISCARD";
		break;
	case SMFIS_CONTINUE:
		priority = LOG_DEBUG;
		reply = "CONTINUE";
		break;
	case SMFIS_ACCEPT:
	default:
		priority = LOG_INFO;
		reply = "ACCEPT";
		break;
	}
	if (! action->msg || ! *action->msg)
		msg(priority, context, arg ? "%s %s %s" : "%s %s",
			reply, command, arg);
	else if (arg)
		msg(priority, context, "%s %s %s: %s",
			reply, command, arg, action->msg);
	else
		msg(priority, context, "%s %s: %s",
			reply, command, action->msg);

	if (action->type == SMFIS_REJECT &&
	    smfi_setreply(ctx, RCODE_REJECT, XCODE_REJECT,
	    (char *)action->msg) != MI_SUCCESS)
		msg(LOG_ERR, context, "smfi_setreply");
	if (action->type == SMFIS_TEMPFAIL &&
	    smfi_setreply(ctx, RCODE_TEMPFAIL, XCODE_TEMPFAIL,
	    (char *)action->msg) != MI_SUCCESS)
		msg(LOG_ERR, context, "smfi_setreply");
	return (action->type);
}

static struct ruleset *
get_ruleset(void)
{
	static struct ruleset *rs[MAXRS] = {};
	static int cur = 0;
	static time_t last_check = 0;
	static struct stat sbo;
	static pthread_mutex_t mutex;
	time_t t = time(NULL);
	int load = 0;

	if (!last_check) {
		drop_privileges ();
		memset(&sbo, 0, sizeof(sbo));
		if (pthread_mutex_init(&mutex, 0)) {
			msg(LOG_ERR, NULL, "parse_ruleset: pthread_mutex_init failed");
			return NULL;
		}
	}
	if (pthread_mutex_lock(&mutex))
		msg(LOG_ERR, NULL, "parse_ruleset: pthread_mutex_lock failed");
	if (t - last_check >= 10) {
		struct stat sb;

		last_check = t;
		memset(&sb, 0, sizeof(sb));
		if (stat(rule_file_name, &sb))
			msg(LOG_ERR, NULL, "get_ruleset: stat: %s: %s",
			    rule_file_name, strerror(errno));
		else if (memcmp(&sb.ST_MTIME, &sbo.ST_MTIME,
		    sizeof(sb.ST_MTIME))) {
			memcpy(&sbo.ST_MTIME, &sb.ST_MTIME,
			    sizeof(sb.ST_MTIME));
			load = 1;
		}
	}
	if (load || rs[cur] == NULL) {
		int i;
		char err[8192];

		msg(LOG_DEBUG, NULL, "loading new configuration file");
		for (i = 0; i < MAXRS; ++i)
			if (rs[i] != NULL && rs[i]->refcnt == 0) {
				msg(LOG_DEBUG, NULL, "freeing unused ruleset "
				    "%d/%d", i, MAXRS);
				free_ruleset(rs[i]);
				rs[i] = NULL;
			}
		for (i = 0; i < MAXRS; ++i)
			if (rs[i] == NULL)
				break;
		if (i == MAXRS)
			msg(LOG_ERR, NULL, "all rulesets are in use, cannot "
			    "load new one", MAXRS);
		else if (parse_ruleset(rule_file_name, &rs[i], err,
		    sizeof(err)) || rs[i] == NULL)
			msg(LOG_ERR, NULL, "parse_ruleset: %s", err);
		else {
			msg(LOG_INFO, NULL, "configuration file %s loaded "
			    "successfully", rule_file_name);
			cur = i;
		}
	}
	if (pthread_mutex_unlock(&mutex))
		msg(LOG_ERR, NULL, "parse_ruleset: pthread_mutex_unlock failed");
	return (rs[cur]);
}

static sfsistat
cb_connect(SMFICTX *ctx, char *name, _SOCK_ADDR *sa)
{
	struct context *context;
	const struct action *action;
	char host[64];

	context = calloc(1, sizeof(*context));
	if (context == NULL) {
		msg(LOG_ERR, NULL, "cb_connect: calloc: %s", strerror(errno));
		return (SMFIS_ACCEPT);
	}
	context->rs = get_ruleset();
	if (context->rs == NULL) {
		free(context);
		msg(LOG_ERR, NULL, "cb_connect: get_ruleset");
		return (SMFIS_ACCEPT);
	}
	context->res = calloc(context->rs->maxidx, sizeof(*context->res));
	if (context->res == NULL) {
		free(context);
		msg(LOG_ERR, NULL, "cb_connect: calloc: %s", strerror(errno));
		return (SMFIS_ACCEPT);
	}
	if (smfi_setpriv(ctx, context) != MI_SUCCESS) {
		free(context->res);
		free(context);
		msg(LOG_ERR, NULL, "cb_connect: smfi_setpriv");
		return (SMFIS_ACCEPT);
	}
	context->rs->refcnt++;

	strlcpy(host, "unknown", sizeof(host));
	switch (sa ? sa->sa_family : 0) {
	case AF_INET: {
		struct sockaddr_in *sin = (struct sockaddr_in *)sa;

		if (inet_ntop(AF_INET, &sin->sin_addr.s_addr, host,
		    sizeof(host)) == NULL)
			msg(LOG_ERR, NULL, "cb_connect: inet_ntop: %s",
			    strerror(errno));
		break;
	}
#ifdef AF_INET6
	case AF_INET6: {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;

		if (inet_ntop(AF_INET6, &sin6->sin6_addr, host,
		    sizeof(host)) == NULL)
			msg(LOG_ERR, NULL, "cb_connect: inet_ntop: %s",
			    strerror(errno));
		break;
	}
#endif
	}
	strlcpy(context->host, name, sizeof(context->host));
	strlcpy(context->addr, host, sizeof(context->addr));
	msg(LOG_DEBUG, context, "cb_connect('%s', '%s')", name, host);
	if ((action = eval_cond(context->rs, context->res, COND_CONNECT,
	    name, host)) != NULL)
		return (setreply(ctx, context, action, "CONNECT", 0));
	if ((action = eval_end(context->rs, context->res, COND_CONNECT)) !=
	    NULL)
		return (setreply(ctx, context, action, "CONNECT", 0));
	return (SMFIS_CONTINUE);
}

static sfsistat
cb_helo(SMFICTX *ctx, char *arg)
{
	struct context *context;
	const struct action *action;

	if ((context = (struct context *)smfi_getpriv(ctx)) == NULL) {
		msg(LOG_ERR, NULL, "cb_helo: smfi_getpriv");
		return (SMFIS_ACCEPT);
	}
	msg(LOG_DEBUG, context, "cb_helo('%s')", arg);
	/* multiple HELO imply RSET in sendmail */
	eval_clear(context->rs, context->res, COND_HELO);
	if ((action = eval_cond(context->rs, context->res, COND_HELO,
	    arg, NULL)) != NULL)
		return (setreply(ctx, context, action, "HELO", arg));
	if ((action = eval_end(context->rs, context->res, COND_HELO)) !=
	    NULL)
		return (setreply(ctx, context, action, "HELO", arg));
	return (SMFIS_CONTINUE);
}

static sfsistat
cb_envfrom(SMFICTX *ctx, char **args)
{
	struct context *context;
	const struct action *action;

	if ((context = (struct context *)smfi_getpriv(ctx)) == NULL) {
		msg(LOG_ERR, NULL, "cb_envfrom: smfi_getpriv");
		return (SMFIS_ACCEPT);
	}
	/* multiple MAIL FROM indicate separate messages */
	eval_clear(context->rs, context->res, COND_ENVFROM);
	context->hdr_subject[0] = 0;
	if (*args != NULL) {
		msg(LOG_DEBUG, context, "cb_envfrom('%s')", *args);
		if ((action = eval_cond(context->rs, context->res, COND_ENVFROM,
		    *args, NULL)) != NULL)
			return (setreply(ctx, context, action, "ENVFROM",
				*args));
	}
	if ((action = eval_end(context->rs, context->res, COND_ENVFROM)) !=
	    NULL)
		return (setreply(ctx, context, action, "ENVFROM", *args));
	return (SMFIS_CONTINUE);
}

static sfsistat
cb_envrcpt(SMFICTX *ctx, char **args)
{
	struct context *context;
	const struct action *action;

	if ((context = (struct context *)smfi_getpriv(ctx)) == NULL) {
		msg(LOG_ERR, NULL, "cb_envrcpt: smfi_getpriv");
		return (SMFIS_ACCEPT);
	}
	if (*args != NULL) {
		msg(LOG_DEBUG, context, "cb_envrcpt('%s')", *args);
		if ((action = eval_cond(context->rs, context->res, COND_ENVRCPT,
		    *args, NULL)) != NULL)
			return (setreply(ctx, context, action, "ENVRCPT",
				*args));
	}
	return (SMFIS_CONTINUE);
}

static sfsistat
cb_header(SMFICTX *ctx, char *name, char *value)
{
	struct context *context;
	const struct action *action;

	if ((context = (struct context *)smfi_getpriv(ctx)) == NULL) {
		msg(LOG_ERR, context, "cb_header: smfi_getpriv");
		return (SMFIS_ACCEPT);
	}
	if ((action = eval_end(context->rs, context->res, COND_ENVRCPT)) !=
	    NULL)
		return (setreply(ctx, context, action, "ENVRCPT", 0));
	msg(LOG_DEBUG, context, "cb_header('%s', '%s')", name, value);
	if (!strcasecmp(name, "From"))
		strlcpy(context->hdr_from, value, sizeof(context->hdr_from));
	else if (!strcasecmp(name, "To"))
		strlcpy(context->hdr_to, value, sizeof(context->hdr_to));
	else if (!strcasecmp(name, "Subject"))
		strlcpy(context->hdr_subject, value,
		    sizeof(context->hdr_subject));
	if ((action = eval_cond(context->rs, context->res, COND_HEADER,
	    name, value)) != NULL)
		return (setreply(ctx, context, action, name, value));
	return (SMFIS_CONTINUE);
}

static sfsistat
cb_eoh(SMFICTX *ctx)
{
	struct context *context;
	const struct action *action;

	if ((context = (struct context *)smfi_getpriv(ctx)) == NULL) {
		msg(LOG_ERR, NULL, "cb_eoh: smfi_getpriv");
		return (SMFIS_ACCEPT);
	}
	msg(LOG_DEBUG, context, "cb_eoh()");
	memset(context->buf, 0, sizeof(context->buf));
	context->pos = 0;
	if ((action = eval_end(context->rs, context->res, COND_HEADER)) !=
	    NULL)
		return (setreply(ctx, context, action, "HEADER", 0));
	return (SMFIS_CONTINUE);
}

static sfsistat
cb_body(SMFICTX *ctx, u_char *chunk, size_t size)
{
	struct context *context;

	if ((context = (struct context *)smfi_getpriv(ctx)) == NULL) {
		msg(LOG_ERR, NULL, "cb_body: smfi_getpriv");
		return (SMFIS_ACCEPT);
	}
	for (; size > 0; size--, chunk++) {
		context->buf[context->pos] = *chunk;
		if (context->buf[context->pos] == '\n' ||
		    context->pos == sizeof(context->buf) - 1) {
			const struct action *action;

			if (context->pos > 0 &&
			    context->buf[context->pos - 1] == '\r')
				context->buf[context->pos - 1] = 0;
			else
				context->buf[context->pos] = 0;
			context->pos = 0;
			msg(LOG_DEBUG, context, "cb_body('%s')", context->buf);
			if ((action = eval_cond(context->rs, context->res,
			    COND_BODY, context->buf, NULL)) != NULL)
				return (setreply(ctx, context, action,
					"BODY", 0));
		} else
			context->pos++;
	}
	return (SMFIS_CONTINUE);
}

static sfsistat
cb_eom(SMFICTX *ctx)
{
	struct context *context;
	const struct action *action;

	if ((context = (struct context *)smfi_getpriv(ctx)) == NULL) {
		msg(LOG_ERR, NULL, "cb_eom: smfi_getpriv");
		return (SMFIS_ACCEPT);
	}
	msg(LOG_DEBUG, context, "cb_eom()");
	if ((action = eval_end(context->rs, context->res, COND_BODY)) !=
	    NULL)
		return (setreply(ctx, context, action, "BODY", 0));
	else
		msg(LOG_DEBUG, context, "ACCEPT, From: %s, To: %s, "
		    "Subject: %s", context->hdr_from, context->hdr_to,
		    context->hdr_subject);
	return (SMFIS_CONTINUE);
}

static sfsistat
cb_abort(SMFICTX *ctx)
{
	struct context *context;

	if ((context = (struct context *)smfi_getpriv(ctx)) == NULL) {
		msg(LOG_ERR, NULL, "cb_abort: smfi_getpriv");
		return (SMFIS_ACCEPT);
	}
	msg(LOG_DEBUG, context, "cb_abort()");
	/* a RSET doesn't clear HELO in sendmail, but MAIL FROM */
	eval_clear(context->rs, context->res, COND_ENVFROM);
	return (SMFIS_CONTINUE);
}

static sfsistat
cb_close(SMFICTX *ctx)
{
	struct context *context;

	context = (struct context *)smfi_getpriv(ctx);
	msg(LOG_DEBUG, context, "cb_close()");
	if (context != NULL) {
		smfi_setpriv(ctx, NULL);
		free(context->res);
		context->rs->refcnt--;
		free(context);
	}
	return (SMFIS_CONTINUE);
}

struct smfiDesc smfilter = {
	"milter-regex",	/* filter name */
	SMFI_VERSION,	/* version code -- do not change */
	SMFIF_ADDHDRS|SMFIF_CHGHDRS|SMFIF_ADDRCPT|SMFIF_DELRCPT, /* flags */
	cb_connect,	/* connection info filter */
	cb_helo,	/* SMTP HELO command filter */
	cb_envfrom,	/* envelope sender filter */
	cb_envrcpt,	/* envelope recipient filter */
	cb_header,	/* header filter */
	cb_eoh,		/* end of header */
	cb_body,	/* body block */
	cb_eom,		/* end of message */
	cb_abort,	/* message aborted */
	cb_close	/* connection cleanup */
};

static void
msg(int priority, struct context *context, const char *fmt, ...)
{
	va_list ap;
	char msg[8192];

	if (priority == LOG_DEBUG && debug == 0)
		return;
	va_start(ap, fmt);
	if (context != NULL)
		snprintf(msg, sizeof(msg), "%s: ", context->host);
	else
		msg[0] = 0;
	vsnprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), fmt, ap);
	if (debug > 1)
		printf("syslog: %s\n", msg);
	else
		syslog(priority, "%s", msg);
	va_end(ap);
}

static void
usage(const char *argv0)
{
	fprintf(stderr, "usage: %s [-d] [-c config] [-u user] "
	    "[-p pipe]\n", argv0);
	exit(1);
}

void
die(const char *reason)
{
	msg(LOG_ERR, NULL, "die: %s", reason);
	smfi_stop();
	sleep(60);
	/* not reached, smfi_stop() kills thread */
	abort();
}

int
main(int argc, char **argv)
{
	int ch;
	const char *oconn = OCONN;
	sfsistat r = MI_FAILURE;
	const char *ofile = NULL;

	tzset();
	openlog("milter-regex", LOG_PID | LOG_NDELAY, LOG_MAIL);

	while ((ch = getopt(argc, argv, "c:dp:u:")) != -1) {
		switch (ch) {
		case 'c':
			rule_file_name = optarg;
			break;
		case 'd':
			++debug;
			break;
		case 'p':
			oconn = optarg;
			break;
		case 'u':
			user = optarg;
			break;
		default:
			usage(argv[0]);
		}
	}
	if (argc != optind) {
		fprintf(stderr, "unknown command line argument: %s ...",
		    argv[optind]);
		usage(argv[0]);
	}

	if (!strncmp(oconn, "unix:", 5))
		ofile = oconn + 5;
	else if (!strncmp(oconn, "local:", 6))
		ofile = oconn + 6;
	if (ofile != NULL)
		unlink(ofile);

	if (smfi_setconn((char *)oconn) != MI_SUCCESS) {
		fprintf(stderr, "smfi_setconn: %s: failed\n", oconn);
		goto done;
	}

	if (smfi_register(smfilter) != MI_SUCCESS) {
		fprintf(stderr, "smfi_register: failed\n");
		goto done;
	}

	if (eval_init(SMFIS_ACCEPT)) {
		fprintf(stderr, "eval_init: failed\n");
		goto done;
	}

	/* daemonize (detach from controlling terminal) */
	if (debug < 2 && daemon(0, 0)) {
		fprintf(stderr, "daemon: %s\n", strerror(errno));
		goto done;
	}
	umask(0177);

	msg(LOG_INFO, NULL, "started: %s", rcsid);
	r = smfi_main();
	if (r != MI_SUCCESS)
		msg(LOG_ERR, NULL, "smfi_main: terminating due to error");
	else
		msg(LOG_INFO, NULL, "smfi_main: terminating without error");

done:
	return (r);
}
