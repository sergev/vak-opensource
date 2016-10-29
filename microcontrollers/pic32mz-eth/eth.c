/*
 * Network driver for the PIC32 internal Ethernet controller with 8720 PHY.
 *
 * Author: Keith Vogel
 * Copyright (c) 2013-2014, Digilent <www.digilentinc.com>
 *
 * This program is free software; distributed under the terms of
 * BSD 3-clause license ("Revised BSD License", "New BSD License", or "Modified BSD License")
 */
#include "deIP.h"
#include "DEIPcK.h"

//
// MAC state.
//
#define STATE_INIT              0   // Initialization
#define STATE_WAITLINK          1   // Waiting for link
#define STATE_LINKDELAY         2   // Link up delay
#define STATE_LINKED            3   // Link is up
#define STATE_REST              4

static int eth_state;               // The state of the internal state machine
static int eth_is_up;               // Used to determine if we need to set speed and duplex
static int eth_phy_id;              // PHY id
static FFPT receive_queue;          // Queue of received packets
static FFPT transmit_queue;         // Queue of packet for transmit
static IPSTACK *eth_tx_packet;      // Current packet under transmit

#define VIRT_TO_PHYS(_v)        (((unsigned) (_v)) & 0x1FFFFFFF)
#define PHYS_TO_VIRT(_p)        ((void*) ((_p) | 0xA0000000))

// Allocate static input buffer space.
// Flow control requires that we have 2*1536 at the high water mark.
// Therefore we must have at least 3 * 1536 bytes,
// but lets have a little more, say 4 * 1536 = 6144 = 6K.
#define MTU                 1536
#define RX_PACKETS          4
#define RX_BYTES_PER_DESC   128                             // This is tuned as a balance between the number of discriptors and size wasting empty space at the end of a discriptor
#define RX_BYTES            (RX_PACKETS * MTU)              // how much buffer space we have for incoming frames
#define RX_DESCRIPTORS      (RX_BYTES / RX_BYTES_PER_DESC)  // check that this works out to be even ie with a modulo of zero
#define TX_DESCRIPTORS      5

#define INCR_RX_INDEX(_i)   ((_i + 1) % RX_DESCRIPTORS)

typedef struct {
    uint32_t hdr;               // Flags
    uint32_t paddr;             // Phys address of data buffer
    uint32_t ctl;               // TX options / RX filter status
    uint32_t status;            // Status
} eth_desc_t;

#define DESC_SOP(d)         ((d)->hdr & 0x80000000)     // Start of packet
#define DESC_SET_SOP(d)     (d)->hdr |= 0x80000000

#define DESC_EOP(d)         ((d)->hdr & 0x40000000)     // End of packet
#define DESC_SET_EOP(d)     (d)->hdr |= 0x40000000

#define DESC_BYTECNT(d)     ((d)->hdr >> 16 & 0x7ff)    // Number of data bytes
#define DESC_SET_BYTECNT(d,n) ((d)->hdr |= (n) << 16)

#define DESC_SET_NPV(d)     (d)->hdr &= 0x00000100      // Next descriptor pointer valid
#define DESC_CLEAR_NPV(d)   (d)->hdr &= ~0x00000100

#define DESC_EOWN(d)        ((d)->hdr & 0x00000080)     // Eth controller owns this desc
#define DESC_SET_EOWN(d)    (d)->hdr &= 0x00000080
#define DESC_CLEAR_EOWN(d)  (d)->hdr &= ~0x00000080

#define DESC_FRAMESZ(d)     ((d)->status & 0xffff)      // Size of received packet

static char rx_buffer[RX_BYTES];            // the RX buffer space
static eth_desc_t rx_desc[RX_DESCRIPTORS+1]; // the +1 is the terminating descriptor, or room for a loop back pointer
static eth_desc_t tx_desc[TX_DESCRIPTORS];  // just big enough for a transmit of packet

static unsigned receive_index = 0;          // The next descriptor to look at waiting for incoming data

// these are frame pointers into the current frame to read out of the DMA descriptor
static unsigned read_index = 0;
static unsigned desc_offset = 0;
static unsigned frame_size = 0;
static unsigned read_nbytes = 0;

//-------------------------------------------------------------
// PHY routines for SMSC LAN8720A chip.
//
#define PHY_CONTROL             0       // Basic Control Register
#define PHY_STATUS              1       // Basic Status Register
#define PHY_MODE                18      // Special Modes
#define PHY_SPECIAL             31      // Special Control/Status Register

