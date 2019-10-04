/*
 * USB HID demo for PIC32 microcontroller.
 *
 * Copyright (C) 2013 Serge Vakulenko, <serge@vak.ru>.
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
#include "usb_device.h"
#include "usb_function_hid.h"
#include "pic32mx.h"

/*
 * HID packet structure.
 */
#define	PACKET_SIZE	    64      // HID packet size
#define REQUEST_SIZE        56      // Number of bytes of a standard request

typedef unsigned char packet_t [PACKET_SIZE];

static packet_t send;           // 64-byte send buffer (EP1 IN to the PC)
static packet_t receive_buffer; // 64-byte receive buffer (EP1 OUT from the PC)
static packet_t receive;        // copy of receive buffer, for processing

static USB_HANDLE request_handle;

/*
 * GPIO pin control.
 */
#define TRIS_VAL(p)     (&p)[0]
#define TRIS_CLR(p)     (&p)[1]
#define TRIS_SET(p)     (&p)[2]
#define TRIS_INV(p)     (&p)[3]
#define PORT_VAL(p)     (&p)[4]
#define PORT_CLR(p)     (&p)[5]
#define PORT_SET(p)     (&p)[6]
#define PORT_INV(p)     (&p)[7]
#define LAT_VAL(p)      (&p)[8]
#define LAT_CLR(p)      (&p)[9]
#define LAT_SET(p)      (&p)[10]
#define LAT_INV(p)      (&p)[11]

/*
 * Chip configuration.
 */
PIC32_DEVCFG (
    DEVCFG0_DEBUG_DISABLED |    /* ICE debugger disabled */
    DEVCFG0_JTAGDIS,            /* Disable JTAG port */
#if 0
    // External crystal 16 MHz.
    DEVCFG1_FNOSC_PRIPLL |      /* Primary oscillator with PLL */
    DEVCFG1_POSCMOD_HS |        /* HS oscillator */
    DEVCFG1_FPBDIV_4 |          /* Peripheral bus clock = SYSCLK/4 */
    DEVCFG1_OSCIOFNC_OFF |      /* CLKO output disabled */
    DEVCFG1_FCKM_DISABLE,       /* Fail-safe clock monitor disable */

    DEVCFG2_FPLLIDIV_4 |        /* PLL divider = 1/4 */
    DEVCFG2_FPLLMUL_20 |        /* PLL multiplier = 20x */
    DEVCFG2_UPLLIDIV_4 |        /* USB PLL divider = 1/4 */
    DEVCFG2_FPLLODIV_2,         /* PLL postscaler = 1/2 */
#else
    // External crystal 8 MHz.
    DEVCFG1_FNOSC_PRIPLL |      /* Primary oscillator with PLL... */
    DEVCFG1_POSCMOD_HS |        /* ...in high speed mode */
    DEVCFG1_FPBDIV_2 |          /* Peripheral bus clock = SYSCLK/2 */
    DEVCFG1_OSCIOFNC_OFF |      /* CLKO output disabled */
    DEVCFG1_FCKM_DISABLE,       /* Fail-safe clock monitor disable */

    DEVCFG2_FPLLIDIV_2 |        /* PLL divider = 1/2 */
    DEVCFG2_FPLLMUL_20 |        /* PLL multiplier = 20x */
    DEVCFG2_UPLLIDIV_2 |        /* USB PLL divider = 1/2 */
    DEVCFG2_FPLLODIV_2,         /* PLL postscaler = 1/2 */
#endif
    DEVCFG3_USERID(0xffff));    /* User-defined ID */

/*
 * Boot code.
 */
asm ("          .section .exception,\"ax\",@progbits");
asm ("          .globl _start");
asm ("          .type _start, function");
asm ("_start:   la      $sp, _estack");
asm ("          la      $ra, main");
asm ("          la      $gp, _gp");
asm ("          jr      $ra");
asm ("          .text");

/*
 * Clear an array.
 */
void memzero (void *data, unsigned nbytes)
{
    unsigned *wordp = (unsigned*) data;
    unsigned nwords = nbytes / sizeof(int);

    while (nwords-- > 0)
        *wordp++ = 0;
}

/*
 * Copy an array.
 */
