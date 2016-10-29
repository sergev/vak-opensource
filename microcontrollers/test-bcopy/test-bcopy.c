/*
 * Include processor definitions.
 */
#include "pic32mz.h"

#define MHZ     200             /* CPU clock in MHz. */

extern void bcopy_under_test(const void *src, void *dest, unsigned n);

/*
 * Main entry point at bd001000.
 * Setup stack pointer and $gp registers, and jump to main().
 */
asm ("          .section .startup");
asm ("          .globl _start");
asm ("          .type _start, function");
asm ("_start:   la      $sp, _estack");
asm ("          la      $ra, main");
asm ("          la      $gp, _gp");
asm ("          jr      $ra");
asm ("          .text");

/*
 * Image header pointer.
 */
asm ("          .section .exception");
asm ("          .org    0xf8");
asm ("_ebase:   .word   0x9d000000");           /* EBase value. */
asm ("_imgptr:  .word   -1");                   /* Image header pointer. */
asm ("          .text");

/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void putch(unsigned char c)
{
    /* Wait for transmitter shift register empty. */
    while (! (U4STA & PIC32_USTA_TRMT))
        continue;

again:
    /* Send byte. */
    U4TXREG = c;

    /* Wait for transmitter shift register empty. */
    while (! (U4STA & PIC32_USTA_TRMT))
        continue;

    if (c == '\n') {
        c = '\r';
        goto again;
    }
}

void putstr(char *p)
{
    while (*p)
        putch(*p++);
}

int hexchar (unsigned val)
{
    return "0123456789abcdef" [val & 0xf];
}

void printhex(unsigned char c)
{
    putch(hexchar(c >> 4));
    putch(hexchar(c));
}

void printuint(char *before, unsigned v, char *after)
{
    putstr(before);
    if (v == 0)
        putch('0');
    else {
        char buf[20], *p = &buf[20];

        *--p = 0;
        while (v != 0) {
            *--p = (v % 10) + '0';
            v /= 10;
        }
        putstr(p);
    }
    putstr(after);
}

#define MAX_NBYTES  36

char src_buf[MAX_NBYTES+4];
char dest_buf[MAX_NBYTES+4];
char valid_buf[MAX_NBYTES+4];

/*
 * Golden reference implementation of the tested function.
 */
void bcopy_trivial(const void *src, void *dest, int n)
{
    const char *from = src;
    char *to = dest;

    while (n-- > 0) {
        *to++ = *from++;
    }
}

/*
 * Fill a buffer with test data.
 */
void fill_buffer(char *buf, int nbytes)
{
    int i;

    for (i=0; i<nbytes; i++)
        buf[i] = 'A' + i;
}

/*
 * Clear buffer to zero.
 * The buffer is word-aligned.
 */
void clear_buffer(void *buf, int nbytes)
{
    int *ptr = buf;
    int nwords = nbytes / sizeof(int);

    while (nwords-- > 0) {
        *ptr++ = 0;
    }
}

/*
 * Check whether two buffers are equal.
 * Buffers are word-aligned.
 * Return 0 when mismatch detected.
 */
int buffers_equal(void *a, void *b, int nbytes)
{
    int *pa = a;
    int *pb = b;
    int nwords = nbytes / sizeof(int);

    while (nwords-- > 0) {
        if (*pa++ != *pb++)
            return 0;
    }
    return 1;
}

/*
 * Print mismatch between the dest and valid buffers.
 */
void print_mismatch(int nbytes)
{
    int i;

    putstr("src  = ");
    for (i=0; i<nbytes; i++) {
        if (i > 0)
            putstr("-");
        printhex(src_buf[i]);
    }
    putstr("\n");
    putstr("dest = ");
    for (i=0; i<nbytes; i++) {
        if (i > 0)
            putstr("-");
        printhex(dest_buf[i]);
    }
    putstr("\n");
    putstr("       ");
    for (i=0; i<nbytes; i++) {
        if (i > 0)
            putstr(" ");
        if (src_buf[i] == dest_buf[i])
            putstr("  ");
        else
            putstr("^^");
    }
    putstr("\n");
}

