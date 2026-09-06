# RTOS based on coroutines

A coroutine-based real-time OS for Arduino-class microcontrollers.

Depends on: *"Proposal: native coroutines for C"*. Every `yield`, `await`,
`co_init`, `co_resume`, `co_value`, `co_cancel` and `CO_CANCEL` below has the meaning
given there.

Status: draft for discussion.

---

## 0. Thesis and design rules

**The entire system API is one yield protocol.** A task is a coroutine whose yield type
is `sys_req *`. Every blocking operation in the system — sleeping, receiving a message,
waiting for a byte on the UART, waiting for an ADC conversion — is the same act: fill in
a request struct and yield its address. The kernel is the code that resumes tasks and
services the requests it receives. There is no second mechanism.

Five rules follow from the target.

1. **No per-task stacks.** This is the whole reason for the design. FreeRTOS on an
   ATmega328P needs on the order of 100–200 bytes of stack per task; with 2 KB of SRAM
   total you get four or five tasks before you are out of memory, and you must guess each
   stack size in advance and hope. A stackless coroutine frame holds only the locals that
   survive a suspension, so a typical task frame is tens of bytes and its size is computed
   by the compiler rather than guessed by you.
2. **No heap, no dynamic task creation.** All tasks and all kernel state are declared at
   compile time. Total RAM use is known at link time. If it does not fit, the link fails
   rather than the device.
3. **The kernel allocates nothing, ever — including internally.** All per-wait state
   lives in the requesting task's own frame (§1.2). Kernel memory is a fixed array of
   small task control blocks and nothing else.
4. **Blocking operations are yields; non-blocking operations are functions.** Writing a
   GPIO pin is a register store that cannot block, so it stays an ordinary function.
   Making it a coroutine would cost bytes and cycles and buy nothing. The rule is
   mechanical and it is what keeps the API from drowning in ceremony.
5. **Cooperative, single-core, no preemption of task code.** Stackless coroutines cannot
   be preempted mid-function — there is no stack to switch away from. Section 12 is
   honest about what this costs.

Prior art worth reading before arguing with any of this: Adam Dunkels' protothreads
(stackless coroutines on AVR via Duff's device, whose famous defect — locals do not
survive a yield — is exactly what compiler-managed frames fix), Contiki, TinyOS's
split-phase model, and Embassy, which is essentially this architecture in Rust and is in
production today.

---

## 1. The syscall protocol

### 1.1 The request

```c
typedef struct sys_req sys_req;

struct sys_req {
    sys_req *next;          // intrusive queue link, owned by the kernel while blocked
    u8       op;
    u8       flags;
    i16      out;           // result, written by the kernel before resuming
    union {
        struct { u32 deadline; }                     sleep;
        struct { os_chan *ch; void *msg; }           chan;
        struct { os_events mask; }                   wait;
        struct { u8 dev; u8 *buf; u16 len; }         io;
        struct { os_task *target; }                  join;
    } u;
};
```

On AVR with 16-bit pointers this is 10–12 bytes.

### 1.2 Why the request lives in the task's frame

A blocked task yields the address of a `sys_req` that is a local in its own coroutine
frame. That frame is address-stable for as long as the task is suspended (Coroutines
§2), which is precisely the window during which the kernel needs the request. So:

- The kernel's wait queues are intrusive lists threaded through `sys_req.next`, living
  inside task frames. A channel's waiter queue costs one pointer in the channel.
- The kernel never allocates a wait record, a timer node, or a queue entry.
- Kernel RAM is `sizeof(os_tcb) * NTASKS` plus a few words. Nothing else scales.

This is the structural reason the design fits in 2 KB, and it is worth defending against
convenience changes.

### 1.3 Making a syscall

The primitive form is explicit and is what everything else expands to:

```c
sys_req r = { .op = SYS_SLEEP, .u.sleep.deadline = os_now() + 100 };
if (yield &r == CO_CANCEL) return;
```

The sugar is macros in `<braam/rt.h>`. Statement form, for syscalls with no useful
result:

```c
#define os_sleep_ms(ms)                                              \
    do { sys_req r__ = { .op = SYS_SLEEP,                            \
                         .u.sleep.deadline = os_now() + (ms) };      \
         if (yield &r__ == CO_CANCEL) return;                        \
    } while (0)
```

