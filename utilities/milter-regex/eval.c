/* $Id: eval.c,v 1.4 2004-11-05 19:15:09 vak Exp $ */

/*
 * Copyright (c) 2004 Daniel Hartmeier
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

static const char rcsid[] = "$Id: eval.c,v 1.4 2004-11-05 19:15:09 vak Exp $";

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "eval.h"

#ifndef REG_BASIC
#define REG_BASIC	0
#endif

extern int	 yyerror(char *, ...);
extern void	 die(const char *);
static void	 mutex_lock(void);
static void	 mutex_unlock(void);
static int	 check_cond(struct cond *, const char *, const char *);
static void	 push_expr_result(struct expr *, int, int *);
static void	 push_cond_result(struct cond *, int, int *);
static int	 build_regex(struct cond_arg *);
static void	 free_expr_list(struct expr_list *, struct expr *);

static pthread_mutex_t	 mutex;
static struct action	 default_action;

int
eval_init(int type)
{
	memset(&default_action, 0, sizeof(default_action));
	default_action.type = type;
	if (pthread_mutex_init(&mutex, 0))
		return (1);
	else
		return (0);
}

static void
mutex_lock(void)
{
	if (pthread_mutex_lock(&mutex))
		die("pthread_mutex_lock");
}

static void
mutex_unlock(void)
{
	if (pthread_mutex_unlock(&mutex))
		die("pthread_mutex_unlock");
}

struct ruleset *
create_ruleset(void)
{
	struct ruleset *rs;

	rs = calloc(1, sizeof(struct ruleset));
	if (rs == NULL) {
		yyerror("create_ruleset: calloc: %s", strerror(errno));
		return (NULL);
	}
	return (rs);
}

struct expr *
create_cond(struct ruleset *rs, int type, const char *a, const char *b,
	int oneshot)
{
	struct cond *c = NULL;
	struct cond_list *cl = NULL;
	struct expr *e = NULL;
	struct expr_list *elc = NULL;

	mutex_lock();
	e = calloc(1, sizeof(struct expr));
	if (e == NULL)
		goto error;
	elc = calloc(1, sizeof(struct expr_list));
	if (elc == NULL)
		goto error;

	for (cl = rs->cond[type]; cl != NULL; cl = cl->next) {
		if ((cl->cond->args[0].src == NULL) != (a == NULL) ||
		    (cl->cond->args[1].src == NULL) != (b == NULL) ||
		    (a != NULL && strcmp(a, cl->cond->args[0].src)) ||
		    (b != NULL && strcmp(b, cl->cond->args[1].src)) ||
		    cl->cond->oneshot != oneshot)
			continue;
		break;
	}
	if (cl != NULL)
		c = cl->cond;
	else {
		cl = calloc(1, sizeof(struct cond_list));
		if (cl == NULL)
			goto error;
		c = calloc(1, sizeof(struct cond));
		if (c == NULL)
			goto error;

		if (a != NULL) {
			c->args[0].src = strdup(a);
			if (c->args[0].src == NULL)
				goto error;
			if (build_regex(&c->args[0]))
				goto error;
		}
		if (b != NULL) {
			c->args[1].src = strdup(b);
			if (c->args[1].src == NULL)
				goto error;
			if (build_regex(&c->args[1]))
				goto error;
		}
		c->idx = rs->maxidx++;
		c->oneshot = oneshot;
		cl->cond = c;
		cl->next = rs->cond[type];
		rs->cond[type] = cl;
	}

	e->type = EXPR_COND;
	e->cond = c;
	e->idx = rs->maxidx++;
	elc->expr = e;
	elc->next = c->expr;
	c->expr = elc;
	mutex_unlock();
	return (e);

error:
	if (elc != NULL)
		free(elc);
	if (e != NULL)
		free(e);
	if (cl != NULL)
		free(cl);
	if (c != NULL) {
		if (!c->args[1].empty)
			regfree(&c->args[1].re);
		if (c->args[1].src != NULL)
			free(c->args[1].src);
		if (!c->args[0].empty)
			regfree(&c->args[0].re);
		if (c->args[0].src != NULL)
			free(c->args[0].src);
		free(c);
	}
	mutex_unlock();
	return (NULL);
}

struct expr *
create_expr(struct ruleset *rs, int type, struct expr *a, struct expr *b)
{
	struct expr *e = NULL;
	struct expr_list *ela = NULL, *elb = NULL;

	mutex_lock();
	e = calloc(1, sizeof(struct expr));
	if (e == NULL)
		goto error;
	if (a != NULL) {
		ela = calloc(1, sizeof(struct expr_list));
		if (ela == NULL)
			goto error;
	}
	if (b != NULL) {
		elb = calloc(1, sizeof(struct expr_list));
		if (elb == NULL)
			goto error;
	}
	e->type = type;
	e->idx = rs->maxidx++;
	if (a != NULL) {
		e->args[0] = a;
		ela->expr = e;
		ela->next = a->expr;
		a->expr = ela;
	}
	if (b != NULL) {
		e->args[1] = b;
		elb->expr = e;
		elb->next = b->expr;
		b->expr = elb;
	}
	mutex_unlock();
	return (e);

error:
	yyerror("create_expr: calloc: %s", strerror(errno));
	if (elb != NULL)
		free(elb);
	if (ela != NULL)
		free(ela);
	if (e != NULL)
		free(e);
	mutex_unlock();
	return (NULL);
}

struct action *
create_action(struct ruleset *rs, int type, const char *msg)
{
	struct action *a = NULL;
	struct action_list *al = NULL;

	mutex_lock();
	a = calloc(1, sizeof(struct action));
	if (a == NULL)
		goto error;
	al = calloc(1, sizeof(struct action_list));
	if (al == NULL)
		goto error;
	a->type = type;
	a->msg = msg == NULL ? NULL : strdup(msg);
	a->idx = rs->maxidx++;
	al->action = a;
	/* tail insert, so actions have same order as file */
	if (rs->action == NULL)
		rs->action = al;
	else {
		struct action_list *t = rs->action;

		while (t->next != NULL)
			t = t->next;
		t->next = al;
	}
	mutex_unlock();
	return (a);

