/*
 * Project: MCP2221 HID Library
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2015 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/mcp2221-hid-library/
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <hidapi/hidapi.h>
#include "libmcp2221.h"

#define UNUSED(var) ((void)(var))

#define DEBUG_INFO_HID  0
#define REPORT_SIZE     MCP2221_REPORT_SIZE
#define HID_REPORT_SIZE REPORT_SIZE + 1 // + 1 for report ID, which is always 0 for MCP2221

#ifdef _WIN32
    #define LIB_EXPORT __declspec(dllexport)
#else
    #define LIB_EXPORT
#endif

#define NEW_REPORT(report) uint8_t report[REPORT_SIZE];

#if !DEBUG_INFO_HID
#define debug_printf(fmt, ...)  ((void)(0))
#define debug_puts(str)         ((void)(0))
#else
#define debug_printf(fmt, ...)  (printf(fmt, ## __VA_ARGS__))
#define debug_puts(str)         (puts(str))
#endif

typedef enum
{
    USB_CMD_STATUSSET               = 0x10,
    USB_CMD_READFLASH               = 0xB0,
    USB_CMD_WRITEFLASH              = 0xB1,
    USB_CMD_FLASHPASS               = 0xB2,
    USB_CMD_I2CWRITE                = 0x90,
    USB_CMD_I2CWRITE_REPEATSTART    = 0x92,
    USB_CMD_I2CWRITE_NOSTOP         = 0x94,
    USB_CMD_I2CREAD                 = 0x91,
    USB_CMD_I2CREAD_REPEATSTART     = 0x93,
    USB_CMD_I2CREAD_GET             = 0x40,
    USB_CMD_SETGPIO                 = 0x50,
    USB_CMD_GETGPIO                 = 0x51,
    USB_CMD_SETSRAM                 = 0x60,
    USB_CMD_GETSRAM                 = 0x61,
    USB_CMD_RESET                   = 0x70
}usb_cmd_t;

typedef enum
{
    FLASH_SECTION_CHIPSETTINGS      = 0x00,
    FLASH_SECTION_GPIOSETTINGS      = 0x01,
    FLASH_SECTION_USBMANUFACTURER   = 0x02,
    FLASH_SECTION_USBPRODUCT        = 0x03,
    FLASH_SECTION_USBSERIAL         = 0x04,
    FLASH_SECTION_FACTORYSERIAL     = 0x05,
}flash_section_t;

typedef struct device_list_t device_list_t;
struct device_list_t{
    device_list_t* next;    // Next device in list
    char* devPath;          // Device path (this is unique to each physical device)
    wchar_t* serial;        // Serial number
    int id;                 // ID
};

// Linked list of devices
static device_list_t* devList;

// Clear linked list of all devices
static void clearUsbDevList(void)
{
    device_list_t* dev = devList;
    while(dev)
    {
        free(dev->devPath);
        free(dev->serial);

        device_list_t* temp = dev;
        dev = dev->next;
        free(temp);
    }
    devList = NULL;
}

// Add a device to linked list
static void addUsbDevList(int id, struct hid_device_info* dev2)
{
    device_list_t* dev = devList;
    if(dev != NULL) // Root is defined
    {
        // Find last item
        for(;dev->next;dev = dev->next);

        // Make next item
        dev->next = malloc(sizeof(device_list_t));
        dev = dev->next;
    }
    else // No root, need to make it
    {
        // Make root item
        devList = malloc(sizeof(device_list_t));
        dev = devList;
    }

    // Assign data
    dev->next = NULL;

    // TODO use wcsdup and stuff here instead of malloc?

    // Path
    dev->devPath = malloc(strlen(dev2->path) + 1);
    strcpy(dev->devPath, dev2->path);

    // Serial
    if(!dev2->serial_number)
        dev->serial = NULL;
    else
    {
        int len = wcslen(dev2->serial_number);
        dev->serial = malloc((len * sizeof(wchar_t)) + sizeof(wchar_t));
        wcsncpy(dev->serial, dev2->serial_number, len);
        dev->serial[len] = L'\0';
    }

    dev->id = id;
}

static mcp2221_error doUSBget(hid_device* handle, void* data)
{
    if(!handle || !data)
        return MCP2221_INVALID_ARG;

    uint8_t reportData[HID_REPORT_SIZE];
    memset(reportData, 0x00, HID_REPORT_SIZE);

    // TODO
    // use hid_read_timeout or maybe use some non-blocking stuff?

    // Get the report
    int res = hid_read(handle, reportData, HID_REPORT_SIZE);

    debug_printf("---- Get Feature Report ----\n");
    debug_printf("  Len: %d\n", res);
    debug_printf("  ");
    for (int i = 0; i < res; i++)
        debug_printf("%02hhx ", reportData[i]);
    debug_puts("");

    if(res != REPORT_SIZE)
    {
        if(res < 0)
            debug_printf("ERR (get): %ls\n", hid_error(handle));
        else
            debug_printf("ERR (get): Data error, returned report length (%d) does not match requested length (%u)\n", res, HID_REPORT_SIZE);
        return MCP2221_ERROR_HID;
    }

    memcpy(data, reportData, res);

    return MCP2221_SUCCESS;
}

static mcp2221_error doUSBsend(hid_device* handle, void* data)
{
    if(!handle || !data)
        return MCP2221_INVALID_ARG;

    uint8_t reportData[HID_REPORT_SIZE];
    memcpy(reportData + 1, data, REPORT_SIZE);
    reportData[0] = 0; // Set the report ID, always 0

    // Send the report
    int res = hid_write(handle, reportData, HID_REPORT_SIZE);

    debug_printf("---- Send Feature Report ----\n");
    debug_printf("  Len: %d\n", res);
    debug_printf("  ");
    for (int i = 0; i < res; i++)
        debug_printf("%02hhx ", reportData[i]);
    debug_puts("");

    if(res < 0)
    {
        debug_printf("ERR (send): %ls\n", hid_error(handle));
        return MCP2221_ERROR_HID;
    }

    return MCP2221_SUCCESS;
}

static mcp2221_error USBget(mcp2221_t* device, void* data)
{
    if(!device)
        return MCP2221_INVALID_ARG;
    return doUSBget(device->handle, data);
}

static mcp2221_error USBsend(mcp2221_t* device, void* data)
{
    if(!device)
        return MCP2221_INVALID_ARG;
    return doUSBsend(device->handle, data);
}

static void clearReport(void* report)
{
    memset(report, 0x00, REPORT_SIZE);
}

static mcp2221_error setReport(mcp2221_t* device, uint8_t* report, uint8_t type)
{
    if(!device)
        return MCP2221_INVALID_ARG;
    clearReport(report);
    report[0] = type;
    return MCP2221_SUCCESS;
}

static mcp2221_error getResponse(mcp2221_t* device, uint8_t* report, uint8_t type)
{
    clearReport(report);
    report[0] = type;
    mcp2221_error res = USBget(device, report);
    return res;
}

static mcp2221_error doTransaction(mcp2221_t* device, uint8_t* report)
{
    uint8_t type = report[0];
    mcp2221_error res;
    if((res = USBsend(device, report)) == MCP2221_SUCCESS)
        res = getResponse(device, report, type);
    return res;
}

// Reads FLASH data for updating
static int saveReport(mcp2221_t* device, uint8_t* report)
{
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_READFLASH)) != MCP2221_SUCCESS)
        return res;
    report[1] = FLASH_SECTION_CHIPSETTINGS;
    res = doTransaction(device, report);
    return res;
}

static void saveReportUpdate(uint8_t* report, uint8_t* reportUpdate)
{
    clearReport(reportUpdate);
    memcpy(reportUpdate, &report[2], REPORT_SIZE - 2);
    reportUpdate[0] = USB_CMD_WRITEFLASH;
    reportUpdate[1] = FLASH_SECTION_CHIPSETTINGS;
}

static mcp2221_error getDescriptor(mcp2221_t* device, uint8_t* report, wchar_t* dest, flash_section_t descriptor)
{
    report[1] = descriptor;

    mcp2221_error res;
    if((res = doTransaction(device, report)) != MCP2221_SUCCESS)
        return res;

    // USB descriptors are 16-bit unicode characters and do not contain a null terminator
    // report[2] is the number of bytes + 2, which is double the number of characters + 1 extra
    int len = (report[2] / 2) - 1;

    if(len > MCP2221_STR_LEN - 1)
        len = MCP2221_STR_LEN - 1;
    else if(len < 1) // Empty string
        len = 0;

    wcsncpy(dest, (wchar_t*)&report[4], len);
    dest[len] = L'\0'; // Make sure string is null terminated

    return res;
}

static mcp2221_error setDescriptor(mcp2221_t* device, wchar_t* buffer, flash_section_t section)
{
    if(!buffer)
        return MCP2221_INVALID_ARG;

    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_WRITEFLASH)) != MCP2221_SUCCESS)
        return res;

    int len = wcslen(buffer);

    if(len > MCP2221_STR_LEN - 1)
        len = MCP2221_STR_LEN - 1;

    report[1] = section;
    report[2] = (len * 2) + 2;
    report[3] = 0x03;
    memcpy(&report[4], buffer, len * 2); // TODO change to wide char copy? we don't want to copy the null term

    res = doTransaction(device, report);
    return res;
}

// Similar to getDescriptor but a new report is created instead of using an existing one
static mcp2221_error getDescriptor2(mcp2221_t* device, wchar_t* buffer, flash_section_t section)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_READFLASH)) != MCP2221_SUCCESS)
        return res;
    report[1] = section;
    res = doTransaction(device, report);
    if(res != MCP2221_SUCCESS)
        return res;

    int len = (report[2] / 2) - 1;

    if(len > MCP2221_STR_LEN - 1)
        len = MCP2221_STR_LEN - 1;

    wcsncpy(buffer, (wchar_t*)&report[4], len);
    buffer[len] = L'\0'; // Make sure string is null terminated

    return res;
}

static mcp2221_error updateGPIOCache(mcp2221_t* device)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_GETSRAM)) != MCP2221_SUCCESS)
        return res;
    res = doTransaction(device, report);
    if(res == MCP2221_SUCCESS)
    {
        for(int i=0;i<MCP2221_GPIO_COUNT;i++)
            device->gpioCache[i] = report[22 + i];
    }
    return res;
}

static mcp2221_error getUSBInfo(mcp2221_t* device)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_READFLASH)) != MCP2221_SUCCESS)
        return res;
    else if((res = getDescriptor(device, report, device->usbInfo.manufacturer,  FLASH_SECTION_USBMANUFACTURER)) != MCP2221_SUCCESS)
        return res;
    else if((res = getDescriptor(device, report, device->usbInfo.product,       FLASH_SECTION_USBPRODUCT))      != MCP2221_SUCCESS)
        return res;
    else if((res = getDescriptor(device, report, device->usbInfo.serial,        FLASH_SECTION_USBSERIAL))       != MCP2221_SUCCESS)
        return res;

    // Factory serial
    report[1] = FLASH_SECTION_FACTORYSERIAL;
    if((res = doTransaction(device, report)) != MCP2221_SUCCESS)
        return res;
    device->usbInfo.factorySerialLen = report[2];
    if(device->usbInfo.factorySerialLen > 60)
        device->usbInfo.factorySerialLen = 60;
    memcpy(device->usbInfo.factorySerial, &report[4], report[2]);
    device->usbInfo.factorySerial[device->usbInfo.factorySerialLen] = 0x00; // Make sure we're null terminated

    // Firmware and hardware version
    setReport(device, report, USB_CMD_STATUSSET);
    if((res = doTransaction(device, report)) != MCP2221_SUCCESS)
        return res;
    device->usbInfo.hardware[0] = report[46];
    device->usbInfo.hardware[1] = report[47];
    device->usbInfo.firmware[0] = report[48];
    device->usbInfo.firmware[1] = report[49];

    // VID & PID
    setReport(device, report, USB_CMD_GETSRAM);
//  setReport(device, report, USB_CMD_READFLASH);
//  report[1] = FLASH_SECTION_CHIPSETTINGS;
    if((res = doTransaction(device, report)) != MCP2221_SUCCESS)
        return res;
    device->usbInfo.vid = report[8] | report[9]<<8;
    device->usbInfo.pid = report[10] | report[11]<<8;
    device->usbInfo.powerSource = (report[12] & 0x40) ? MCP2221_PWRSRC_SELFPOWERED : MCP2221_PWRSRC_BUSPOWERED;
    device->usbInfo.remoteWakeup = (report[12] & 0x20) ? MCP2221_WAKEUP_ENABLED : MCP2221_WAKEUP_DISABLED;
    device->usbInfo.milliamps = report[13] * 2;

    return MCP2221_SUCCESS;
}

// Open handle to device
static mcp2221_t* open(char* devPath)
{
    if(!devPath)
        return NULL;

    // Open device
    hid_device* handle = hid_open_path(devPath);
    if(!handle)
        return NULL;

    // TODO use strdup?

    // Store device info
    mcp2221_t* device = calloc(1, sizeof(mcp2221_t));
    device->handle = handle;
    device->path = malloc(strlen(devPath) + 1);
    strcpy(device->path, devPath);

    mcp2221_error res;
    if((res = updateGPIOCache(device)) != MCP2221_SUCCESS || (res = getUSBInfo(device)) != MCP2221_SUCCESS)
    {
        mcp2221_close(device);
        return NULL;
    }

    return device;
}

// Init, must be called before anything else!
mcp2221_error LIB_EXPORT mcp2221_init()
{
    clearUsbDevList();

    int res = hid_init();
    if(res < 0)
    {
        debug_puts("HIDAPI Init failed");
        return MCP2221_ERROR_HID;
    }

    return MCP2221_SUCCESS;
}

void LIB_EXPORT mcp2221_exit()
{
    clearUsbDevList();
    hid_exit();

    // TODO return errors from hid_exit
}

static int checkThing(wchar_t* val1, wchar_t* val2)
{
    if(!val2)
        return 1;
    else if(!val1 && val2)
        return 0;
    else if(wcscmp(val1, val2) == 0)
        return 1;
    return 0;
}

int LIB_EXPORT mcp2221_find(int vid, int pid, wchar_t* manufacturer, wchar_t* product, wchar_t* serial)
{
    int count = 0;

    clearUsbDevList();

    struct hid_device_info* allDevices = hid_enumerate(vid, pid);
    struct hid_device_info* currentDevice;
    currentDevice = allDevices;

    while (currentDevice)
    {
        debug_printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", currentDevice->vendor_id, currentDevice->product_id, currentDevice->path, currentDevice->serial_number ? currentDevice->serial_number : L"(NONE)");
        debug_printf("\n");
        debug_printf("  Manufacturer: %ls\n", currentDevice->manufacturer_string);
        debug_printf("  Product:      %ls\n", currentDevice->product_string);
        debug_printf("  Release:      %hx\n", currentDevice->release_number);
        debug_printf("  Interface:    %d\n",  currentDevice->interface_number);
        debug_printf("\n");

        if(
            checkThing(currentDevice->manufacturer_string, manufacturer) &&
            checkThing(currentDevice->product_string, product) &&
            checkThing(currentDevice->serial_number, serial)
        )
        {
            addUsbDevList(count, currentDevice);
            count++;
        }
        currentDevice = currentDevice->next;
    }

    hid_free_enumeration(allDevices);

    return count;
}

int LIB_EXPORT mcp2221_sameDevice(mcp2221_t* dev1, mcp2221_t* dev2)
{
    if(!dev1 || !dev2)
        return 0;
    else if(!dev1->path || !dev2->path)
        return 0;

    return strcmp(dev1->path, dev2->path) == 0;
}

// Open first MCP2221 found
mcp2221_t* LIB_EXPORT mcp2221_open()
{
    if(devList)
        return open(devList->devPath);
    return NULL;
}

mcp2221_t* LIB_EXPORT mcp2221_open_byIndex(int idx)
{
    // Find device with ID
    char* devPath = NULL;
    for(device_list_t* dev = devList; dev; dev = dev->next)
    {
        if(dev->id == idx)
        {
            devPath = dev->devPath;
            break;
        }
    }

    return open(devPath);
}

mcp2221_t* LIB_EXPORT mcp2221_open_bySerial(wchar_t* serial)
{
    if(!serial)
        return NULL;

    char* devPath = NULL;
    for(device_list_t* dev = devList; dev; dev = dev->next)
    {
        if(dev->serial && wcscmp(dev->serial, serial) == 0)
        {
            devPath = dev->devPath;
            break;
        }
    }

    return open(devPath);
}

// Close handle
void LIB_EXPORT mcp2221_close(mcp2221_t* device)
{
    if(device)
    {
        hid_close(device->handle);
        device->handle = NULL;
        free(device);
        //device = NULL; // needed? this isnt a pointer to a pointer
    }
}

mcp2221_error LIB_EXPORT mcp2221_reset(mcp2221_t* device)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_RESET)) != MCP2221_SUCCESS)
        return res;
    report[1] = 0xAB;
    report[2] = 0xCD;
    report[3] = 0xEF;
    res = doTransaction(device, report);
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_isConnected(mcp2221_t* device)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_STATUSSET)) != MCP2221_SUCCESS)
        return res;
    res = doTransaction(device, report);
    if(res != MCP2221_SUCCESS)
        return res;
    else if(report[0] != 0x10)
        return MCP2221_ERROR;
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_rawReport(mcp2221_t* device, uint8_t* report)
{
    return doTransaction(device, report);
}

mcp2221_error LIB_EXPORT mcp2221_setClockOut(mcp2221_t* device, mcp2221_clkdiv_t div, mcp2221_clkduty_t duty)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_SETSRAM)) != MCP2221_SUCCESS)
        return res;
    report[2] = 0x80 | duty | div;
    res = doTransaction(device, report);
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_getClockOut(mcp2221_t* device, mcp2221_clkdiv_t* div, mcp2221_clkduty_t* duty)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_GETSRAM)) != MCP2221_SUCCESS)
        return res;
    res = doTransaction(device, report);
    if(res == MCP2221_SUCCESS)
    {
        *div = report[5] & 0x07;
        *duty = report[5] & 0x18;
    }
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_setDAC(mcp2221_t* device, mcp2221_dac_ref_t ref, int value)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_SETSRAM)) != MCP2221_SUCCESS)
        return res;
    if(value < 0)
        value = 0;
    else if(value > MCP2221_DAC_MAX)
        value = MCP2221_DAC_MAX;
    report[3] = 0x80 | ref;
    report[4] = 0x80 | value;
    res = doTransaction(device, report);
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_getDAC(mcp2221_t* device, mcp2221_dac_ref_t* ref, int* value)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_GETSRAM)) != MCP2221_SUCCESS)
        return res;
    res = doTransaction(device, report);
    if(res == MCP2221_SUCCESS)
    {
        uint8_t temp = report[6]>>5;
        if(!(temp & 0x01) && (temp & 0x06)) // VDD is selected for ref voltage, but an internal voltage reference is still set
            temp = MCP2221_DAC_REF_VDD;
        *ref = temp;
        *value = report[6] & 0x1F;
    }
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_setADC(mcp2221_t* device, mcp2221_adc_ref_t ref)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_SETSRAM)) != MCP2221_SUCCESS)
        return res;
    report[5] = 0x80 | ref;
    res = doTransaction(device, report);
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_getADC(mcp2221_t* device, mcp2221_adc_ref_t* ref)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_GETSRAM)) != MCP2221_SUCCESS)
        return res;
    res = doTransaction(device, report);
    if(res == MCP2221_SUCCESS)
        *ref = (report[7]>>2) & 7;
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_readADC(mcp2221_t* device, int values[MCP2221_ADC_COUNT])
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_STATUSSET)) != MCP2221_SUCCESS)
        return res;
    res = doTransaction(device, report);
    if(res == MCP2221_SUCCESS)
    {
        for(int i=0;i<MCP2221_ADC_COUNT;i++)
            values[i] = (report[51 + (i * 2)]<<8) | report[50 + (i * 2)];
    }
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_setInterrupt(mcp2221_t* device, mcp2221_int_trig_t trig, int clearInt)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_SETSRAM)) != MCP2221_SUCCESS)
        return res;
    report[6] = 0x80 | 0x04 | 0x10 | trig;
    if(clearInt)
        report[6] |= 1;
    res = doTransaction(device, report);
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_getInterrupt(mcp2221_t* device, mcp2221_int_trig_t* trig)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_GETSRAM)) != MCP2221_SUCCESS)
        return res;
    res = doTransaction(device, report);
    if(res == MCP2221_SUCCESS)
        *trig = (report[7]>>5);
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_readInterrupt(mcp2221_t* device, int* state)
{
    *state = 0;
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_STATUSSET)) != MCP2221_SUCCESS)
        return res;
    res = doTransaction(device, report);
    if(res == MCP2221_SUCCESS)
        *state = report[24];
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_clearInterrupt(mcp2221_t* device)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_SETSRAM)) != MCP2221_SUCCESS)
        return res;
    report[6] = 0x81;
    res = doTransaction(device, report);
    return res;
}

mcp2221_gpioconfset_t LIB_EXPORT mcp2221_GPIOConfInit()
{
    mcp2221_gpioconfset_t confSet;
    for(int i=0;i<MCP2221_GPIO_COUNT;i++)
    {
        confSet.conf[i].gpios       = 0;
        confSet.conf[i].mode        = MCP2221_GPIO_MODE_INVALID;
        confSet.conf[i].direction   = MCP2221_GPIO_DIR_INVALID;
        confSet.conf[i].value       = MCP2221_GPIO_VALUE_INVALID;
    }
    return confSet;
}

mcp2221_error LIB_EXPORT mcp2221_setGPIOConf(mcp2221_t* device, mcp2221_gpioconfset_t* confSet)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_SETSRAM)) != MCP2221_SUCCESS)
        return res;

    report[7] = 0x80; // datasheet says this should be 1, but should actually be 0x80

    // Load current GPIO settings
    // When writing GPIO stuff to SRAM all GPIOs must be reconfigured, even if we only want to change one
    // Instead of reading from the device we store GPIO settings locally to speed things up a bit
    for(int i=0;i<MCP2221_GPIO_COUNT;i++)
        report[8 + i] = device->gpioCache[i];

    for(int i=0;i<MCP2221_GPIO_COUNT;i++)
    {
        uint8_t val = 0;

        if(confSet->conf[i].value == MCP2221_GPIO_VALUE_HIGH)
            val |= 16;

        if(confSet->conf[i].direction == MCP2221_GPIO_DIR_INPUT)
            val |= 8;

        val |= confSet->conf[i].mode;

        if(confSet->conf[i].gpios & MCP2221_GPIO0)
            report[8] = val;

        if(confSet->conf[i].gpios & MCP2221_GPIO1)
            report[9] = val;

        if(confSet->conf[i].gpios & MCP2221_GPIO2)
            report[10] = val;

        if(confSet->conf[i].gpios & MCP2221_GPIO3)
            report[11] = val;
    }

    // Save to cache
    for(int i=0;i<MCP2221_GPIO_COUNT;i++)
        device->gpioCache[i] = report[8 + i];

    res = doTransaction(device, report);
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_setGPIO(mcp2221_t* device, mcp2221_gpio_t pins, mcp2221_gpio_value_t value)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_SETGPIO)) != MCP2221_SUCCESS)
        return res;

    for(int i=0;i<MCP2221_GPIO_COUNT;i++)
    {
        if(pins & (1 << i))
        {
            int idx = (i * 4) + 2;
            report[idx] = 1;
            report[idx + 1] = value;

            // Save to cache for use in mcp2221_setGPIOConf()
            if(value == MCP2221_GPIO_VALUE_HIGH)
                device->gpioCache[i] |= 16;
            else
                device->gpioCache[i] &= ~16;
        }
    }

    res = doTransaction(device, report);
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_getGPIO(mcp2221_t* device, mcp2221_gpioconfset_t* confGet)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_GETSRAM)) != MCP2221_SUCCESS)
        return res;
    res = doTransaction(device, report);

    if(res == MCP2221_SUCCESS)
    {
        for(int i=0;i<MCP2221_GPIO_COUNT;i++)
        {
            uint8_t val = report[22 + i];
            confGet->conf[i].value = (val & 16) ? MCP2221_GPIO_VALUE_HIGH : MCP2221_GPIO_VALUE_LOW;
            confGet->conf[i].direction = (val & 8) ? MCP2221_GPIO_DIR_INPUT : MCP2221_GPIO_DIR_OUTPUT;
            confGet->conf[i].mode = val & 7;
        }
    }

    return res;
}

mcp2221_error LIB_EXPORT mcp2221_readGPIO(mcp2221_t* device, mcp2221_gpio_value_t values[MCP2221_GPIO_COUNT])
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_GETGPIO)) != MCP2221_SUCCESS)
        return res;
    res = doTransaction(device, report);
    if(res == MCP2221_SUCCESS)
    {
        for(int i=0;i<MCP2221_GPIO_COUNT;i++)
            values[i] = report[(i * 2) + 2];
    }
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_saveManufacturer(mcp2221_t* device, wchar_t* buffer)
{
    return setDescriptor(device, buffer, FLASH_SECTION_USBMANUFACTURER);
}

mcp2221_error LIB_EXPORT mcp2221_saveProduct(mcp2221_t* device, wchar_t* buffer)
{
    return setDescriptor(device, buffer, FLASH_SECTION_USBPRODUCT);
}

mcp2221_error LIB_EXPORT mcp2221_saveSerial(mcp2221_t* device, wchar_t* buffer)
{
    return setDescriptor(device, buffer, FLASH_SECTION_USBSERIAL);
}

mcp2221_error LIB_EXPORT mcp2221_saveVIDPID(mcp2221_t* device, int vid, int pid)
{
    if(vid < 1 || pid < 1 || vid > USHRT_MAX || pid > USHRT_MAX)
        return MCP2221_INVALID_ARG;
/*
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_READFLASH)) != MCP2221_SUCCESS)
        return res;
    report[1] = FLASH_SECTION_CHIPSETTINGS;
    res = doTransaction(device, report);
    if(res != MCP2221_SUCCESS)
        return res;
*/
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;

    if(vid != (report[8] | (report[9]<<8)) || pid != (report[10] | (report[11]<<8)))
    {
        NEW_REPORT(reportUpdate);
        clearReport(reportUpdate);
        memcpy(reportUpdate, &report[2], REPORT_SIZE - 2);
        reportUpdate[0] = USB_CMD_WRITEFLASH;
        reportUpdate[1] = FLASH_SECTION_CHIPSETTINGS;
        reportUpdate[6] = vid;
        reportUpdate[7] = vid>>8;
        reportUpdate[8] = pid;
        reportUpdate[9] = pid>>8;
        res = doTransaction(device, reportUpdate);
    }

    return res;
}