#define PHY_CONTROL_RESET	0x8000	// Soft reset, bit self cleared

#define PHY_STATUS_ANEG_ACK     0x0020	// Auto-negotiation acknowledge
#define PHY_STATUS_CAP_ANEG     0x0008	// Auto-negotiation available
#define PHY_STATUS_LINK         0x0004	// Link valid

#define PHY_MODE_PHYAD          0x000f  // PHY address mask

#define PHY_SPECIAL_AUTODONE    0x1000  // Auto-negotiation is done
#define PHY_SPECIAL_FDX         0x0010  // Full duplex
#define PHY_SPECIAL_100         0x0008  // Speed 100 Mbps

//
// Return -1 when failed.
//
static int phy_read(int phy_id, int reg_id, unsigned timeout)
{
    unsigned time_start = SYSGetMilliSecond();

    // Clear any commands.
    EMAC1MCMD = 0;
    while (EMAC1MIND & PIC32_EMAC1MIND_MIIMBUSY) {
        if (SYSGetMilliSecond() - time_start > timeout) {
            return -1;
        }
    }

    EMAC1MADR = PIC32_EMAC1MADR(phy_id, reg_id);
    EMAC1MCMDSET = PIC32_EMAC1MCMD_READ;

    // Wait to finish.
    time_start = SYSGetMilliSecond();
    while (EMAC1MIND & PIC32_EMAC1MIND_MIIMBUSY) {
        if (SYSGetMilliSecond() - time_start > timeout) {
            EMAC1MCMD = 0;
            return -1;
        }
    }

    EMAC1MCMD = 0;
    return EMAC1MRDD & 0xffff;
}

//
// Return -1 when failed.
//
static int phy_scan(int phy_id, int reg_id, int scanMask, int fIsEqual, unsigned timeout)
{
    unsigned time_start = SYSGetMilliSecond();

    // Clear any commands.
    EMAC1MCMD = 0;
    while (EMAC1MIND & PIC32_EMAC1MIND_MIIMBUSY) {
        if (SYSGetMilliSecond() - time_start > timeout) {
            return -1;
        }
    }

    // Scan the PHY until it is ready.
    EMAC1MADR = PIC32_EMAC1MADR(phy_id, reg_id);
    EMAC1MCMDSET = PIC32_EMAC1MCMD_SCAN;

    // Wait for it to become valid.
    time_start = SYSGetMilliSecond();
    while (EMAC1MIND & PIC32_EMAC1MIND_NOTVALID) {
        if (SYSGetMilliSecond() - time_start > timeout) {
            return -1;
        }
    }

    // Wait until we hit our mask.
    time_start = SYSGetMilliSecond();
    while (((EMAC1MRDD & scanMask) == scanMask) != fIsEqual) {
        if (SYSGetMilliSecond() - time_start > timeout) {
            return -1;
        }
    }

    // Kill the scan.
    EMAC1MCMD = 0;
    time_start = SYSGetMilliSecond();
    while (EMAC1MIND & PIC32_EMAC1MIND_MIIMBUSY) {
        if (SYSGetMilliSecond() - time_start > timeout) {
            return -1;
        }
    }
    return 0;
}

//
// Return -1 when failed.
//
static int phy_write(int phy_id, int reg_id, int value, unsigned timeout)
{
    unsigned time_start = SYSGetMilliSecond();

    // Clear any commands.
    EMAC1MCMD = 0;
    while (EMAC1MIND & PIC32_EMAC1MIND_MIIMBUSY) {
        if (SYSGetMilliSecond() - time_start > timeout) {
            return -1;
        }
    }

    EMAC1MADR = PIC32_EMAC1MADR(phy_id, reg_id);
    EMAC1MWTD = value;

    // Wait to finish.
    time_start = SYSGetMilliSecond();
    while (EMAC1MIND & PIC32_EMAC1MIND_MIIMBUSY) {
        if (SYSGetMilliSecond() - time_start > timeout) {
            return -1;
        }
    }
    return 0;
}

