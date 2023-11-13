#!/usr/bin/env python3
# From: https://alphasec.io/getting-started-with-google-titan-security-key-a-beginners-guide/

# Import the necessary modules from fido2 library
from fido2.hid import CtapHidDevice

# Find the first connected FIDO2 device
dev = next(CtapHidDevice.list_devices(), None)
if not dev:
    print("No FIDO2 device found.")
    exit()

# Get and print device info to the console
print(f"        Device: {dev}")                 # Device: CtapHidDevice('4295128513')
print(f"  Product Name: {dev.product_name}")    # Product Name: ePass FIDO
print(f"       Version: {dev.version}")         # Version: 2
print(f" Serial Number: {dev.serial_number}")   # Serial Number: None
print(f"Device Version: {dev.device_version}")  # Device Version: (1, 0, 1)
print(f"  Capabilities: {dev.capabilities}")    # Capabilities: 3           = WINK+LOCK
