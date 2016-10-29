/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 *
 * Updated  August/3/2011 by Lowell Scott Hanson to be compatable with chipKIT boards
 * Updated  April/13/2012 by Serge Vakulenko for RetroBSD project
 */
#ifndef	W5100_H_INCLUDED
#define	W5100_H_INCLUDED

#include <stdint.h>

#define MAX_SOCK_NUM    4

#define IDM_OR          0x8000
#define IDM_AR0         0x8001
#define IDM_AR1         0x8002
#define IDM_DR          0x8003

#define SnMR_CLOSE      0x00
#define SnMR_TCP        0x01
#define SnMR_UDP        0x02
#define SnMR_IPRAW      0x03
#define SnMR_MACRAW     0x04
#define SnMR_PPPOE      0x05
#define SnMR_ND         0x20
#define SnMR_MULTI      0x80

#define Sock_OPEN       0x01
#define Sock_LISTEN     0x02
#define Sock_CONNECT    0x04
#define Sock_DISCON     0x08
#define Sock_CLOSE      0x10
#define Sock_SEND       0x20
#define Sock_SEND_MAC   0x21
#define Sock_SEND_KEEP  0x22
#define Sock_RECV       0x40

#define SnIR_SEND_OK    0x10
#define SnIR_TIMEOUT    0x08
#define SnIR_RECV       0x04
#define SnIR_DISCON     0x02
#define SnIR_CON        0x01

#define SnSR_CLOSED     0x00
#define SnSR_INIT       0x13
#define SnSR_LISTEN     0x14
#define SnSR_SYNSENT    0x15
#define SnSR_SYNRECV    0x16
#define SnSR_ESTABLISHED 0x17
#define SnSR_FIN_WAIT   0x18
#define SnSR_CLOSING    0x1A
#define SnSR_TIME_WAIT  0x1B
#define SnSR_CLOSE_WAIT 0x1C
#define SnSR_LAST_ACK   0x1D
#define SnSR_UDP        0x22
#define SnSR_IPRAW      0x32
#define SnSR_MACRAW     0x42
#define SnSR_PPPOE      0x5F

#define IPPROTO_IP      0
#define IPPROTO_ICMP    1
#define IPPROTO_IGMP    2
#define IPPROTO_GGP     3
#define IPPROTO_TCP     6
#define IPPROTO_PUP     12
#define IPPROTO_UDP     17
#define IPPROTO_IDP     22
#define IPPROTO_ND      77
#define IPPROTO_RAW     255

#define W5100_SSIZE     2048    // Max Tx buffer size
#define W5100_RSIZE     2048    // Max Rx buffer size

#define CH_BASE         0x0400
#define CH_SIZE         0x0100

/*----------------------------------------------
 * W5100 Registers
 */
unsigned w5100_write_byte (unsigned addr, int byte);
unsigned w5100_write (unsigned addr, uint8_t *buf, unsigned len);
unsigned w5100_read_byte (unsigned addr);
unsigned w5100_read (unsigned addr, uint8_t *buf, unsigned len);

#define __GP_REGISTER8(name, address)                           \
    static inline void w5100_write##name (unsigned data) {      \
        w5100_write_byte (address, data);                       \
    }                                                           \
    static inline unsigned w5100_read##name() {                 \
        return w5100_read_byte (address);                       \
    }
#define __GP_REGISTER16(name, address)                          \
    static inline void w5100_write##name(unsigned data) {       \
        w5100_write_byte (address,   data >> 8);                \
        w5100_write_byte (address+1, data & 0xFF);              \
    }                                                           \
    static inline unsigned w5100_read##name() {                 \
        unsigned res = w5100_read_byte (address);               \
        res = (res << 8) + w5100_read_byte (address + 1);       \
        return res;                                             \
    }
#define __GP_REGISTER_N(name, address, size)                    \
    static inline unsigned w5100_write##name(uint8_t *buff) {   \
        return w5100_write(address, buff, size);                \
    }                                                           \
    static inline unsigned w5100_read##name(uint8_t *buff) {    \
        return w5100_read(address, buff, size);                 \
    }

