/*
 * Disassembler for PIC16 processors.
 *
 * Copyright (C) 2018 Serge Vakulenko
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
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#define TXTSIZE         0x8000  /* command segment size */
#define DATSIZE         32      /* data segment size */

/*
 * Processor configuration.
 */
#define CFGADDR         0x8007  /* CONFIG1 */

int debug;
char *infile;

unsigned char data[DATSIZE];
unsigned short text[TXTSIZE];
unsigned char tbusy[TXTSIZE];

union {
    uint32_t word32;

    struct {
        //
        // CONFIG1 @ 8007
        //
        unsigned fosc : 3;  // Oscillator Selection Bits
#define FOSC_ECH        7   // External Clock, High Power Mode (4-20 MHz): device clock supplied to CLKIN pins
#define FOSC_ECM        6   // External Clock, Medium Power Mode (0.5-4 MHz): device clock supplied to CLKIN pins
#define FOSC_ECL        5   // External Clock, Low Power Mode (0-0.5 MHz): device clock supplied to CLKIN pins
#define FOSC_INTOSC     4   // INTOSC oscillator: I/O function on CLKIN pin
#define FOSC_EXTRC      3   // EXTRC oscillator: External RC circuit connected to CLKIN pin
#define FOSC_HS         2   // HS Oscillator, High-speed crystal/resonator connected between OSC1 and OSC2 pins
#define FOSC_XT         1   // XT Oscillator, Crystal/resonator connected between OSC1 and OSC2 pins
#define FOSC_LP         0   // LP Oscillator, Low-power crystal connected between OSC1 and OSC2 pins

        unsigned wdte : 2;  // Watchdog Timer Enable
#define WDTE_ON         3   // Watchdog enabled
#define WDTE_NSLEEP     2   // Watchdog enabled while running and disabled in Sleep
#define WDTE_SWDTEN     1   // Watchdog controlled by the SWDTEN bit in the WDTCON register
#define WDTE_OFF        0   // Watchdog disabled

        unsigned pwrte : 1; // Power-up Timer Enable
#define PWRTE_OFF       1   // Power-up Timer disabled
#define PWRTE_ON        0   // Power-up Timer enabled

        unsigned mclre : 1; // MCLR Pin Function Select
#define MCLRE_ON        1   // MCLR/VPP pin function is MCLR
#define MCLRE_OFF       0   // MCLR/VPP pin function is digital input

        unsigned cp : 1;    // Flash Program Memory Code Protection
#define CP_OFF          1   // Program memory code protection is disabled
#define CP_ON           0   // Program memory code protection is enabled

        unsigned _u1 : 1;

        unsigned boren : 2; // Brown-out Reset Enable
#define BOREN_ON        3   // Brown-out Reset enabled
#define BOREN_NSLEEP    2   // Brown-out Reset enabled while running and disabled in Sleep
#define BOREN_SBODEN    1   // Brown-out Reset controlled by the SBOREN bit in the BORCON register
#define BOREN_OFF       0   // Brown-out Reset disabled

        unsigned clkouten : 1; // Clock Out Enable
#define CLKOUTEN_OFF    1   // CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin
#define CLKOUTEN_ON     0   // CLKOUT function is enabled on the CLKOUT pin

        unsigned ieso : 1;  // Internal/External Switchover Mode
#define IESO_ON         1   // Internal/External Switchover Mode is enabled
#define IESO_OFF        0   // Internal/External Switchover Mode is disabled

        unsigned fcmen : 1; // Fail-Safe Clock Monitor Enable
#define FCMEN_ON        1   // Fail-Safe Clock Monitor is enabled
#define FCMEN_OFF       0   // Fail-Safe Clock Monitor is disabled

        unsigned _u2      : 2;

        //
        // CONFIG2 @ 8008
        //
        unsigned wrt : 2;   // Flash Memory Self-Write Protection
#define WRT_OFF         3   // Write protection off
#define WRT_BOOT        2   // 000h to 1FFh write protected, 200h to 1FFFh may be modified by EECON control
#define WRT_HALF        1   // 000h to FFFh write protected, 1000h to 1FFFh may be modified by EECON control
#define WRT_ALL         0   // 000h to 1FFFh write protected, no addresses may be modified by EECON control