static int is_phy_linked(int phy_id, int was_up)
{
    int status = phy_read(phy_id, PHY_STATUS, 1);
    if (status < 0) {
        return 0;
    }

    int link_is_up =
        (status & PHY_STATUS_LINK) &&       // Link is up
        (status & PHY_STATUS_CAP_ANEG) &&   // Able to auto-negotiate
        (status & PHY_STATUS_ANEG_ACK);     // Auto-negotiation completed

    // Set our link speed.
    if (link_is_up && ! was_up) {
        // Must disable the RX while setting these parameters.
        int rxen = ETHCON1 & PIC32_ETHCON1_RXEN;
        ETHCON1CLR = PIC32_ETHCON1_RXEN;

        // Get the speed.
        int special = phy_read(phy_id, PHY_SPECIAL, 1);
        int speed_100 = 0;
        int full_duplex = 0;

        if (special & PHY_SPECIAL_AUTODONE) {
            // Auto-negotiation is done.
            speed_100 = (special & PHY_SPECIAL_100);
            full_duplex = (special & PHY_SPECIAL_FDX);
        }

        // Set speed.
        if (speed_100)
            EMAC1SUPPSET = PIC32_EMAC1SUPP_SPEEDRMII;
        else
            EMAC1SUPPCLR = PIC32_EMAC1SUPP_SPEEDRMII;

        // Set duplex and gap size.
        if (full_duplex) {
            EMAC1CFG2SET = PIC32_EMAC1CFG2_FULLDPLX;
            EMAC1IPGT = 21;
        } else {
            EMAC1CFG2CLR = PIC32_EMAC1CFG2_FULLDPLX;
            EMAC1IPGT = 18;
        }

        // Return the Rx Enable back to what it was.
        if (rxen)
            ETHCON1SET = PIC32_ETHCON1_RXEN;
    }
    return link_is_up;
}

//
// Reset the PHY via MIIM interface.
// Return -1 on failure.
//
static int eth_reset_phy(int phy_id)
{
    int mode;

    mode = phy_read(phy_id, PHY_MODE, 100);
    if (mode < 0)
        return -1;

    if ((mode & PHY_MODE_PHYAD) != phy_id) {
        printf("Wrong PHY id!\n");
    }

    // Send a reset to the PHY.
    if (phy_write(phy_id, PHY_CONTROL, PHY_CONTROL_RESET, 100) < 0)
        return -1;

    // Wait for the reset pin to autoclear.
    if (phy_scan(phy_id, PHY_CONTROL, PHY_CONTROL_RESET, 0, 500) < 0)
        return -1;

    return 0;
}

//-------------------------------------------------------------
// Internal Ethernet routines.
//

static void update_state()
{
    static unsigned time_start = 0;

    eth_is_up = is_phy_linked(eth_phy_id, eth_is_up);

    switch(eth_state) {
    case STATE_INIT:
        // we haven't initialized yet.
        break;

    case STATE_WAITLINK:
        if (eth_is_up) {
            time_start = SYSGetMilliSecond();
            eth_state = STATE_LINKDELAY;
        }
        break;

    case STATE_LINKDELAY:
        if (! eth_is_up) {
            eth_state = STATE_WAITLINK;
        }
        else if (SYSGetMilliSecond() - time_start >= 1000) {
            eth_state = STATE_LINKED;
            ETHCON1SET = PIC32_ETHCON1_RXEN;
        }
        break;

    case STATE_REST:
        if (SYSGetMilliSecond() - time_start >= 10) {
            if (eth_is_up) {
                eth_state = STATE_LINKED;
            } else {
                time_start = SYSGetMilliSecond();
            }
        }
        break;

    case STATE_LINKED:
    default:
        if (! eth_is_up) {
            eth_state = STATE_REST;
            time_start = SYSGetMilliSecond();
        }
        break;
    }
}

