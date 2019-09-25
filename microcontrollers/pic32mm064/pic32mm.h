/*
 * Hardware register defines for Microchip PIC32MM microcontrollers.
 *
 * Copyright (C) 2019 Serge Vakulenko
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _IO_PIC32MM_H
#define _IO_PIC32MM_H

/*--------------------------------------
 * Coprocessor 0 registers.
 */
#define C0_UserLocal        4       // Select 2: User information that can be
                                    // written by privileged software and read
                                    // via RDHWR instruction
#define C0_HWREna           7       // Enables access via the RDHWR instruction
                                    // to selected hardware registers
                                    // in Non-Privileged mode
#define C0_BadVAddr         8       // Reports the address for the most recent
                                    // address related exception
#define C0_Count            9       // Processor cycle count
#define C0_Compare          11      // Timer interrupt control
#define C0_Status           12      // Processor status and control
#define C0_IntCtl           12      // Select 1: Interrupt control
#define C0_SRSCtl           12      // Select 2: Shadow set control
#define C0_SRSMap           12      // Select 3: Vector to shadow set mapping
#define C0_View_IPL         12      // Select 4: Contiguous view of IM and IPL fields
#define C0_SRSMap2          12      // Select 5: Vector to shadow set mapping
#define C0_Cause            13      // Cause of last exception
#define C0_View_RIPL        13      // Select 4: Contiguous view of IP and RIPL fields
#define C0_EPC              14      // Program Counter at last exception
#define C0_PRId             15      // Processor identification and revision
#define C0_EBase            15      // Select 1: Exception base address
#define C0_CDMMBase         15      // Select 2: Common Device Memory Map Base register
#define C0_Config           16      // Configuration register
#define C0_Config1          16      // Select 1: Configuration register 1
#define C0_Config2          16      // Select 2: Configuration register 2
#define C0_Config3          16      // Select 3: Configuration register 3
#define C0_Config4          16      // Select 4: Configuration register 4
#define C0_Config5          16      // Select 5: Configuration register 5
#define C0_Config7          16      // Select 7: Configuration register 7
#define C0_Debug            23      // EJTAG Debug register
#define C0_TraceControl     23      // Select 1: EJTAG Trace Control register
#define C0_TraceControl2    23      // Select 2: EJTAG Trace Control Register 2
#define C0_UserTraceData1   23      // Select 3: EJTAG User Trace Data 1 register
#define C0_TraceBPC         23      // Select 4: EJTAG Trace Breakpoint register
#define C0_Debug2           23      // Select 6: EJTAG Debug Register 2
#define C0_DEPC             24      // Program Counter at last debug exception
#define C0_UserTraceData2   24      // Select 3: EJTAG User Trace Data 2 register
#define C0_PerfCtl0         25      // Performance Counter 0 control
#define C0_PerfCnt0         25      // Select 1: Performance Counter 0
#define C0_PerfCtl1         25      // Select 2: Performance Counter 1 control
#define C0_PerfCnt1         25      // Select 3: Performance Counter 1
#define C0_ErrCtl           26      // Software parity check enable
#define C0_CacheErr         27      // Records information about SRAM parity errors
#define C0_ErrorEPC         30      // Program Counter at last error
#define C0_DESAVE           31      // EJTAG debug exception save register

/*
 * Status register.
 */
#define ST_CU0		0x10000000	/* Access to coprocessor 0 allowed (in user mode) */
#define ST_RP		0x08000000	/* Enable reduced power mode */
#define ST_RE		0x02000000	/* Reverse endianness (in user mode) */
#define ST_BEV		0x00400000	/* Exception vectors: bootstrap */
#define ST_SR		0x00100000	/* Soft reset */
#define ST_NMI		0x00080000	/* NMI reset */
#define ST_IPL(x)	((x) << 10)	/* Current interrupt priority level */
#define ST_UM		0x00000010	/* User mode */
#define ST_ERL		0x00000004	/* Error level */
#define ST_EXL		0x00000002	/* Exception level */
#define ST_IE		0x00000001	/* Interrupt enable */

/*
 * Сause register.
 */
#define CA_BD		0x80000000	/* Exception occured in delay slot */
#define CA_TI		0x40000000	/* Timer interrupt is pending */
#define CA_CE		0x30000000	/* Coprocessor exception */
#define CA_DC		0x08000000	/* Disable COUNT register */
#define CA_IV		0x00800000	/* Use special interrupt vector 0x200 */
#define CA_RIPL(r)	((r)>>10 & 63)	/* Requested interrupt priority level */
#define CA_IP1		0x00020000	/* Request software interrupt 1 */
#define CA_IP0		0x00010000	/* Request software interrupt 0 */
#define CA_EXC_CODE	0x0000007c	/* Exception code */