        unsigned pps1way : 1; // Peripheral Pin Select one-way control
#define PPS1WAY_ON      1   // The PPSLOCK bit cannot be cleared once it is set by software
#define PPS1WAY_OFF     0   // The PPSLOCK bit can be set and cleared repeatedly by software

        unsigned _u3 : 4;

        unsigned zcddis : 1; // Zero-cross detect disable
#define ZCDDIS_ON       1   // Zero-cross detect circuit is disabled at POR
#define ZCDDIS_OFF      0   // Zero-cross detect circuit is enabled at POR

        unsigned pllen : 1; // Phase Lock Loop enable
#define PLLEN_ON        1   // 4x PLL is always enabled
#define PLLEN_OFF       0   // 4x PLL is enabled when software sets the SPLLEN bit

        unsigned stvren : 1; // Stack Overflow/Underflow Reset Enable
#define STVREN_ON       1   // Stack Overflow or Underflow will cause a Reset
#define STVREN_OFF      0   // Stack Overflow or Underflow will not cause a Reset

        unsigned borv : 1;  // Brown-out Reset Voltage Selection
#define BORV_LO         1   // Brown-out Reset Voltage (Vbor), low trip point selected.
#define BORV_HI         0   // Brown-out Reset Voltage (Vbor), high trip point selected.

        unsigned lpbor : 1; // Low-Power Brown Out Reset
#define LPBOR_OFF       1   // Low-Power BOR is disabled
#define LPBOR_ON        0   // Low-Power BOR is enabled

        unsigned debug : 1; // In-Circuit Debugger Mode bit
#define DEBUG_OFF       1   // In-Circuit Debugger disabled, ICSPCLK and ICSPDAT are general purpose I/O pins
#define DEBUG_ON        0   // In-Circuit Debugger enabled, ICSPCLK and ICSPDAT are dedicated to the debugger

        unsigned lvp : 1;   // Low-Voltage Programming Enable
#define LVP_ON          1   // Low-voltage programming enabled
#define LVP_OFF         0   // High-voltage on MCLR/VPP must be used for programming

        unsigned _u4      : 2;
    } field;
} config;