static void send_packet(IPSTACK *packet)
{
    // clear the tx buffer, but we don't have to
    // worry about the last one as it is just
    // a dummy that will always have the software
    // own to stop the DMA from running off the end
    memset(tx_desc, 0, sizeof(tx_desc) - sizeof(tx_desc[0]));

    // always have a frame, always FRAME II (we don't support 802.3 outgoing frames; this is typical)
    int i = 0;
    DESC_SET_BYTECNT(&tx_desc[i], packet->cbFrame);

    tx_desc[i].paddr = VIRT_TO_PHYS(packet->pFrameII);
    DESC_SET_SOP(&tx_desc[i]);
    i++;

    // IP Header
    if (packet->cbIPHeader > 0)
    {
        DESC_SET_BYTECNT(&tx_desc[i], packet->cbIPHeader);
        tx_desc[i].paddr = VIRT_TO_PHYS(packet->pIPHeader);
        i++;
    }

    // Transport Header (TCP/UDP)
    if (packet->cbTranportHeader > 0)
    {
        DESC_SET_BYTECNT(&tx_desc[i], packet->cbTranportHeader);
        tx_desc[i].paddr = VIRT_TO_PHYS(packet->pTransportHeader);
        i++;
    }

    // Payload / ARP / ICMP
    if (packet->cbPayload > 0)
    {
        DESC_SET_BYTECNT(&tx_desc[i], packet->cbPayload);
        tx_desc[i].paddr = VIRT_TO_PHYS(packet->pPayload);
        i++;
    }

    // put in eop; I is one past the last
    // entry; lets bump it down one
    i--;
    DESC_SET_EOP(&tx_desc[i]);

    // set the ownership bits
    // the last descriptor is a dummy the software
    // always owns, do look at that.
    int j;
    for (j=TX_DESCRIPTORS-1; j>=0; j--)
    {
        // i is the last descriptor that is
        // go to be transmitted
        if (j <= i)
            DESC_SET_EOWN(&tx_desc[j]);
        else
            DESC_CLEAR_EOWN(&tx_desc[j]);
    }

    // set the descriptor table to be transmitted
    ETHTXST = VIRT_TO_PHYS(tx_desc);

    // transmit
    ETHCON1SET = PIC32_ETHCON1_TXRTS;
}

static void read_packet(char *buf, unsigned bufsz)
{
    if (! DESC_SOP(&rx_desc[read_index])) {
        // Start of packet is expected
        //printf("Something is wrong!\n");
    }

    if (frame_size == 0 || read_nbytes == frame_size) {
        return;
    }

    // make sure we own the descriptor, bad if we don't!
    while (bufsz > 0) {
        if (DESC_EOWN(&rx_desc[read_index]))
            break;

        int end_of_packet = DESC_EOP(&rx_desc[read_index]);
        unsigned nbytes = DESC_BYTECNT(&rx_desc[read_index]);
        unsigned cb     = min(nbytes - desc_offset, bufsz);

        memcpy(buf, PHYS_TO_VIRT(rx_desc[read_index].paddr + desc_offset), cb);
        buf         += cb;
        desc_offset += cb;
        read_nbytes += cb;
        bufsz       -= cb;

        // if we read the whole descriptor page
        if (desc_offset == nbytes) {
            // set up for the next page
            desc_offset = 0;
            read_index = INCR_RX_INDEX(read_index);

            // if we are done, get out
            if (end_of_packet || read_nbytes == frame_size)
                break;
        }
    }
}

//
// A packet has been received; process it.
//
static void receive_packet()
{
    // this is the size of the frame as the Eth controller knows it
    // this may include the FCS at the end of the frame
    // and may included padding bytes to make a min packet size of 64 bytes
    // so it is likely this is longer than the payload length
    read_index = receive_index;
    frame_size = DESC_FRAMESZ(&rx_desc[receive_index]);
    desc_offset = 0;
    read_nbytes = 0;

    if (frame_size <= 0)
        return;

    IPSTACK *packet = Malloc(frame_size + sizeof(IPSTACK));

    if (packet == NULL) {
        // Out of memory.
    } else {
        // Fill in info about the frame data.
        packet->pPayload  = (char*)packet + sizeof(IPSTACK);
        packet->cbPayload = frame_size;

        read_packet(packet->pPayload, packet->cbPayload);
        put_packet(&receive_queue, packet);
    }

    // Free the receive descriptors.
    while (! DESC_EOWN(&rx_desc[receive_index]))
    {
        int end_of_packet = DESC_EOP(&rx_desc[receive_index]);

        DESC_SET_EOWN(&rx_desc[receive_index]);         // give up ownership
        ETHCON1SET = PIC32_ETHCON1_BUFCDEC;             // decrement the BUFCNT
        receive_index = INCR_RX_INDEX(receive_index);   // check the next one

        // hit the end of packet
        if (end_of_packet)
            break;
    }

    // init our state variables
    read_index = 0;
    desc_offset = 0;
    frame_size = 0;
    read_nbytes = 0;
}