mcp2221_error LIB_EXPORT mcp2221_saveSerialEnumerate(mcp2221_t* device, int enumerate)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;

    uint8_t val = report[4] & ~0x80;
    val |= (!!enumerate)<<7;

    if(val != report[4])
    {
        NEW_REPORT(reportUpdate);
        saveReportUpdate(report, reportUpdate);
        reportUpdate[2] = val;
        res = doTransaction(device, reportUpdate);
    }

    return res;
}

mcp2221_error LIB_EXPORT mcp2221_saveMilliamps(mcp2221_t* device, int milliamps)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;

    if(milliamps < 2)
        milliamps = 2;
    else if(milliamps > 500)
        milliamps = 500;

    milliamps /= 2;

    if(milliamps != report[13])
    {
        NEW_REPORT(reportUpdate);
        saveReportUpdate(report, reportUpdate);
        reportUpdate[11] = milliamps;
        res = doTransaction(device, reportUpdate);
    }

    return res;
}

mcp2221_error LIB_EXPORT mcp2221_savePowerSource(mcp2221_t* device, mcp2221_pwrsrc_t source)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;

    uint8_t val = (report[12] & ~0x40) | source;

    if(val != report[13])
    {
        NEW_REPORT(reportUpdate);
        saveReportUpdate(report, reportUpdate);
        reportUpdate[10] = val; // REG IS SHARED WITH BOTH POWER SOURCE AND REMOTE WAKEUP
        res = doTransaction(device, reportUpdate);
    }

    return res;
}

