#define Halt    #1
#define Write   #4
#define StdOut  #1

        .text
        .align  2                   // Make sure everything is aligned properly
        .global _start
_start:
        mov     X0, StdOut          // Print to StdOut
        adr     X1, text            // Put address of text message into register X1
        mov     X2, #23             // Length of our string
        mov     X16, Write          // Unix write() system call
        svc     #0x80               // Call kernel to output the string

        mov     X0, #0              // Exit code
        mov     X16, Halt           // Unix _exit() system call
        svc     #0x80               // Call kernel to terminate the program

text:   .ascii  "Hi, aarch64 assembler!\n"
