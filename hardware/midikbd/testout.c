#include <stdio.h>
#include <CoreMIDI/CoreMIDI.h>

/*
 * Play the list of MIDI packets
 * on all MIDI output devices which the computer knows about.
 * (Send the MIDI message(s) to all MIDI out ports).
 */
void playPacketListOnAllDevices(MIDIPortRef midiout, const MIDIPacketList* pktlist)
{
    // send MIDI message to all MIDI output devices connected to computer:
    ItemCount nDests = MIDIGetNumberOfDestinations();
    ItemCount i;

    for (i=0; i<nDests; i++) {
        MIDIEndpointRef dest = MIDIGetDestination(i);
        OSStatus status = MIDISend(midiout, dest, pktlist);
        if (status) {
            printf("Error sendint MIDI data on port %d\n", (int)i);
            exit(status);
        }
    }
}

int main()
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
     * Get the number of MIDI destinations.
     */
    unsigned int ndest = MIDIGetNumberOfDestinations();
    if (ndest < 1) {
        printf("No MIDI destinations available.\n");
        return 0;
    }
    //printf("Total %u MIDI outputs(s)\n", (int)ndest);

    /*
     * Create MIDI output port.
     */
    MIDIPortRef output_port = 0;
    status = MIDIOutputPortCreate(client, CFSTR("Output Port"), &output_port);
    if (status != 0) {
        printf("Cannot create MIDI output port\n");
        exit(status);
    }

    ItemCount i;
    for (i=0; i<ndest; i++) {
        MIDIEndpointRef dest = MIDIGetDestination(i);

        /* Print destination name. */
        CFStringRef name = 0;
        MIDIObjectGetStringProperty(dest, kMIDIPropertyName, &name);
        printf("Output %u: %s\n", (int)i,
            CFStringGetCStringPtr(name, kCFStringEncodingUTF8));
        CFRelease(name);
    }
    printf("--------\n");

    int k;
    for (k=0; k<256; k++) {
        // Prepare a MIDI Note-On message to send
        MIDITimeStamp timestamp = 0;   // 0 will mean play now.
        Byte buffer[1024];             // storage space for MIDI Packets (max 65536)
        MIDIPacketList *packetlist = (MIDIPacketList*)buffer;
        MIDIPacket *currentpacket = MIDIPacketListInit(packetlist);
        Byte msg[3] = {
            //0xB0,
            0x90,
            k & 0x7F,
            k<128 ? 127 : 0,
        };
        currentpacket = MIDIPacketListAdd(packetlist, sizeof(buffer),
             currentpacket, timestamp, sizeof(msg), msg);

        // send the MIDI data and wait for one second:
        printf("%02x %02x %02x\n", msg[0], msg[1], msg[2]);
        playPacketListOnAllDevices(output_port, packetlist);
        usleep(500000);
    }
    return 0;
}
