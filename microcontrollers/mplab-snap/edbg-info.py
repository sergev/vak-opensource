#!/usr/bin/env python3
"""
Example usage of pyedbglib to read debugger firmware version and target voltage
"""
from pyedbglib.hidtransport.hidtransportfactory import hid_transport
from pyedbglib.protocols.housekeepingprotocol import Jtagice3HousekeepingProtocol
from pyedbglib.version import VERSION as pyedbglib_version

# Report library version
print("pyedbglib version {}".format(pyedbglib_version))

# Make a connection using HID transport
transport = hid_transport()
transport.connect()

# Create a housekeeper
housekeeper = Jtagice3HousekeepingProtocol(transport)
housekeeper.start_session()

# Read out debugger firmware version
major = housekeeper.get_byte(Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_CONFIG,
                             Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONFIG_FWREV_MAJ)
minor = housekeeper.get_byte(Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_CONFIG,
                             Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONFIG_FWREV_MIN)
build = housekeeper.get_le16(Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_CONFIG,
                             Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONFIG_BUILD)
print ("Debugger firmware is version {}.{}.{}".format(major, minor,build))

# Read out target voltage
target_voltage = housekeeper.get_le16(Jtagice3HousekeepingProtocol.HOUSEKEEPING_CONTEXT_ANALOG,
                                      Jtagice3HousekeepingProtocol.HOUSEKEEPING_ANALOG_VTREF)
print ("Target voltage is {:.02f}V".format(target_voltage/1000.0))

# Tear down
housekeeper.end_session()
transport.disconnect()
