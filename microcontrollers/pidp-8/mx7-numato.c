/*
 * Numato PIC32 MX development board.
 *
 * Copyright (C) 2016 Serge Vakulenko, <serge@vak.ru>
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */
#include "pidp8.h"
#include "pic32mx.h"
#include "usb-device.h"
#include "usb-function-hid.h"

#define MHZ     80              /* CPU clock is 80 MHz. */

#define PIN(n)  (1 << (n))

/*
 * HID packet data.
 */
static unsigned char send [PACKET_SIZE];           // 64-byte send buffer (EP1 IN to the PC)
static unsigned char receive_buffer [PACKET_SIZE]; // 64-byte receive buffer (EP1 OUT from the PC)
static unsigned char receive [PACKET_SIZE];        // copy of receive buffer

static USB_HANDLE request_handle;
static USB_HANDLE reply_handle;
static int packet_received;

/*
 * Main entry point at bd003000.
 * Setup stack pointer and $gp registers, and jump to main().
 */
asm ("          .section .exception");
asm ("          .globl _start");
asm ("          .type _start, function");
asm ("_start:   la      $sp, _estack");
asm ("          la      $ra, main");
asm ("          la      $gp, _gp");
asm ("          jr      $ra");
asm ("          .previous");

/*
 * Secondary entry point at bd004000.
 */
asm ("          .section .startup");
asm ("          .globl _init");
asm ("          .type _init, function");
asm ("_init:    la      $ra, _start");
asm ("          jr      $ra");
asm ("          .previous");

/*
 * Delay for a given number of microseconds.
 * The processor has a 32-bit hardware Count register,
 * which increments at half CPU rate.
 * We use it to get a precise delay.
 */
void udelay (unsigned usec)
{
    unsigned now = mfc0 (C0_COUNT, 0);
    unsigned final = now + usec * MHZ / 2;

    for (;;) {
        now = mfc0 (C0_COUNT, 0);

        /* This comparison is valid only when using a signed type. */
        if ((int) (now - final) >= 0)
            break;
    }
}

/*
 * Copy an array.
 */
void memcopy (void *from, void *to, unsigned nbytes)
{
    unsigned char *src = (unsigned char*) from;
    unsigned char *dst = (unsigned char*) to;

    while (nbytes-- > 0)
        *dst++ = *src++;
}

/*
 * Poll the USB port.
 */
static void poll_usb()
{
    // Check bus status and service USB interrupts.
    usb_device_tasks();
    if (usb_device_state < CONFIGURED_STATE || usb_is_device_suspended())
        return;

    // Are we done sending the last response.  We need to be before we
    // receive the next command because we clear the send buffer
    // once we receive a command
    if (reply_handle != 0) {
        if (usb_handle_busy (reply_handle))
            return;
        reply_handle = 0;
    }

    if (packet_received) {
        pidp_request(receive, send);
        reply_handle = usb_transfer_one_packet (HID_EP,
            IN_TO_HOST, &send[0], PACKET_SIZE);
        packet_received = 0;
        return;
    }

    // Did we receive a command?
    if (usb_handle_busy (request_handle))
        return;

    // Make a copy of received data.
    memcopy (receive_buffer, receive, PACKET_SIZE);
    packet_received = 1;

    // Restart receiver, to be ready for a next packet.
    request_handle = usb_transfer_one_packet (HID_EP, OUT_FROM_HOST,
        (unsigned char*) receive_buffer, PACKET_SIZE);
}

int main()
{
    /* Set memory wait states, for speedup. */
    CHECON = 2;
    BMXCONCLR = 0x40;
    CHECONSET = 0x30;

    /* Enable cache for kseg0 segment. */
    int config = mfc0 (C0_CONFIG, 0);
    mtc0 (C0_CONFIG, 0, config | 3);

    /* Initialize coprocessor 0. */
    mtc0 (C0_COUNT, 0, 0);
    mtc0 (C0_COMPARE, 0, -1);
    mtc0 (C0_EBASE, 1, 0x9fc00000);     /* Vector base */
    mtc0 (C0_INTCTL, 1, 1 << 5);        /* Vector spacing 32 bytes */
    mtc0 (C0_CAUSE, 0, 1 << 23);        /* Set IV */
    mtc0 (C0_STATUS, 0, 0);             /* Clear BEV */

    /* Use all ports as digital. */
    AD1PCFG = ~0;
    LATB = 0;
    LATC = 0;
    LATD = 0;
    LATE = 0;
    LATF = 0;
    LATG = 0;

    pidp_init();

    // Initialize USB module SFRs and firmware variables to known states.
    // USB port uses pins RG2 and RG3.
    TRISGSET = PIN(2) | PIN(3);
    usb_device_init();

    for (;;) {
        poll_usb();

        // Simulate one cycle.
        pidp_step();
    }
}

/*
 * USB Callback Functions
 */
/*
 * Process device-specific SETUP requests.
 */
void usbcb_check_other_req()
{
    hid_check_request();
}

/*
 * This function is called when the device becomes initialized.
 * It should initialize the endpoints for the device's usage
 * according to the current configuration.
 */
void usbcb_init_ep()
{
    // Enable the HID endpoint
    usb_enable_endpoint (HID_EP, USB_IN_ENABLED | USB_OUT_ENABLED |
        USB_HANDSHAKE_ENABLED | USB_DISALLOW_SETUP);

    // Arm the OUT endpoint for the first packet
    request_handle = usb_rx_one_packet (HID_EP,
        &receive_buffer[0], PACKET_SIZE);
}

/*
 * The device descriptor.
 */