mcp2221_error LIB_EXPORT mcp2221_saveRemoteWakeup(mcp2221_t* device, mcp2221_wakeup_t wakeup)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;

    uint8_t val = (report[12] & ~0x20) | wakeup;

    if(val != report[13])
    {
        NEW_REPORT(reportUpdate);
        saveReportUpdate(report, reportUpdate);
        reportUpdate[10] = val; // REG IS SHARED WITH BOTH POWER SOURCE AND REMOTE WAKEUP
        res = doTransaction(device, reportUpdate);
    }

    return res;
}

mcp2221_error LIB_EXPORT mcp2221_savePolarity(mcp2221_t* device, mcp2221_dedipin_t pin, int polarity)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;

    uint8_t val = report[4] & ~(1<<pin);
    val |= (!!polarity)<<pin;

    if(val != report[4])
    {
        NEW_REPORT(reportUpdate);
        saveReportUpdate(report, reportUpdate);
        reportUpdate[2] = val;
        res = doTransaction(device, reportUpdate);
    }

    return res;
}

mcp2221_error LIB_EXPORT mcp2221_saveClockOut(mcp2221_t* device, mcp2221_clkdiv_t clkdiv, mcp2221_clkduty_t duty)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;

    if(clkdiv != (report[5] & 0x07) || duty != (report[5] & 0x18))
    {
        NEW_REPORT(reportUpdate);
        saveReportUpdate(report, reportUpdate);
        reportUpdate[3] = clkdiv | duty;
        res = doTransaction(device, reportUpdate);
    }

    return res;
}