void memcopy (void *from, void *to, unsigned nbytes)
{
    unsigned *src = (unsigned*) from;
    unsigned *dst = (unsigned*) to;
    unsigned nwords = nbytes / sizeof(int);

    while (nwords-- > 0)
        *dst++ = *src++;
}

/*
 * A command packet was received from PC.
 * Process it and return 1 when a reply is ready.
 */
static int handle_packet()
{
    // TODO
    return 0;
}

/*
 * Main program entry point.
 */
int main()
{
    /* Initialize STATUS register: master interrupt disable. */
    mtc0 (C0_STATUS, 0, ST_CU0 | ST_BEV);

    /* Clear CAUSE register: use special interrupt vector 0x200. */
    mtc0 (C0_CAUSE, 0, CA_IV);

    /* Config register: enable kseg0 caching. */
    //mtc0 (C0_CONFIG, 0, mfc0 (C0_CONFIG, 0) | 3);
    asm volatile ("ehb");

    /* Disable JTAG port, to use it for i/o. */
    ANSELA = 0;
    ANSELB = 0;
    LATA = 0;
    LATB = 0;

    /* Initialize all .bss variables by zeros. */
    extern unsigned char __bss_start, __bss_end;
    memzero (&__bss_start, &__bss_end - &__bss_start);

    /* LED is used to indicate a USB status.
     * Configured by parameters BL_LED_PORT and BL_LED_PIN.
     * For Microstick II board LED is A0 */
    LAT_CLR(BL_LED_PORT) = 1 << BL_LED_PIN;
    TRIS_CLR(BL_LED_PORT) = 1 << BL_LED_PIN;

    // Initialize USB module SFRs and firmware variables to known states.
    TRISBSET = 1 << 10;
    TRISBSET = 1 << 11;
    usb_device_init();

    USB_HANDLE reply_handle = 0;
    int packet_received = 0;
    unsigned led_count = 0;
    for (;;) {
	// Check bus status and service USB interrupts.
        usb_device_tasks();

        // Blink the LEDs according to the USB device status
        if (led_count == 0)
            led_count = 100000;
        led_count--;

        if (! usb_is_device_suspended() && usb_device_state == CONFIGURED_STATE) {
            // Light LED.
            LAT_SET(BL_LED_PORT) = 1 << BL_LED_PIN;
        }
        else if (led_count == 0) {
            // Toggle LED.
            LAT_INV(BL_LED_PORT) = 1 << BL_LED_PIN;
        }

        // User Application USB tasks
        if (usb_device_state < CONFIGURED_STATE || usb_is_device_suspended())
            continue;

        // Are we done sending the last response.  We need to be before we
        // receive the next command because we clear the send buffer
        // once we receive a command
        if (reply_handle != 0) {
            if (usb_handle_busy (reply_handle))
                continue;
            reply_handle = 0;
        }

        if (packet_received) {
            if (handle_packet())
                reply_handle = usb_transfer_one_packet (HID_EP,
                    IN_TO_HOST, &send[0], PACKET_SIZE);
            packet_received = 0;
            continue;
        }

        // Did we receive a command?
        if (usb_handle_busy (request_handle))
            continue;

        // Make a copy of received data.
        memcopy (&receive_buffer, &receive, PACKET_SIZE);
        packet_received = 1;

        // Restart receiver, to be ready for a next packet.
        request_handle = usb_transfer_one_packet (HID_EP, OUT_FROM_HOST,
            (unsigned char*) &receive_buffer, PACKET_SIZE);
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
    0x04D8,                 // Vendor ID
    0x003C,                 // Product ID: USB HID Bootloader
    0x0002,                 // Device release number in BCD format
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
    50,                     // Max power consumption (2X mA)

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

static const USB_STRING_INIT(25) string1_descriptor = {
    sizeof(string1_descriptor),
    USB_DESCRIPTOR_STRING,              /* Manufacturer */
    { 'M','i','c','r','o','c','h','i','p',' ',
      'T','e','c','h','n','o','l','o','g','y',
      ' ','I','n','c','.' },
};

static const USB_STRING_INIT(18) string2_descriptor = {
    sizeof(string2_descriptor),
    USB_DESCRIPTOR_STRING,              /* Product */
    { 'U','S','B',' ','H','I','D',' ','D','e',
      'm','o' },
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
