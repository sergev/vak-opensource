/* $Id: eval.h,v 1.3 2004-11-05 19:15:09 vak Exp $ */

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

#ifndef _EVAL_H_
#define _EVAL_H_

#include <regex.h>

enum { VAL_UNDEF=0, VAL_TRUE, VAL_FALSE };
enum { COND_CONNECT, COND_HELO, COND_ENVFROM, COND_ENVRCPT,
    COND_HEADER, COND_BODY, COND_MAX };
enum { EXPR_AND, EXPR_OR, EXPR_NOT, EXPR_COND };

struct expr;

struct cond {
	struct cond_arg {
		char	*src;
		int	 empty;
		int	 not;
		int	 debug;
		regex_t	 re;
	}			 args[2];
	struct expr_list	*expr;
	unsigned		 idx;
	int			 oneshot;
};

struct cond_list {
	struct cond		*cond;
	struct cond_list	*next;
};

struct action;

struct expr {
	int			 type;
	struct expr		*args[2];
	struct cond		*cond;
	struct action		*action;
	struct expr_list	*expr;
	unsigned		 idx;
};

struct expr_list {
	struct expr		*expr;
	struct expr_list	*next;
};

struct action {
	int			 type;
	char			*msg;
	unsigned		 idx;
};

struct action_list {
	struct action		*action;
	struct action_list	*next;
};

struct ruleset {
	struct cond_list	*cond[COND_MAX];
	struct action_list	*action;
	unsigned		 maxidx;
	unsigned		 refcnt;
};

int		 eval_init(int);
struct ruleset	*create_ruleset(void);
struct expr	*create_cond(struct ruleset *, int, const char *,
		    const char *, int);
struct expr	*create_expr(struct ruleset *, int, struct expr *,
		    struct expr *);
struct action	*create_action(struct ruleset *, int, const char *);
struct action	*eval_cond(struct ruleset *, int *, int,
		    const char *, const char *);
struct action	*eval_end(struct ruleset *, int *, int);
void		 eval_clear(struct ruleset *, int *, int);
void		 free_ruleset(struct ruleset *);

#endif