mcp2221_error LIB_EXPORT mcp2221_saveDAC(mcp2221_t* device, mcp2221_dac_ref_t ref, int value)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;

    if(value < 0)
        value = 0;
    else if(value > MCP2221_DAC_MAX)
        value = MCP2221_DAC_MAX;

    ref <<= 5;

    if(ref != (report[6] & 0xE0) || value != (report[6] & 0x1F))
    {
        NEW_REPORT(reportUpdate);
        saveReportUpdate(report, reportUpdate);
        reportUpdate[4] = ref | value;
        res = doTransaction(device, reportUpdate);
    }

    return res;
}

mcp2221_error LIB_EXPORT mcp2221_saveADC(mcp2221_t* device, mcp2221_adc_ref_t ref)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;

    ref <<= 2;

    if(ref != (report[7] & 0x1C))
    {
        NEW_REPORT(reportUpdate);
        saveReportUpdate(report, reportUpdate);
        reportUpdate[5] = (reportUpdate[5] & 0x60) | ref; // this is shared with both ADC and interrupt stuff!
        res = doTransaction(device, reportUpdate);
    }

    return res;
}

mcp2221_error LIB_EXPORT mcp2221_saveInterrupt(mcp2221_t* device, mcp2221_int_trig_t trig)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;

    uint8_t trigVal = 0;
    switch(trig)
    {
        case MCP2221_INT_TRIG_RISING:
            trigVal |= 0x20;
            break;
        case MCP2221_INT_TRIG_FALLING:
            trigVal |= 0x40;
            break;
        case MCP2221_INT_TRIG_BOTH:
            trigVal |= 0x60;
            break;
        default:
            return MCP2221_INVALID_ARG;
    }

    if(trigVal != (report[7] & 0x60))
    {
        NEW_REPORT(reportUpdate);
        saveReportUpdate(report, reportUpdate);
        reportUpdate[5] = (reportUpdate[5] & 0x9F) | trigVal; // this is shared with both ADC and interrupt stuff!
        res = doTransaction(device, reportUpdate);
    }

    return res;
}

