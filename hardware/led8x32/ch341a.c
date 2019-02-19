/*
 * SPI interface via ch341 chip.
 *
 * Copyright (C) 2018 Serge Vakulenko
 * Based on ch341 sources by Pluto Yang (yangyj.ee@gmail.com).
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * verbose functionality forked from https://github.com/vSlipenchuk/ch341prog/commit/5afb03fe27b54dbcc88f6584417971d045dd8dab
 */
#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "ch341a.h"

#define DEFAULT_TIMEOUT         1000     // 1000mS for USB timeouts
#define BULK_WRITE_ENDPOINT     0x02
#define BULK_READ_ENDPOINT      0x82

#define CH341_PACKET_LENGTH     0x20
#define CH341_MAX_PACKETS       256
#define CH341_MAX_PACKET_LEN    (CH341_PACKET_LENGTH * CH341_MAX_PACKETS)
#define CH341A_USB_VENDOR       0x1A86
#define CH341A_USB_PRODUCT      0x5512

#define CH341A_CMD_SET_OUTPUT   0xA1
#define CH341A_CMD_IO_ADDR      0xA2
#define CH341A_CMD_PRINT_OUT    0xA3
#define CH341A_CMD_SPI_STREAM   0xA8
#define CH341A_CMD_SIO_STREAM   0xA9
#define CH341A_CMD_I2C_STREAM   0xAA
#define CH341A_CMD_UIO_STREAM   0xAB

#define CH341A_CMD_I2C_STM_STA  0x74
#define CH341A_CMD_I2C_STM_STO  0x75
#define CH341A_CMD_I2C_STM_OUT  0x80
#define CH341A_CMD_I2C_STM_IN   0xC0
#define CH341A_CMD_I2C_STM_MAX  (min(0x3F, CH341_PACKET_LENGTH))
#define CH341A_CMD_I2C_STM_SET  0x60
#define CH341A_CMD_I2C_STM_US   0x40
#define CH341A_CMD_I2C_STM_MS   0x50
#define CH341A_CMD_I2C_STM_DLY  0x0F
#define CH341A_CMD_I2C_STM_END  0x00

#define CH341A_CMD_UIO_STM_IN   0x00
#define CH341A_CMD_UIO_STM_DIR  0x40
#define CH341A_CMD_UIO_STM_OUT  0x80
#define CH341A_CMD_UIO_STM_US   0xC0
#define CH341A_CMD_UIO_STM_END  0x20

#define CH341A_STM_I2C_20K      0x00
#define CH341A_STM_I2C_100K     0x01
#define CH341A_STM_I2C_400K     0x02
#define CH341A_STM_I2C_750K     0x03
#define CH341A_STM_SPI_DBL      0x04

static struct libusb_device_handle *devHandle = NULL;

/*
 * Configure CH341A, find the device and set the default interface.
 */
int spi_open()
{
    struct libusb_device *dev;
    int32_t ret;
    uint8_t desc[0x12];
    const uint16_t vid = CH341A_USB_VENDOR;
    const uint16_t pid = CH341A_USB_PRODUCT;

    if (devHandle != NULL) {
        fprintf(stderr, "Call spi_close() before re-configure\n");
        return -1;
    }
    ret = libusb_init(NULL);
    if (ret < 0) {
        fprintf(stderr, "Couldn't initialise libusb\n");
        return -1;
    }

    libusb_set_debug(NULL, 3);

    devHandle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (!devHandle) {
        fprintf(stderr, "Couldn't open device [%04x:%04x].\n", vid, pid);
        return -1;
    }

    dev = libusb_get_device(devHandle);
    if (!dev) {
        fprintf(stderr, "Couldn't get bus number and address.\n");
        goto failed;
    }

    if (libusb_kernel_driver_active(devHandle, 0)) {
        ret = libusb_detach_kernel_driver(devHandle, 0);
        if (ret) {
            fprintf(stderr, "Failed to detach kernel driver: '%s'\n",
                libusb_error_name(ret));
            goto failed;
        }
    }

    ret = libusb_claim_interface(devHandle, 0);
    if (ret) {
        fprintf(stderr, "Failed to claim interface 0: '%s'\n",
            libusb_error_name(ret));
        goto failed;
    }

    ret = libusb_get_descriptor(devHandle, LIBUSB_DT_DEVICE, 0x00, desc, 0x12);
    if (ret < 0) {
        fprintf(stderr, "Failed to get device descriptor: '%s'\n",
            libusb_error_name(ret));
        libusb_release_interface(devHandle, 0);
failed:
        libusb_close(devHandle);
        devHandle = NULL;
        return -1;
    }
    //printf("CH341 revision %d.%02d\n", desc[12], desc[13]);
    return 0;
}

/*
 * Release libusb structure and ready to exit
 */
int spi_close(void)
{
    if (devHandle == NULL)
        return -1;

    libusb_release_interface(devHandle, 0);
    libusb_close(devHandle);
    libusb_exit(NULL);
    devHandle = NULL;
    return 0;
}

/*
 * Helper function for libusb_bulk_transfer,
 * display error message with the caller name
 */
