/*
 * Detect the RD-5R radio.
 *
 * Copyright (C) 2018 Serge Vakulenko
 */
#include <windows.h>
#include <setupapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

//
// Find a HID device with given GUID, vendor ID and product ID.
//
HANDLE find_device(GUID *guid, int vid, int pid)
{
    // Prepare a pattern for device name matching.
    char pattern[128];
    int pattern_len;
    sprintf(pattern, "\\\\?\\usb#vid_%04x&pid_%04x", vid, pid);
    pattern_len = strlen(pattern);

    // Get access to device information.
    HDEVINFO devinfo = SetupDiGetClassDevs(guid, NULL, NULL,
        DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
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

        // Check vid/pid.
        if (strncmp(detail->DevicePath, pattern, pattern_len) != 0) {
            printf("Wrong vid/pid.\n");
            continue;
        }

        // Figure out the COM port name.
        HKEY key = SetupDiOpenDevRegKey(devinfo, &did, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        static char comname[128];
        DWORD size = sizeof(comname), type = REG_SZ;
        if (ERROR_SUCCESS != RegQueryValueEx(key, "PortName",
            NULL, &type, (LPBYTE)comname, &size)) {
            printf("Cannot find 'portname' in registry!\n");
            RegCloseKey(key);
            continue;
        }
        RegCloseKey(key);
        printf("COM port: %s\n", comname);

        // Required device found.
        return comname;
    }
    SetupDiDestroyDeviceInfoList(devinfo);
    fprintf(stderr, "Cannot find USB device %04x:%04x\n", vid, pid);
    return 0;
}

int main()
{
    static GUID guid = { 0xa5dcbf10, 0x6530, 0x11d2, { 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed } };

    // Find D868UV on USB bus.
    const char *devname = find_device(&guid, 0x28e9, 0x018a);
    if (!devname) {
        exit(-1);
    }

    printf("Serial port: %s\n", devname);
    return 0;
}
