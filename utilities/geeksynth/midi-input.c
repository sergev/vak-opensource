#include <stdio.h>
#include <CoreMIDI/CoreMIDI.h>
#include "midi.h"

struct midi_state_t midi;

int print_message(const unsigned char data[], int nbytes, double sec)
{
    int n, ch;
#if 0
    if (sec == 0)
        printf("           ");
    else
        printf("%9.3lf  ", sec);
#endif
    if (! (data[0] & 0x80)) {
        //printf("%02x    Bad status bit\n", data[0]);
        return 1;
    }
    ch = (data[0] & 0x0f) + 1;
    switch (data[0] & 0xf0) {
    default:
        //printf("----             Undefined %02x\n", data[0]);
        return 1;

    case 0x80: /* 1000nnnn 0kkkkkkk 0vvvvvvv */
        //printf("Ch%-2d  %3d = %-3d  Note Off\n", ch, data[1], data[2]);
        return 3;

    case 0x90: /* 1001nnnn 0kkkkkkk 0vvvvvvv */
        //printf("Ch%-2d  %3d = %-3d  Note On\n", ch, data[1], data[2]);
        return 3;

    case 0xA0: /* 1010nnnn 0kkkkkkk 0vvvvvvv */
        //printf("Ch%-2d  %3d = %-3d  Polyphonic Key Pressure\n", ch, data[1], data[2]);
        return 3;

    case 0xB0: /* 1011nnnn 0ccccccc 0vvvvvvv */
        //printf("Ch%-2d  %3d = %-3d  Control Change\n", ch, data[1], data[2]);
        if (data[1] <= 127 && data[2] <= 127) {
            midi.control[data[1]] = data[2];
        }
        return 3;

    case 0xC0: /* 1100nnnn 0ppppppp */
        //printf("Ch%-2d  %3d        Program Change\n", ch, data[1]);
        if (data[1] <= 127) {
            midi.knob_prog = data[1];
        }
        return 2;

    case 0xD0: /* 1101nnnn 0vvvvvvv */
        //printf("Ch%-2d  %3d        Channel Pressure\n", ch, data[1]);
        return 2;

    case 0xE0: /* 1110nnnn 0lllllll 0mmmmmmm */
        //printf("Ch%-2d  %3d = %-3d  Pitch Bend Change\n", ch, data[1], data[2]);
        return 3;

    case 0xF0: /* System Common Messages. */
        switch (data[0] & 0xff) {
        case 0xF0: /* 11110000 0iiiiiii 0ddddddd ... 11110111 */
            for (n=1; n<nbytes; n++)
                if (data[n] == 0xF7)
                    break;
            if (n == 7 &&
                data[1] == 0x7F &&  /* Universal Real Time Sys Ex header */
                data[2] == 0x7F &&  /* Device ID (7F = All devices) */
                data[3] == 4 &&     /* Device Control message */
                data[4] == 1 &&     /* Master Volume */
                data[5] == 0)       /* Volume LSB */
            {
                /* Master Volume */
                //printf("----  %3d        Master Volume\n", data[6]);
                if (data[6] <= 127) {
                    midi.slider_volume = data[6];
                }

            } else if (n == 10 &&
                data[1] == 0x42 &&
                data[2] == 0x40 &&
                data[3] == 0 &&
                data[4] == 1 &&
                data[5] == 4 &&
                data[6] == 0 &&
                data[7] == 0x5F &&
                data[8] == 0x4F)
            {
                /* Scene number, Korg specific */
                //printf("----  %3d        Scene Change\n", data[9] + 1);
                if (data[9] <= 3) {
                    midi.switch_bank = data[9];
                }

            } else {
#if 0
                int i;
                printf("----             System Exclusive:");
                for (i=1; i<n; i++)
                    printf(" %02x", data[i]);
                printf("\n");
#endif
            }
            return n+1;

        case 0xF1: /* 11110001 0nnndddd */
            //printf("----  %3d = %-3d  Time Code Quarter Frame\n", data[1] >> 4, data[1] & 0x0f);
            return 2;

        case 0xF2: /* 11110010 0lllllll 0mmmmmmm */
            //printf("----  %3d = %-3d  Song Position Pointer\n", data[1], data[2]);
            return 3;

        case 0xF3: /* 11110011 0sssssss */
            //printf("----  %3d        Song Select\n", data[1]);
            return 2;

        default:
        case 0xF4: /* 11110100 */
            //printf("----             Undefined %02x\n", data[0]);
            return 1;

        case 0xF5: /* 11110101 */
            //printf("----             Undefined %02x\n", data[0]);
            return 1;

        case 0xF6: /* 11110110 */
            //printf("----             Tune Request\n");
            return 1;

        case 0xF7: /* 11110111 */
            //printf("----             End of Exclusive\n");
            return 1;

        case 0xF8: /* 11111000 */
            //printf("----             Timing Clock\n");
            return 1;

        case 0xF9: /* 11111001 */
            //printf("----             Undefined %02x\n", data[0]);
            return 1;

        case 0xFA: /* 11111010 */
            //printf("----             Start\n");
            return 1;

        case 0xFB: /* 11111011 */
            //printf("----             Continue\n");
            return 1;

        case 0xFC: /* 11111100 */
            //printf("----             Stop\n");
            return 1;

        case 0xFD: /* 11111101 */
            //printf("----             Undefined %02x\n", data[0]);
            return 1;

        case 0xFE: /* 11111110 */
            //printf("----             Active Sensing\n");
            return 1;

        case 0xFF: /* 11111111 */
            //printf("----             Reset\n");
            return 1;
        }
    }
}

