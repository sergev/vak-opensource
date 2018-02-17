#!/usr/bin/python

from cobs import cobs
import serial, time, binascii, json

# Timeout for buffered serial I/O in seconds.
IO_TIMEOUT_SEC = 2

# Path to a serial device.
device_name = "/dev/ttyUSB0"

# Bit rate for serial communication.
baud_rate = 38400

#
# Connect to the microcontroller on a serial port.
#
print "Open:", device_name
serial_port = serial.Serial(
    port = device_name,
    baudrate = baud_rate,
    bytesize = 8,
    parity = 'N',
    stopbits = 1,
    timeout = IO_TIMEOUT_SEC
)
if not serial_port.isOpen():
    raise ValueError("Couldn't open %s" % port)
time.sleep(1)

# holds reads until we encounter a 0-byte (COBS!!!)
receive_buf = [None] * 256
receive_pos = 0

#
# Send a command to the microcontroller.
#
def send_command(cmd = ""):
    data = {"Command": cmd}

    # JSON encoding.
    data = json.dumps(data, separators=(',',':'))

    # Append CRC32 checksum.
    checksum = binascii.crc32(data) & 0xFFFFFFFF
    data += chr(97 + (checksum >> 28 & 0xf))
    data += chr(97 + (checksum >> 24 & 0xf))
    data += chr(97 + (checksum >> 20 & 0xf))
    data += chr(97 + (checksum >> 16 & 0xf))
    data += chr(97 + (checksum >> 12 & 0xf))
    data += chr(97 + (checksum >> 8 & 0xf))
    data += chr(97 + (checksum >> 4 & 0xf))
    data += chr(97 + (checksum & 0xf))

    # Encode to COBS format.
    encoded = cobs.encode(str(bytearray(data)))
    #print "Send:", repr(encoded)
    #print "Checksum: %08x" % checksum
    serial_port.write(encoded + '\r')

def reset_receive_buf():
    global receive_pos
    receive_buf[0:receive_pos] = [None] * receive_pos
    receive_pos = 0

#
# Reads a full line from the microcontroller
#
def recv_command():
    global receive_pos
    #print "Receive:",
    while True:
        c = serial_port.read(1)
        if not c:
            raise serial.SerialTimeoutException(
                "Couldn't recv command in %d seconds" % IO_TIMEOUT_SEC)

        if c == '\r':
            # End of packet.
            data = cobs.decode(str(bytearray(receive_buf[0:receive_pos])))
            reset_receive_buf()

            # Ignore short packets.
            if len(data) <= 8:
                print "Short packet"
                continue

            # Verify checksum.
            sum = data[-8:]
            data = data[:-8]
            checksum = binascii.crc32(data) & 0xFFFFFFFF
            if sum[0] != chr(97 + (checksum >> 28 & 0xf)) or \
               sum[1] != chr(97 + (checksum >> 24 & 0xf)) or \
               sum[2] != chr(97 + (checksum >> 20 & 0xf)) or \
               sum[3] != chr(97 + (checksum >> 16 & 0xf)) or \
               sum[4] != chr(97 + (checksum >> 12 & 0xf)) or \
               sum[5] != chr(97 + (checksum >> 8 & 0xf)) or \
               sum[6] != chr(97 + (checksum >> 4 & 0xf)) or \
               sum[7] != chr(97 + (checksum & 0xf)):
                print "Bad checksum:", sum
                continue

            # Return received data.
            return data

        receive_buf[receive_pos] = c
        receive_pos += 1

        if receive_pos == len(receive_buf):
            # Buffer overflow.
            reset_receive_buf()
            raise RuntimeError("IO read buffer overflow")

send_command("version")
reply = json.loads(recv_command())
print "Transistor Meter Version", reply["Version"]

send_command("njfet")
reply = json.loads(recv_command())
if "Error" in reply:
    print "Error!", reply["Error"]
else:
    print "Result =", reply
