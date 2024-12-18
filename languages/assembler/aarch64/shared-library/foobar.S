#include <sys/syscall.h>

        .text
        .globl  _start
_start:
	adrp	x0, foo
	add	x0, x0, :lo12:foo
	bl	say                     // say("foo")

	adrp	x0, bar
	add	x0, x0, :lo12:bar       // say("bar")
	bl	say

        mov     x8, #SYS_exit           // syscall exit(status)
        mov     x0, #0                  // status - 0
        svc     #0                      // do system call

foo:    .asciz  "foo"
bar:    .asciz  "bar"