void uerror(char *s, ...)
{
    va_list ap;

    va_start(ap, s);
    if (infile)
        fprintf(stderr, "%s: ", infile);
    vfprintf(stderr, s, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    exit(1);
}

int hexdig(char *p)
{
    unsigned char val = 0;

    if      (*p >= '0' && *p <= '9') val = (*p - '0') << 4;
    else if (*p >= 'A' && *p <= 'F') val = (*p - 'A' + 10) << 4;
    else uerror("bad hex digit");
    ++p;
    if      (*p >= '0' && *p <= '9') val += *p - '0';
    else if (*p >= 'A' && *p <= 'F') val += *p - 'A' + 10;
    else uerror("bad hex digit");
    return val;
}

int gethex(int *len, long *addr, unsigned char *line)
{
    char buf[80];
    unsigned char sum;
    int i, eof;
    static int high;
again:
    if (! fgets(buf, sizeof(buf), stdin))
        uerror("unexpected EOF");
    if (buf[0] != ':')
        goto again;
    *len = hexdig(buf+1);
    if (strlen(buf) < *len * 2 + 11)
        uerror("too short hex line");
    *addr = (long) high << 16 | hexdig(buf+3) << 8 | hexdig(buf+5);
    eof = hexdig(buf+7);

    sum = 0;
    for (i=0; i<*len; ++i) {
        line[i] = hexdig(buf+9 + i + i);
        sum += line[i];
    }

    sum += eof + *len + (*addr & 0xff) + (*addr >> 8 & 0xff);
    if (sum != (unsigned char) - hexdig(buf+9 + *len + *len))
        uerror("bad hex checksum");

    if (eof == 4) {
        if (*len != 2)
            uerror("invalid hex linear address record length");
        high = line[0] << 8 | line[1];
        goto again;
    }
    if (eof == 1)
        return 0;
    if (eof)
        uerror("unknown hex record");
    return 1;
}

void readimage()
{
    int len, i;
    long addr;
    unsigned char line[16];

    while (gethex(&len, &addr, line)) {
if (debug)
printf("%d %04lx %02x-%02x-%02x-%02x\n", len, addr, line[0], line[1], line[2], line[3]);
        if (addr == CFGADDR*2) {
            if (len != 4)
                uerror("invalid config length");
            config.word32 = line[0] | line[1] << 8 | line[2] << 16 | line[3] << 24;
            continue;
        }
        if (addr & 1)
            uerror("odd address");
        if (len & 1)
            uerror("odd length");
        addr /= 2;
        if (addr > TXTSIZE)
            uerror("invalid hex address");
        for (i=0; i<len; i+=2) {
            text[addr] = line[i] | line[i+1] << 8;
            tbusy[addr] = 1;
            ++addr;
        }
    }
}

void print(int addr, int cmd)
{
    char *m;
    int arg;

    printf("%04x: %02x %02x\t", addr, cmd >> 8 & 0xff, cmd & 0xff);

    switch (cmd) {
    case 0x000: printf("nop");   goto done;
    case 0x002: printf("option");goto done;
    case 0x003: printf("sleep"); goto done;
    case 0x004: printf("awake"); goto done;
    case 0x006: printf("tris");  goto done;
    case 0x007: printf("dump");  goto done;
    case 0x008: printf("halt");  goto done;
    case 0x040: printf("az");    goto done;
    case 0x643: printf("z?");    goto done;
    case 0x743: printf("nz?");   goto done;
    case 0x603: printf("c?");    goto done;
    case 0x703: printf("nc?");   goto done;
    }

    switch (cmd >> 8) {
    case 0x8: m = "ret";  goto byteop;
    case 0x9: m = "call"; goto byteop;
    case 0xc: m = "cta";  goto byteop;
    case 0xd: m = "a|c";  goto byteop;
    case 0xe: m = "a&c";  goto byteop;
    case 0xf: m = "a^c";
byteop:
        printf("%s", m);
        arg = cmd & 0xff;
        if (arg > 9)
            printf("\t0x%x", arg);
        else if (arg)
            printf("\t%d", arg);
        goto done;

    case 0x4: m = "bz";  goto bitop;
    case 0x5: m = "bs";  goto bitop;
    case 0x6: m = "bs?"; goto bitop;
    case 0x7: m = "bz?"; goto bitop;
bitop:
        printf("%s\t", m);
        arg = cmd & 0x1f;
        if (arg > 9)
            printf("0x%x", arg);
        else
            printf("%d", arg);
        printf(", %d", cmd >> 5 & 7);
        goto done;
    }

    switch (cmd >> 4 & 0xfe) {
    case 0x02: m = "atx";   goto shortop;
    case 0x06: m = "xz";    goto shortop;
    case 0x08: m = "a-x";   goto shortop;
    case 0x0a: m = "x-a";   goto shortop;
    case 0x0c: m = "x--a";  goto shortop;
    case 0x0e: m = "x--";   goto shortop;
    case 0x10: m = "a|x";   goto shortop;
    case 0x12: m = "x|a";   goto shortop;
    case 0x14: m = "a&x";   goto shortop;
    case 0x16: m = "x&a";   goto shortop;
    case 0x18: m = "a^x";   goto shortop;
    case 0x1a: m = "x^a";   goto shortop;
    case 0x1c: m = "a+x";   goto shortop;
    case 0x1e: m = "x+a";   goto shortop;
    case 0x20: m = "xta";   goto shortop;
    case 0x22: m = "tst";   goto shortop;
    case 0x24: m = "xca";   goto shortop;
    case 0x26: m = "xc";    goto shortop;
    case 0x28: m = "x++a";  goto shortop;
    case 0x2a: m = "x++";   goto shortop;
    case 0x2c: m = "x--a?"; goto shortop;
    case 0x2e: m = "x--?";  goto shortop;
    case 0x30: m = "xc>>a"; goto shortop;
    case 0x32: m = "xc>>x"; goto shortop;
    case 0x34: m = "xc<<a"; goto shortop;
    case 0x36: m = "xc<<x"; goto shortop;
    case 0x38: m = "xwa";   goto shortop;
    case 0x3a: m = "xw";    goto shortop;
    case 0x3c: m = "x++a?"; goto shortop;
    case 0x3e: m = "x++?";
shortop:
        printf("%s", m);
        arg = cmd & 0x1f;
        if (arg > 9)
            printf("\t0x%x", arg);
        else if (arg)
            printf("\t%d", arg);
        break;

    case 0xa0: case 0xa2: case 0xa4: case 0xa6:
    case 0xa8: case 0xaa: case 0xac: case 0xae:
    case 0xb0: case 0xb2: case 0xb4: case 0xb6:
    case 0xb8: case 0xba: case 0xbc: case 0xbe:
        printf("goto");
        arg = cmd & 0x1ff;
        if (arg > 9)
            printf("\t0x%x", arg);
        else if (arg)
            printf("\t%d", arg);
        break;

    default:
        printf("?");
        if (cmd >= ' ' && cmd <= '~')
            printf("\t\t; `%c'", cmd);
        break;
    }
done:
    printf("\n");
}

void print_config()
{
    if (debug)
        printf("\t\tconfig = %08x\n", config.word32);

    switch (config.field.fosc) {
    case FOSC_ECH:      /* Default: External Clock, High Power Mode (4-20 MHz) */ break;
    case FOSC_ECM:      printf("\t\tconfig FOSC = ECM\t; External Clock, Medium Power Mode (0.5-4 MHz)\n"); break;
    case FOSC_ECL:      printf("\t\tconfig FOSC = ECL\t; External Clock, Low Power Mode (0-0.5 MHz)\n"); break;
    case FOSC_INTOSC:   printf("\t\tconfig FOSC = INTOSC\t; Internal oscillator\n"); break;
    case FOSC_EXTRC:    printf("\t\tconfig FOSC = EXTRC\t; External RC oscillator\n"); break;
    case FOSC_HS:       printf("\t\tconfig FOSC = HS\t; High-speed crystal/resonator\n"); break;
    case FOSC_XT:       printf("\t\tconfig FOSC = XT\t; Crystal/resonator\n"); break;
    case FOSC_LP:       printf("\t\tconfig FOSC = LP\t; Low-power crystal\n"); break;
    }
    switch (config.field.wdte) {
    case WDTE_ON:       /* Default: Watchdog enabled */ break;
    case WDTE_NSLEEP:   printf("\t\tconfig WDTE = NSLEEP\t; Watchdog enabled while running and disabled in Sleep\n"); break;
    case WDTE_SWDTEN:   printf("\t\tconfig WDTE = SWDTEN\t; Watchdog controlled by WDTCON.SWDTEN bit\n"); break;
    case WDTE_OFF:      printf("\t\tconfig WDTE = OFF\t; Watchdog disabled\n"); break;
    }
    switch (config.field.pwrte) {
    case PWRTE_OFF:     /* Default: Power-up Timer disabled */ break;
    case PWRTE_ON:      printf("\t\tconfig PWRTE = ON\t; Power-up Timer enabled\n"); break;
    }
    switch (config.field.mclre) {
    case MCLRE_ON:      /* Default: MCLR/VPP pin function is MCLR */ break;
    case MCLRE_OFF:     printf("\t\tconfig MCLRE = OFF\t; MCLR/VPP pin function is digital input\n"); break;
    }
    switch (config.field.cp) {
    case CP_OFF:        /* Default: Program memory code protection is disabled */ break;
    case CP_ON:         printf("\t\tconfig CP = ON\t; Program memory code protection is enabled\n"); break;
    }
    switch (config.field.boren) {
    case BOREN_ON:      /* Default: Brown-out Reset enabled */ break;
    case BOREN_NSLEEP:  printf("\t\tconfig BOREN = NSLEEP\t; Brown-out Reset enabled while running and disabled in Sleep\n"); break;
    case BOREN_SBODEN:  printf("\t\tconfig BOREN = SBODEN\t; Brown-out Reset controlled by BORCON.SBOREN bit\n"); break;
    case BOREN_OFF:     printf("\t\tconfig BOREN = OFF\t; Brown-out Reset disabled\n"); break;
    }
    switch (config.field.clkouten) {
    case CLKOUTEN_OFF:  /* Default: CLKOUT function is disabled */ break;
    case CLKOUTEN_ON:   printf("\t\tconfig CLKOUTEN = ON\t; CLKOUT function is enabled on the CLKOUT pin\n"); break;
    }
    switch (config.field.ieso) {
    case IESO_ON:       /* Default: Internal/External Switchover is enabled */ break;
    case IESO_OFF:      printf("\t\tconfig IESO = OFF\t; Internal/External Switchover is disabled\n"); break;
    }
    switch (config.field.fcmen) {
    case FCMEN_ON:      /* Default: Fail-Safe Clock Monitor is enabled */ break;
    case FCMEN_OFF:     printf("\t\tconfig FCMEN = OFF\t; Fail-Safe Clock Monitor is disabled\n"); break;
    }
    switch (config.field.wrt) {
    case WRT_OFF:       /* Default: Write protection off */ break;
    case WRT_BOOT:      printf("\t\tconfig WRT = BOOT\t; 000h to 1FFh write protected, 200h to 1FFFh may be modified by EECON control\n"); break;
    case WRT_HALF:      printf("\t\tconfig WRT = HALF\t; 000h to FFFh write protected, 1000h to 1FFFh may be modified by EECON control\n"); break;
    case WRT_ALL:       printf("\t\tconfig WRT = ALL\t; 000h to 1FFFh write protected, no addresses may be modified by EECON control\n"); break;
    }
    switch (config.field.pps1way) {
    case PPS1WAY_ON:    /* Default: The PPSLOCK bit cannot be cleared once it is set by software */ break;
    case PPS1WAY_OFF:   printf("\t\tconfig PPS1WAY = OFF\t; The PPSLOCK bit can be set and cleared repeatedly by software\n"); break;
    }
    switch (config.field.zcddis) {
    case ZCDDIS_ON:     /* Default: Zero-cross detect circuit is disabled at POR */ break;
    case ZCDDIS_OFF:    printf("\t\tconfig ZCDDIS = OFF\t; Zero-cross detect circuit is enabled at POR\n"); break;
    }
    switch (config.field.pllen) {
    case PLLEN_ON :     /* Default: 4x PLL is always enabled */ break;
    case PLLEN_OFF:     printf("\t\tconfig PLLEN = OFF\t; 4x PLL is enabled when software sets the SPLLEN bit\n"); break;
    }
    switch (config.field.stvren) {
    case STVREN_ON:     /* Default: Stack Overflow or Underflow will cause a Reset */ break;
    case STVREN_OFF:    printf("\t\tconfig STVREN = OFF\t; Stack Overflow or Underflow will not cause a Reset\n"); break;
    }
    switch (config.field.borv) {
    case BORV_LO:       /* Default: Brown-out Reset Voltage, low trip point selected */ break;
    case BORV_HI:       printf("\t\tconfig BORV = HI\t; Brown-out Reset Voltage, high trip point selected\n"); break;
    }
    switch (config.field.lpbor) {
    case LPBOR_OFF:     /* Default: Low-Power BOR is disabled */ break;
    case LPBOR_ON:      printf("\t\tconfig LPBOR = ON\t; Low-Power BOR is enabled\n"); break;
    }
    switch (config.field.debug) {
    case DEBUG_OFF:     /* Default: In-Circuit Debugger disabled, ICSPCLK and ICSPDAT are general purpose I/O pins */ break;
    case DEBUG_ON:      printf("\t\tconfig DEBUG = ON\t; In-Circuit Debugger enabled, ICSPCLK and ICSPDAT are dedicated to the debugger\n"); break;
    }
    switch (config.field.lvp) {
    case LVP_ON:        /* Default: Low-voltage programming enabled */ break;
    case LVP_OFF:       printf("\t\tconfig LVP = OFF\t; High-voltage on MCLR/VPP must be used for programming\n"); break;
    }
}

void disasm()
{
    int i;

    if (config.word32)
        print_config();

    for (i=0; i<TXTSIZE; ++i)
        if (tbusy[i])
            print(i, text[i]);
}

int main(int argc, char **argv)
{
    int i;
    char *cp;

    for (i=1; i<argc; i++)
        switch (argv[i][0]) {
        case '-':
            for (cp=argv[i]; *cp; cp++) switch (*cp) {
            case 'd':
                debug++;
                break;
            }
            break;
        default:
            if (infile)
                goto usage;
            infile = argv[i];
            if (! freopen(infile, "r", stdin))
                uerror("cannot open");
            break;
        }

    if (! infile) {
usage:
        printf("PIC16 Disassembler, by Serge Vakulenko\n\n");
        printf("Usage:\n\tdis16 infile.hex\n\n");
        return -1;
    }

    readimage();
    disasm();
    return 0;
}
