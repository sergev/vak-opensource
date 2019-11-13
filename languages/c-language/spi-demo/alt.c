/*
 * Peripheral port select registers Microchip PIC32MZ microcontroller.
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
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/mman.h>
#include "gpio.h"

//
// Control registers for input mapping
//
#define INT1R           0x1404
#define INT2R           0x1408
#define INT3R           0x140C
#define INT4R           0x1410
#define T2CKR           0x1418
#define T3CKR           0x141C
#define T4CKR           0x1420
#define T5CKR           0x1424
#define T6CKR           0x1428
#define T7CKR           0x142C
#define T8CKR           0x1430
#define T9CKR           0x1434
#define IC1R            0x1438
#define IC2R            0x143C
#define IC3R            0x1440
#define IC4R            0x1444
#define IC5R            0x1448
#define IC6R            0x144C
#define IC7R            0x1450
#define IC8R            0x1454
#define IC9R            0x1458
#define OCFAR           0x1460
#define U1RXR           0x1468
#define U1CTSR          0x146C
#define U2RXR           0x1470
#define U2CTSR          0x1474
#define U3RXR           0x1478
#define U3CTSR          0x147C
#define U4RXR           0x1480
#define U4CTSR          0x1484
#define U5RXR           0x1488
#define U5CTSR          0x148C
#define U6RXR           0x1490
#define U6CTSR          0x1494
#define SDI1R           0x149C
#define SS1R            0x14A0
#define SDI2R           0x14A8
#define SS2R            0x14AC
#define SDI3R           0x14B4
#define SS3R            0x14B8
#define SDI4R           0x14C0
#define SS4R            0x14C4
#define SDI5R           0x14CC
#define SS5R            0x14D0
#define SDI6R           0x14D8
#define SS6R            0x14DC
#define C1RXR           0x14E0
#define C2RXR           0x14E4
#define REFCLKI1R       0x14E8
#define REFCLKI3R       0x14F0
#define REFCLKI4R       0x14F4

//
// Control registers for output mapping
//
#define RPA14R          0x1538
#define RPA15R          0x153C
#define RPB0R           0x1540
#define RPB1R           0x1544
#define RPB2R           0x1548
#define RPB3R           0x154C
#define RPB5R           0x1554
#define RPB6R           0x1558
#define RPB7R           0x155C
#define RPB8R           0x1560
#define RPB9R           0x1564
#define RPB10R          0x1568
#define RPB14R          0x1578
#define RPB15R          0x157C
#define RPC1R           0x1584
#define RPC2R           0x1588
#define RPC3R           0x158C
#define RPC4R           0x1590
#define RPC13R          0x15B4
#define RPC14R          0x15B8
#define RPD0R           0x15C0
#define RPD1R           0x15C4
#define RPD2R           0x15C8
#define RPD3R           0x15CC
#define RPD4R           0x15D0
#define RPD5R           0x15D4
#define RPD6R           0x15D8
#define RPD7R           0x15DC
#define RPD9R           0x15E4
#define RPD10R          0x15E8
#define RPD11R          0x15EC
#define RPD12R          0x15F0
#define RPD14R          0x15F8
#define RPD15R          0x15FC
#define RPE3R           0x160C
#define RPE5R           0x1614
#define RPE8R           0x1620
#define RPE9R           0x1624
#define RPF0R           0x1640
#define RPF1R           0x1644
#define RPF2R           0x1648
#define RPF3R           0x164C
#define RPF4R           0x1650
#define RPF5R           0x1654
#define RPF8R           0x1660
#define RPF12R          0x1670
#define RPF13R          0x1674
#define RPG0R           0x1680
#define RPG1R           0x1684
#define RPG6R           0x1698
#define RPG7R           0x169C
#define RPG8R           0x16A0
#define RPG9R           0x16A4

static ptrdiff_t pps_base;          // PPS registers mapped here

//
// Get access to PPS control registers.
// Set pps_base to a base address of the appropriate page.
//
static void pps_init()
{
    const int PPS_ADDR = 0x1f801000;
    extern int gpio_mem_fd;

    // Map a page of memory to the PPS address
    pps_base = (ptrdiff_t) mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
        gpio_mem_fd, PPS_ADDR);
    if (pps_base < 0) {
        printf("PPS mmap failed: %s\n", strerror(errno));
        exit(-1);
    }
}

//
// Read PPS control register.
// Return bits 3:0.
//
static uint32_t read_sfr(int offset)
{
    if (!pps_base)
        pps_init();

    volatile uint32_t *regp = (uint32_t*) (pps_base + (offset & 0xfff));
    uint32_t value = *regp;
    if (gpio_debug > 1)
        printf("--- %s: [%04x] -> %08x\n", __func__, offset, value);
    return value & 0xf;
}

//
// Write PPS control register.
//
static void write_sfr(int offset, uint32_t value)
{
    if (!pps_base)
        pps_init();

    volatile uint32_t *regp = (uint32_t*) (pps_base + (offset & 0xfff));
    *regp = value;
    if (gpio_debug > 0)
        printf("--- %s: %08x -> [%04x]\n", __func__, value, offset);
}

//
// Clear PPS control register.
//
static void clear_sfr(int offset)
{
    if (!pps_base)
        pps_init();

    volatile uint32_t *regp = (uint32_t*) (pps_base + (offset & 0xfff));
    uint32_t value = *regp;
    if (value & 0xf) {
        *regp = 0;
        if (gpio_debug > 0)
            printf("--- %s: [%04x] -> %08x, 0 -> [%04x]\n", __func__, offset, value, offset);
    }
}

//
// Get mode value for output group 1.
// See pic32mz-da data sheet, table 12-2 on page 266.
//
static gpio_mode_t output_group1_to_mode(uint32_t value)
{
    switch (value) {
    default:
    case 0:  return 0;          // No Connect
    case 1:  return MODE_U3TX;
    case 2:  return MODE_U4RTS;
    case 3:  return 0;          // Reserved
    case 4:  return 0;          // Reserved
    case 5:  return MODE_SDO1;
    case 6:  return MODE_SDO2;
    case 7:  return MODE_SDO3;
    case 8:  return 0;          // Reserved
    case 9:  return MODE_SDO5;
    case 10: return MODE_SS6O;
    case 11: return MODE_OC3;
    case 12: return MODE_OC6;
    case 13: return MODE_REFCLKO4;
    case 14: return MODE_C2OUT;
    case 15: return MODE_C1TX;
    }
}

//
// Get mode value for output group 2.
// See pic32mz-da data sheet, table 12-2 on page 266.
//
static gpio_mode_t output_group2_to_mode(uint32_t value)
{
    switch (value) {
    default:
    case 0:  return 0;          // No Connect
    case 1:  return MODE_U1TX;
    case 2:  return MODE_U2RTS;
    case 3:  return MODE_U5TX;
    case 4:  return MODE_U6RTS;
    case 5:  return MODE_SDO1;
    case 6:  return MODE_SDO2;
    case 7:  return MODE_SDO3;
    case 8:  return MODE_SDO4;
    case 9:  return MODE_SDO5;
    case 10: return 0;          // Reserved
    case 11: return MODE_OC4;
    case 12: return MODE_OC7;
    case 13: return 0;          // Reserved
    case 14: return 0;          // Reserved
    case 15: return MODE_REFCLKO1;
    }
}

//
// Get mode value for output group 3.
// See pic32mz-da data sheet, table 12-2 on page 267.
//
static gpio_mode_t output_group3_to_mode(uint32_t value)
{
    switch (value) {
    default:
    case 0:  return 0;          // No Connect
    case 1:  return MODE_U3RTS;
    case 2:  return MODE_U4TX;
    case 3:  return 0;          // Reserved
    case 4:  return MODE_U6TX;
    case 5:  return MODE_SS1O;
    case 6:  return 0;          // Reserved
    case 7:  return MODE_SS3O;
    case 8:  return MODE_SS4O;
    case 9:  return MODE_SS5O;
    case 10: return MODE_SDO6;
    case 11: return MODE_OC5;
    case 12: return MODE_OC8;
    case 13: return 0;          // Reserved
    case 14: return MODE_C1OUT;
    case 15: return MODE_REFCLKO3;
    }
}

//
// Get mode value for output group 4.
// See pic32mz-da data sheet, table 12-2 on page 267.
//
static gpio_mode_t output_group4_to_mode(uint32_t value)
{
    switch (value) {
    default:
    case 0:  return 0;          // No Connect
    case 1:  return MODE_U1RTS;
    case 2:  return MODE_U2TX;
    case 3:  return MODE_U5RTS;
    case 4:  return MODE_U6TX;
    case 5:  return 0;          // Reserved
    case 6:  return MODE_SS2O;
    case 7:  return 0;          // Reserved
    case 8:  return MODE_SDO4;
    case 9:  return 0;          // Reserved
    case 10: return MODE_SDO6;
    case 11: return MODE_OC2;
    case 12: return MODE_OC1;
    case 13: return MODE_OC9;
    case 14: return 0;          // Reserved
    case 15: return MODE_C2TX;
    }
}

//
// Get output mapping for a given pin.
//
gpio_mode_t gpio_get_output_mapping(int pin)
{
    switch (pin) {
    case GPIO_PIN('D',2):  return output_group1_to_mode(read_sfr(RPD2R));
    case GPIO_PIN('G',8):  return output_group1_to_mode(read_sfr(RPG8R));
    case GPIO_PIN('F',4):  return output_group1_to_mode(read_sfr(RPF4R));
    case GPIO_PIN('F',1):  return output_group1_to_mode(read_sfr(RPF1R));
    case GPIO_PIN('B',9):  return output_group1_to_mode(read_sfr(RPB9R));
    case GPIO_PIN('B',10): return output_group1_to_mode(read_sfr(RPB10R));
    case GPIO_PIN('B',5):  return output_group1_to_mode(read_sfr(RPB5R));
    case GPIO_PIN('C',1):  return output_group1_to_mode(read_sfr(RPC1R));
    case GPIO_PIN('D',14): return output_group1_to_mode(read_sfr(RPD14R));
    case GPIO_PIN('G',1):  return output_group1_to_mode(read_sfr(RPG1R));
    case GPIO_PIN('A',14): return output_group1_to_mode(read_sfr(RPA14R));
    case GPIO_PIN('D',6):  return output_group1_to_mode(read_sfr(RPD6R));

    case GPIO_PIN('D',3):  return output_group2_to_mode(read_sfr(RPD3R));
    case GPIO_PIN('G',7):  return output_group2_to_mode(read_sfr(RPG7R));
    case GPIO_PIN('F',5):  return output_group2_to_mode(read_sfr(RPF5R));
    case GPIO_PIN('D',11): return output_group2_to_mode(read_sfr(RPD11R));
    case GPIO_PIN('F',0):  return output_group2_to_mode(read_sfr(RPF0R));
    case GPIO_PIN('B',1):  return output_group2_to_mode(read_sfr(RPB1R));
    case GPIO_PIN('E',5):  return output_group2_to_mode(read_sfr(RPE5R));
    case GPIO_PIN('B',3):  return output_group2_to_mode(read_sfr(RPB3R));
    case GPIO_PIN('C',4):  return output_group2_to_mode(read_sfr(RPC4R));
    case GPIO_PIN('G',0):  return output_group2_to_mode(read_sfr(RPG0R));
    case GPIO_PIN('A',15): return output_group2_to_mode(read_sfr(RPA15R));
    case GPIO_PIN('D',7):  return output_group2_to_mode(read_sfr(RPD7R));

    case GPIO_PIN('D',9):  return output_group3_to_mode(read_sfr(RPD9R));
    case GPIO_PIN('B',8):  return output_group3_to_mode(read_sfr(RPB8R));
    case GPIO_PIN('B',15): return output_group3_to_mode(read_sfr(RPB15R));
    case GPIO_PIN('D',4):  return output_group3_to_mode(read_sfr(RPD4R));
    case GPIO_PIN('B',0):  return output_group3_to_mode(read_sfr(RPB0R));
    case GPIO_PIN('E',3):  return output_group3_to_mode(read_sfr(RPE3R));
    case GPIO_PIN('B',7):  return output_group3_to_mode(read_sfr(RPB7R));
    case GPIO_PIN('F',12): return output_group3_to_mode(read_sfr(RPF12R));
    case GPIO_PIN('D',12): return output_group3_to_mode(read_sfr(RPD12R));
    case GPIO_PIN('F',8):  return output_group3_to_mode(read_sfr(RPF8R));
    case GPIO_PIN('C',3):  return output_group3_to_mode(read_sfr(RPC3R));
    case GPIO_PIN('E',9):  return output_group3_to_mode(read_sfr(RPE9R));

    case GPIO_PIN('G',9):  return output_group4_to_mode(read_sfr(RPG9R));
    case GPIO_PIN('D',0):  return output_group4_to_mode(read_sfr(RPD0R));
    case GPIO_PIN('B',6):  return output_group4_to_mode(read_sfr(RPB6R));
    case GPIO_PIN('D',5):  return output_group4_to_mode(read_sfr(RPD5R));
    case GPIO_PIN('B',2):  return output_group4_to_mode(read_sfr(RPB2R));
    case GPIO_PIN('F',3):  return output_group4_to_mode(read_sfr(RPF3R));
    case GPIO_PIN('C',2):  return output_group4_to_mode(read_sfr(RPC2R));
    case GPIO_PIN('E',8):  return output_group4_to_mode(read_sfr(RPE8R));
    case GPIO_PIN('F',2):  return output_group4_to_mode(read_sfr(RPF2R));
    }
    return 0;
}

//
// Read mode value for a given pin index in input group 1.
// See pic32mz-da data sheet, table 12-1 on page 263.
//
static gpio_mode_t read_input_group1(int value)
{
    if (read_sfr(INT3R)     == value) return MODE_INT3;
    if (read_sfr(T2CKR)     == value) return MODE_T2CK;
    if (read_sfr(T6CKR)     == value) return MODE_T6CK;
    if (read_sfr(IC3R)      == value) return MODE_IC3;
    if (read_sfr(IC7R)      == value) return MODE_IC7;
    if (read_sfr(U1RXR)     == value) return MODE_U1RX;
    if (read_sfr(U2CTSR)    == value) return MODE_U2CTS;
    if (read_sfr(U5RXR)     == value) return MODE_U5RX;
    if (read_sfr(U6CTSR)    == value) return MODE_U6CTS;
    if (read_sfr(SDI1R)     == value) return MODE_SDI1;
    if (read_sfr(SDI3R)     == value) return MODE_SDI3;
    if (read_sfr(SDI5R)     == value) return MODE_SDI5;
    if (read_sfr(SS6R)      == value) return MODE_SS6I;
    if (read_sfr(REFCLKI1R) == value) return MODE_REFCLKI1;
    return 0;
}

//
// Read mode value for a given pin index in input group 2.
// See pic32mz-da data sheet, table 12-1 on page 263.
//
static gpio_mode_t read_input_group2(int value)
{
    if (read_sfr(INT4R)     == value) return MODE_INT4;
    if (read_sfr(T5CKR)     == value) return MODE_T5CK;
    if (read_sfr(T7CKR)     == value) return MODE_T7CK;
    if (read_sfr(IC4R)      == value) return MODE_IC4;
    if (read_sfr(IC8R)      == value) return MODE_IC8;
    if (read_sfr(U3RXR)     == value) return MODE_U3RX;
    if (read_sfr(U4CTSR)    == value) return MODE_U4CTS;
    if (read_sfr(SDI2R)     == value) return MODE_SDI2;
    if (read_sfr(SDI4R)     == value) return MODE_SDI4;
    if (read_sfr(C1RXR)     == value) return MODE_C1RX;
    if (read_sfr(REFCLKI4R) == value) return MODE_REFCLKI4;
    return 0;
}

//
// Read mode value for a given pin index in input group 3.
// See pic32mz-da data sheet, table 12-1 on page 264.
//
static gpio_mode_t read_input_group3(int value)
{
    if (read_sfr(INT2R)  == value) return MODE_INT2;
    if (read_sfr(T3CKR)  == value) return MODE_T3CK;
    if (read_sfr(T8CKR)  == value) return MODE_T8CK;
    if (read_sfr(IC2R)   == value) return MODE_IC2;
    if (read_sfr(IC5R)   == value) return MODE_IC5;
    if (read_sfr(IC9R)   == value) return MODE_IC9;
    if (read_sfr(U1CTSR) == value) return MODE_U1CTS;
    if (read_sfr(U2RXR)  == value) return MODE_U2RX;
    if (read_sfr(U5CTSR) == value) return MODE_U5CTS;
    if (read_sfr(SS1R)   == value) return MODE_SS1I;
    if (read_sfr(SS3R)   == value) return MODE_SS3I;
    if (read_sfr(SS4R)   == value) return MODE_SS4I;
    if (read_sfr(SS5R)   == value) return MODE_SS5I;
    if (read_sfr(C2RXR)  == value) return MODE_C2RX;
    return 0;
}

//
// Read mode value for a given pin index in input group 4.
// See pic32mz-da data sheet, table 12-1 on page 264.
//
static gpio_mode_t read_input_group4(int value)
{
    if (read_sfr(INT1R)     == value) return MODE_INT1;
    if (read_sfr(T4CKR)     == value) return MODE_T4CK;
    if (read_sfr(T9CKR)     == value) return MODE_T9CK;
    if (read_sfr(IC1R)      == value) return MODE_IC1;
    if (read_sfr(IC6R)      == value) return MODE_IC6;
    if (read_sfr(U3CTSR)    == value) return MODE_U3CTS;
    if (read_sfr(U4RXR)     == value) return MODE_U4RX;
    if (read_sfr(U6RXR)     == value) return MODE_U6RX;
    if (read_sfr(SS2R)      == value) return MODE_SS2I;
    if (read_sfr(SDI6R)     == value) return MODE_SDI6;
    if (read_sfr(OCFAR)     == value) return MODE_OCFA;
    if (read_sfr(REFCLKI3R) == value) return MODE_REFCLKI3;
    return 0;
}

//
// Get input mapping for a given pin.
//
gpio_mode_t gpio_get_input_mapping(int pin)
{
    switch (pin) {
    case GPIO_PIN('A',14): return read_input_group1(13);
    case GPIO_PIN('A',15): return read_input_group2(13);
    case GPIO_PIN('B',0):  return read_input_group3(5);
    case GPIO_PIN('B',1):  return read_input_group2(5);
    case GPIO_PIN('B',2):  return read_input_group4(7);
    case GPIO_PIN('B',3):  return read_input_group2(8);
    case GPIO_PIN('B',5):  return read_input_group1(8);
    case GPIO_PIN('B',6):  return read_input_group4(5);
    case GPIO_PIN('B',7):  return read_input_group3(7);
    case GPIO_PIN('B',8):  return read_input_group3(2);
    case GPIO_PIN('B',9):  return read_input_group1(5);
    case GPIO_PIN('B',10): return read_input_group1(6);
    case GPIO_PIN('B',15): return read_input_group3(3);
    case GPIO_PIN('C',1):  return read_input_group1(10);
    case GPIO_PIN('C',2):  return read_input_group4(12);
    case GPIO_PIN('C',3):  return read_input_group3(12);
    case GPIO_PIN('C',4):  return read_input_group2(10);
    case GPIO_PIN('C',13): return read_input_group2(7);
    case GPIO_PIN('C',14): return read_input_group1(7);
    case GPIO_PIN('D',0):  return read_input_group4(3);
    case GPIO_PIN('D',2):  return read_input_group1(0);
    case GPIO_PIN('D',3):  return read_input_group2(0);
    case GPIO_PIN('D',4):  return read_input_group3(4);
    case GPIO_PIN('D',5):  return read_input_group4(6);
    case GPIO_PIN('D',6):  return read_input_group1(14);
    case GPIO_PIN('D',7):  return read_input_group2(14);
    case GPIO_PIN('D',9):  return read_input_group3(0);
    case GPIO_PIN('D',11): return read_input_group2(3);
    case GPIO_PIN('D',12): return read_input_group3(10);
    case GPIO_PIN('D',14): return read_input_group1(11);
    case GPIO_PIN('E',3):  return read_input_group3(6);
    case GPIO_PIN('E',5):  return read_input_group2(6);
    case GPIO_PIN('E',8):  return read_input_group4(13);
    case GPIO_PIN('E',9):  return read_input_group3(13);
    case GPIO_PIN('F',0):  return read_input_group2(4);
    case GPIO_PIN('F',1):  return read_input_group1(4);
    case GPIO_PIN('F',2):  return read_input_group4(11);
    case GPIO_PIN('F',3):  return read_input_group4(8);
    case GPIO_PIN('F',4):  return read_input_group1(2);
    case GPIO_PIN('F',5):  return read_input_group2(2);
    case GPIO_PIN('F',8):  return read_input_group3(11);
    case GPIO_PIN('F',12): return read_input_group3(9);
    case GPIO_PIN('G',0):  return read_input_group2(12);
    case GPIO_PIN('G',1):  return read_input_group1(12);
    case GPIO_PIN('G',7):  return read_input_group2(1);
    case GPIO_PIN('G',8):  return read_input_group1(1);
    case GPIO_PIN('G',9):  return read_input_group4(1);
    }
    return 0;
}

//
// Clear a given mapping for input group 1.
//
static gpio_mode_t clear_input_group1(int value)
{
    if (read_sfr(INT3R)     == value) write_sfr(INT3R, 15);
    if (read_sfr(T2CKR)     == value) write_sfr(T2CKR, 15);
    if (read_sfr(T6CKR)     == value) write_sfr(T6CKR, 15);
    if (read_sfr(IC3R)      == value) write_sfr(IC3R, 15);
    if (read_sfr(IC7R)      == value) write_sfr(IC7R, 15);
    if (read_sfr(U1RXR)     == value) write_sfr(U1RXR, 15);
    if (read_sfr(U2CTSR)    == value) write_sfr(U2CTSR, 15);
    if (read_sfr(U5RXR)     == value) write_sfr(U5RXR, 15);
    if (read_sfr(U6CTSR)    == value) write_sfr(U6CTSR, 15);
    if (read_sfr(SDI1R)     == value) write_sfr(SDI1R, 15);
    if (read_sfr(SDI3R)     == value) write_sfr(SDI3R, 15);
    if (read_sfr(SDI5R)     == value) write_sfr(SDI5R, 15);
    if (read_sfr(SS6R)      == value) write_sfr(SS6R, 15);
    if (read_sfr(REFCLKI1R) == value) write_sfr(REFCLKI1R, 15);
    return 0;
}

//
// Clear a given mapping for input group 2.
//
static gpio_mode_t clear_input_group2(int value)
{
    if (read_sfr(INT4R)     == value) write_sfr(INT4R, 15);
    if (read_sfr(T5CKR)     == value) write_sfr(T5CKR, 15);
    if (read_sfr(T7CKR)     == value) write_sfr(T7CKR, 15);
    if (read_sfr(IC4R)      == value) write_sfr(IC4R, 15);
    if (read_sfr(IC8R)      == value) write_sfr(IC8R, 15);
    if (read_sfr(U3RXR)     == value) write_sfr(U3RXR, 15);
    if (read_sfr(U4CTSR)    == value) write_sfr(U4CTSR, 15);
    if (read_sfr(SDI2R)     == value) write_sfr(SDI2R, 15);
    if (read_sfr(SDI4R)     == value) write_sfr(SDI4R, 15);
    if (read_sfr(C1RXR)     == value) write_sfr(C1RXR, 15);
    if (read_sfr(REFCLKI4R) == value) write_sfr(REFCLKI4R, 15);
    return 0;
}

//
// Clear a given mapping for input group 3.
//
static gpio_mode_t clear_input_group3(int value)
{
    if (read_sfr(INT2R)  == value) write_sfr(INT2R, 15);
    if (read_sfr(T3CKR)  == value) write_sfr(T3CKR, 15);
    if (read_sfr(T8CKR)  == value) write_sfr(T8CKR, 15);
    if (read_sfr(IC2R)   == value) write_sfr(IC2R, 15);
    if (read_sfr(IC5R)   == value) write_sfr(IC5R, 15);
    if (read_sfr(IC9R)   == value) write_sfr(IC9R, 15);
    if (read_sfr(U1CTSR) == value) write_sfr(U1CTSR, 15);
    if (read_sfr(U2RXR)  == value) write_sfr(U2RXR, 15);
    if (read_sfr(U5CTSR) == value) write_sfr(U5CTSR, 15);
    if (read_sfr(SS1R)   == value) write_sfr(SS1R, 15);
    if (read_sfr(SS3R)   == value) write_sfr(SS3R, 15);
    if (read_sfr(SS4R)   == value) write_sfr(SS4R, 15);
    if (read_sfr(SS5R)   == value) write_sfr(SS5R, 15);
    if (read_sfr(C2RXR)  == value) write_sfr(C2RXR, 15);
    return 0;
}

//
// Clear a given mapping for input group 4.
//
static gpio_mode_t clear_input_group4(int value)
{
    if (read_sfr(INT1R)     == value) write_sfr(INT1R, 15);
    if (read_sfr(T4CKR)     == value) write_sfr(T4CKR, 15);
    if (read_sfr(T9CKR)     == value) write_sfr(T9CKR, 15);
    if (read_sfr(IC1R)      == value) write_sfr(IC1R, 15);
    if (read_sfr(IC6R)      == value) write_sfr(IC6R, 15);
    if (read_sfr(U3CTSR)    == value) write_sfr(U3CTSR, 15);
    if (read_sfr(U4RXR)     == value) write_sfr(U4RXR, 15);
    if (read_sfr(U6RXR)     == value) write_sfr(U6RXR, 15);
    if (read_sfr(SS2R)      == value) write_sfr(SS2R, 15);
    if (read_sfr(SDI6R)     == value) write_sfr(SDI6R, 15);
    if (read_sfr(OCFAR)     == value) write_sfr(OCFAR, 15);
    if (read_sfr(REFCLKI3R) == value) write_sfr(REFCLKI3R, 15);
    return 0;
}

//
// Clear mapping for a given pin.
//
void gpio_clear_mapping(int pin)
{
    switch (pin) {
    case GPIO_PIN('A',14): clear_input_group1(13); clear_sfr(RPA14R); break;
    case GPIO_PIN('A',15): clear_input_group2(13); clear_sfr(RPA15R); break;
    case GPIO_PIN('B',0):  clear_input_group3(5);  clear_sfr(RPB0R);  break;
    case GPIO_PIN('B',10): clear_input_group1(6);  clear_sfr(RPB10R); break;
    case GPIO_PIN('B',15): clear_input_group3(3);  clear_sfr(RPB15R); break;
    case GPIO_PIN('B',1):  clear_input_group2(5);  clear_sfr(RPB1R);  break;
    case GPIO_PIN('B',2):  clear_input_group4(7);  clear_sfr(RPB2R);  break;
    case GPIO_PIN('B',3):  clear_input_group2(8);  clear_sfr(RPB3R);  break;
    case GPIO_PIN('B',5):  clear_input_group1(8);  clear_sfr(RPB5R);  break;
    case GPIO_PIN('B',6):  clear_input_group4(5);  clear_sfr(RPB6R);  break;
    case GPIO_PIN('B',7):  clear_input_group3(7);  clear_sfr(RPB7R);  break;
    case GPIO_PIN('B',8):  clear_input_group3(2);  clear_sfr(RPB8R);  break;
    case GPIO_PIN('B',9):  clear_input_group1(5);  clear_sfr(RPB9R);  break;
    case GPIO_PIN('C',13): clear_input_group2(7);                     break;
    case GPIO_PIN('C',14): clear_input_group1(7);                     break;
    case GPIO_PIN('C',1):  clear_input_group1(10); clear_sfr(RPC1R);  break;
    case GPIO_PIN('C',2):  clear_input_group4(12); clear_sfr(RPC2R);  break;
    case GPIO_PIN('C',3):  clear_input_group3(12); clear_sfr(RPC3R);  break;
    case GPIO_PIN('C',4):  clear_input_group2(10); clear_sfr(RPC4R);  break;
    case GPIO_PIN('D',0):  clear_input_group4(3);  clear_sfr(RPD0R);  break;
    case GPIO_PIN('D',11): clear_input_group2(3);  clear_sfr(RPD11R); break;
    case GPIO_PIN('D',12): clear_input_group3(10); clear_sfr(RPD12R); break;
    case GPIO_PIN('D',14): clear_input_group1(11); clear_sfr(RPD14R); break;
    case GPIO_PIN('D',2):  clear_input_group1(0);  clear_sfr(RPD2R);  break;
    case GPIO_PIN('D',3):  clear_input_group2(0);  clear_sfr(RPD3R);  break;
    case GPIO_PIN('D',4):  clear_input_group3(4);  clear_sfr(RPD4R);  break;
    case GPIO_PIN('D',5):  clear_input_group4(6);  clear_sfr(RPD5R);  break;
    case GPIO_PIN('D',6):  clear_input_group1(14); clear_sfr(RPD6R);  break;
    case GPIO_PIN('D',7):  clear_input_group2(14); clear_sfr(RPD7R);  break;
    case GPIO_PIN('D',9):  clear_input_group3(0);  clear_sfr(RPD9R);  break;
    case GPIO_PIN('E',3):  clear_input_group3(6);  clear_sfr(RPE3R);  break;
    case GPIO_PIN('E',5):  clear_input_group2(6);  clear_sfr(RPE5R);  break;
    case GPIO_PIN('E',8):  clear_input_group4(13); clear_sfr(RPE8R);  break;
    case GPIO_PIN('E',9):  clear_input_group3(13); clear_sfr(RPE9R);  break;
    case GPIO_PIN('F',0):  clear_input_group2(4);  clear_sfr(RPF0R);  break;
    case GPIO_PIN('F',12): clear_input_group3(9);  clear_sfr(RPF12R); break;
    case GPIO_PIN('F',1):  clear_input_group1(4);  clear_sfr(RPF1R);  break;
    case GPIO_PIN('F',2):  clear_input_group4(11); clear_sfr(RPF2R);  break;
    case GPIO_PIN('F',3):  clear_input_group4(8);  clear_sfr(RPF3R);  break;
    case GPIO_PIN('F',4):  clear_input_group1(2);  clear_sfr(RPF4R);  break;
    case GPIO_PIN('F',5):  clear_input_group2(2);  clear_sfr(RPF5R);  break;
    case GPIO_PIN('F',8):  clear_input_group3(11); clear_sfr(RPF8R);  break;
    case GPIO_PIN('G',0):  clear_input_group2(12); clear_sfr(RPG0R);  break;
    case GPIO_PIN('G',1):  clear_input_group1(12); clear_sfr(RPG1R);  break;
    case GPIO_PIN('G',7):  clear_input_group2(1);  clear_sfr(RPG7R);  break;
    case GPIO_PIN('G',8):  clear_input_group1(1);  clear_sfr(RPG8R);  break;
    case GPIO_PIN('G',9):  clear_input_group4(1);  clear_sfr(RPG9R);  break;
    default:
        break;
    }
}

static int pin_to_input_group1(int pin)
{
    switch (pin) {
    default:
        fprintf(stderr, "gpio: Wrong mode for this pin!\n");
        exit(-1);
    case GPIO_PIN('A',14): return 13;
    case GPIO_PIN('B',10): return 6;
    case GPIO_PIN('B',5):  return 8;
    case GPIO_PIN('B',9):  return 5;
    case GPIO_PIN('C',14): return 7;
    case GPIO_PIN('C',1):  return 10;
    case GPIO_PIN('D',14): return 11;
    case GPIO_PIN('D',2):  return 0;
    case GPIO_PIN('D',6):  return 14;
    case GPIO_PIN('F',1):  return 4;
    case GPIO_PIN('F',4):  return 2;
    case GPIO_PIN('G',1):  return 12;
    case GPIO_PIN('G',8):  return 1;
    }
}

static int pin_to_input_group2(int pin)
{
    switch (pin) {
    default:
        fprintf(stderr, "gpio: Wrong mode for this pin!\n");
        exit(-1);
    case GPIO_PIN('A',15): return 13;
    case GPIO_PIN('B',1):  return 5;
    case GPIO_PIN('B',3):  return 8;
    case GPIO_PIN('C',13): return 7;
    case GPIO_PIN('C',4):  return 10;
    case GPIO_PIN('D',11): return 3;
    case GPIO_PIN('D',3):  return 0;
    case GPIO_PIN('D',7):  return 14;
    case GPIO_PIN('E',5):  return 6;
    case GPIO_PIN('F',0):  return 4;
    case GPIO_PIN('F',5):  return 2;
    case GPIO_PIN('G',0):  return 12;
    case GPIO_PIN('G',7):  return 1;
    }
}

static int pin_to_input_group3(int pin)
{
    switch (pin) {
    default:
        fprintf(stderr, "gpio: Wrong mode for this pin!\n");
        exit(-1);
    case GPIO_PIN('B',0):  return 5;
    case GPIO_PIN('B',15): return 3;
    case GPIO_PIN('B',7):  return 7;
    case GPIO_PIN('B',8):  return 2;
    case GPIO_PIN('C',3):  return 12;
    case GPIO_PIN('D',12): return 10;
    case GPIO_PIN('D',4):  return 4;
    case GPIO_PIN('D',9):  return 0;
    case GPIO_PIN('E',3):  return 6;
    case GPIO_PIN('E',9):  return 13;
    case GPIO_PIN('F',12): return 9;
    case GPIO_PIN('F',8):  return 11;
    }
}

static int pin_to_input_group4(int pin)
{
    switch (pin) {
    default:
        fprintf(stderr, "gpio: Wrong mode for this pin!\n");
        exit(-1);
    case GPIO_PIN('B',2): return 7;
    case GPIO_PIN('B',6): return 5;
    case GPIO_PIN('C',2): return 12;
    case GPIO_PIN('D',0): return 3;
    case GPIO_PIN('D',5): return 6;
    case GPIO_PIN('E',8): return 13;
    case GPIO_PIN('F',2): return 11;
    case GPIO_PIN('F',3): return 8;
    case GPIO_PIN('G',9): return 1;
    }
}

//
// Get mode index for output group 1.
// See pic32mz-da data sheet, table 12-2 on page 266.
//
static int mode_to_output_group1(gpio_mode_t mode)
{
    switch (mode) {
    default:
        fprintf(stderr, "gpio: Wrong mode for this pin!\n");
        exit(-1);
    case MODE_C1TX:     return 15;
    case MODE_C2OUT:    return 14;
    case MODE_OC3:      return 11;
    case MODE_OC6:      return 12;
    case MODE_REFCLKO4: return 13;
    case MODE_SDO1:     return 5;
    case MODE_SDO2:     return 6;
    case MODE_SDO3:     return 7;
    case MODE_SDO5:     return 9;
    case MODE_SS6O:     return 10;
    case MODE_U3TX:     return 1;
    case MODE_U4RTS:    return 2;
    }
}

//
// Get mode index for output group 2.
// See pic32mz-da data sheet, table 12-2 on page 266.
//
static int mode_to_output_group2(gpio_mode_t mode)
{
    switch (mode) {
    default:
        fprintf(stderr, "gpio: Wrong mode for this pin!\n");
        exit(-1);
    case MODE_OC4:      return 11;
    case MODE_OC7:      return 12;
    case MODE_REFCLKO1: return 15;
    case MODE_SDO1:     return 5;
    case MODE_SDO2:     return 6;
    case MODE_SDO3:     return 7;
    case MODE_SDO4:     return 8;
    case MODE_SDO5:     return 9;
    case MODE_U1TX:     return 1;
    case MODE_U2RTS:    return 2;
    case MODE_U5TX:     return 3;
    case MODE_U6RTS:    return 4;
    }
}

//
// Get mode index for output group 3.
// See pic32mz-da data sheet, table 12-2 on page 267.
//
static int mode_to_output_group3(gpio_mode_t mode)
{
    switch (mode) {
    default:
        fprintf(stderr, "gpio: Wrong mode for this pin!\n");
        exit(-1);
    case MODE_C1OUT:    return 14;
    case MODE_OC5:      return 11;
    case MODE_OC8:      return 12;
    case MODE_REFCLKO3: return 15;
    case MODE_SDO6:     return 10;
    case MODE_SS1O:     return 5;
    case MODE_SS3O:     return 7;
    case MODE_SS4O:     return 8;
    case MODE_SS5O:     return 9;
    case MODE_U3RTS:    return 1;
    case MODE_U4TX:     return 2;
    case MODE_U6TX:     return 4;
    }
}

//
// Get mode index for output group 4.
// See pic32mz-da data sheet, table 12-2 on page 267.
//
static int mode_to_output_group4(gpio_mode_t mode)
{
    switch (mode) {
    default:
        fprintf(stderr, "gpio: Wrong mode for this pin!\n");
        exit(-1);
    case MODE_C2TX:  return 15;
    case MODE_OC1:   return 12;
    case MODE_OC2:   return 11;
    case MODE_OC9:   return 13;
    case MODE_SDO4:  return 8;
    case MODE_SDO6:  return 10;
    case MODE_SS2O:  return 6;
    case MODE_U1RTS: return 1;
    case MODE_U2TX:  return 2;
    case MODE_U5RTS: return 3;
    case MODE_U6TX:  return 4;
    }
}

//
// Set given pin to a specified mode.
//
void gpio_set_mapping(int pin, gpio_mode_t mode)
{
    //
    // Input modes.
    //
    switch (mode) {
    case MODE_C1RX:      write_sfr(C1RXR,     pin_to_input_group2(pin)); return;
    case MODE_C2RX:      write_sfr(C2RXR,     pin_to_input_group3(pin)); return;
    case MODE_IC1:       write_sfr(IC1R,      pin_to_input_group4(pin)); return;
    case MODE_IC2:       write_sfr(IC2R,      pin_to_input_group3(pin)); return;
    case MODE_IC3:       write_sfr(IC3R,      pin_to_input_group1(pin)); return;
    case MODE_IC4:       write_sfr(IC4R,      pin_to_input_group2(pin)); return;
    case MODE_IC5:       write_sfr(IC5R,      pin_to_input_group3(pin)); return;
    case MODE_IC6:       write_sfr(IC6R,      pin_to_input_group4(pin)); return;
    case MODE_IC7:       write_sfr(IC7R,      pin_to_input_group1(pin)); return;
    case MODE_IC8:       write_sfr(IC8R,      pin_to_input_group2(pin)); return;
    case MODE_IC9:       write_sfr(IC9R,      pin_to_input_group3(pin)); return;
    case MODE_INT1:      write_sfr(INT1R,     pin_to_input_group4(pin)); return;
    case MODE_INT2:      write_sfr(INT2R,     pin_to_input_group3(pin)); return;
    case MODE_INT3:      write_sfr(INT3R,     pin_to_input_group1(pin)); return;
    case MODE_INT4:      write_sfr(INT4R,     pin_to_input_group2(pin)); return;
    case MODE_OCFA:      write_sfr(OCFAR,     pin_to_input_group4(pin)); return;
    case MODE_REFCLKI1:  write_sfr(REFCLKI1R, pin_to_input_group1(pin)); return;
    case MODE_REFCLKI3:  write_sfr(REFCLKI3R, pin_to_input_group4(pin)); return;
    case MODE_REFCLKI4:  write_sfr(REFCLKI4R, pin_to_input_group2(pin)); return;
    case MODE_SDI1:      write_sfr(SDI1R,     pin_to_input_group1(pin)); return;
    case MODE_SDI2:      write_sfr(SDI2R,     pin_to_input_group2(pin)); return;
    case MODE_SDI3:      write_sfr(SDI3R,     pin_to_input_group1(pin)); return;
    case MODE_SDI4:      write_sfr(SDI4R,     pin_to_input_group2(pin)); return;
    case MODE_SDI5:      write_sfr(SDI5R,     pin_to_input_group1(pin)); return;
    case MODE_SDI6:      write_sfr(SDI6R,     pin_to_input_group4(pin)); return;
    case MODE_SS1I:      write_sfr(SS1R,      pin_to_input_group3(pin)); return;
    case MODE_SS2I:      write_sfr(SS2R,      pin_to_input_group4(pin)); return;
    case MODE_SS3I:      write_sfr(SS3R,      pin_to_input_group3(pin)); return;
    case MODE_SS4I:      write_sfr(SS4R,      pin_to_input_group3(pin)); return;
    case MODE_SS5I:      write_sfr(SS5R,      pin_to_input_group3(pin)); return;
    case MODE_SS6I:      write_sfr(SS6R,      pin_to_input_group1(pin)); return;
    case MODE_T2CK:      write_sfr(T2CKR,     pin_to_input_group1(pin)); return;
    case MODE_T3CK:      write_sfr(T3CKR,     pin_to_input_group3(pin)); return;
    case MODE_T4CK:      write_sfr(T4CKR,     pin_to_input_group4(pin)); return;
    case MODE_T5CK:      write_sfr(T5CKR,     pin_to_input_group2(pin)); return;
    case MODE_T6CK:      write_sfr(T6CKR,     pin_to_input_group1(pin)); return;
    case MODE_T7CK:      write_sfr(T7CKR,     pin_to_input_group2(pin)); return;
    case MODE_T8CK:      write_sfr(T8CKR,     pin_to_input_group3(pin)); return;
    case MODE_T9CK:      write_sfr(T9CKR,     pin_to_input_group4(pin)); return;
    case MODE_U1CTS:     write_sfr(U1CTSR,    pin_to_input_group3(pin)); return;
    case MODE_U1RX:      write_sfr(U1RXR,     pin_to_input_group1(pin)); return;
    case MODE_U2CTS:     write_sfr(U2CTSR,    pin_to_input_group1(pin)); return;
    case MODE_U2RX:      write_sfr(U2RXR,     pin_to_input_group3(pin)); return;
    case MODE_U3CTS:     write_sfr(U3CTSR,    pin_to_input_group4(pin)); return;
    case MODE_U3RX:      write_sfr(U3RXR,     pin_to_input_group2(pin)); return;
    case MODE_U4CTS:     write_sfr(U4CTSR,    pin_to_input_group2(pin)); return;
    case MODE_U4RX:      write_sfr(U4RXR,     pin_to_input_group4(pin)); return;
    case MODE_U5CTS:     write_sfr(U5CTSR,    pin_to_input_group3(pin)); return;
    case MODE_U5RX:      write_sfr(U5RXR,     pin_to_input_group1(pin)); return;
    case MODE_U6CTS:     write_sfr(U6CTSR,    pin_to_input_group1(pin)); return;
    case MODE_U6RX:      write_sfr(U6RXR,     pin_to_input_group4(pin)); return;
    default:
        break;
    }

    //
    // Output modes.
    //
    switch (pin) {
    case GPIO_PIN('A',14): write_sfr(RPA14R, mode_to_output_group1(mode)); return;
    case GPIO_PIN('A',15): write_sfr(RPA15R, mode_to_output_group2(mode)); return;
    case GPIO_PIN('B',0):  write_sfr(RPB0R,  mode_to_output_group3(mode)); return;
    case GPIO_PIN('B',10): write_sfr(RPB10R, mode_to_output_group1(mode)); return;
    case GPIO_PIN('B',15): write_sfr(RPB15R, mode_to_output_group3(mode)); return;
    case GPIO_PIN('B',1):  write_sfr(RPB1R,  mode_to_output_group2(mode)); return;
    case GPIO_PIN('B',2):  write_sfr(RPB2R,  mode_to_output_group4(mode)); return;
    case GPIO_PIN('B',3):  write_sfr(RPB3R,  mode_to_output_group2(mode)); return;
    case GPIO_PIN('B',5):  write_sfr(RPB5R,  mode_to_output_group1(mode)); return;
    case GPIO_PIN('B',6):  write_sfr(RPB6R,  mode_to_output_group4(mode)); return;
    case GPIO_PIN('B',7):  write_sfr(RPB7R,  mode_to_output_group3(mode)); return;
    case GPIO_PIN('B',8):  write_sfr(RPB8R,  mode_to_output_group3(mode)); return;
    case GPIO_PIN('B',9):  write_sfr(RPB9R,  mode_to_output_group1(mode)); return;
    case GPIO_PIN('C',1):  write_sfr(RPC1R,  mode_to_output_group1(mode)); return;
    case GPIO_PIN('C',2):  write_sfr(RPC2R,  mode_to_output_group4(mode)); return;
    case GPIO_PIN('C',3):  write_sfr(RPC3R,  mode_to_output_group3(mode)); return;
    case GPIO_PIN('C',4):  write_sfr(RPC4R,  mode_to_output_group2(mode)); return;
    case GPIO_PIN('D',0):  write_sfr(RPD0R,  mode_to_output_group4(mode)); return;
    case GPIO_PIN('D',11): write_sfr(RPD11R, mode_to_output_group2(mode)); return;
    case GPIO_PIN('D',12): write_sfr(RPD12R, mode_to_output_group3(mode)); return;
    case GPIO_PIN('D',14): write_sfr(RPD14R, mode_to_output_group1(mode)); return;
    case GPIO_PIN('D',2):  write_sfr(RPD2R,  mode_to_output_group1(mode)); return;
    case GPIO_PIN('D',3):  write_sfr(RPD3R,  mode_to_output_group2(mode)); return;
    case GPIO_PIN('D',4):  write_sfr(RPD4R,  mode_to_output_group3(mode)); return;
    case GPIO_PIN('D',5):  write_sfr(RPD5R,  mode_to_output_group4(mode)); return;
    case GPIO_PIN('D',6):  write_sfr(RPD6R,  mode_to_output_group1(mode)); return;
    case GPIO_PIN('D',7):  write_sfr(RPD7R,  mode_to_output_group2(mode)); return;
    case GPIO_PIN('D',9):  write_sfr(RPD9R,  mode_to_output_group3(mode)); return;
    case GPIO_PIN('E',3):  write_sfr(RPE3R,  mode_to_output_group3(mode)); return;
    case GPIO_PIN('E',5):  write_sfr(RPE5R,  mode_to_output_group2(mode)); return;
    case GPIO_PIN('E',8):  write_sfr(RPE8R,  mode_to_output_group4(mode)); return;
    case GPIO_PIN('E',9):  write_sfr(RPE9R,  mode_to_output_group3(mode)); return;
    case GPIO_PIN('F',0):  write_sfr(RPF0R,  mode_to_output_group2(mode)); return;
    case GPIO_PIN('F',12): write_sfr(RPF12R, mode_to_output_group3(mode)); return;
    case GPIO_PIN('F',1):  write_sfr(RPF1R,  mode_to_output_group1(mode)); return;
    case GPIO_PIN('F',2):  write_sfr(RPF2R,  mode_to_output_group4(mode)); return;
    case GPIO_PIN('F',3):  write_sfr(RPF3R,  mode_to_output_group4(mode)); return;
    case GPIO_PIN('F',4):  write_sfr(RPF4R,  mode_to_output_group1(mode)); return;
    case GPIO_PIN('F',5):  write_sfr(RPF5R,  mode_to_output_group2(mode)); return;
    case GPIO_PIN('F',8):  write_sfr(RPF8R,  mode_to_output_group3(mode)); return;
    case GPIO_PIN('G',0):  write_sfr(RPG0R,  mode_to_output_group2(mode)); return;
    case GPIO_PIN('G',1):  write_sfr(RPG1R,  mode_to_output_group1(mode)); return;
    case GPIO_PIN('G',7):  write_sfr(RPG7R,  mode_to_output_group2(mode)); return;
    case GPIO_PIN('G',8):  write_sfr(RPG8R,  mode_to_output_group1(mode)); return;
    case GPIO_PIN('G',9):  write_sfr(RPG9R,  mode_to_output_group4(mode)); return;
    default:
        break;
    }
}

static int pin_in_input_group1(int pin)
{
    switch (pin) {
    default:               return 0;
    case GPIO_PIN('A',14): return 1;
    case GPIO_PIN('B',10): return 1;
    case GPIO_PIN('B',5):  return 1;
    case GPIO_PIN('B',9):  return 1;
    case GPIO_PIN('C',14): return 1;
    case GPIO_PIN('C',1):  return 1;
    case GPIO_PIN('D',14): return 1;
    case GPIO_PIN('D',2):  return 1;
    case GPIO_PIN('D',6):  return 1;
    case GPIO_PIN('F',1):  return 1;
    case GPIO_PIN('F',4):  return 1;
    case GPIO_PIN('G',1):  return 1;
    case GPIO_PIN('G',8):  return 1;
    }
}

static int pin_in_input_group2(int pin)
{
    switch (pin) {
    default:               return 0;
    case GPIO_PIN('A',15): return 1;
    case GPIO_PIN('B',1):  return 1;
    case GPIO_PIN('B',3):  return 1;
    case GPIO_PIN('C',13): return 1;
    case GPIO_PIN('C',4):  return 1;
    case GPIO_PIN('D',11): return 1;
    case GPIO_PIN('D',3):  return 1;
    case GPIO_PIN('D',7):  return 1;
    case GPIO_PIN('E',5):  return 1;
    case GPIO_PIN('F',0):  return 1;
    case GPIO_PIN('F',5):  return 1;
    case GPIO_PIN('G',0):  return 1;
    case GPIO_PIN('G',7):  return 1;
    }
}

static int pin_in_input_group3(int pin)
{
    switch (pin) {
    default:               return 0;
    case GPIO_PIN('B',0):  return 1;
    case GPIO_PIN('B',15): return 1;
    case GPIO_PIN('B',7):  return 1;
    case GPIO_PIN('B',8):  return 1;
    case GPIO_PIN('C',3):  return 1;
    case GPIO_PIN('D',12): return 1;
    case GPIO_PIN('D',4):  return 1;
    case GPIO_PIN('D',9):  return 1;
    case GPIO_PIN('E',3):  return 1;
    case GPIO_PIN('E',9):  return 1;
    case GPIO_PIN('F',12): return 1;
    case GPIO_PIN('F',8):  return 1;
    }
}

static int pin_in_input_group4(int pin)
{
    switch (pin) {
    default:              return 0;
    case GPIO_PIN('B',2): return 1;
    case GPIO_PIN('B',6): return 1;
    case GPIO_PIN('C',2): return 1;
    case GPIO_PIN('D',0): return 1;
    case GPIO_PIN('D',5): return 1;
    case GPIO_PIN('E',8): return 1;
    case GPIO_PIN('F',2): return 1;
    case GPIO_PIN('F',3): return 1;
    case GPIO_PIN('G',9): return 1;
    }
}

//
// Check the mode belongs to output group 1.
// See pic32mz-da data sheet, table 12-2 on page 266.
//
static int mode_in_output_group1(gpio_mode_t mode)
{
    switch (mode) {
    default:            return 0;
    case MODE_C1TX:     return 1;
    case MODE_C2OUT:    return 1;
    case MODE_OC3:      return 1;
    case MODE_OC6:      return 1;
    case MODE_REFCLKO4: return 1;
    case MODE_SDO1:     return 1;
    case MODE_SDO2:     return 1;
    case MODE_SDO3:     return 1;
    case MODE_SDO5:     return 1;
    case MODE_SS6O:     return 1;
    case MODE_U3TX:     return 1;
    case MODE_U4RTS:    return 1;
    }
}

//
// Check the mode belongs to output group 2.
// See pic32mz-da data sheet, table 12-2 on page 266.
//
static int mode_in_output_group2(gpio_mode_t mode)
{
    switch (mode) {
    default:            return 0;
    case MODE_OC4:      return 1;
    case MODE_OC7:      return 1;
    case MODE_REFCLKO1: return 1;
    case MODE_SDO1:     return 1;
    case MODE_SDO2:     return 1;
    case MODE_SDO3:     return 1;
    case MODE_SDO4:     return 1;
    case MODE_SDO5:     return 1;
    case MODE_U1TX:     return 1;
    case MODE_U2RTS:    return 1;
    case MODE_U5TX:     return 1;
    case MODE_U6RTS:    return 1;
    }
}

//
// Check the mode belongs to output group 3.
// See pic32mz-da data sheet, table 12-2 on page 267.
//
static int mode_in_output_group3(gpio_mode_t mode)
{
    switch (mode) {
    default:            return 0;
    case MODE_C1OUT:    return 1;
    case MODE_OC5:      return 1;
    case MODE_OC8:      return 1;
    case MODE_REFCLKO3: return 1;
    case MODE_SDO6:     return 1;
    case MODE_SS1O:     return 1;
    case MODE_SS3O:     return 1;
    case MODE_SS4O:     return 1;
    case MODE_SS5O:     return 1;
    case MODE_U3RTS:    return 1;
    case MODE_U4TX:     return 1;
    case MODE_U6TX:     return 1;
    }
}

//
// Check the mode belongs to output group 4.
// See pic32mz-da data sheet, table 12-2 on page 267.
//
static int mode_in_output_group4(gpio_mode_t mode)
{
    switch (mode) {
    default:         return 0;
    case MODE_C2TX:  return 1;
    case MODE_OC1:   return 1;
    case MODE_OC2:   return 1;
    case MODE_OC9:   return 1;
    case MODE_SDO4:  return 1;
    case MODE_SDO6:  return 1;
    case MODE_SS2O:  return 1;
    case MODE_U1RTS: return 1;
    case MODE_U2TX:  return 1;
    case MODE_U5RTS: return 1;
    case MODE_U6TX:  return 1;
    }
}

//
// Check whether a given pin supports a specified mode.
//
int gpio_has_mapping(int pin, gpio_mode_t mode)
{
    //
    // Input modes.
    //
    switch (mode) {
    case MODE_C1RX:     return pin_in_input_group2(pin);
    case MODE_C2RX:     return pin_in_input_group3(pin);
    case MODE_IC1:      return pin_in_input_group4(pin);
    case MODE_IC2:      return pin_in_input_group3(pin);
    case MODE_IC3:      return pin_in_input_group1(pin);
    case MODE_IC4:      return pin_in_input_group2(pin);
    case MODE_IC5:      return pin_in_input_group3(pin);
    case MODE_IC6:      return pin_in_input_group4(pin);
    case MODE_IC7:      return pin_in_input_group1(pin);
    case MODE_IC8:      return pin_in_input_group2(pin);
    case MODE_IC9:      return pin_in_input_group3(pin);
    case MODE_INT1:     return pin_in_input_group4(pin);
    case MODE_INT2:     return pin_in_input_group3(pin);
    case MODE_INT3:     return pin_in_input_group1(pin);
    case MODE_INT4:     return pin_in_input_group2(pin);
    case MODE_OCFA:     return pin_in_input_group4(pin);
    case MODE_REFCLKI1: return pin_in_input_group1(pin);
    case MODE_REFCLKI3: return pin_in_input_group4(pin);
    case MODE_REFCLKI4: return pin_in_input_group2(pin);
    case MODE_SDI1:     return pin_in_input_group1(pin);
    case MODE_SDI2:     return pin_in_input_group2(pin);
    case MODE_SDI3:     return pin_in_input_group1(pin);
    case MODE_SDI4:     return pin_in_input_group2(pin);
    case MODE_SDI5:     return pin_in_input_group1(pin);
    case MODE_SDI6:     return pin_in_input_group4(pin);
    case MODE_SS1I:     return pin_in_input_group3(pin);
    case MODE_SS2I:     return pin_in_input_group4(pin);
    case MODE_SS3I:     return pin_in_input_group3(pin);
    case MODE_SS4I:     return pin_in_input_group3(pin);
    case MODE_SS5I:     return pin_in_input_group3(pin);
    case MODE_SS6I:     return pin_in_input_group1(pin);
    case MODE_T2CK:     return pin_in_input_group1(pin);
    case MODE_T3CK:     return pin_in_input_group3(pin);
    case MODE_T4CK:     return pin_in_input_group4(pin);
    case MODE_T5CK:     return pin_in_input_group2(pin);
    case MODE_T6CK:     return pin_in_input_group1(pin);
    case MODE_T7CK:     return pin_in_input_group2(pin);
    case MODE_T8CK:     return pin_in_input_group3(pin);
    case MODE_T9CK:     return pin_in_input_group4(pin);
    case MODE_U1CTS:    return pin_in_input_group3(pin);
    case MODE_U1RX:     return pin_in_input_group1(pin);
    case MODE_U2CTS:    return pin_in_input_group1(pin);
    case MODE_U2RX:     return pin_in_input_group3(pin);
    case MODE_U3CTS:    return pin_in_input_group4(pin);
    case MODE_U3RX:     return pin_in_input_group2(pin);
    case MODE_U4CTS:    return pin_in_input_group2(pin);
    case MODE_U4RX:     return pin_in_input_group4(pin);
    case MODE_U5CTS:    return pin_in_input_group3(pin);
    case MODE_U5RX:     return pin_in_input_group1(pin);
    case MODE_U6CTS:    return pin_in_input_group1(pin);
    case MODE_U6RX:     return pin_in_input_group4(pin);
    default:
        break;
    }

    //
    // Output modes.
    //
    switch (pin) {
    case GPIO_PIN('A',14): return mode_in_output_group1(mode);
    case GPIO_PIN('A',15): return mode_in_output_group2(mode);
    case GPIO_PIN('B',0):  return mode_in_output_group3(mode);
    case GPIO_PIN('B',10): return mode_in_output_group1(mode);
    case GPIO_PIN('B',15): return mode_in_output_group3(mode);
    case GPIO_PIN('B',1):  return mode_in_output_group2(mode);
    case GPIO_PIN('B',2):  return mode_in_output_group4(mode);
    case GPIO_PIN('B',3):  return mode_in_output_group2(mode);
    case GPIO_PIN('B',5):  return mode_in_output_group1(mode);
    case GPIO_PIN('B',6):  return mode_in_output_group4(mode);
    case GPIO_PIN('B',7):  return mode_in_output_group3(mode);
    case GPIO_PIN('B',8):  return mode_in_output_group3(mode);
    case GPIO_PIN('B',9):  return mode_in_output_group1(mode);
    case GPIO_PIN('C',1):  return mode_in_output_group1(mode);
    case GPIO_PIN('C',2):  return mode_in_output_group4(mode);
    case GPIO_PIN('C',3):  return mode_in_output_group3(mode);
    case GPIO_PIN('C',4):  return mode_in_output_group2(mode);
    case GPIO_PIN('D',0):  return mode_in_output_group4(mode);
    case GPIO_PIN('D',11): return mode_in_output_group2(mode);
    case GPIO_PIN('D',12): return mode_in_output_group3(mode);
    case GPIO_PIN('D',14): return mode_in_output_group1(mode);
    case GPIO_PIN('D',2):  return mode_in_output_group1(mode);
    case GPIO_PIN('D',3):  return mode_in_output_group2(mode);
    case GPIO_PIN('D',4):  return mode_in_output_group3(mode);
    case GPIO_PIN('D',5):  return mode_in_output_group4(mode);
    case GPIO_PIN('D',6):  return mode_in_output_group1(mode);
    case GPIO_PIN('D',7):  return mode_in_output_group2(mode);
    case GPIO_PIN('D',9):  return mode_in_output_group3(mode);
    case GPIO_PIN('E',3):  return mode_in_output_group3(mode);
    case GPIO_PIN('E',5):  return mode_in_output_group2(mode);
    case GPIO_PIN('E',8):  return mode_in_output_group4(mode);
    case GPIO_PIN('E',9):  return mode_in_output_group3(mode);
    case GPIO_PIN('F',0):  return mode_in_output_group2(mode);
    case GPIO_PIN('F',12): return mode_in_output_group3(mode);
    case GPIO_PIN('F',1):  return mode_in_output_group1(mode);
    case GPIO_PIN('F',2):  return mode_in_output_group4(mode);
    case GPIO_PIN('F',3):  return mode_in_output_group4(mode);
    case GPIO_PIN('F',4):  return mode_in_output_group1(mode);
    case GPIO_PIN('F',5):  return mode_in_output_group2(mode);
    case GPIO_PIN('F',8):  return mode_in_output_group3(mode);
    case GPIO_PIN('G',0):  return mode_in_output_group2(mode);
    case GPIO_PIN('G',1):  return mode_in_output_group1(mode);
    case GPIO_PIN('G',7):  return mode_in_output_group2(mode);
    case GPIO_PIN('G',8):  return mode_in_output_group1(mode);
    case GPIO_PIN('G',9):  return mode_in_output_group4(mode);
    default:
        break;
    }
    return 0;
}