error:
	yyerror("create_action: calloc: %s", strerror(errno));
	if (al != NULL)
		free(al);
	if (a != NULL)
		free(a);
	mutex_unlock();
	return (NULL);
}

static void
clear_actions (struct ruleset *rs, int *res, int type)
{
	struct cond_list *cl;
	struct action_list *al;

	/* Deactivate all actions. */
	for (al = rs->action; al != NULL; al = al->next)
		res[al->action->idx] = VAL_FALSE;

	/* Clear one-shot conditions. */
	for (cl = rs->cond[type]; cl != NULL; cl = cl->next)
		if (cl->cond->oneshot && res[cl->cond->idx] == VAL_TRUE)
			push_cond_result(cl->cond, VAL_UNDEF, res);
}

struct action *
eval_cond(struct ruleset *rs, int *res, int type,
    const char *a, const char *b)
{
	struct cond_list *cl;
	struct action_list *al;

	mutex_lock();
	for (cl = rs->cond[type]; cl != NULL; cl = cl->next) {
		int r;

		if (res[cl->cond->idx] != VAL_UNDEF)
			continue;
		r = check_cond(cl->cond, a, b);
		if (r < 0) {
			mutex_unlock();
			return (NULL);
		} else if (!r)
			push_cond_result(cl->cond, VAL_TRUE, res);
	}
	for (al = rs->action; al != NULL; al = al->next)
		if (res[al->action->idx] == VAL_TRUE) {
			clear_actions (rs, res, type);
			mutex_unlock();
			return (al->action);
		}
	mutex_unlock();
	return (NULL);
}

struct action *
eval_end(struct ruleset *rs, int *res, int type)
{
	struct cond_list *cl;
	struct action_list *al;

	mutex_lock();
	for (cl = rs->cond[type]; cl != NULL; cl = cl->next)
		if (res[cl->cond->idx] == VAL_UNDEF)
			push_cond_result(cl->cond, VAL_FALSE, res);
	for (al = rs->action; al != NULL; al = al->next)
		if (res[al->action->idx] == VAL_TRUE) {
			clear_actions (rs, res, type);
			mutex_unlock();
			return (al->action);
		}
	for (++type; type < COND_MAX; ++type)
		for (cl = rs->cond[type]; cl != NULL; cl = cl->next)
			if (res[cl->cond->idx] == VAL_UNDEF) {
				mutex_unlock();
				return (NULL);
			}
	mutex_unlock();
	return (&default_action);
}

void
eval_clear(struct ruleset *rs, int *res, int type)
{
	struct cond_list *cl;

	mutex_lock();
	for (; type < COND_MAX; ++type)
		for (cl = rs->cond[type]; cl != NULL; cl = cl->next)
			push_cond_result(cl->cond, VAL_UNDEF, res);
	mutex_unlock();
}

static int
check_cond(struct cond *c, const char *a, const char *b)
{
	int i;

	for (i = 0; i < 2; ++i) {
		const char *d = i ? b : a;
		int r;

		if (c->args[i].src == NULL || c->args[i].empty)
			continue;
		if (d == NULL)
			return (-1);
		r = regexec(&c->args[i].re, d, 0, NULL, 0);
		if (c->args[i].debug) {
			syslog(LOG_NOTICE, "pattern %s", c->args[i].src);
			if (! r)
				syslog(LOG_NOTICE, "match %s", d);
			else if (r == REG_NOMATCH)
				syslog(LOG_NOTICE, "nomatch %s", d);
			else
				syslog(LOG_NOTICE, "error%d %s", r, d);
		}
		if (r && r != REG_NOMATCH)
			return (-1);
		if ((r == REG_NOMATCH) != c->args[i].not)
			return (1);
	}
	return (0);
}

