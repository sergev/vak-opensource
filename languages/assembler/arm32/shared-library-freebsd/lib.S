#include <sys/syscall.h>

        .text
        .globl  say
say:
        stmfd   sp!, {r7, lr}           // save registers

        mov     r1, r0                  // message
	mov     r0, #1                  // file descriptor 1 - stdout
	mov     r2, #4                  // character count
	mov     r7, #SYS_write          // syscall 4 - write
	svc     #0                      // do system call

        ldmfd   sp!, {r7, pc}           // return