#define CA_Int		0		/* Interrupt */
#define CA_AdEL		(4 << 2)	/* Address error, load or instruction fetch */
#define CA_AdES		(5 << 2)	/* Address error, store */
#define CA_IBE		(6 << 2)	/* Bus error, instruction fetch */
#define CA_DBE		(7 << 2)	/* Bus error, load or store */
#define CA_Sys		(8 << 2)	/* Syscall */
#define CA_Bp		(9 << 2)	/* Breakpoint */
#define CA_RI		(10 << 2)	/* Reserved instruction */
#define CA_CPU		(11 << 2)	/* Coprocessor unusable */
#define CA_Ov		(12 << 2)	/* Arithmetic overflow */
#define CA_Tr		(13 << 2)	/* Trap */

#define DB_DBD          (1 << 31)       /* Debug exception in a branch delay slot */
#define DB_DM           (1 << 30)       /* Debug mode */
#define DB_NODCR        (1 << 29)       /* No dseg present */
#define DB_LSNM         (1 << 28)       /* Load/stores in dseg go to main memory */
#define DB_DOZE         (1 << 27)       /* Processor was in low-power mode */
#define DB_HALT         (1 << 26)       /* Internal system bus clock was running */
#define DB_COUNTDM      (1 << 25)       /* Count register is running in Debug mode */
#define DB_IBUSEP       (1 << 24)       /* Instruction fetch bus error exception */
#define DB_DBUSEP       (1 << 21)       /* Data access bus error exception */
#define DB_IEXI         (1 << 20)       /* Imprecise error exception */
#define DB_VER          (7 << 15)       /* EJTAG version number */
#define DB_DEXCCODE     (0x1f << 10)    /* Cause of exception in Debug mode */
#define DB_SST          (1 << 8)        /* Single step exception enabled */
#define DB_DIBImpr      (1 << 6)        /* Imprecise debug instruction break */
#define DB_DINT         (1 << 5)        /* Debug interrupt exception */
#define DB_DIB          (1 << 4)        /* Debug instruction break exception */
#define DB_DDBS         (1 << 3)        /* Debug data break exception on store */
#define DB_DDBL         (1 << 2)        /* Debug data break exception on load */
#define DB_DBP          (1 << 1)        /* Debug software breakpoint exception */
#define DB_DSS          (1 << 0)        /* Debug single-step exception */

/*
 * Read C0 coprocessor register.
 */
#define mfc0(reg, sel) ({ int __value;				\
	asm volatile (						\
	"mfc0	%0, $%1, %2"					\
	: "=r" (__value) : "K" (reg), "K" (sel));		\
	__value; })

/*
 * Write coprocessor 0 register.
 */
#define mtc0(reg, sel, value) asm volatile (                    \
	"mtc0	%z0, $%1, %2"                                   \
	: : "r" ((unsigned) (value)), "K" (reg), "K" (sel))

/*--------------------------------------
 * Peripheral registers.
 */
#define PIC32_R(a)		*(volatile unsigned*)(0xBF800000 + (a))

/*--------------------------------------
 * Configuration registers.
 */
#define PIC32_DEVCFG(cfg0, cfg1, cfg2, cfg3) \
    asm (".section .config"); \
    unsigned __DEVCFG0 __attribute__ ((section (".config0"))) = (cfg0) ^ 0x7fffffff; \
    unsigned __DEVCFG1 __attribute__ ((section (".config1"))) = (cfg1) | DEVCFG1_UNUSED; \
    unsigned __DEVCFG2 __attribute__ ((section (".config2"))) = (cfg2) | DEVCFG2_UNUSED; \
    unsigned __DEVCFG3 __attribute__ ((section (".config3"))) = (cfg3) | DEVCFG3_UNUSED

/*
 * Config0 register at 1fc02ffc, inverted.
 */
#define DEVCFG0_DEBUG_MASK      0x00000003 /* Debugger enable bits */
#define DEVCFG0_DEBUG_DISABLED  0x00000000 /* Debugger disabled */
#define DEVCFG0_DEBUG_ENABLED   0x00000002 /* Debugger enabled */
#define DEVCFG0_JTAGDIS         0x00000004 /* Disable JTAG port */
#define DEVCFG0_ICESEL          0x00000008 /* Use PGC1/PGD1 (default PGC2/PGD2) */
#define DEVCFG0_PWP_MASK        0x000ff000 /* Program flash write protect */
#define DEVCFG0_BWP             0x01000000 /* Boot flash write protect */
#define DEVCFG0_CP              0x10000000 /* Code protect */

/*
 * Config1 register at 1fc02ff8.
 */