static int transfer(const char *func, uint8_t type, uint8_t *buf, int len)
{
    int32_t ret;
    int transfered, retry;

    if (devHandle == NULL)
        return -1;
    for (retry = 0; retry < 3; retry++) {
        ret = libusb_bulk_transfer(devHandle, type, buf, len, &transfered, DEFAULT_TIMEOUT);
        if (ret >= 0)
            return transfered;

        if (ret != LIBUSB_ERROR_PIPE)
            break;

        // Sometimes the chip does not recognize the command, for unknown reason.
        // Need to repeat.
        usleep(10000);
    }
    fprintf(stderr, "%s: Failed to %s %d bytes '%s'\n", func,
            (type == BULK_WRITE_ENDPOINT) ? "write" : "read", len,
            libusb_error_name(ret));
    return -1;
}

/*
 * Set the i2c bus speed (speed(b1b0): 0 = 20kHz; 1 = 100kHz, 2 = 400kHz, 3 = 750kHz)
 * Set the spi bus data width(speed(b2): 0 = Single, 1 = Double)
 */
int spi_set_speed(int speed)
{
    uint8_t buf[3];

    if (devHandle == NULL)
        return -1;
    buf[0] = CH341A_CMD_I2C_STREAM;
    buf[1] = CH341A_CMD_I2C_STM_SET | (speed & 0x7);
    buf[2] = CH341A_CMD_I2C_STM_END;

    return transfer(__func__, BULK_WRITE_ENDPOINT, buf, 3);
}

/*
 * CH341 requres LSB first, swap the bit order before send and after receive.
 */
static uint8_t swapByte(uint8_t c)
{
    uint8_t result = 0;

    for (int i = 0; i < 8; ++i) {
        result = result << 1;
        result |= (c & 1);
        c = c >> 1;
    }
    return result;
}

/*
 * Assert or deassert the chip-select pin of the spi device.
 */
static void chip_select(uint8_t *ptr, bool selected)
{
    *ptr++ = CH341A_CMD_UIO_STREAM;
    *ptr++ = CH341A_CMD_UIO_STM_OUT | (selected ? 0x36 : 0x37);
    if (selected)
        *ptr++ = CH341A_CMD_UIO_STM_DIR | 0x3F; // pin direction
    *ptr++ = CH341A_CMD_UIO_STM_END;
}

/*
 * Transfer len bytes of data to the spi device.
 */
int spi_send_receive(uint8_t *out, uint8_t *in, int len)
{
    uint8_t inBuf[CH341_PACKET_LENGTH], outBuf[CH341_PACKET_LENGTH], *inPtr, *outPtr;
    int32_t ret, packetLen;
    bool done;

    if (devHandle == NULL)
        return -1;

    chip_select(outBuf, true);
    ret = transfer(__func__, BULK_WRITE_ENDPOINT, outBuf, 4);
    if (ret < 0)
        return -1;

    inPtr = in;
    do {
        done = true;
        packetLen = len+1;    // STREAM COMMAND + data length
        if (packetLen>CH341_PACKET_LENGTH) {
            packetLen = CH341_PACKET_LENGTH;
            done = false;
        }
        outPtr = outBuf;
        *outPtr++ = CH341A_CMD_SPI_STREAM;
        for (int i = 0; i < packetLen-1; ++i)
            *outPtr++ = swapByte(*out++);

        ret = transfer(__func__, BULK_WRITE_ENDPOINT, outBuf, packetLen);
        if (ret < 0)
            return -1;

        ret = transfer(__func__, BULK_READ_ENDPOINT, inBuf, packetLen-1);
        if (ret < 0)
            return -1;

        len -= ret;
        for (int i = 0; i < ret; ++i) // swap the buffer
            *inPtr++ = swapByte(inBuf[i]);
    } while (!done);

    chip_select(outBuf, false);
    ret = transfer(__func__, BULK_WRITE_ENDPOINT, outBuf, 3);
    if (ret < 0)
        return -1;
    return 0;
}

/*
 * Transfer len bytes of data to the spi device.
 */
int spi_send(uint8_t *out, int len)
{
    uint8_t inBuf[CH341_PACKET_LENGTH], outBuf[CH341_PACKET_LENGTH], *outPtr;
    int32_t ret, packetLen;
    bool done;

    if (devHandle == NULL)
        return -1;

    chip_select(outBuf, true);
    ret = transfer(__func__, BULK_WRITE_ENDPOINT, outBuf, 4);
    if (ret < 0)
        return -1;

    do {
        done = true;
        packetLen = len+1;    // STREAM COMMAND + data length
        if (packetLen>CH341_PACKET_LENGTH) {
            packetLen = CH341_PACKET_LENGTH;
            done = false;
        }
        outPtr = outBuf;
        *outPtr++ = CH341A_CMD_SPI_STREAM;
        for (int i = 0; i < packetLen-1; ++i)
            *outPtr++ = swapByte(*out++);

        ret = transfer(__func__, BULK_WRITE_ENDPOINT, outBuf, packetLen);
        if (ret < 0)
            return -1;

        ret = transfer(__func__, BULK_READ_ENDPOINT, inBuf, packetLen-1);
        if (ret < 0)
            return -1;

        len -= ret;
    } while (!done);

    chip_select(outBuf, false);
    ret = transfer(__func__, BULK_WRITE_ENDPOINT, outBuf, 3);
    if (ret < 0)
        return -1;
    return 0;
}
