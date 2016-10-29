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
#include <stdio.h>
#include <string.h>
#include "rtlsim.h"

/*
 * Set a value of the signal.
 * Value will be updated on next simulation cycle.
 * If the value changed, put the signal to active list.
 */
void signal_set (signal_t *sig, value_t value)
{
    sig->new_value = value;

    if (value != sig->value && sig->next == 0) {
        /* Value changed - put to list of active signals. */
        sig->next = signal_active;
        signal_active = sig;
        //printf ("(%llu) Signal '%s' changed %s\n", time_ticks, sig->name, sig->new_value ? "HIGH" : "LOW");
    }
}

/*
 * Delay the current process by a given number of clock ticks.
 */
void process_delay (unsigned ticks)
{
    /* On first call, initialize the main process. */
    if (process_current == 0) {
        process_main.name = "main";
        process_current = &process_main;
    }

    /* Put the current process to queue of pending events.
     * Keep the queue sorted. */
    process_t **q = &process_queue;
    process_t *p;
    while ((p = *q) && p->delay <= ticks) {
        if (p->delay > 0)
            ticks -= p->delay;
        q = &p->next;
    }
    process_current->delay = ticks;
    process_current->next = p;
    if (p)
            p->delay -= ticks;
    *q = process_current;

    /* Switch to next active process. */
    process_wait();
}

/*
 * Wait for activation of any signal from a sensitivity list.
 * Switch to next active process.
 */
void process_wait (void)
{
    process_t *old = process_current;

    if (process_queue == 0) {
        /* Cannot happen. */
        printf ("Internal error: empty process queue\n");
        exit (-1);
    }
    if (process_queue->delay != 0) {
        /* Delta cycle finished.
         * Schedule processes for active signals. */
        while (signal_active != 0) {
            hook_t *hook = signal_active->activate;
            signal_t *next = signal_active->next;

            /* Handle all processes, sensitive to this signal. */
            for (; hook != 0; hook = hook->next) {
                if (hook->process->next == 0) {
                    /* Signal change should matches the edge flag. */
                    if ((hook->edge & POSEDGE) &&
                        (signal_active->value != 0 ||
                         signal_active->new_value == 0))
                        continue;
                    if ((hook->edge & NEGEDGE) &&
                        (signal_active->value == 0 ||
                         signal_active->new_value != 0))
                        continue;

                    /* Put the process to queue of pending events. */
                    hook->process->next = process_queue;
                    process_queue = hook->process;
                    //printf ("(%llu) Process '%s' activated\n", time_ticks, hook->process->name);
                }
            }
            /* Setup a new signal value. */
            signal_active->value = signal_active->new_value;
            signal_active->next = 0;
            signal_active = next;
        }
    	//printf ("(%llu) ---\n", time_ticks);
    }
    /* Select next process from the queue. */
    process_current = process_queue;
    process_queue = process_queue->next;
    process_current->next = 0;
    if (process_current->delay != 0) {
        /* Advance time. */
        time_ticks += process_current->delay;
    }
    if (process_current == old)
        return;

    /* Switch to new process. */
    //printf ("(%llu) Switch process '%s' -> '%s'\n", time_ticks, old->name, process_current->name);
#if defined (__i386__)
    asm (
        "mov %%esp, 0(%1) \n"       /* Save ESP to context[0] */
        "call 1f \n"
     "1: pop %%ebx \n"              /* Compute address of label 1 */
        "lea 2f-1b(%%ebx), %%ebx \n"
        "mov %%ebx, 4(%1) \n"       /* Save address of label 2 to context[1] */
        "mov 0(%0), %%esp \n"       /* Restore ESP from context[0] */
        "mov 4(%0), %%ecx \n"       /* Get address from context[1] */
        "jmp *%%ecx \n "            /* Jump to address */
     "2: "
        : : "r" (process_current->context), "r" (old->context)
        : "bx", "si", "di", "bp");

#elif defined __x86_64__
    asm (
        "mov %%rsp, 0(%1) \n"       /* Save RSP to context[0] */
        "call 1f \n"
     "1: pop %%rbx \n"              /* Compute address of label 1 */
        "lea 2f-1b(%%rbx), %%rbx \n"
        "mov %%rbx, 8(%1) \n"       /* Save address of label 2 to context[1] */
        "mov %%r12, 16(%1) \n"      /* Save R12 to context[2] */
        "mov %%r13, 24(%1) \n"      /* Save R13 to context[3] */
        "mov %%r14, 32(%1) \n"      /* Save R14 to context[4] */
        "mov %%r15, 40(%1) \n"      /* Save R15 to context[5] */
        "mov 0(%0), %%rsp \n"       /* Restore RSP from context[0] */
        "mov 8(%0), %%rcx \n"       /* Get address from context[1] */
        "mov 16(%0), %%r12 \n"      /* Restore R12 from context[2] */
        "mov 24(%0), %%r13 \n"      /* Restore R13 from context[3] */
        "mov 32(%0), %%r14 \n"      /* Restore R14 from context[4] */
        "mov 40(%0), %%r15 \n"      /* Restore R15 from context[5] */
        "jmp *%%rcx \n "            /* Jump to address */
     "2: "
        : : "r" (process_current->context), "r" (old->context)
        : "bx", "bp");
#else
    if (! _setjmp (old->context))
        longjmp(process_current->context, 1);
#endif
}

/*
 * Setup a context of the process, to call a given function
 * on a given stack.
 */
process_t *_process_setup (process_t *proc, const char *name,
    void (*func)(), unsigned nbytes)
{
    memset (proc, 0, sizeof(process_t));
    proc->name = name;

#if defined(__i386__) || defined(__x86_64__)
    /* For i386 and  x86_64 architectures, we need to set SP and IP.
     * Stack must be aligned at 16-byte boundary.
     * We use jmp_buf in non-standard way. */
    size_t *context = (size_t*) proc->context;
    context[0] = (((size_t) proc + nbytes) & ~15) - sizeof(size_t);
    context[1] = (size_t) func;

#else
#error Coroutine setup not implemented for this architecture.
    /* Need to set at least a stack pointer
     * and a program counter. */
#endif

    proc->next = process_queue;
    return proc;
}