#define DEVCFG1_UNUSED          0xff600858
#define DEVCFG1_FNOSC_MASK      0x00000007 /* Oscillator selection */
#define DEVCFG1_FNOSC_FRC       0x00000000 /* Fast RC */
#define DEVCFG1_FNOSC_FRCPLL    0x00000001 /* Fast RC with PLL */
#define DEVCFG1_FNOSC_PRI       0x00000002 /* Primary oscillator XT, HS, EC */
#define DEVCFG1_FNOSC_PRIPLL    0x00000003 /* Primary with PLL */
#define DEVCFG1_FNOSC_SEC       0x00000004 /* Secondary oscillator */
#define DEVCFG1_FNOSC_LPRC      0x00000005 /* Low-power RC */
#define DEVCFG1_FNOSC_FRCDIV    0x00000007 /* Fast RC with divide-by-N */
#define DEVCFG1_FSOSCEN         0x00000020 /* Secondary oscillator enable */
#define DEVCFG1_IESO            0x00000080 /* Internal-external switch over */
#define DEVCFG1_POSCMOD_MASK    0x00000300 /* Primary oscillator config */
#define DEVCFG1_POSCMOD_EXT     0x00000000 /* External mode */
#define DEVCFG1_POSCMOD_XT      0x00000100 /* XT oscillator */
#define DEVCFG1_POSCMOD_HS      0x00000200 /* HS oscillator */
#define DEVCFG1_POSCMOD_DISABLE 0x00000300 /* Disabled */
#define DEVCFG1_OSCIOFNC_OFF    0x00000400 /* CLKO output disable */
#define DEVCFG1_FPBDIV_MASK     0x00003000 /* Peripheral bus clock divisor */
#define DEVCFG1_FPBDIV_1        0x00000000 /* SYSCLK / 1 */
#define DEVCFG1_FPBDIV_2        0x00001000 /* SYSCLK / 2 */
#define DEVCFG1_FPBDIV_4        0x00002000 /* SYSCLK / 4 */
#define DEVCFG1_FPBDIV_8        0x00003000 /* SYSCLK / 8 */
#define DEVCFG1_FCKM_DISABLE    0x00004000 /* Fail-safe clock monitor disable */
#define DEVCFG1_FCKS_DISABLE    0x00008000 /* Clock switching disable */
#define DEVCFG1_WDTPS_MASK      0x001f0000 /* Watchdog postscale */
#define DEVCFG1_WDTPS_1         0x00000000 /* 1:1 */
#define DEVCFG1_WDTPS_2         0x00010000 /* 1:2 */
#define DEVCFG1_WDTPS_4         0x00020000 /* 1:4 */
#define DEVCFG1_WDTPS_8         0x00030000 /* 1:8 */
#define DEVCFG1_WDTPS_16        0x00040000 /* 1:16 */
#define DEVCFG1_WDTPS_32        0x00050000 /* 1:32 */
#define DEVCFG1_WDTPS_64        0x00060000 /* 1:64 */
#define DEVCFG1_WDTPS_128       0x00070000 /* 1:128 */
#define DEVCFG1_WDTPS_256       0x00080000 /* 1:256 */
#define DEVCFG1_WDTPS_512       0x00090000 /* 1:512 */
#define DEVCFG1_WDTPS_1024      0x000a0000 /* 1:1024 */
#define DEVCFG1_WDTPS_2048      0x000b0000 /* 1:2048 */
#define DEVCFG1_WDTPS_4096      0x000c0000 /* 1:4096 */
#define DEVCFG1_WDTPS_8192      0x000d0000 /* 1:8192 */
#define DEVCFG1_WDTPS_16384     0x000e0000 /* 1:16384 */
#define DEVCFG1_WDTPS_32768     0x000f0000 /* 1:32768 */
#define DEVCFG1_WDTPS_65536     0x00100000 /* 1:65536 */
#define DEVCFG1_WDTPS_131072    0x00110000 /* 1:131072 */
#define DEVCFG1_WDTPS_262144    0x00120000 /* 1:262144 */
#define DEVCFG1_WDTPS_524288    0x00130000 /* 1:524288 */
#define DEVCFG1_WDTPS_1048576   0x00140000 /* 1:1048576 */
#define DEVCFG1_FWDTEN          0x00800000 /* Watchdog enable */

/*
 * Config2 register at 1fc02ff4.
 */