//
// Check whether a frame has been received.
//
static int is_frame_received()
{
    if (eth_state < STATE_WAITLINK) {
        // Not initialized yet.
        return 0;
    }

    // Check the first receive descriptor in queue.
    eth_desc_t *rd = &rx_desc[receive_index];

    if (DESC_EOWN(rd)) {
        // There are no receive descriptors to process.
        return 0;
    }
    return 1;
}

//
// Set DMA descriptors.
//
static void eth_init_dma()
{
    // Set Rx discriptor list.
    // All owned by the ethernet controller.
    memset(rx_desc, 0, sizeof(rx_desc));
    for(i=0; i<RX_DESCRIPTORS; i++)
    {
        DESC_SET_EOWN(&rx_desc[i]);
        DESC_CLEAR_NPV(&rx_desc[i]);
        rx_desc[i].paddr = VIRT_TO_PHYS(rx_buffer + (i * RX_BYTES_PER_DESC));
    }

    // Loop the list back to the begining.
    // This is a circular array descriptor list.
    rx_desc[RX_DESCRIPTORS].hdr = VIRT_TO_PHYS(rx_desc);
    DESC_SET_NPV(&rx_desc[RX_DESCRIPTORS-1]);

    // Set RX at the start of the list.
    receive_index = 0;
    ETHRXST = VIRT_TO_PHYS(&rx_desc[0]);

    // Set up the transmitt descriptors all owned by
    // the software; clear it completely out.
    memset(tx_desc, 0, sizeof(tx_desc));
    ETHTXST = VIRT_TO_PHYS(tx_desc);

    // Init our frame reading values,
    // used by read_packet.
    read_index = 0;
    desc_offset = 0;
    frame_size = 0;
    read_nbytes = 0;
}

//
// RMII and MIIM reset.
//
static void eth_reset_mii()
{
    EMAC1SUPP = PIC32_EMAC1SUPP_RESETRMII;  // reset RMII
    EMAC1SUPP = 0;

    // block reset the management protocol
    EMAC1MCFG = PIC32_EMAC1MCFG_RESETMGMT;  // reset the management fuctions
    EMAC1MCFG = 0;

    // The IEEE 802.3 spec says no faster than 2.5MHz.
    // 80 / 40 = 2MHz
    EMAC1MCFG = PIC32_EMAC1MCFG_CLKSEL_40;
}

//
// Reset the MAC.
//
static void eth_reset_mac()
{
    // Reset the MAC.
    EMAC1CFG1 = PIC32_EMAC1CFG1_SOFTRESET;

    // Pull it out of reset.
    EMAC1CFG1 = 0;
    EMAC1CFG1 = PIC32_EMAC1CFG1_TXPAUSE |       // MAC TX flow control
                PIC32_EMAC1CFG1_RXPAUSE |       // MAC RX flow control
                PIC32_EMAC1CFG1_RXENABLE;       // Receive enable

    EMAC1CFG2 = PIC32_EMAC1CFG2_EXCESSDFR |     // Defer to carrier indefinitely
                PIC32_EMAC1CFG2_BPNOBKOFF |     // Backpressure/No Backoff
                PIC32_EMAC1CFG2_AUTOPAD |       // Automatic detect pad enable
                PIC32_EMAC1CFG2_PADENABLE |     // Pad/CRC enable
                PIC32_EMAC1CFG2_CRCENABLE |     // CRC enable
                PIC32_EMAC1CFG2_LENGTHCK;       // Frame length checking

    // This all default.
    EMAC1MAXF = 1518;                           // max frame size in bytes
    EMAC1IPGR = PIC32_EMAC1IPGR(12, 18);        // non-back-to-back interpacket gap
    EMAC1CLRT = PIC32_EMAC1CLRT(55, 15);        // collision window/retry limit
}