mcp2221_error LIB_EXPORT mcp2221_saveGPIOConf(mcp2221_t* device, mcp2221_gpioconfset_t* confSet)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_READFLASH)) != MCP2221_SUCCESS)
        return res;
    report[1] = FLASH_SECTION_GPIOSETTINGS;
    res = doTransaction(device, report);
    if(res != MCP2221_SUCCESS)
        return res;

    NEW_REPORT(reportUpdate);
    clearReport(reportUpdate);
    memcpy(reportUpdate, &report[2], REPORT_SIZE - 2);
    reportUpdate[0] = USB_CMD_WRITEFLASH;
    reportUpdate[1] = FLASH_SECTION_GPIOSETTINGS;

    for(int i=0;i<MCP2221_GPIO_COUNT;i++)
    {
        uint8_t val = 0;

        if(confSet->conf[i].value == MCP2221_GPIO_VALUE_HIGH)
            val |= 16;

        if(confSet->conf[i].direction == MCP2221_GPIO_DIR_INPUT)
            val |= 8;

        val |= confSet->conf[i].mode;

        if(confSet->conf[i].gpios & MCP2221_GPIO0)
            reportUpdate[2] = val;

        if(confSet->conf[i].gpios & MCP2221_GPIO1)
            reportUpdate[3] = val;

        if(confSet->conf[i].gpios & MCP2221_GPIO2)
            reportUpdate[4] = val;

        if(confSet->conf[i].gpios & MCP2221_GPIO3)
            reportUpdate[5] = val;
    }

    if(memcmp(&report[4], &reportUpdate[2], 4) != 0) // Only update if something is different
        res = doTransaction(device, reportUpdate);

    return res;
}

