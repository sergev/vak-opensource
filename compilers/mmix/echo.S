#define Halt    0
#define Fputs   7
#define StdOut  1

#define argc    $0
#define argv    $1

        .text
        .global _init
_init   SUB     argc, argc, 2       // decrement argc by 2
        PBNN    argc, 2F            // if argc >= 0 - enter loop
        JMP     9F                  // otherwise skip the loop

1H      LDA     $255, Blank         // get address of blank string
        TRAP    0, Fputs, StdOut    // print space
        SUB     argc, argc, 1       // decrement argc
        ADD     argv, argv, 8       // increment argv
2H      LDOU    $255, argv, 8       // get address of argument
        TRAP    0, Fputs, StdOut    // print argument
        PBNZ    argc, 1B            // if arc != 0 - next iteration
9H      LDA     $255, NewLine       // get address of newline string
        TRAP    0, Fputs, StdOut    // print newline
        SET     $255, 0             // exit code
        TRAP    0, Halt, 0          // terminate

        .section .rodata
Blank   BYTE    ' ', 0              // string " "
NewLine BYTE    #a, 0               // string "\n"