/*
 * Print MIDI packet.
 *
 * struct MIDIPacket {
 *      MIDITimeStamp   timeStamp;
 *      UInt16          length;
 *      Byte            data[256];
 * };
 * timeStamp =  The time at which the events occurred (if receiving MIDI),
 *              or the time at which the events are to be played (if sending
 *              MIDI).  Zero means "now" when sending MIDI data.  The time
 *              stamp applies to the first MIDI byte in the packet.
 * length    =  The number of valid MIDI bytes which follow in data[].
 *              It may be larger than 256 bytes if the packet is dynamically
 *              allocated.
 * data      =  A variable-length stream of MIDI messages. Running status
 *              is not allowed.  In the case of system-exclusive messages,
 *              a packet may only contain a single message, or portion
 *              of one, with no other MIDI events.  The MIDI messages in
 *              the packet must always be complete, except for
 *              system-exclusive messages.  data[] is declared to be 256
 *              bytes in length so clients don't have to create custom data
 *              structures in simple situations.
 */
void print_packet(const MIDIPacket* packet)
{
    double timeinsec = packet->timeStamp / (double)1e9;
    int i;

    for (i=0; i<packet->length; ) {
        i += print_message(packet->data + i, packet->length - i,
                           i==0 ? timeinsec : 0);
    }
}

/*
 * Receive MIDI input packets.
 *
 * This is a callback function through which a client receives
 * incoming MIDI messages.  A MIDIReadProc function pointer is
 * passed to the MIDIInputPortCreate and MIDIDestinationCreate
 * functions. The CoreMIDI framework will create a high-priority
 * receive thread on your client's behalf, and from that thread,
 * your MIDIReadProc will be called when incoming MIDI messages
 * arrive. Because this function is called from a separate thread,
 * be aware of the synchronization issues when accessing data in
 * this callback.
 */
void midi_input_callback(
    const MIDIPacketList    *list,  /* The incoming MIDI message(s). */
    void                    *arg,   /* A pointer which identifies the port */
    void                    *src)   /* A pointer which identifies the source. */
{
    MIDIPacket *packet = (MIDIPacket*)list->packet;
    int count = list->numPackets;
    int j;

    for (j=0; j<count; j++) {
        print_packet(packet);
        packet = MIDIPacketNext(packet);
    }
}

void midi_init()
{
    /*
     * Create MIDI client.
     */
    MIDIClientRef client = 0;
    OSStatus status;
    status = MIDIClientCreate(CFSTR("MIDI Demo"), 0, 0, &client);
    if (status != 0) {
        printf("Cannot create MIDI Client\n");
        exit(status);
    }

    /*
     * Create MIDI input port.
     */
    MIDIPortRef input_port = 0;
    status = MIDIInputPortCreate(
        client,                 /* The client to own the newly-created port */
        CFSTR("Input Port"),    /* The name of the port */
        midi_input_callback,    /* A callback function which will be called with incoming MIDI */
        0,                      /* The arg passed to the callback */
        &input_port);           /* On successful return, points to the newly-create MIDIPort */
    if (status != 0) {
        printf("Cannot create MIDI input port\n");
        exit(status);
    }

    /*
     * Get the number of MIDI sources.
     */
    ItemCount nsources = MIDIGetNumberOfSources();
    if (nsources < 1) {
        printf("No MIDI sources available.\n");
        exit(-1);
    }

    ItemCount i;
    for (i=0; i<nsources; i++) {
        MIDIEndpointRef src = MIDIGetSource(i);

        /* Print source name. */
        CFStringRef name = 0;
        MIDIObjectGetStringProperty(src, kMIDIPropertyName, &name);
        printf("Input %u: %s\n", (int)i,
            CFStringGetCStringPtr(name, kCFStringEncodingUTF8));
        CFRelease(name);

        /*
         * Establish a connection from a source
         * to a client's input port.
         */
        MIDIPortConnectSource(
            input_port, /* The port to which the create the connection.
                         * This port's readProc is called with
                         * incomming MIDI from the source. */
            src,        /* The source from which to create the connection. */
            0);         /* This pointer is passed to the MIDIReadProc,
                         * as a way to identify the source. */
    }
}
