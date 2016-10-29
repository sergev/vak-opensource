/*
 * Include processor definitions.
 */
#include "pic32mz.h"

#define MHZ     200             /* CPU clock. */

#define PMDATA  PMDIN

#define LCDREG_CMD      0       /* Address of LCD command register */
#define LCDREG_DATA     1       /* Address of LCD data register */

void lcd_init (void);
void lcd_write (int addr, int c);
int lcd_read (int addr);
void lcd_puts (char *s);

#define lcd_putchar(d)  lcd_write (LCDREG_DATA, (d))
#define lcd_cmd(c)      lcd_write (LCDREG_CMD, (c))

#define lcd_clr()       lcd_write (LCDREG_CMD, 1)   // 1 msec delay needed
#define lcd_home()      lcd_write (LCDREG_CMD, 2)

#define lcd_set_col(a)  lcd_write (LCDREG_CMD, (a) | 0x80)

#define lcd_get()       lcd_read (LCDREG_DATA)
#define lcd_busy()      (lcd_read (LCDREG_CMD) & 0x80)
#define lcd_addr()      (lcd_read (LCDREG_CMD) & 0x7F)

/*
 * Delay for a given number of microseconds.
 * The processor has a 32-bit hardware Count register,
 * which increments at half CPU rate.
 * We use it to get a precise delay.
 */
void udelay (unsigned usec)
{
    unsigned now = mfc0 (C0_COUNT, 0);
    unsigned final = now + usec * MHZ / 2;

    for (;;) {
        now = mfc0 (C0_COUNT, 0);

        /* This comparison is valid only when using a signed type. */
        if ((int) (now - final) >= 0)
            break;
    }
}

void pmp_set_address (unsigned addr)
{
    while (PMMODE & PIC32_PMMODE_BUSY)
        ;
    PMADDR = addr;
}

void pmp_master_write (unsigned value)
{
    while (PMMODE & PIC32_PMMODE_BUSY)
        ;
    PMDIN = value;
}

int pmp_master_read()
{
    while (PMMODE & PIC32_PMMODE_BUSY)
        ;
    return PMDIN;
}

void lcd_init (void)
{
    /* PMP initialization: 8-bit data bus,
     * only PMA0 enabled. */
    PMCON  = PIC32_PMCON_ON |           /* Parallel master port enable */
             PIC32_PMCON_PTRDEN	|       /* Read/write strobe port enable */
             PIC32_PMCON_PTWREN	|       /* Write enable strobe port enable */
             PIC32_PMCON_RDSP |         /* Read strobe polarity active-high */
             PIC32_PMCON_WRSP;          /* Write strobe polarity active-high */
    PMMODE = PIC32_PMMODE_MODE_MAST1 |	/* Master mode 1 */
             PIC32_PMMODE_WAITB(4) |    /* Wait states: data setup to RW strobe */
             PIC32_PMMODE_WAITM(15) |   /* Wait states: data RW strobe */
             PIC32_PMMODE_WAITE(4);     /* Wait states: data hold after RW strobe */
    PMAEN  = 0x0001;

    // wait for >30ms
    udelay (30000);

    // initiate the HD44780 display 8-bit init sequence
    pmp_set_address (LCDREG_CMD);       // select command register
    pmp_master_write (0x38);            // 8-bit int, 2 lines, 5x7
    udelay (50);                        // > 48 us

    pmp_master_write (0x0c);            // ON, no cursor, no blink
    udelay (50);                        // > 48 us

    pmp_master_write (0x01);            // clear display
    udelay (2000);                      // > 1.6ms

    pmp_master_write (0x06);            // increment cursor, no shift
    udelay (2000);                      // > 1.6ms
}

int lcd_read (int addr)
{
    pmp_set_address (addr);             // select register
    pmp_master_read();                  // initiate read sequence
    return pmp_master_read();           // read actual data
}

void lcd_write (int addr, int c)
{
    while (lcd_busy())
        ;
    pmp_set_address (addr);             // select register
    pmp_master_write (c);               // initiate write sequence
}

