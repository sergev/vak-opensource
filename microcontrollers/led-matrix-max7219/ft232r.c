/*
 * We use FT232RL chip in synchronous bit bang mode, as described
 * in FTDI Application Note AN232R-01:
 * "Bit Bang Modes for the FT232R and FT245R".
 *
 * On Linux, access to FTDI USB devices is restricted to root by default.
 * You can use 'sudo' to run the program, or open the user access
 * by creaating a rule for udev service:
 *      gedit /etc/udev/rules.d/10-ftdi.rules
 *
 * Add the following line:
 *      # FTDI USB-Serial
 *      SUBSYSTEM=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", GROUP="users", MODE="0666"
 *
 * On Mac OS X, in case you have FTDI Virtual Com Port (VCP) driver installed,
 * this needs to be disabled before bitbang mode will work;
 * the two cannot coexist. In a Terminal window, type:
 *
 *      sudo kextunload /System/Library/Extensions/IOUSBFamily.kext/Contents/PlugIns/AppleUSBFTDI.kext
 *
 * To restore the driver and resume using Arduino or other FTDI serial devices:
 *
 *      sudo kextload /System/Library/Extensions/IOUSBFamily.kext/Contents/PlugIns/AppleUSBFTDI.kext
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <usb.h>
#include "bitbang.h"

#define VID_FTDI    0x0403
#define PID_FT232R  0x6001

/*
 * USB endpoints.
 */
#define IN_EP			0x02
#define OUT_EP			0x81

/* Requests */
#define SIO_RESET		0   /* Reset the port */
#define SIO_MODEM_CTRL		1   /* Set the modem control register */
#define SIO_SET_FLOW_CTRL	2   /* Set flow control register */
#define SIO_SET_BAUD_RATE	3   /* Set baud rate */
#define SIO_SET_DATA		4   /* Set the data characteristics of the port */
#define SIO_POLL_MODEM_STATUS	5
#define SIO_SET_EVENT_CHAR	6
#define SIO_SET_ERROR_CHAR	7
#define SIO_SET_LATENCY_TIMER	9
#define SIO_GET_LATENCY_TIMER	10
#define SIO_SET_BITMODE		11
#define SIO_READ_PINS		12
#define SIO_READ_EEPROM		0x90
#define SIO_WRITE_EEPROM	0x91
#define SIO_ERASE_EEPROM	0x92

static usb_dev_handle *adapter;

/*
 * On INT signal (^C pressed), close the connection and
 * terminate the process.
 */
static void sigint(int sig)
{
    bitbang_close();
    exit(-1);
}

/*
 * Connect to FT232RL adapter by name.
 * Configure needed pins as outputs by mask.
 */
int bitbang_open(char *device_desc, int output_mask)
{
    struct usb_bus *bus;
    struct usb_device *dev;
    char string [256];

    usb_init();
    usb_find_busses();
    usb_find_devices();
    for (bus = usb_get_busses(); bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            if (dev->descriptor.idVendor == VID_FTDI &&
                dev->descriptor.idProduct == PID_FT232R)
            {
                adapter = usb_open (dev);
                if (! adapter)
                    continue;

                /* Get product description string. */
                if (usb_get_string_simple (adapter, dev->descriptor.iProduct,
                    string, sizeof(string)) <= 0) {
                    if (errno == EPERM) {
                        printf("No access to FTDI USB-Serial device from user mode.\n");
                        printf("Please, run this application via 'sudo', or enable\n");
                        printf("user access to FTDI USB-Serial devices in /etc/udev/rules.d\n\n");
                    } else
                        printf("Cannot get iProduct string\n");
                    usb_close (adapter);
                    continue;
                }

                /* Select device by description. */
                if (strncmp (string, device_desc, sizeof(string)) != 0) {
                    printf("Device name '%s' don't match\n", string);
                    usb_close (adapter);
                    continue;
                }
                goto found;
            }
        }
    }
    printf("FTDI FT232R adapter not found.\n");
    return -1;

found:;

#if ! defined (__CYGWIN32__) && ! defined (MINGW32) && ! defined (__APPLE__)
    if (usb_get_driver_np (adapter, 0, string, sizeof(string)) == 0) {
        if (usb_detach_kernel_driver_np (adapter, 0) < 0) {
            printf("Failed to detach the '%s' kernel driver.\n", string);
            usb_close(adapter);
            return -1;
        }
    }