static void
push_expr_result(struct expr *e, int val, int *res)
{
	struct expr_list *el;

	if (res[e->idx] == val)
		return;
	res[e->idx] = val;
	if (e->action != NULL)
		res[e->action->idx] = val;
	for (el = e->expr; el != NULL; el = el->next) {
		struct expr *p = el->expr;

		switch (p->type) {
		case EXPR_AND:
			if (res[p->args[0]->idx] == VAL_TRUE &&
			    res[p->args[1]->idx] == VAL_TRUE)
				push_expr_result(p, VAL_TRUE, res);
			else if (res[p->args[0]->idx] == VAL_FALSE ||
			    res[p->args[1]->idx] == VAL_FALSE)
				push_expr_result(p, VAL_FALSE, res);
			else
				push_expr_result(p, VAL_UNDEF, res);
			break;
		case EXPR_OR:
			if (res[p->args[0]->idx] == VAL_TRUE ||
			    res[p->args[1]->idx] == VAL_TRUE)
				push_expr_result(p, VAL_TRUE, res);
			else if (res[p->args[0]->idx] == VAL_FALSE &&
			    res[p->args[1]->idx] == VAL_FALSE)
				push_expr_result(p, VAL_FALSE, res);
			else
				push_expr_result(p, VAL_UNDEF, res);
			break;
		case EXPR_NOT:
			if (val == VAL_TRUE)
				push_expr_result(p, VAL_FALSE, res);
			else if (val == VAL_FALSE)
				push_expr_result(p, VAL_TRUE, res);
			else
				push_expr_result(p, VAL_UNDEF, res);
			break;
		default:
			break;
		}
	}
}

static void
push_cond_result(struct cond *c, int val, int *res)
{
	struct expr_list *el;

	if (res[c->idx] == val)
		return;
	res[c->idx] = val;
	for (el = c->expr; el != NULL; el = el->next) {
		struct expr *e = el->expr;

		if (e->type != EXPR_COND)
			continue;
		push_expr_result(e, val, res);
	}
}

static int
build_regex(struct cond_arg *a)
{
	char del;
	const char *s = a->src, *t;

	a->empty = 1;
	a->not = 0;
	while (*s == ' ' || *s == '\t')
		s++;
	if (!*s) {
		yyerror("build_regex: empty argument");
		return (1);
	}
	del = *s++;
	t = s;
	while (*s && *s != del)
		s++;
	if (!*s) {
		yyerror("build_regex: missing closing delimiter %s", a->src);
		return (1);
	}
	if (s == t) {
		if (s[1]) {
			yyerror("build_regex: empty expression with flags %s",
			    a->src);
			return (1);
		}
	} else {
		char *u;
		int flags = 0, r;

		u = malloc(s - t + 1);
		if (u == NULL) {
			yyerror("build_regex: malloc: %s", strerror(errno));
			return (1);
		}
		memcpy(u, t, s - t);
		u[s - t] = 0;
		s++;
		while (*s) {
			switch (*s) {
			case 'e':
				flags |= REG_EXTENDED;
				break;
			case 'i':
				flags |= REG_ICASE;
				break;
			case 'n':
				a->not = 1;
				break;
			case 'd':
				a->debug = 1;
				break;
			default:
				yyerror("invalid flag %c in %s", *s, a->src);
				free(u);
				return (1);
			}
			++s;
		}
		if (!(flags & REG_EXTENDED))
			flags |= REG_BASIC;
		r = regcomp(&a->re, u, flags);
		if (r) {
			char e[8192];

			regerror(r, &a->re, e, sizeof(e));
			yyerror("regcomp: %s: %s\n", u, e);
			free(u);
			return (1);
		}
		free(u);
		a->empty = 0;
	}
	return (0);
}

static void
free_expr_list(struct expr_list *el, struct expr *a)
{
	struct expr_list *eln;

	while (el != NULL) {
		struct expr *e = el->expr;

		eln = el->next;
		if (e != NULL) {
			int i, used = 0;

			for (i = 0; i < 2; ++i)
				if (e->args[i] != NULL) {
					if (e->args[i] == a)
						e->args[i] = NULL;
					else
						used = 1;
				}
			if (!used) {
				free_expr_list(e->expr, e);
				free(e);
			}
		}
		free(el);
		el = eln;
	}
}

void
free_ruleset(struct ruleset *rs)
{
	int i;
	struct action_list *al, *aln;

	mutex_lock();
	if (rs == NULL || rs->refcnt) {
		mutex_unlock();
		return;
	}
	for (i = 0; i < COND_MAX; ++i) {
		struct cond_list *cl = rs->cond[i], *cln;

		while (cl != NULL) {
			struct cond *c = cl->cond;

			cln = cl->next;
			if (c != NULL) {
				int j;

				for (j = 0; j < 2; ++j)
					if (c->args[j].src != NULL) {
						free(c->args[j].src);
						if (!c->args[j].empty)
							regfree(&c->args[j].re);
					}
				free_expr_list(c->expr, NULL);
				free(c);
			}
			free(cl);
			cl = cln;
		}
	}
	al = rs->action;
	while (al != NULL) {
		struct action *a = al->action;

		aln = al->next;
		if (a != NULL) {
			if (a->msg != NULL)
				free(a->msg);
			free(a);
		}
		free(al);
		al = aln;
	}
	free(rs);
	mutex_unlock();
}
