#define Halt    0
#define Fputs   7
#define StdOut  1

        .text
        .global _init
_init   LDA     $255, text          // Put address of text message into register $255
        TRAP    0, Fputs, StdOut    // System call Fputs()
        SET     $255, 0             // Exit code
        TRAP    0, Halt, 0          // System call Halt()

        .section .rodata
text    BYTE    "Hi, GNU assembler!", #a, #0
