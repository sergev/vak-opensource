#!/usr/bin/python

from cobs import cobs
import numpy as np
import serial, time, binascii, json, math

# Timeout for buffered serial I/O in seconds.
IO_TIMEOUT_SEC = 2

# Path to a serial device.
#device_name = "/dev/ttyUSB0"
device_name = "/dev/tty.wchusbserialfd130"

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
receive_buf = [None] * 4096
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
    raise ValueError(reply["Error"])
    print "Error!",

#
# Copy data into Numpy arrays.
#
reply_Vg = reply["Vgate"]
reply_Id = reply["Idrain"]
N = len(reply_Vg)
Vg = np.empty(N)
Id = np.empty(N)
print "  Vg, V  Id, mA"
print "  -------------"
for i in range(0, N):
    Vg[i] = reply_Vg[i]
    Id[i] = reply_Id[i]
    print "  %.3f  %.3f" % (Vg[i], Id[i])

#
# Least squares method.
# Compute A and B coefficients for equation y = A*x + B.
#
def least_squares(x, y):
    K = len(x)
    sum_x = np.sum(x)
    sum_y = np.sum(y)
    sum_xy = np.sum(np.multiply(x, y))
    sum_x2 = np.sum(np.multiply(x, x))
    a = (K * sum_xy - sum_x * sum_y) / (K * sum_x2 - sum_x * sum_x)
    b = (sum_y - a * sum_x) / K
    return a, b

#
# Obtain Idss and Yfs by the least squares method.
# Use first five samples to find a linear approximation:
#
(Yfs, Idss) = least_squares(Vg[:7], Id[:7])

#
# Compute error in quadratic approximation: y[i] = (x[i] - x0)^2 * c
#
def compute_error(x0, x, y):
    K = len(x)
    ln_y = np.log(y)
    ln_x = np.log(x - x0)
    c = np.exp(np.sum(ln_y - 2*ln_x) / K)
    err = y - (x - x0)**2 * c
    return np.sum(err**2)

#
# Enhance cutoff approximation.
# Use the least squares method to draw a quadratic line via
# a few last points.
#
def compute_cutoff(x0, x, y):
    e0 = compute_error(x0, x, y)
    while 1:
        e1 = compute_error(x0 - 0.01, x, y)
        if e1 > e0:
            break

        x0 -= 0.01
        e0 = e1

    return x0

#
# Compute cutoff voltage.
# Use last point as first approximation.
#
Voff1 = int(Vg[-1] * 100 - 1) * 0.01
Voff = compute_cutoff(Voff1, Vg[-8:-1], Id[-8:-1])

print
print "    Idss =", round(Idss, 2), "mA"
print "Vds(off) =", round(Voff, 2), "V"
print "     Yfs =", round(Yfs, 2), "mA/V"

#
# Compute saturation voltage Vsat.
#
Vsat = 2 * (-Voff - Idss/Yfs)
print "    Vsat =", round(Vsat, 2), "V"
