/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 *
 * Edit History
 *  Aug  3,  2011 <Lowell Scott Hanson> ported toh chipKIT boards
 *  Sept 13, 2011 <Gene Apperson> change SPI clock divider from DIV8 to DIV32
 *  Apr  16, 2012 <Serge Vakulenko> ported to RetroBSD
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/spi.h>

#include "w5100.h"

#define SPI_DEVNAME         "/dev/spi"
#define SPI_CHANNEL         2       // W5100 chip is connected to SPI2
#define SPI_KHZ             5000    // Clock speed 5 MHz
#define SPI_SELPIN          0x0404  // chipKIT board: select pin D4

#define TX_RX_MAX_BUF_SIZE  2048
#define TX_BUF              0x1100
#define RX_BUF              (TX_BUF + TX_RX_MAX_BUF_SIZE)

#define TXBUF_BASE          0x4000
#define RXBUF_BASE          0x6000

#define RST                 7       // Reset BIT

#define SMASK               0x07FF  // Tx buffer MASK
#define RMASK               0x07FF  // Rx buffer MASK

static uint16_t SBASE [MAX_SOCK_NUM];   // Tx buffer base address
static uint16_t RBASE [MAX_SOCK_NUM];   // Rx buffer base address

static int spi;                     // SPI driver descriptor

void w5100_init()
{
    int i;

    spi = open (SPI_DEVNAME, O_RDWR);
    if (spi < 0) {
        perror (SPI_DEVNAME);
        exit (-1);
    }

    ioctl (spi, SPICTL_SETCHAN, SPI_CHANNEL);
    ioctl (spi, SPICTL_SETRATE, SPI_KHZ);
    ioctl (spi, SPICTL_SETSELPIN, SPI_SELPIN);

    w5100_writeMR (1 << RST);
    w5100_writeTMSR (0x55);
    w5100_writeRMSR (0x55);

    for (i=0; i<MAX_SOCK_NUM; i++) {
        SBASE[i] = TXBUF_BASE + W5100_SSIZE * i;
        RBASE[i] = RXBUF_BASE + W5100_RSIZE * i;
    }
}

unsigned w5100_getTXFreeSize (unsigned sock)
{
    unsigned val = 0, val1 = 0;

    do {
        val1 = w5100_readSnTX_FSR (sock);
        if (val1 != 0)
            val = w5100_readSnTX_FSR (sock);
    }
    while (val != val1);
    return val;
}

unsigned w5100_getRXReceivedSize (unsigned sock)
{
    unsigned val = 0, val1 = 0;

    do {
        val1 = w5100_readSnRX_RSR (sock);
        if (val1 != 0)
            val = w5100_readSnRX_RSR (sock);
    }
    while (val != val1);
    return val;
}

void w5100_send_data_processing (unsigned sock, uint8_t *data, unsigned len)
{
    unsigned ptr = w5100_readSnTX_WR (sock);
    unsigned offset = ptr & SMASK;
    unsigned dstAddr = offset + SBASE[sock];

    if (offset + len > W5100_SSIZE) {
        // Wrap around circular buffer
        unsigned size = W5100_SSIZE - offset;
        w5100_write (dstAddr, data, size);
        w5100_write (SBASE[sock], data + size, len - size);
    } else {
        w5100_write (dstAddr, data, len);
    }

    ptr += len;
    w5100_writeSnTX_WR (sock, ptr);
}

void w5100_recv_data_processing (unsigned sock, uint8_t *data,
    unsigned len, int peek)
{
    unsigned ptr = w5100_readSnRX_RD (sock);

    /* removed unint8_t pointer cast in read_data due to pic32 pointer
     * values are 32 bit and compiler errors out due to data loss
     * from cast to 16-bit int -LSH */
    w5100_read_data (sock, ptr, data, len);

    if (! peek) {
        ptr += len;
        w5100_writeSnRX_RD (sock, ptr);
    }
}

void w5100_socket_cmd (unsigned sock, int cmd)
{
    // Send command to socket
    w5100_writeSnCR (sock, cmd);

    // Wait for command to complete
    while (w5100_readSnCR (sock))
        ;
}

/* removed unint8_t pointer cast in read_data due to pic32 pointer
 * values are 32 bit and compiler errors out due to data loss
 * from cast to 16-bit int -LSH */
void w5100_read_data (unsigned sock,  unsigned src,
    volatile uint8_t *dst, unsigned len)
{
    unsigned size;
    unsigned src_mask;
    unsigned src_ptr;

    src_mask = src & RMASK;
    src_ptr = RBASE[sock] + src_mask;

    if (src_mask + len > W5100_RSIZE)
    {
        size = W5100_RSIZE - src_mask;
        w5100_read (src_ptr, (uint8_t *) dst, size);
        dst += size;
        w5100_read (RBASE[sock], (uint8_t *) dst, len - size);
    } else
        w5100_read (src_ptr, (uint8_t *) dst, len);
}

unsigned w5100_write_byte (unsigned addr, int byte)
{
    uint8_t data[4];

    data[0] = 0xF0;
    data[1] = addr >> 8;
    data[2] = addr;
    data[3] = byte;
    ioctl (spi, SPICTL_IO32, data);
    return 1;
}

unsigned w5100_write (unsigned addr, uint8_t *buf, unsigned len)
{
    uint8_t data[4];
    unsigned i;

    for (i=0; i<len; i++) {
        data[0] = 0xF0;
        data[1] = addr >> 8;
        data[2] = addr;
        data[3] = buf[i];
        ioctl (spi, SPICTL_IO32, data);
        addr++;
    }
    return len;
}

unsigned w5100_read_byte (unsigned addr)
{
    uint8_t data[4];

    data[0] = 0x0F;
    data[1] = addr >> 8;
    data[2] = addr;
    data[3] = 0xFF;
    ioctl (spi, SPICTL_IO32, data);

    return data[3];
}

unsigned w5100_read (unsigned addr, uint8_t *buf, unsigned len)
{
    uint8_t data[4];
    unsigned i;

    for (i=0; i<len; i++) {
        data[0] = 0x0F;
        data[1] = addr >> 8;
        data[2] = addr;
        data[3] = 0xFF;
        ioctl (spi, SPICTL_IO32, data);
        addr++;
        buf[i] = data[3];
    }
    return len;
}