__GP_REGISTER8 (MR,     0x0000);    // Mode
__GP_REGISTER_N(GAR,    0x0001, 4); // Gateway IP address
__GP_REGISTER_N(SUBR,   0x0005, 4); // Subnet mask address
__GP_REGISTER_N(SHAR,   0x0009, 6); // Source MAC address
__GP_REGISTER_N(SIPR,   0x000F, 4); // Source IP address
__GP_REGISTER8 (IR,     0x0015);    // Interrupt
__GP_REGISTER8 (IMR,    0x0016);    // Interrupt Mask
__GP_REGISTER16(RTR,    0x0017);    // Timeout address
__GP_REGISTER8 (RCR,    0x0019);    // Retry count
__GP_REGISTER8 (RMSR,   0x001A);    // Receive memory size
__GP_REGISTER8 (TMSR,   0x001B);    // Transmit memory size
__GP_REGISTER8 (PATR,   0x001C);    // Authentication type address in PPPoE mode
__GP_REGISTER8 (PTIMER, 0x0028);    // PPP LCP Request Timer
__GP_REGISTER8 (PMAGIC, 0x0029);    // PPP LCP Magic Number
__GP_REGISTER_N(UIPR,   0x002A, 4); // Unreachable IP address in UDP mode
__GP_REGISTER16(UPORT,  0x002E);    // Unreachable Port address in UDP mode

#undef __GP_REGISTER8
#undef __GP_REGISTER16
#undef __GP_REGISTER_N

/*----------------------------------------------
 * W5100 Socket registers
 */
static inline unsigned
w5100_readSn_byte (unsigned sock, unsigned addr)
{
    return w5100_read_byte (CH_BASE + sock*CH_SIZE + addr);
}

static inline unsigned
w5100_writeSn_byte (unsigned sock, unsigned addr, unsigned data)
{
    return w5100_write_byte (CH_BASE + sock*CH_SIZE + addr, data);
}

static inline unsigned
w5100_readSn (unsigned sock, unsigned addr, uint8_t *buf, unsigned len)
{
    return w5100_read (CH_BASE + sock*CH_SIZE + addr, buf, len);
}

static inline unsigned
w5100_writeSn (unsigned sock, unsigned addr, uint8_t *buf, unsigned len)
{
    return w5100_write (CH_BASE + sock*CH_SIZE + addr, buf, len);
}

#define __SOCKET_REGISTER8(name, address)                                   \
    static inline void w5100_write##name (unsigned sock, unsigned data) {   \
        w5100_writeSn_byte (sock, address, data);                           \
    }                                                                       \
    static inline unsigned w5100_read##name (unsigned sock) {               \
        return w5100_readSn_byte (sock, address);                           \
    }
#define __SOCKET_REGISTER16(name, address)                                  \
    static inline void w5100_write##name (unsigned sock, unsigned data) {   \
        w5100_writeSn_byte (sock, address,   data >> 8);                    \
        w5100_writeSn_byte (sock, address+1, data & 0xFF);                  \
    }                                                                       \
    static inline unsigned w5100_read##name (unsigned sock) {               \
        unsigned res = w5100_readSn_byte (sock, address);                   \
        res = (res << 8) + w5100_readSn_byte (sock, address + 1);           \
        return res;                                                         \
    }
#define __SOCKET_REGISTER_N(name, address, size)                            \
    static inline unsigned w5100_write##name (unsigned sock, uint8_t *buf) { \
        return w5100_writeSn (sock, address, buf, size);                    \
    }                                                                       \
    static inline unsigned read##name (unsigned sock, uint8_t *buf) {       \
        return w5100_readSn (sock, address, buf, size);                     \
    }