mcp2221_error LIB_EXPORT mcp2221_loadManufacturer(mcp2221_t* device, wchar_t* buffer)
{
    return getDescriptor2(device, buffer, FLASH_SECTION_USBMANUFACTURER);
}

mcp2221_error LIB_EXPORT mcp2221_loadProduct(mcp2221_t* device, wchar_t* buffer)
{
    return getDescriptor2(device, buffer, FLASH_SECTION_USBPRODUCT);
}

mcp2221_error LIB_EXPORT mcp2221_loadSerial(mcp2221_t* device, wchar_t* buffer)
{
    return getDescriptor2(device, buffer, FLASH_SECTION_USBSERIAL);
}

mcp2221_error mcp2221_loadVIDPID(mcp2221_t* device, int* vid, int* pid)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;
    *vid = (report[8] | (report[9]<<8));
    *pid = (report[10] | (report[11]<<8));
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_loadSerialEnumerate(mcp2221_t* device, int* enumerate)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;
    *enumerate = report[4]>>7;
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_loadMilliamps(mcp2221_t* device, int* milliamps)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;
    *milliamps = report[13] * 2;
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_loadPowerSource(mcp2221_t* device, mcp2221_pwrsrc_t* source)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;
    *source = (report[12] & 0x40) ? MCP2221_PWRSRC_SELFPOWERED : MCP2221_PWRSRC_BUSPOWERED;
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_loadRemoteWakeup(mcp2221_t* device, mcp2221_wakeup_t* wakeup)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;
    *wakeup = (report[12] & 0x20) ? MCP2221_WAKEUP_ENABLED : MCP2221_WAKEUP_DISABLED;
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_loadPolarity(mcp2221_t* device, mcp2221_dedipin_t pin, int* polarity)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;
    *polarity = !!(report[4] & (1<<pin));
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_loadClockOut(mcp2221_t* device, mcp2221_clkdiv_t* div, mcp2221_clkduty_t* duty)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;
    *div = (report[5] & 0x07);
    *duty = (report[5] & 0x18);
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_loadDAC(mcp2221_t* device, mcp2221_dac_ref_t* ref, int* value)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;
    uint8_t temp = (report[6] & 0xE0)>>5;
    if(!(temp & 0x01) && (temp & 0x06)) // VDD is selected for ref voltage, but an internal voltage reference is still set
        temp = MCP2221_DAC_REF_VDD;
    *ref = temp;
    *value = (report[6] & 0x1F);
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_loadADC(mcp2221_t* device, mcp2221_adc_ref_t* ref)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;
    *ref = (report[7] & 0x1C)>>2;
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_loadInterrupt(mcp2221_t* device, mcp2221_int_trig_t* trig)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = saveReport(device, report)) != MCP2221_SUCCESS)
        return res;
    if(report[7] & 0x60)
        *trig = MCP2221_INT_TRIG_BOTH;
    else if(report[7] & 0x40)
        *trig = MCP2221_INT_TRIG_FALLING;
    else if(report[7] & 0x20)
        *trig = MCP2221_INT_TRIG_RISING;
    else
        *trig = MCP2221_INT_TRIG_INVALID;
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_loadGPIOConf(mcp2221_t* device, mcp2221_gpioconfset_t* confGet)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_READFLASH)) != MCP2221_SUCCESS)
        return res;
    report[1] = FLASH_SECTION_GPIOSETTINGS;
    res = doTransaction(device, report);

    if(res == MCP2221_SUCCESS)
    {
        for(int i=0;i<MCP2221_GPIO_COUNT;i++)
        {
            uint8_t val = report[4 + i];
            confGet->conf[i].value = (val & 16) ? MCP2221_GPIO_VALUE_HIGH : MCP2221_GPIO_VALUE_LOW;
            confGet->conf[i].direction = (val & 8) ? MCP2221_GPIO_DIR_INPUT : MCP2221_GPIO_DIR_OUTPUT;
            confGet->conf[i].mode = val & 7;
        }
    }

    return res;
}