#define DEVCFG2_UNUSED          0xfff87888
#define DEVCFG2_FPLLIDIV_MASK   0x00000007 /* PLL input divider */
#define DEVCFG2_FPLLIDIV_1      0x00000000 /* 1x */
#define DEVCFG2_FPLLIDIV_2      0x00000001 /* 2x */
#define DEVCFG2_FPLLIDIV_3      0x00000002 /* 3x */
#define DEVCFG2_FPLLIDIV_4      0x00000003 /* 4x */
#define DEVCFG2_FPLLIDIV_5      0x00000004 /* 5x */
#define DEVCFG2_FPLLIDIV_6      0x00000005 /* 6x */
#define DEVCFG2_FPLLIDIV_10     0x00000006 /* 10x */
#define DEVCFG2_FPLLIDIV_12     0x00000007 /* 12x */
#define DEVCFG2_FPLLMUL_MASK    0x00000070 /* PLL multiplier */
#define DEVCFG2_FPLLMUL_15      0x00000000 /* 15x */
#define DEVCFG2_FPLLMUL_16      0x00000010 /* 16x */
#define DEVCFG2_FPLLMUL_17      0x00000020 /* 17x */
#define DEVCFG2_FPLLMUL_18      0x00000030 /* 18x */
#define DEVCFG2_FPLLMUL_19      0x00000040 /* 19x */
#define DEVCFG2_FPLLMUL_20      0x00000050 /* 20x */
#define DEVCFG2_FPLLMUL_21      0x00000060 /* 21x */
#define DEVCFG2_FPLLMUL_24      0x00000070 /* 24x */
#define DEVCFG2_UPLLIDIV_MASK   0x00000700 /* USB PLL input divider */
#define DEVCFG2_UPLLIDIV_1      0x00000000 /* 1x */
#define DEVCFG2_UPLLIDIV_2      0x00000100 /* 2x */
#define DEVCFG2_UPLLIDIV_3      0x00000200 /* 3x */
#define DEVCFG2_UPLLIDIV_4      0x00000300 /* 4x */
#define DEVCFG2_UPLLIDIV_5      0x00000400 /* 5x */
#define DEVCFG2_UPLLIDIV_6      0x00000500 /* 6x */
#define DEVCFG2_UPLLIDIV_10     0x00000600 /* 10x */
#define DEVCFG2_UPLLIDIV_12     0x00000700 /* 12x */
#define DEVCFG2_UPLLDIS         0x00008000 /* Disable USB PLL */
#define DEVCFG2_FPLLODIV_MASK   0x00070000 /* Default postscaler for PLL */
#define DEVCFG2_FPLLODIV_1      0x00000000 /* 1x */
#define DEVCFG2_FPLLODIV_2      0x00010000 /* 2x */
#define DEVCFG2_FPLLODIV_4      0x00020000 /* 4x */
#define DEVCFG2_FPLLODIV_8      0x00030000 /* 8x */
#define DEVCFG2_FPLLODIV_16     0x00040000 /* 16x */
#define DEVCFG2_FPLLODIV_32     0x00050000 /* 32x */
#define DEVCFG2_FPLLODIV_64     0x00060000 /* 64x */
#define DEVCFG2_FPLLODIV_256    0x00070000 /* 256x */

/*
 * Config3 register at 1fc02ff0.
 */
#define DEVCFG3_UNUSED          0x38f80000
#define DEVCFG3_USERID_MASK     0x0000ffff /* User-defined ID */
#define DEVCFG3_USERID(x)       ((x) & 0xffff)
#define DEVCFG3_FSRSSEL_MASK    0x00070000 /* SRS select */
#define DEVCFG3_FSRSSEL_ALL     0x00000000 /* All irqs assigned to shadow set */
#define DEVCFG3_FSRSSEL_1       0x00010000 /* Assign irq priority 1 to shadow set */
#define DEVCFG3_FSRSSEL_2       0x00020000 /* Assign irq priority 2 to shadow set */
#define DEVCFG3_FSRSSEL_3       0x00030000 /* Assign irq priority 3 to shadow set */
#define DEVCFG3_FSRSSEL_4       0x00040000 /* Assign irq priority 4 to shadow set */
#define DEVCFG3_FSRSSEL_5       0x00050000 /* Assign irq priority 5 to shadow set */
#define DEVCFG3_FSRSSEL_6       0x00060000 /* Assign irq priority 6 to shadow set */
#define DEVCFG3_FSRSSEL_7       0x00070000 /* Assign irq priority 7 to shadow set */
#define DEVCFG3_FMIIEN          0x01000000 /* Ethernet MII enable */
#define DEVCFG3_FETHIO          0x02000000 /* Ethernet pins default */
#define DEVCFG3_FCANIO          0x04000000 /* CAN pins default */
#define DEVCFG3_FUSBIDIO        0x40000000 /* USBID pin: controlled by USB */
#define DEVCFG3_FVBUSONIO       0x80000000 /* VBuson pin: controlled by USB */

#endif /* _IO_PIC32MM_H */
