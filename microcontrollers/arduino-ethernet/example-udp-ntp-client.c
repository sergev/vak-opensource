/*
 * UDP NTP Client
 *
 * Get the time from a Network Time Protocol (NTP) time server.
 * Demonstrates use of UDP send_packet and receive_packet.
 * For more on NTP time servers and the messages needed to communicate with them,
 * see http://en.wikipedia.org/wiki/Network_Time_Protocol
 *
 *  4 Sep 2010 created by Michael Margolis
 * 17 Sep 2010 modified by Tom Igoe
 * 16 Apr 2012 ported to RetroBSD by Serge Vakulenko
 *
 * This code is in the public domain.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ethernet.h"
#include "udp.h"

#define NTP_PORT            123     /* Local port to listen for UDP packets */
#define LOCAL_PORT          8888    /* Local port to listen for UDP packets */
#define NTP_PACKET_SIZE     48      /* NTP time stamp is in the first 48 bytes of the message */

/*
 * Enter a MAC address and IP address for your controller below.
 * The IP address will be dependent on your local network.
 */
unsigned char mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

unsigned char ip[] = { 192,168,1,177 };

/*
 * time.nist.gov NTP server.
 */
unsigned char timeServer[] = { 192,43,244,18 };

/*
 * Buffer to hold incoming and outgoing packets.
 */
unsigned char packetBuffer [NTP_PACKET_SIZE];

/*
 * A UDP instance to let us send and receive packets over UDP.
 */
udp_t udp;

/*
 * Send an NTP request to the time server at the given address.
 */
void send_ntp_packet (unsigned char *address)
{
    /* Set all bytes in the buffer to 0. */
    memset (packetBuffer, 0, NTP_PACKET_SIZE);

    /* Initialize values needed to form NTP request
     * (see URL above for details on the packets). */
    packetBuffer[0] = 0b11100011;   /* LI, Version, Mode */
    packetBuffer[1] = 0;            /* Stratum, or type of clock */
    packetBuffer[2] = 6;            /* Polling Interval */
    packetBuffer[3] = 0xEC;         /* Peer Clock Precision */

    /* 8 bytes of zero for Root Delay & Root Dispersion. */
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    /* All NTP fields have been given values, now
     * you can send a packet requesting a timestamp. */
    udp_send_packet (&udp, packetBuffer, NTP_PACKET_SIZE, address, NTP_PORT);
}

int main()
{
    /* Start Ethernet and UDP. */
    ethernet_init (mac, ip, 0, 0);
    udp_init (&udp, LOCAL_PORT);

    for (;;) {
        send_ntp_packet (timeServer); // send an NTP packet to a time server

        // wait to see if a reply is available
        usleep (1000000);

        if (udp_available (&udp)) {
            /* Read the packet into the buffer. */
            udp_read_packet (&udp, packetBuffer, NTP_PACKET_SIZE, 0, 0);

            /* The timestamp starts at byte 40 of the received packet
             * and is four bytes, or two words, long.
             * Combine the four bytes into a long integer.
             * This is NTP time (seconds since Jan 1 1900). */
            unsigned long secsSince1900 = packetBuffer[40] << 24 |
                                          packetBuffer[41] << 16 |
                                          packetBuffer[42] << 8 |
                                          packetBuffer[43];
            printf ("Seconds since Jan 1 1900 = %lu\n", secsSince1900);

            /* Now convert NTP time into everyday time.
             * Unix time starts on Jan 1 1970.
             * In seconds, that's 2208988800.
             * Subtract seventy years. */
            const unsigned long seventyYears = 2208988800UL;
            unsigned long epoch = secsSince1900 - seventyYears;

            /* Print Unix time. */
            printf ("Unix time = %lu\n", epoch);

            /* Print the hour, minute and second.
             * UTC is the time at Greenwich Meridian (GMT). */
            printf ("The UTC time is %lu:%02lu:%02lu\n",
                (epoch  % 86400L) / 3600,   // hour (86400 equals secs per day)
                (epoch  % 3600) / 60,       // minute (3600 equals secs per minute)
                epoch % 60);                // second
        }

        /* Wait ten seconds before asking for the time again. */
        sleep (10);
    }
}