#endif
    usb_claim_interface (adapter, 0);

    /* Reset the device. */
    if (usb_control_msg (adapter,
        USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
        SIO_RESET, 0, 0, 0, 0, 1000) != 0) {
            /* Unable to reset device. */
            printf("FT232R: Unable to reset device.\n");
            return -1;
    }

    /* Sync bit bang mode. */
    if (usb_control_msg (adapter,
        USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
        SIO_SET_BITMODE, output_mask | 0x400,
        0, 0, 0, 1000) != 0) {
            printf("FT232R: Cannot set sync bitbang mode.\n");
            return -1;
    }

    /* Ровно 500 нсек между выдачами. */
    unsigned divisor = 1;
    unsigned latency_timer = 1;

    /* Frequency divisor is 14-bit non-zero value. */
    if (usb_control_msg (adapter,
        USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
        SIO_SET_BAUD_RATE, divisor,
        0, 0, 0, 1000) != 0) {
            printf("FT232R: Cannot set baud rate.\n");
            return -1;
    }
    if (usb_control_msg (adapter,
        USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
        SIO_SET_LATENCY_TIMER, latency_timer, 0, 0, 0, 1000) != 0) {
            printf("FT232R: Unable to set latency timer.\n");
            return -1;
    }

    atexit(bitbang_close);
    signal(SIGINT, sigint);
    return 0;
}

/*
 * Properly close the connection to FT232R adapter.
 */
void bitbang_close()
{
    if (adapter) {
        usb_release_interface(adapter, 0);
        usb_close(adapter);
        adapter = 0;
    }
}

/*
 * Perform sync bitbang output/input transaction.
 * Array output[] contains data to send.
 * Counter nbytes gives a length.
 * On return, received data are put back to array reply[64].
 */
int bitbang_io(unsigned char *output, int nbytes, unsigned char *reply)
{
    int bytes_to_write, bytes_written, n, txdone, rxdone;
    int empty_rxfifo, bytes_to_read, bytes_read;
    unsigned char buf[64];

    /* FIFO TX buffer of FT232R chip has size of 128 bytes.
     * FIFO RX buffer has 256 bytes. First two receive bytes
     * contain modem and line status. */
    if (nbytes <= 0)
        return -1;

    /* RX buffer is empty, except two status bytes. */
    empty_rxfifo = sizeof(buf) - 2;

    /* Indexes in data buffer. */
    txdone = 0;
    rxdone = 0;
    while (rxdone < nbytes) {
        /* Try to send as much as possible,
         * but avoid overflow of receive buffer. */
        bytes_to_write = 64;
        if (bytes_to_write > nbytes - txdone)
            bytes_to_write = nbytes - txdone;
        if (bytes_to_write > empty_rxfifo)
            bytes_to_write = empty_rxfifo;

        /* Write data. */
        bytes_written = 0;
        while (bytes_written < bytes_to_write) {
            /*printf("--usb bulk write %d bytes\n",
                bytes_to_write - bytes_written);*/
            n = usb_bulk_write (adapter, IN_EP,
                (char*) output + txdone + bytes_written,
                bytes_to_write - bytes_written, 1000);
            if (n < 0) {
                printf("FT232R: write failed (%u bytes)\n",
                    bytes_to_write - bytes_written);
                exit(-1);
            }
            /*if (n != bytes_to_write)
                printf("--usb bulk written %d bytes of %d\n",
                    n, bytes_to_write - bytes_written);*/
            bytes_written += n;
        }
        txdone += bytes_written;
        empty_rxfifo -= bytes_written;

        if (empty_rxfifo == 0 || txdone == nbytes) {
            /* Get reply. */
            bytes_to_read = sizeof(buf) - empty_rxfifo - 2;
            bytes_read = 0;
            while (bytes_read < bytes_to_read) {
                n = usb_bulk_read (adapter, OUT_EP,
                    (char*) buf,
                    bytes_to_read - bytes_read + 2, 2000);
                if (n < 0) {
                    printf("FT232R: read failed\n");
                    exit(-1);
                }
                /*if (n != bytes_to_read + 2)
                    printf("--usb error: bulk read %d bytes of %d\n",
                        n, bytes_to_read - bytes_read + 2);
                else
                    printf("--usb bulk read %d bytes\n", n)*/;

                if (n > 2) {
                    /* Copy data. */
                    if (reply)
                        memcpy (reply + rxdone, buf + 2, n - 2);
                    bytes_read += n;
                    rxdone += n - 2;
                }
            }
            empty_rxfifo = sizeof(buf) - 2;
        }
    }
    return 0;
}