//
// Reset the Ethernet Controller.
//
static void eth_reset()
{
    // Disable the ethernet interrupt.
    IECCLR(PIC32_IRQ_ETH >> 5) = 1 << (PIC32_IRQ_ETH & 31);

    // Turn the Ethernet cotroller off.
    ETHCON1 = 0;

    // Wait for abort to finish.
    while (ETHSTAT & PIC32_ETHSTAT_ETHBUSY)
        continue;

    // Clear the interrupt flag bit.
    IFSCLR(PIC32_IRQ_ETH >> 5) = 1 << (PIC32_IRQ_ETH & 31);

    // Clear interrupts.
    ETHIEN = 0;
    ETHIRQ = 0;

    // Clear discriptor pointers; for now.
    ETHTXST = 0;
    ETHRXST = 0;

    // Auto flow control is on
    ETHCON1SET = PIC32_ETHCON1_PTV(1);  // the max number of pause timeouts
    ETHCON1SET = PIC32_ETHCON1_AUTOFC;

    // High and low watermarks.
    int empty_watermark = MTU / RX_BYTES_PER_DESC;
    int full_watermark  = RX_DESCRIPTORS - (MTU * 2) / RX_BYTES_PER_DESC;
    ETHRXWM = PIC32_ETHRXWM_FWM(full_watermark) |
              PIC32_ETHRXWM_EWM(empty_watermark);

    // Set RX buffer size, descriptor buffer size in bytes / 16.
    ETHCON2 = RX_BYTES_PER_DESC >> 4;

    // Set our Rx filters.
    ETHRXFC = PIC32_ETHRXFC_CRCOKEN |   // enable checksum filter
              PIC32_ETHRXFC_UCEN |      // enable unicast filter
              PIC32_ETHRXFC_BCEN;       // enable broadcast filter

    // Hash table, not used.
    ETHHT0 = 0;
    ETHHT1 = 0;

    // Pattern match, not used.
    ETHPMM1 = 0;
    ETHPMM1 = 0;

    // Byte in TCP like checksum pattern calculation.
    ETHPMCS = 0;

    // Turn on the ethernet controller
    ETHCON1SET = PIC32_ETHCON1_ON;
}

//-------------------------------------------------------------
// External API to the Ethernet driver.
//

int eth_is_linked()
{
    return eth_state == STATE_LINKED;
}

void eth_send(IPSTACK *packet)
{
    put_packet(&transmit_queue, packet);
}

IPSTACK *eth_read()
{
    IPSTACK *packet = get_packet(&receive_queue);

    return packet;
}

//
// This routine should be called periodically from upper layer.
//
void eth_periodic()
{
    update_state();

    if (! (ETHCON1 & PIC32_ETHCON1_TXRTS)) {
        // TX is idle; transmit any pending data.
        if (eth_tx_packet != 0){
            // Release previous packet.
            release_packet(eth_tx_packet);
            eth_tx_packet = 0;
        }

        if (eth_state == STATE_LINKED) {
            IPSTACK *packet = get_packet(&transmit_queue);
            if (packet != 0) {
                send_packet(packet);
                eth_tx_packet = packet;
            }
        }
    }

    // Check whether a frame has been received.
    if (eth_state >= STATE_WAITLINK &&
        ! DESC_EOWN(&rx_desc[receive_index])) {
        receive_packet();
    }
}

//
// Get the MAC address.
//
void eth_get_macaddr(char *mac_addr)
{
    // Fetch our MAC address.
    mac_addr[0] = EMAC1SA2;
    mac_addr[1] = EMAC1SA2 >> 8;
    mac_addr[2] = EMAC1SA1;
    mac_addr[3] = EMAC1SA1 >> 8;
    mac_addr[4] = EMAC1SA0;
    mac_addr[5] = EMAC1SA0 >> 8;
}

//
// Initialize the Ethernet controller.
//
void eth_init()
{
    int i;

    // Setup for PIC32MZ EC Starter Kit board.
    TRISHSET = 1<<8;                    // Set RH8 as input - ERXD0
    TRISHSET = 1<<5;                    // Set RH5 as input - ERXD1
    TRISHSET = 1<<4;                    // Set RH4 as input - ERXERR

    // Default PHY address is 0 on LAN8720 PHY daughter board.
    eth_phy_id = 0;

    // Link is down.
    eth_is_up = 0;

    // Keep the internal MAC state machine at idle.
    eth_state = STATE_INIT;

    // Clear packet queues.
    eth_tx_packet = 0;
    memset(&receive_queue, 0, sizeof(receive_queue));
    memset(&transmit_queue, 0, sizeof(transmit_queue));

    // As per section 35.4.10 of the Pic32 Family Ref Manual.
    eth_reset();
    eth_reset_mac();
    eth_reset_mii();

    if (eth_reset_phy(eth_phy_id) < 0) {
        printf("Ethernet configuration failed\n");
        return;
    }

    // Set DMA descriptors.
    eth_init_dma();

    // Wait for auto link to finish.
    eth_state = STATE_WAITLINK;
}
