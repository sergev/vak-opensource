#!/usr/bin/python

from cobs import cobs
import serial, time

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
time.sleep(2)

# holds reads until we encounter a 0-byte (COBS!!!)
receive_buf = [None] * 256
receive_pos = 0

#
# Send a command to the microcontroller.
#
def send_command(data = []):
    # TODO: crc32
    encoded = cobs.encode(str(bytearray(data)))
    print "Send:", repr(encoded)
    serial_port.write(encoded + '\r')

def reset_receive_buf():
    receive_buf[0:receive_pos] = [None] * receive_pos
    receive_pos = 0

#
# Reads a full line from the microcontroller
#
def recv_command():
    print "Receive:"
    while True:
        c = serial_port.read(1)
        if not c:
            raise serial.SerialTimeoutException(
                "Couldn't recv command in %d seconds" % IO_TIMEOUT_SEC)

        if c == '\x00':
            # Return received data.
            data = receive_buf[0:receive_pos]
            reset_receive_buf()

            # TODO: crc32
            return cobs.decode(str(bytearray(data)))

        receive_buf[receive_pos] = c
        receive_pos += 1

        if receive_pos == len(receive_buf):
            # Buffer overflow.
            reset_receive_buf()
            raise RuntimeError("IO read buffer overflow")

send_command("abcqqqqtttt")

print recv_command()
