#!/usr/bin/env python3
# From: https://alphasec.io/getting-started-with-google-titan-security-key-a-beginners-guide/

# Import the necessary modules from fido2 library
from fido2.client import Fido2Client, ClientError
from fido2.hid import CtapHidDevice

# Find the first connected FIDO2 device
dev = next(CtapHidDevice.list_devices(), None)
if not dev:
    print("No FIDO2 device found.")
    exit()

# Set the origin of the relying party that the FIDO2 client will communicate with
origin = 'https://example.com'

def verify(rp_id, origin):
    # Perform verification logic here
    # Return True if RP ID is valid and belongs to the expected origin, else False
    print(f"Verify Relying Party: {rp_id}")
    return True

try:
    # Get and print device info to the console
    client = Fido2Client(dev, origin, verify)
    print(f"Authenticator Attestation GUID: {client.info.aaguid}")  # Authenticator Attestation GUID: 00000000-0000-0000-0000-000000000000
    print(f"Versions: {client.info.versions}")                      # Versions: ['U2F_V2']
    print(f"Firmware Version: {client.info.firmware_version}")      # Firmware Version: None
    print(f"Options: {client.info.options}")                        # Options: {}
    print(f"Algorithms: {client.info.algorithms}")                  # Algorithms: None
    print(f"Extensions: {client.info.extensions}")                  # Extensions: []
    print(f"Transports: {client.info.transports}")                  # Transports: []
    print(f"Certifications: {client.info.certifications}")          # Certifications: None
    print(f"Max Message Size: {client.info.max_msg_size}")          # Max Message Size: 1024
    print(f"Min PIN Length: {client.info.min_pin_length}")          # Min PIN Length: 4
    print(f"Force PIN Change: {client.info.force_pin_change}")      # Force PIN Change: False
    print(f"PIN UV Protocols: {client.info.pin_uv_protocols}")      # PIN UV Protocols: []

except ClientError as e:
    print(f"Error communicating with device: {e}")
