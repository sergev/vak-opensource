/*
 * Simple RTL simulator.
 *
 * Copyright (C) 2013 Serge Vakulenko <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
#include <stdlib.h>
#include <setjmp.h>

/*
 * Forward typedefs
 */
typedef struct signal_t signal_t;       /* Signal */
typedef struct process_t process_t;     /* Process */
typedef struct hook_t hook_t;           /* Sensitivity hook */
typedef unsigned long long value_t;     /* Value of signal or time */

/*--------------------------------------
 * Time
 */
value_t time_ticks;             /* Current simulation time */

/*--------------------------------------
 * Signal
 */
struct signal_t {
    signal_t    *next;          /* Member of active list */
    hook_t      *activate;      /* Sensitivity list: processes to activate */
    const char  *name;          /* Name for log file */
    value_t     value;          /* Current value */
    value_t     new_value;      /* Value for next cycle */
};

signal_t *signal_active;        /* List of active signals for the current cycle */

void signal_set (signal_t *sig, value_t value);

#define signal_init(_name, _value) { 0, 0, _name, _value, _value }

/*--------------------------------------
 * Process
 */
struct process_t {
    process_t   *next;          /* Member of event queue */
    const char  *name;          /* Name for log file */
    value_t     delay;          /* Time to wait */
    jmp_buf     context;        /* User context for thread switching */
};

process_t *process_current;     /* Current running process */
process_t *process_queue;       /* Queue of pending events */
process_t process_main;         /* Main process */

void process_wait (void);
void process_delay (unsigned ticks);
process_t *_process_setup (process_t *proc, const char *name,
    void (*func)(), unsigned nbytes);

#define process_init(_name, _func, _nbytes) (process_queue = \
    _process_setup (alloca (_nbytes), _name, _func, _nbytes))


/*--------------------------------------
 * Sensitivity hook
 */
struct hook_t {
    hook_t      *next, *prev;   /* Member of sensitivity list */
    process_t   *process;       /* Process to activate */
    int         edge;           /* Edge, if nonzero */
#define POSEDGE 1
#define NEGEDGE 2
};

#define _signal_hook(_sig, _edge, _hook) { \
        (_hook)->process = process_current; \
        (_hook)->edge = (_edge); \
        (_hook)->next = (_sig)->activate; \
        (_hook)->prev = 0; \
        if ((_hook)->next != 0) (_hook)->next->prev = (_hook); \
        (_sig)->activate = (_hook); \
    }

#define _signal_unhook(_sig, _hook) { \
        if ((_hook)->next != 0) (_hook)->next->prev = (_hook)->prev; \
        if ((_hook)->prev != 0) (_hook)->prev->next = (_hook)->next; \
        if ((_sig)->activate == (_hook)) (_sig)->activate = (_hook)->next; \
    }

#define process_sensitive(_sig, _edge) { \
        hook_t *_hook = alloca (sizeof(hook_t)); \
        _signal_hook (_sig, _edge, _hook); \
    }

#define process_wait1(_sig, _edge) { \
        hook_t _hook; \
        _signal_hook (_sig, _edge, &_hook); \
        process_wait(); \
        _signal_unhook (_sig, &_hook); \
    }

#define process_wait2(_sig1, _edge1, _sig2, _edge2) { \
        hook_t _hook1, _hook2; \
        _signal_hook (_sig1, _edge1, &_hook1); \
        _signal_hook (_sig2, _edge2, &_hook2); \
        process_wait(); \
        _signal_unhook (_sig1, &_hook1); \
        _signal_unhook (_sig2, &_hook2); \
    }

#define process_wait3(_sig1, _edge1, _sig2, _edge2, _sig3, _edge3) { \
        hook_t _hook1, _hook2, _hook3; \
        _signal_hook (_sig1, _edge1, &_hook1); \
        _signal_hook (_sig2, _edge2, &_hook2); \
        _signal_hook (_sig3, _edge3, &_hook3); \
        process_wait(); \
        _signal_unhook (_sig1, &_hook1); \
        _signal_unhook (_sig2, &_hook2); \
        _signal_unhook (_sig3, &_hook3); \
    }