Note that these macros contain a `return`. That is deliberate: the default response to
cancellation is to unwind the task, and making it invisible would be worse than making it
a documented property of every `os_*` call. `os_try_*` variants (§8.3) surface
cancellation as a value instead, for tasks that must clean up.

> **Open question.** Value-returning syscalls (`os_recv`, `os_read`) want expression-form
> macros, which needs block expressions in C. Without them the primitive form —
> yield, then read `r.out` — is the fallback and is perfectly usable, just wordier.

---

## 2. Tasks

```c
typedef coro(sys_req *) void os_body;
```

A task is a coroutine yielding `sys_req *` and returning nothing. It is declared and
given storage statically:

```c
coro(sys_req *) void blink(u8 pin, u16 period) {
    gpio_mode(pin, GPIO_OUT);
    for (;;) {
        gpio_toggle(pin);
        os_sleep_ms(period);
    }
}

OS_TASK(blink_led, blink, /*prio*/ 2, /*args*/ 13, 500);
```

`OS_TASK(name, fn, prio, args...)` declares:

```c
static coro_frame(fn) name##__frame;
static os_tcb        name = { .frame = &name##__frame, .resume = &fn__resume,
                              .prio = prio, .state = OS_INIT };
```

and registers an initializer that calls `co_init(&name##__frame, fn, args...)`.

The task control block:

```c
typedef struct {
    void      *frame;
    os_resume  resume;
    sys_req   *pending;     // points into frame; NULL when ready
    u8         state;       // OS_INIT / OS_READY / OS_BLOCKED / OS_DONE
    u8         prio;
} os_tcb;                   // 8 bytes on AVR
```

The task set is fixed:

```c
OS_TASKS(&blink_led, &uart_echo, &sensor_poll);
```

This expands to a `const` array in flash plus a count. There is no `os_spawn`. A task
that must come and go is written as a task that waits for a start signal.

---

## 3. The kernel

### 3.1 Scheduler

Fixed-priority, non-preemptive between suspend points. The scheduling point is the
return from `co_resume`.

```c
void os_run(void) {
    os_init_all();
    for (;;) {
        os_tcb *t = highest_priority_ready();
        if (!t) { os_idle(); continue; }
        t->state = OS_RUNNING;
        if (t->resume(t->frame, CO_CONTINUE) == CO_DONE) {
            t->state = OS_DONE;
            os_signal_joiners(t);
            continue;
        }
        t->pending = co_value_of(t);       // the yielded sys_req *
        os_service(t);                     // §3.2
    }
}
```

`os_service` inspects `pending->op`. Requests that can be satisfied immediately (a sleep
whose deadline has passed, a `os_recv` on a non-empty channel) mark the task `OS_READY`
again. Requests that cannot link `pending` into the appropriate wait queue and mark the
task `OS_BLOCKED`.

Priority is advisory in the sense that matters most: a high-priority task cannot run
until the current task yields. Latency is bounded by the longest run-to-yield interval in
the system, not by the scheduler.

### 3.2 Idle and power

When no task is ready, the kernel computes the nearest deadline in the timer queue,
programs the hardware timer for that instant, and sleeps the MCU. Tickless by
construction — there is no periodic tick interrupt, so a system whose tasks are all
sleeping for 500 ms wakes twice a second and not 1000 times.

`os_idle()` is a weak symbol; the default implementation on AVR sets the sleep mode
appropriate to the pending timer and executes `sleep_cpu()`.

### 3.3 The watchdog

Cooperative scheduling means one runaway task hangs the system, and this design cannot
preempt it. The mitigation is explicit rather than hoped-for. A hardware timer ISR
records the time of the last scheduler dispatch; if a task has run longer than
`OS_MAX_RUN_US` without yielding, the ISR fires `os_panic(OS_TRAP_RUNAWAY, tcb)`, which
by default resets the device and records the offending task ID in a no-init RAM location
for the next boot to report.

Set `OS_MAX_RUN_US` to your real-time deadline, not to a comfortable number.

---

## 4. Interrupts

**ISRs are not coroutines.** They are ordinary functions, they cannot yield, and they
must not call any `os_*` macro from §1.3. Attempting to do so is a compile-time error:
the coroutine spec already rejects `yield` outside a coroutine body, which gives this
rule for free.

An ISR communicates with tasks through exactly three operations, all of which are
constant-time, allocation-free, and safe to call with interrupts disabled:

```c
void os_isr_post(os_events *ev, os_events bits);   // set event bits, wake waiters
bool os_isr_send(os_chan *ch, const void *msg);    // non-blocking send; false if full
void os_isr_wake(os_task *t);                      // mark a specific task ready
```

Each is a short critical section. On AVR, atomicity is `cli`/`sei` with the prior `SREG`
saved; on Cortex-M it is `BASEPRI` or a load-linked loop. The kernel provides
`OS_ATOMIC { ... }` for this and it is the only place in the system where interrupt
masking appears.

The rule to hold onto: **an ISR makes a task ready; it never performs work on the task's
behalf.** Device drivers are tasks that wait on events posted by tiny ISRs.

---

## 5. Time

```c
u32  os_now(void);                    // monotonic milliseconds since boot, wraps at 2^32
void os_sleep_ms(u16 ms);             // yields SYS_SLEEP
void os_sleep_until(u32 deadline);    // absolute; for drift-free periodic work
```

`os_sleep_until` exists because `os_sleep_ms` in a loop accumulates the task's own
execution time as drift. Periodic tasks should be written:

```c
u32 next = os_now();
for (;;) {
    do_work();
    next += PERIOD_MS;
    os_sleep_until(next);
}
```

The timer queue is a delta list of `sys_req`s sorted by deadline, threaded through
`sys_req.next`. Insertion is O(n) in the number of sleeping tasks, which on a device with
eight tasks is not worth improving.

Deadline comparison uses wrapping arithmetic: `(i32)(a - b) < 0`. A task may not sleep
longer than 2^31 ms (~24 days) in one call; longer sleeps are a loop, and the request is
rejected with `OS_ETOOLONG` rather than silently misbehaving.

---

## 6. Synchronization

### 6.1 What you do not need

Because task code runs to its next suspend point without interruption from other tasks,
**any critical section that contains no `yield` needs no lock.** Read-modify-write of a
shared variable, updating a struct, walking a list — all are atomic with respect to other
tasks for free. Only ISRs can interleave, and only `OS_ATOMIC` guards against those.

This deletes most of the mutex traffic of a conventional RTOS. It is the largest
ergonomic win of cooperative scheduling and it should be taught first.

### 6.2 Channels

The primary inter-task primitive. Statically sized ring buffer, fixed element size.

```c
OS_CHAN(readings, i16, 4);         // declares storage for 4 i16 elements

os_send(&readings, &value);        // blocks while full
os_recv(&readings, &out);          // blocks while empty
bool os_try_send(&readings, &v);   // never blocks
bool os_try_recv(&readings, &out); // never blocks
```

Waiters queue intrusively through their own `sys_req`s (§1.2), so a channel costs its
buffer plus two pointers plus two indices. A zero-capacity channel is legal and gives
rendezvous semantics.

### 6.3 Events

A bitmask per event group; the natural target for ISR notification.

```c
OS_EVENTS(uart_ev);
#define UART_RX_READY  (1u << 0)
#define UART_TX_EMPTY  (1u << 1)

os_events got = os_wait_any(&uart_ev, UART_RX_READY | UART_TX_EMPTY);  // blocks
os_events got = os_wait_all(&uart_ev, UART_RX_READY);
```

Bits are cleared on consumption. From an ISR: `os_isr_post(&uart_ev, UART_RX_READY)`.

### 6.4 Mutexes

Provided, but needed only to hold a resource **across** a suspend point — the one case
§6.1 does not cover.

```c
OS_MUTEX(i2c_bus);
os_lock(&i2c_bus);
... transaction with yields inside ...
os_unlock(&i2c_bus);
```

Priority inheritance is **not** implemented in 0.1. With non-preemptive scheduling the
classical priority inversion scenario requires a specific shape (low-priority holder
blocks inside the critical section while a medium task runs) which is real but rare, and
inheritance costs kernel complexity and RAM. Documented limitation, not an oversight.
See §13.

---

## 7. Device I/O

Per rule 4, the split is mechanical.

**Ordinary functions** (cannot block, compile to a few instructions):

```c
void gpio_mode(u8 pin, u8 mode);
void gpio_write(u8 pin, bool v);
bool gpio_read(u8 pin);
void gpio_toggle(u8 pin);
```

**Syscalls** (can block):