/*
 * Test bcopy function with given length,
 * and all combinations of byte offsets (from 0 to 3).
 */
void test(int nbytes)
{
    int src_offset, dest_offset;
    unsigned t0, t1, time_aligned = 0, time_unaligned = 0;

    for (src_offset=0; src_offset<4; src_offset++) {
        for (dest_offset=0; dest_offset<4; dest_offset++) {
            /* Prepare buffers. */
            fill_buffer(src_buf, MAX_NBYTES+4);
            clear_buffer(dest_buf, MAX_NBYTES+4);
            clear_buffer(valid_buf, MAX_NBYTES+4);

            /* Run golden reference function. */
            bcopy_trivial(src_buf + src_offset, valid_buf + dest_offset, nbytes);

            /* Run function under test. */
            t0 = mfc0(9, 0);
            bcopy_under_test(src_buf + src_offset, dest_buf + dest_offset, nbytes);
            t1 = mfc0(9, 0);

            /* Count timing. */
            if (src_offset == 0 && dest_offset == 0)
                time_aligned += t1 - t0;
            else
                time_unaligned += t1 - t0;

            /* Compare results. */
            if (! buffers_equal(dest_buf, valid_buf, MAX_NBYTES+4)) {
                /* Mismatch detected. */
                putstr("\nMismatch detected:\n");
                printuint("nbytes = ", nbytes, ", ");
                printuint("src offset = ", src_offset, ", ");
                printuint("dest offset = ", dest_offset, "\n");
                print_mismatch(nbytes);
            }
        }
    }
    printuint("(", nbytes, ") ");
    printuint("time = ", time_aligned, ", ");
    printuint("", time_unaligned, "\n");
}

int main()
{
    /* Initialize UART. */
    U4BRG = PIC32_BRG_BAUD (MHZ * 500000, 115200);
    U4STA = 0;
    U4MODE = PIC32_UMODE_PDSEL_8NPAR |      /* 8-bit data, no parity */
             PIC32_UMODE_ON;                /* UART Enable */
    U4STASET = PIC32_USTA_URXEN |           /* Receiver Enable */
               PIC32_USTA_UTXEN;            /* Transmit Enable */
#if 0
    /*
     * Test bcopy function with length from 0 up to maximum.
     */
    int nbytes;
    for (nbytes=0; nbytes<=MAX_NBYTES; nbytes++) {
        test(nbytes);
    }
#else
    static const char src[] = "/etc/master.passwd";
    //char *dst = malloc(1024);
    static char dst[1024];

    int slen = sizeof(src);
    const char *s;
    char *d;

    // Place an initial copy at the start of the scratchpad.
    bcopy_under_test(src, dst, slen);  // memcpy
    putstr("Copy from internal to dst: "); putstr(dst); putstr("\n");

    s = dst;
    d = dst + 64;
    bcopy_under_test(s, d, slen);      // 1
    putstr("Copy from offset=0 to offset=64: "); putstr(d); putstr("\n");

    s = dst + 64;
    d = dst;
    bcopy_under_test(s, d, slen);      // 2
    putstr("Copy from offset=64 to offset=0: "); putstr(d); putstr("\n");

    s = dst;
    d = dst + 65;
    bcopy_under_test(s, d, slen);      // 3
    putstr("Copy from offset=0 to offset=65: "); putstr(d); putstr("\n");

    s = src;
    d = dst + 64;
    bcopy_under_test(s, d, slen);      // memcpy
    putstr("Copy from internal to offset=64: "); putstr(dst); putstr("\n");

    s = dst + 64;
    d = dst + 1;
    bcopy_under_test(s, d, slen);      // 4
    putstr("Copy from offset=64 to offset=1: "); putstr(d); putstr("\n");
#endif
    while (1) {
        /* Stop simulation. */
        asm volatile ("sltiu $zero, $zero, 0xABC2");
    }
}
