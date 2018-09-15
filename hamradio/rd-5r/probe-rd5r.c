/*
 * Detect the RD-5R radio.
 *
 * Copyright (C) 2018 Serge Vakulenko
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "hidapi.h"

/* Device handle for libusb. */
hid_device *hiddev;

unsigned char reply [64];
int reply_len;

int debug_level = 1;

/*
 * Identifiers of USB device.
 */
#define FREESCALE_VID           0x15a2
#define HID_PID                 0x0073

/*
 * Send a request to the device.
 * Store the reply into the reply[] array.
 */
static void send_recv(const unsigned char *data, unsigned nbytes)
{
    unsigned char buf [64];
    unsigned k;

    memset(buf, 0, sizeof(buf));
    buf[0] = 1;
    buf[1] = 0;
    buf[2] = nbytes;
    buf[3] = nbytes >> 8;
    if (nbytes > 0)
        memcpy(buf+4, data, nbytes);
    nbytes += 4;

    if (debug_level > 0) {
        fprintf(stderr, "---Send");
        for (k=0; k<nbytes; ++k) {
            if (k != 0 && (k & 15) == 0)
                fprintf(stderr, "\n       ");
            fprintf(stderr, " %02x", buf[k]);
        }
        fprintf(stderr, "\n");
    }
#define SEND_LEN 42
#define RECV_LEN 42
    hid_write(hiddev, buf, SEND_LEN);

    memset(reply, 0, sizeof(reply));
    reply_len = hid_read_timeout(hiddev, reply, RECV_LEN, 4000);
    if (reply_len == 0) {
        fprintf(stderr, "Timed out.\n");
        exit(-1);
    }
    if (reply_len != RECV_LEN) {
        fprintf(stderr, "error %d receiving packet\n", reply_len);
        exit(-1);
    }
    if (debug_level > 0) {
        fprintf(stderr, "---Recv");
        for (k=0; k<reply_len; ++k) {
            if (k != 0 && (k & 15) == 0)
                fprintf(stderr, "\n       ");
            fprintf(stderr, " %02x", reply[k]);
        }
        fprintf(stderr, "\n");
    }
}

int main()
{
    hiddev = hid_open(FREESCALE_VID, HID_PID, 0);
    if (! hiddev) {
        fprintf(stderr, "Device not found: vid=%04x, pid=%04x\n", FREESCALE_VID, HID_PID);
        return 0;
    }

    static const unsigned char CMD_PRG[]  = "\2PROGRA";
    static const unsigned char CMD_PRG2[] = "M\2";
    static const unsigned char CMD_ACK    = 0x41;

    /* Read version of adapter. */
    send_recv(CMD_PRG, 7);
    if (reply[0] != CMD_ACK) {
        fprintf(stderr, "Wrong reply %#x, expected %#x\n", reply[0], CMD_ACK);
        return 0;
    }

    send_recv(CMD_PRG2, 2);

    //TODO
    return 0;
}