const USB_DEVICE_DESCRIPTOR usb_device = {
    sizeof (usb_device),    // Size of this descriptor in bytes
    USB_DESCRIPTOR_DEVICE,  // DEVICE descriptor type
    0x0200,                 // USB Spec Release Number in BCD format
    0x00,                   // Class Code
    0x00,                   // Subclass code
    0x00,                   // Protocol code
    USB_EP0_BUFF_SIZE,      // Max packet size for EP0
    0x2dec,                 // Fake Vendor ID: DEC
    0xbdb8,                 // Product ID: PDP-8
    0x0101,                 // Device release number in BCD format
    1,                      // Manufacturer string index
    2,                      // Product string index
    0,                      // Device serial number string index
    1,                      // Number of possible configurations
};

/*
 * Configuration 1 descriptor
 */
const unsigned char usb_config1_descriptor[] = {
    /*
     * Configuration descriptor
     */
    sizeof (USB_CONFIGURATION_DESCRIPTOR),
    USB_DESCRIPTOR_CONFIGURATION,
    0x29, 0x00,             // Total length of data for this cfg
    1,                      // Number of interfaces in this cfg
    1,                      // Index value of this configuration
    0,                      // Configuration string index
    _DEFAULT | _SELF,       // Attributes
    500 / 2,                // Max power consumption (2X mA)

    /*
     * Interface descriptor
     */
    sizeof (USB_INTERFACE_DESCRIPTOR),
    USB_DESCRIPTOR_INTERFACE,
    0,                      // Interface Number
    0,                      // Alternate Setting Number
    2,                      // Number of endpoints in this intf
    HID_INTF,               // Class code
    0,                      // Subclass code
    0,                      // Protocol code
    0,                      // Interface string index

    /*
     * HID Class-Specific descriptor
     */
    sizeof (USB_HID_DSC) + 3,
    DSC_HID,
    0x11, 0x01,             // HID Spec Release Number in BCD format (1.11)
    0x00,                   // Country Code (0x00 for Not supported)
    HID_NUM_OF_DSC,         // Number of class descriptors
    DSC_RPT,                // Report descriptor type
    HID_RPT01_SIZE, 0x00,   // Size of the report descriptor

    /*
     * Endpoint descriptor
     */
    sizeof (USB_ENDPOINT_DESCRIPTOR),
    USB_DESCRIPTOR_ENDPOINT,
    HID_EP | _EP_IN,        // EndpointAddress
    _INTERRUPT,             // Attributes
    PACKET_SIZE, 0,         // Size
    1,                      // Interval

    /*
     * Endpoint descriptor
     */
    sizeof (USB_ENDPOINT_DESCRIPTOR),
    USB_DESCRIPTOR_ENDPOINT,
    HID_EP | _EP_OUT,       // EndpointAddress
    _INTERRUPT,             // Attributes
    PACKET_SIZE, 0,         // Size
    1,                      // Interval
};

/*
 * Class specific descriptor - HID
 */
const unsigned char hid_rpt01 [HID_RPT01_SIZE] = {
    0x06, 0x00, 0xFF,       // Usage Page = 0xFF00 (Vendor Defined Page 1)
    0x09, 0x01,             // Usage (Vendor Usage 1)
    0xA1, 0x01,             // Collection (Application)

    0x19, 0x01,             // Usage Minimum
    0x29, 0x40,             // Usage Maximum 64 input usages total (0x01 to 0x40)
    0x15, 0x00,             // Logical Minimum (data bytes in the report may have minimum value = 0x00)
    0x26, 0xFF, 0x00,       // Logical Maximum (data bytes in the report may have maximum value = 0x00FF = unsigned 255)
    0x75, 0x08,             // Report Size: 8-bit field size
    0x95, 0x40,             // Report Count: Make sixty-four 8-bit fields (the next time the parser hits an "Input", "Output", or "Feature" item)
    0x81, 0x00,             // Input (Data, Array, Abs): Instantiates input packet fields based on the above report size, count, logical min/max, and usage.

    0x19, 0x01,             // Usage Minimum
    0x29, 0x40,             // Usage Maximum 64 output usages total (0x01 to 0x40)
    0x91, 0x00,             // Output (Data, Array, Abs): Instantiates output packet fields.  Uses same report size and count as "Input" fields, since nothing new/different was specified to the parser since the "Input" item.

    0xC0,                   // End Collection
};

/*
 * USB Strings
 */
static const USB_STRING_INIT(1) string0_descriptor = {
    sizeof(string0_descriptor),
    USB_DESCRIPTOR_STRING,              /* Language code */
    { 0x0409 },
};

static const USB_STRING_INIT(29) string1_descriptor = {
    sizeof(string1_descriptor),
    USB_DESCRIPTOR_STRING,              /* Manufacturer */
    { 'S','e','r','g','e','y',' ','V','a','k',
      'u','l','e','n','k','o',' ','s','e','r',
      'g','e','@','v','a','k','.','r','u' },
};

static const USB_STRING_INIT(14) string2_descriptor = {
    sizeof(string2_descriptor),
    USB_DESCRIPTOR_STRING,              /* Product */
    { 'P','i','D','P','-','8',' ','R','e','p',
      'l','i','c','a' },
};

/*
 * Array of configuration descriptors
 */
const unsigned char *const usb_config[] = {
    (const unsigned char *const) &usb_config1_descriptor,
};

/*
 * Array of string descriptors
 */
const unsigned char *const usb_string[USB_NUM_STRING_DESCRIPTORS] = {
    (const unsigned char *const) &string0_descriptor,
    (const unsigned char *const) &string1_descriptor,
    (const unsigned char *const) &string2_descriptor,
};