```c
i16  os_uart_read(u8 port, u8 *buf, u16 len);     // returns bytes read, or negative error
i16  os_uart_write(u8 port, const u8 *buf, u16 len);
i16  os_adc_read(u8 channel);                     // conversion takes ~100 µs; yield it
i16  os_i2c_xfer(u8 addr, const u8 *tx, u16 ntx, u8 *rx, u16 nrx);
void os_wait_pin(u8 pin, u8 edge);                // pin-change interrupt behind an event
```

Every driver has the same shape: a small ISR that posts an event or fills a buffer, and a
syscall handler in the kernel that parks the requesting task's `sys_req` on the device's
waiter list. Adding a device means writing those two pieces; it does not mean touching
the scheduler.

Errors are negative values in `sys_req.out`, from a single flat enum
(`OS_ETIMEDOUT`, `OS_EAGAIN`, `OS_ECANCELLED`, `OS_EDEV`, `OS_ETOOLONG`). No errno.

---

## 8. Cancellation, timeouts, lifetime

### 8.1 Cancellation

Built directly on `CO_CANCEL`. `os_cancel(&task)` causes the task's next resume to
deliver `CO_CANCEL` at its current suspend point, which the `os_*` macros turn into an
immediate `return` from the task body.

If the extended C adopts `defer`, cleanup on that path is automatic and this section is finished.
If it does not, tasks holding resources must use the `os_try_*` forms and clean up by
hand. This remains the largest open dependency inherited from the coroutine spec.

### 8.2 Timeouts

A timeout is not a per-syscall parameter — that would double the API. It is a wrapper:

```c
i16 r = os_deadline(50, os_recv(&readings, &v));
if (r == OS_ETIMEDOUT) { ... }
```

`os_deadline` sets a `SYS_DEADLINE` bit and a deadline field on the request before it is
yielded, so the kernel parks the request on both the device queue and the timer queue and
resolves whichever fires first. One mechanism, no per-call plumbing.

### 8.3 Explicit cancellation handling

```c
if (os_try_sleep_ms(100) == OS_ECANCELLED) { release_things(); return; }
```

Every `os_x` has an `os_try_x` that returns rather than unwinds.

### 8.4 Joining

```c
os_join(&sensor_task);        // blocks until that task's body returns
bool os_is_done(&sensor_task);
```

A task that has returned stays `OS_DONE`; its frame is not reused. `os_restart(&t)` calls
`co_init` again with the original arguments and marks it ready. This is how you get
task lifecycle without dynamic creation.

---

## 9. Memory budget

Design targets for an ATmega328P (2 KB SRAM). These are budgets to hold the design to,
not measurements of an implementation that does not exist yet.

| Item | Target |
|---|---|
| `os_tcb` | 8 bytes |
| Kernel globals (queues, clock, flags) | ~20 bytes |
| Typical task frame (blink) | 10–16 bytes |
| Typical task frame (UART protocol, a few locals + a `sys_req`) | 24–40 bytes |
| `OS_CHAN(x, i16, 4)` | 8 + 6 bytes |
| `OS_EVENTS` group | 4 bytes |

Eight tasks, three channels and two event groups should land near 400 bytes, leaving the
application most of the device. Compare: the same eight tasks under a stack-per-task RTOS
do not fit at all.

`os_report_sizes()` is a build-time tool, not a runtime one: it emits every
`co_sizeof(fn)` and the kernel table sizes so the budget appears in the build log and can
be regression-tested.

---

## 10. Determinism guarantees

Three properties this architecture gets that a stack-per-task RTOS does not:

1. **The whole-program stack bound is computable.** No task holds machine stack across a
   suspension, so the only stack in the system is the kernel's plus the deepest ISR
   nesting. That is a static call-graph analysis with no per-task guessing and no stack
   canaries. On a 2 KB device this is close to the whole argument.
2. **Total RAM is a link-time constant.** No heap, no dynamic tasks, no kernel
   allocation (§1.2). Overflow is a link error.
3. **All misuse traps.** Inherited from the coroutine spec (resume-after-done,
   reentrant resume, reading a value that is not there) plus kernel traps:
   `OS_TRAP_RUNAWAY` (§3.3), `OS_TRAP_ISR_YIELD` (compile-time), `OS_TRAP_QUEUE_CORRUPT`,
   `OS_TRAP_NO_TASKS` (every task done or blocked forever — a defined deadlock report,
   not a silent hang).