void lcd_puts (char *s)
{
    int c;

    for (; *s; s++) {
        switch (*s) {
        case '\n':                      // point to second line
            lcd_set_col (0x40);
            break;
        case '\r':                      // home, point to first line
            lcd_set_col (0);
            break;
        case '\t':                      // advance next tab (8) positions
            c = lcd_addr();
            while (c & 7) {
                lcd_putchar (' ');
                c++;
            }
            if (c > 15)                 // if necessary move to second line
                lcd_set_col (0x40);
            break;
        default:                        // print character
            lcd_putchar (*s);
            break;
        }
    }
}

/*
 * Chip configuration.
 */
PIC32_DEVCFG (
    DEVCFG0_JTAG_DISABLE |      /* Disable JTAG port */
    DEVCFG0_TRC_DISABLE,        /* Disable trace port */
#if 0
    /* Case #1: using internal fast RC oscillator.
     * The frequency is around 8 MHz.
     * PLL multiplies it to 200 MHz. */
    DEVCFG1_FNOSC_SPLL |        /* System clock supplied by SPLL */
    DEVCFG1_POSCMOD_DISABLE |   /* Primary oscillator disabled */
    DEVCFG1_CLKO_DISABLE,       /* CLKO output disable */

    DEVCFG2_FPLLIDIV_1 |        /* PLL input divider = 1 */
    DEVCFG2_FPLLRNG_5_10 |      /* PLL input range is 5-10 MHz */
    DEVCFG2_FPLLICLK_FRC |      /* Select FRC as input to PLL */
    DEVCFG2_FPLLMULT(50) |      /* PLL multiplier = 50x */
    DEVCFG2_FPLLODIV_2,         /* PLL postscaler = 1/2 */
#endif
#if 1
    /* Case #2: using primary oscillator with external crystal 24 MHz.
     * PLL multiplies it to 200 MHz. */
    DEVCFG1_FNOSC_SPLL |        /* System clock supplied by SPLL */
    DEVCFG1_POSCMOD_EXT |       /* External generator */
    DEVCFG1_FCKS_ENABLE |       /* Enable clock switching */
    DEVCFG1_FCKM_ENABLE |       /* Enable fail-safe clock monitoring */
    DEVCFG1_IESO |              /* Internal-external switch over enable */
    DEVCFG1_CLKO_DISABLE,       /* CLKO output disable */

    DEVCFG2_FPLLIDIV_3 |        /* PLL input divider = 3 */
    DEVCFG2_FPLLRNG_5_10 |      /* PLL input range is 5-10 MHz */
    DEVCFG2_FPLLMULT(50) |      /* PLL multiplier = 50x */
    DEVCFG2_FPLLODIV_2,         /* PLL postscaler = 1/2 */
#endif
    DEVCFG3_FETHIO |            /* Default Ethernet pins */
    DEVCFG3_USERID(0xffff));    /* User-defined ID */

/*
 * Boot code at bfc00000.
 * Setup stack pointer and $gp registers, and jump to main().
 */
asm ("          .section .exception");
asm ("          .globl _start");
asm ("          .type _start, function");
asm ("_start:   la      $sp, _estack");
asm ("          la      $ra, main");
asm ("          la      $gp, _gp");
asm ("          jr      $ra");
asm ("          .text");
asm ("_estack   = _end + 0x1000");

int main()
{
    /* Initialize coprocessor 0. */
    mtc0 (C0_COUNT, 0, 0);
    mtc0 (C0_COMPARE, 0, -1);
    mtc0 (C0_EBASE, 1, 0x9fc00000);     /* Vector base */
    mtc0 (C0_INTCTL, 1, 1 << 5);        /* Vector spacing 32 bytes */
    mtc0 (C0_CAUSE, 0, 1 << 23);        /* Set IV */
    mtc0 (C0_STATUS, 0, 0);             /* Clear BEV */

    /* Set A and B ports as output. */
    LATA = 0;
    TRISA = 0;

    lcd_init();
    lcd_puts ("\rSimple LCD demo");
    lcd_puts ("\n--------");

    int pos = 0;
    int to_left = 1;
    for (;;) {
        LATA = 1 << pos;

        /* Shift the value. */
        if (to_left)
            pos++;
        else
            pos--;

        /* Reverse direction. */
        if (pos == 7)
            to_left = 0;
        else if (pos == 0)
            to_left = 1;

        lcd_set_col (0x40 + 7 - pos);
        lcd_putchar ('#');

        /* Wait 200 msec. */
        udelay (200000);

        lcd_set_col (0x40 + 7 - pos);
        lcd_putchar ('-');
    }
}