mcp2221_error LIB_EXPORT mcp2221_i2cWrite(mcp2221_t* device, int address, void* data, int len, mcp2221_i2crw_t type)
{
    address <<= 1;

    // TODO: support len >= 60
    if(len > 60)
        len = 60;

    usb_cmd_t cmd;
    switch(type)
    {
        case MCP2221_I2CRW_NORMAL:
            cmd = USB_CMD_I2CWRITE;
            break;
        case MCP2221_I2CRW_REPEATED:
            cmd = USB_CMD_I2CWRITE_REPEATSTART;
            break;
        case MCP2221_I2CRW_NOSTOP:
            cmd = USB_CMD_I2CWRITE_NOSTOP;
            break;
        default:
            cmd = USB_CMD_I2CWRITE;
            break;
    }

    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, cmd)) != MCP2221_SUCCESS)
        return res;
    report[1] = len;
    report[2] = len>>8;
    report[3] = address;
    memcpy(&report[4], data, len);
    res = doTransaction(device, report);
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_i2cRead(mcp2221_t* device, int address, int len, mcp2221_i2crw_t type)
{
    address <<= 1;

    // TODO: support len >= 60
    if(len > 60)
        len = 60;

    usb_cmd_t cmd;
    switch(type)
    {
        case MCP2221_I2CRW_NORMAL:
            cmd = USB_CMD_I2CREAD;
            break;
        case MCP2221_I2CRW_REPEATED:
            cmd = USB_CMD_I2CREAD_REPEATSTART;
            break;
        default:
            cmd = USB_CMD_I2CREAD;
            break;
    }

    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, cmd)) != MCP2221_SUCCESS)
        return res;
    report[1] = len;
    report[2] = len>>8;
    report[3] = address;
    res = doTransaction(device, report);
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_i2cGet(mcp2221_t* device, void* data, int len)
{
    // TODO: support len >= 60
    if(len > 60)
        len = 60;

    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_I2CREAD_GET)) != MCP2221_SUCCESS)
        return res;
    report[1] = len;
    report[2] = len>>8;
    res = doTransaction(device, report);
    if(res == MCP2221_SUCCESS)
        memcpy(data, &report[4], len);
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_i2cCancel(mcp2221_t* device)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_STATUSSET)) != MCP2221_SUCCESS)
        return res;
    report[2] = 0x10;
    res = doTransaction(device, report);
    // TODO check response
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_i2cState(mcp2221_t* device, mcp2221_i2c_state_t* state)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_STATUSSET)) != MCP2221_SUCCESS)
        return res;
    res = doTransaction(device, report);
    if(res == MCP2221_SUCCESS)
        *state = report[8];
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_i2cDivider(mcp2221_t* device, int i2cdiv)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_STATUSSET)) != MCP2221_SUCCESS)
        return res;
    report[3] = 0x20;
    report[4] = i2cdiv;
    res = doTransaction(device, report);
    // TODO check response
    return res;
}

mcp2221_error LIB_EXPORT mcp2221_i2cReadPins(mcp2221_t* device, mcp2221_i2cpins_t* pins)
{
    NEW_REPORT(report);
    mcp2221_error res;
    if((res = setReport(device, report, USB_CMD_STATUSSET)) != MCP2221_SUCCESS)
        return res;
    res = doTransaction(device, report);
    if(res == MCP2221_SUCCESS)
    {
        pins->SCL = report[22];
        pins->SDA = report[23];
    }
    return res;
}
