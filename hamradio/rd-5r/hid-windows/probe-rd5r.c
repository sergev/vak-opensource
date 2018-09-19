/*
 * Detect the RD-5R radio.
 *
 * Copyright (C) 2018 Serge Vakulenko
 */
#include <windows.h>
#include <setupapi.h>
#include <hidsdi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

HANDLE dev;

unsigned char reply [64];

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
    DWORD reply_len;

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
    if (!WriteFile(dev, buf, SEND_LEN, NULL, NULL)) {
        fprintf(stderr, "WriteFile error %#lx!\n", GetLastError());
        exit(-1);
    }

    memset(reply, 0, sizeof(reply));

    if (!ReadFile(dev, reply, RECV_LEN, &reply_len, NULL)) {
        fprintf(stderr, "ReadFile error %#lx!\n", GetLastError());
        exit(-1);
    }

    if (reply_len == 0) {
        fprintf(stderr, "Timed out.\n");
        exit(-1);
    }
    if (reply_len != RECV_LEN) {
        fprintf(stderr, "Short received packet: %d bytes, expected %d bytes!\n",
            (int)reply_len, RECV_LEN);
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

//
// Find a HID device with given GUID, vendor ID and product ID.
//
HANDLE find_hid_device(GUID *guid, int vid, int pid)
{
    HANDLE h = INVALID_HANDLE_VALUE;

    HDEVINFO devinfo = SetupDiGetClassDevs(guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    if (devinfo == INVALID_HANDLE_VALUE) {
        printf("Cannot get devinfo!\n");
        return 0;
    }

    // Loop through available devices with a given GUID.
    int index;
    SP_INTERFACE_DEVICE_DATA iface;
    iface.cbSize = sizeof(iface);
    for (index=0; SetupDiEnumDeviceInterfaces(devinfo, NULL, guid, index, &iface); ++index) {

        // Obtain a required size of device detail structure.
        DWORD needed;
        SetupDiGetDeviceInterfaceDetail(devinfo, &iface, NULL, 0, &needed, NULL);

        // Allocate the device detail structure.
        PSP_INTERFACE_DEVICE_DETAIL_DATA detail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)alloca(needed);
        detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
        SP_DEVINFO_DATA did = { sizeof(SP_DEVINFO_DATA) };

        // Get device information.
        if (!SetupDiGetDeviceInterfaceDetail(devinfo, &iface, detail, needed, NULL, &did)) {
            printf("Device %d: cannot get path!\n", index);
            continue;
        }
        printf("Device %d: path %s\n", index, detail->DevicePath);

        h = CreateFile(detail->DevicePath, GENERIC_WRITE | GENERIC_READ,
            0, NULL, OPEN_EXISTING, 0, NULL);
        if (h == INVALID_HANDLE_VALUE) {
            continue;
        }

        // Get the Vendor ID and Product ID for this device.
        HIDD_ATTRIBUTES attrib;
        attrib.Size = sizeof(HIDD_ATTRIBUTES);
        HidD_GetAttributes(h, &attrib);
        printf("Product/Vendor: %x %x\n", attrib.ProductID, attrib.VendorID);

        // Check the VID/PID.
        if (attrib.VendorID != vid || attrib.ProductID != pid) {
            CloseHandle(h);
            h = INVALID_HANDLE_VALUE;
            continue;
        }

        // Required device found.
        break;
    }
    SetupDiDestroyDeviceInfoList(devinfo);
    return h;
}

int main()
{
    static GUID guid = { 0x4d1e55b2, 0xf16f, 0x11cf, { 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } };

    // Find HID device.
    dev = find_hid_device(&guid, FREESCALE_VID, HID_PID);
    if (dev == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Cannot find USB device %04x:%04x\n", FREESCALE_VID, HID_PID);
        exit(-1);
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
