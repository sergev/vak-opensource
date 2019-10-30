/*
 * File:   main.c
 * Author: vak
 *
 * Created on October 29, 2019, 6:59 PM
 */
#include "xc.h"
#include "p32xxxx.h"

/*** DEVCFG0 ***/
#pragma config DEBUG        = OFF
#pragma config JTAGEN       = OFF
#pragma config ICESEL       = ICS_PGx2
#pragma config TRCEN        = OFF
#pragma config BOOTISA      = MIPS32
#pragma config FECCCON      = OFF_UNLOCKED
#pragma config FSLEEP       = OFF
#pragma config DBGPER       = PG_ALL
#pragma config SMCLR        = MCLR_NORM
#pragma config SOSCGAIN     = GAIN_2X
#pragma config SOSCBOOST    = ON
#pragma config POSCGAIN     = GAIN_2X
#pragma config POSCBOOST    = ON
#pragma config POSCFGAIN    = GAIN_G3
#pragma config POSCAGCDLY   = POSCAGCDLY1
#pragma config POSCAGC      = OFF
#pragma config EJTAGBEN     = NORMAL

/*** DEVCFG1 ***/
#pragma config FNOSC        = SPLL
#pragma config DMTINTV      = WIN_127_128
#pragma config FSOSCEN      = ON
#pragma config IESO         = ON
#pragma config POSCMOD      = EC
#pragma config OSCIOFNC     = OFF
#pragma config FCKSM        = CSECME
#pragma config WDTPS        = PS1048576
#pragma config WDTSPGM      = STOP
#pragma config WINDIS       = NORMAL
#pragma config FWDTEN       = OFF
#pragma config FWDTWINSZ    = WINSZ_25
#pragma config DMTCNT       = DMT31
#pragma config FDMTEN       = OFF

/*** DEVCFG2 ***/
#pragma config FPLLIDIV     = DIV_3
#pragma config FPLLRNG      = RANGE_5_10_MHZ
#pragma config FPLLICLK     = PLL_POSC
#pragma config FPLLMULT     = MUL_50
#pragma config FPLLODIV     = DIV_2
#pragma config VBATBOREN    = ON
#pragma config DSBOREN      = ON
#pragma config DSWDTPS      = DSPS32
#pragma config DSWDTOSC     = LPRC
#pragma config DSWDTEN      = OFF
#pragma config FDSEN        = ON
#pragma config UPLLFSEL     = FREQ_24MHZ

/*** DEVCFG3 ***/
#pragma config USERID       = 0xffff
#pragma config EXTDDRSIZE   = DDR_SIZE_128MB
#pragma config FMIIEN       = ON
#pragma config FETHIO       = ON
#pragma config PGL1WAY      = ON
#pragma config PMDL1WAY     = ON
#pragma config IOL1WAY      = ON

/*** DEVCFG4 ***/
#pragma config SWDTPS       = SPS1048576

/*** BF1SEQ0 ***/
#pragma config TSEQ         = 0x0
#pragma config CSEQ         = 0xffff

// application defines
#define SYS_FREQ            (200*1000*1000)

void DelayMs(unsigned msec)
{
    int i;
    for (i = 0; i < 1400000; i += 1);
}

int main(int argc, char** argv)
{
    // Configure the device for maximum performance.
    //SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

    // LED setup
    // Turn off leds before configuring the IO pin as output
    LATDCLR = 0x0007;

    // Set RD0, RD1 and RD2 as outputs
    TRISDCLR = 0x0007;

    while (1) {
        // toggle LED0
        LATDINV = 0x0001;
        DelayMs(100);

        // toggle LED1
        LATDINV = 0x0002;
        DelayMs(100);

        // toggle LED2
        LATDINV = 0x0004;
        DelayMs(100);
    };
    return 0;
}
