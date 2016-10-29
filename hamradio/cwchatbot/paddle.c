/*
 * Interface to Morse paddle via homemade USB FT232R-based adapter.
 *
 * Signals:
 *  RXD - input  - paddle to left
 *  CTS - input  - paddle to right
 *  DTR - output - LED indicator
 *
 * Copyright (C) 2012 Serge Vakulenko, <serge@vak.ru>
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
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <usb.h>

#include "paddle.h"

/*
 * Identifiers of USB adapter.
 */
#define FT232R_VID      0x0403
#define FT232R_PID      0x7777  /* homemade RS232R adapter */

/*
 * Sync bit bang mode is implemented, as described in FTDI Application
 * Note AN232R-01: "Bit Bang Modes for the FT232R and FT245R".
 */
#define MASK_TXD        (1 << 0)    /* Dbus 0 (0x01) - TXD output */
#define MASK_RXD        (1 << 1)    /* Dbus 1 (0x02) - RXD input */
#define MASK_RTS        (1 << 2)    /* Dbus 2 (0x04) - /RTS output */
#define MASK_CTS        (1 << 3)    /* Dbus 3 (0x08) - /CTS input */
#define MASK_DTR        (1 << 4)    /* Dbus 4 (0x10) - /DTR output */
#define MASK_DSR        (1 << 5)    /* Dbus 5 (0x20) - /DSR input */
#define MASK_DCD        (1 << 6)    /* Dbus 6 (0x40) - /DCD input */
#define MASK_RI         (1 << 7)    /* Dbus 7 (0x80) - /RI input */

/*
 * USB endpoints.
 */
#define IN_EP           0x02
#define OUT_EP          0x81

/* Requests */
#define SIO_RESET               0 /* Reset the port */
#define SIO_MODEM_CTRL          1 /* Set the modem control register */
#define SIO_SET_FLOW_CTRL       2 /* Set flow control register */
#define SIO_SET_BAUD_RATE       3 /* Set baud rate */
#define SIO_SET_DATA            4 /* Set the data characteristics of the port */
#define SIO_POLL_MODEM_STATUS   5
#define SIO_SET_EVENT_CHAR      6
#define SIO_SET_ERROR_CHAR      7
#define SIO_SET_LATENCY_TIMER   9
#define SIO_GET_LATENCY_TIMER   10
#define SIO_SET_BITMODE         11
#define SIO_READ_PINS           12
#define SIO_READ_EEPROM         0x90
#define SIO_WRITE_EEPROM        0x91
#define SIO_ERASE_EEPROM        0x92

/* Access to FT232R via libusb. */
static usb_dev_handle *usbdev;

int paddle_debug;

/*
 * Perform sync bitbang output/input transaction.
 * Return a state of left and right input contacts.
 */
void paddle_poll (int *left_key, int *right_key)
{
    unsigned char output[2], reply[4];
    int n;

    /* Poll key signals.
     * Turn LED on. */
    output[0] = ~MASK_DTR;
    output[1] = ~MASK_DTR;

    /* Write data. */
    if (paddle_debug)
        fprintf (stderr, "usb bulk write 2 bytes: %02x-%02x\n",
            output[0], output[1]);
    n = usb_bulk_write (usbdev, IN_EP, (char*) output, 2, 1000);
    if (n != 2) {
        fprintf (stderr, "usb bulk write failed, error %d\n", n);
        exit (-1);
    }

    /* Get reply. */
    n = usb_bulk_read (usbdev, OUT_EP, (char*) reply, 4, 1000);
    if (n == 2)
        n += usb_bulk_read (usbdev, OUT_EP, (char*) reply+2, 2, 1000);
    if (n != 4) {
        fprintf (stderr, "usb bulk read failed, error = %d\n", n);
        exit (-1);
    }
    if (paddle_debug)
        fprintf (stderr, "usb bulk read 4 bytes: %02x-%02x-%02x-%02x\n",
            reply[0], reply[1], reply[2], reply[3]);

    /* Get key state. */
    *right_key = ! (reply[3] & MASK_RXD);
    *left_key  = ! (reply[3] & MASK_CTS);
}