__SOCKET_REGISTER8(SnMR,        0x0000)     // Mode
__SOCKET_REGISTER8(SnCR,        0x0001)     // Command
__SOCKET_REGISTER8(SnIR,        0x0002)     // Interrupt
__SOCKET_REGISTER8(SnSR,        0x0003)     // Status
__SOCKET_REGISTER16(SnPORT,     0x0004)     // Source Port
__SOCKET_REGISTER_N(SnDHAR,     0x0006, 6)  // Destination Hardw Addr
__SOCKET_REGISTER_N(SnDIPR,     0x000C, 4)  // Destination IP Addr
__SOCKET_REGISTER16(SnDPORT,    0x0010)     // Destination Port
__SOCKET_REGISTER16(SnMSSR,     0x0012)     // Max Segment Size
__SOCKET_REGISTER8(SnPROTO,     0x0014)     // Protocol in IP RAW Mode
__SOCKET_REGISTER8(SnTOS,       0x0015)     // IP TOS
__SOCKET_REGISTER8(SnTTL,       0x0016)     // IP TTL
__SOCKET_REGISTER16(SnTX_FSR,   0x0020)     // TX Free Size
__SOCKET_REGISTER16(SnTX_RD,    0x0022)     // TX Read Pointer
__SOCKET_REGISTER16(SnTX_WR,    0x0024)     // TX Write Pointer
__SOCKET_REGISTER16(SnRX_RSR,   0x0026)     // RX Free Size
__SOCKET_REGISTER16(SnRX_RD,    0x0028)     // RX Read Pointer
__SOCKET_REGISTER16(SnRX_WR,    0x002A)     // RX Write Pointer (supported?)

#undef __SOCKET_REGISTER8
#undef __SOCKET_REGISTER16
#undef __SOCKET_REGISTER_N

/*----------------------------------------------
 * W5100 functions
 */
void w5100_init();

/*
 * This function is being used for copy the data form Receive buffer
 * of the chip to application buffer.
 *
 * It calculate the actual physical address where one has to read
 * the data from Receive buffer. Here also take care of the condition
 * while it exceed the Rx memory uper-bound of socket.
 */
void w5100_read_data (unsigned sock, unsigned src, volatile uint8_t * dst, unsigned len);

/*
 * This function is being called by send() and sendto() function also.
 *
 * This function read the Tx write pointer register and after copy the data
 * in buffer update the Tx write pointer register. User should read
 * upper byte first and lower byte later to get proper value.
 */
void w5100_send_data_processing (unsigned sock, uint8_t *data, unsigned len);

/*
 * This function is being called by recv() also.
 *
 * This function read the Rx read pointer register and after copy
 * the data from receive buffer update the Rx write pointer register.
 * User should read upper byte first and lower byte later to get proper value.
 */
void w5100_recv_data_processing (unsigned sock, uint8_t *data, unsigned len, int peek);

static inline void w5100_getGatewayIp (uint8_t *addr)
{
    w5100_readGAR (addr);
}

static inline void w5100_setGatewayIp (uint8_t *addr)
{
    w5100_writeGAR (addr);
}

static inline void w5100_getSubnetMask (uint8_t *addr)
{
    w5100_readSUBR (addr);
}

static inline void w5100_setSubnetMask (uint8_t *addr)
{
    w5100_writeSUBR (addr);
}

static inline void w5100_getMACAddress (uint8_t *addr)
{
    w5100_readSHAR (addr);
}

static inline void w5100_setMACAddress (uint8_t *addr)
{
    w5100_writeSHAR (addr);
}

static inline void w5100_getIPAddress (uint8_t *addr)
{
    w5100_readSIPR (addr);
}

static inline void w5100_setIPAddress (uint8_t *addr)
{
    w5100_writeSIPR (addr);
}

static inline void w5100_setRetransmissionTime (unsigned timeout)
{
    w5100_writeRTR (timeout);
}

static inline void w5100_setRetransmissionCount (unsigned retry)
{
    w5100_writeRCR (retry);
}

void w5100_socket_cmd (unsigned sock, int cmd);

unsigned w5100_getTXFreeSize (unsigned sock);
unsigned w5100_getRXReceivedSize (unsigned sock);

#endif