That last one deserves emphasis: when no task is ready and the timer queue is empty and
no interrupt source is enabled, the system is deadlocked, the kernel can *prove* it, and
it says so instead of sleeping forever.

---

## 11. Worked example

```c
#include <braam/rt.h>

OS_CHAN(samples, i16, 4);
OS_EVENTS(button_ev);
#define BTN_PRESSED (1u << 0)

/* --- ISR: makes a task ready, does no work ------------------------------ */
ISR(PCINT0_vect) {
    os_isr_post(&button_ev, BTN_PRESSED);
}

/* --- periodic sensor, drift-free ---------------------------------------- */
coro(sys_req *) void sensor(u8 ch, u16 period) {
    u32 next = os_now();
    for (;;) {
        i16 v = os_adc_read(ch);              // yields; ADC ISR wakes us
        os_try_send(&samples, &v);            // drop on overrun rather than block
        next += period;
        os_sleep_until(next);
    }
}

/* --- consumer with a timeout -------------------------------------------- */
coro(sys_req *) void reporter(void) {
    for (;;) {
        i16 v;
        i16 r = os_deadline(2000, os_recv(&samples, &v));
        if (r == OS_ETIMEDOUT) { os_uart_write(0, (const u8 *)"stall\n", 6); continue; }
        u8 line[8];
        os_uart_write(0, line, fmt_i16(line, v));
    }
}

/* --- event-driven task --------------------------------------------------- */
coro(sys_req *) void on_button(void) {
    for (;;) {
        os_wait_any(&button_ev, BTN_PRESSED);
        gpio_toggle(13);
        os_sleep_ms(50);                      // debounce, costs nothing while asleep
    }
}

OS_TASK(t_sensor,   sensor,    1, 0, 100);
OS_TASK(t_reporter, reporter,  1);
OS_TASK(t_button,   on_button, 3);
OS_TASKS(&t_sensor, &t_reporter, &t_button);

int main(void) { os_run(); }
```

Three concurrent activities, no stacks, no heap, no tick interrupt, and the device sleeps
between events. The `os_run` loop is under a hundred instructions.

---

## 12. What this cannot do

Stated plainly, because these are consequences of the architecture and no amount of API
design removes them.

- **No preemption of task code.** A task that computes for 10 ms delays every other task
  by up to 10 ms. Hard real-time deadlines shorter than your longest run-to-yield
  interval must be served from an ISR, not a task. §3.3 detects violations; it cannot
  prevent them.
- **No blocking inside a non-coroutine call.** A helper function called by a task cannot
  yield. Anything that might block must itself be a coroutine and be reached by `await`,
  which means the async/sync split propagates up call chains exactly as it does in Rust
  and C#. Deep library code written in plain C cannot block. This is the real cost of
  stacklessness and it should be weighed honestly against the RAM it saves.
- **No priority inheritance** (§6.4).
- **No memory protection.** There is no MPU on an ATmega and no protection domains here.
  A wild pointer corrupts anything. Extended C's bounds-checked slices are the mitigation, at
  the language level, not the OS level.
- **Single core.** Extending to the RP2040's second core means either pinning tasks to
  cores with per-core kernels and a channel between them, or a real SMP scheduler with
  locks. The former is in scope for a future version; the latter is not.

---

## 13. Open questions

1. **`defer`.** Same dependency as the coroutine spec (§8.1). Everything about
   cancellation cleanliness turns on it.
2. **Expression-form syscall macros** need block expressions in extended C.
3. **Priority inheritance** — is the non-preemptive inversion window narrow enough to
   ignore permanently, or does 0.2 need it?
4. **Should `sys_req` be shared rather than per-frame?** A single kernel-owned request
   buffer would shrink frames by ~10 bytes each, but only works for requests resolved
   before the next resume, which excludes every blocking case. Probably no, but the RAM
   at stake on a 328P is enough to justify measuring.
5. **Static analysis of run-to-yield time.** `OS_MAX_RUN_US` is currently enforced at
   runtime. A worst-case-execution-time pass over the call graph between suspend points
   would turn §3.3 from a crash report into a compile-time guarantee, which is the single
   highest-value tool this system could have.
6. **Driver model.** §7 describes the shape by example. It should become a documented
   interface so that third-party drivers are possible without kernel patches.