void paddle_close (void)
{
    char reply [4];

    if (! usbdev)
        return;

    printf ("\nTerminated.\n");

    /* Turn LED off. */
    if (usb_bulk_write (usbdev, IN_EP, "\xff\xff", 2, 1000) == 2)
        usb_bulk_read (usbdev, OUT_EP, reply, 4, 1000);

    usb_release_interface (usbdev, 0);
    usb_close (usbdev);
    usbdev = 0;
}

/*
 * Open USB adapter.
 */
void paddle_open (void)
{
    struct usb_bus *bus;
    struct usb_device *dev;

    usb_init();
    usb_find_busses();
    usb_find_devices();
    for (bus = usb_get_busses(); bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            if (dev->descriptor.idVendor == FT232R_VID &&
                dev->descriptor.idProduct == FT232R_PID)
                goto found;
        }
    }
    fprintf (stderr, "USB adapter not found: vid=%04x, pid=%04x\n",
        FT232R_VID, FT232R_PID);
    exit (1);
found:
    usbdev = usb_open (dev);
    if (! usbdev) {
        fprintf (stderr, "usb_open() failed\n");
        exit (1);
    }
#if ! defined (__CYGWIN32__) && ! defined (MINGW32)
    char driver_name [100];
    if (usb_get_driver_np (usbdev, 0, driver_name, sizeof(driver_name)) == 0) {
	if (usb_detach_kernel_driver_np (usbdev, 0) < 0) {
            printf("Failed to detach the %s kernel driver.\n", driver_name);
            usb_close (usbdev);
            exit (1);
	}
    }
#endif
    usb_claim_interface (usbdev, 0);

    /* Reset the ftdi device. */
    if (usb_control_msg (usbdev,
        USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
        SIO_RESET, 0, 0, 0, 0, 1000) != 0) {
        if (errno == EPERM)
            fprintf (stderr, "Superuser privileges needed.\n");
        else
            fprintf (stderr, "FTDI reset failed\n");
failed: usb_release_interface (usbdev, 0);
        usb_close (usbdev);
        exit (1);
    }

    /* Sync bit bang mode. */
    if (usb_control_msg (usbdev,
        USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
        SIO_SET_BITMODE, MASK_TXD | MASK_RTS | MASK_DTR | 0x400,
        0, 0, 0, 1000) != 0) {
        fprintf (stderr, "Can't set sync bitbang mode\n");
        goto failed;
    }

    /* Exactly 500nsec between samples. */
    unsigned divisor = 0;
    unsigned char latency_timer = 1;
    //int baud = (divisor == 0) ? 3000000 :
    //    (divisor == 1) ? 2000000 : 3000000 / divisor;
    //fprintf (stderr, "Speed %d samples/sec\n", baud);

    /* Frequency divisor is 14-bit non-zero value. */
    if (usb_control_msg (usbdev,
        USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
        SIO_SET_BAUD_RATE, divisor,
        0, 0, 0, 1000) != 0) {
        fprintf (stderr, "Can't set baud rate\n");
        goto failed;
    }

    if (usb_control_msg (usbdev,
        USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
        SIO_SET_LATENCY_TIMER, latency_timer, 0, 0, 0, 1000) != 0) {
        fprintf (stderr, "unable to set latency timer\n");
        goto failed;
    }
    if (usb_control_msg (usbdev,
        USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
        SIO_GET_LATENCY_TIMER, 0, 0, (char*) &latency_timer, 1, 1000) != 1) {
        fprintf (stderr, "unable to get latency timer\n");
        goto failed;
    }
    //fprintf (stderr, "Latency timer: %u usec\n", latency_timer);
}
