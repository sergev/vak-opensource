#!/usr/bin/env python3
#
# This script prints contents of an IEEE 1685-2009 IP-XACT XML file.
#
#   show_registers.py input_file
#
import sys
from pathlib import Path
import xml.etree.ElementTree as ET

def print_ipxact_contents(file_path):
    # Convert file_path to Path object
    file_path = Path(file_path)

    # Check if file exists
    if not file_path.is_file():
        print(f"Error: File {file_path} does not exist.")
        sys.exit(1)

    try:
        # Parse XML file for IEEE 1685-2009
        tree = ET.parse(file_path)
        root = tree.getroot()
        ns = {'spirit': 'http://www.spiritconsortium.org/XMLSchema/SPIRIT/1685-2009'}

        # Print VLNV information
        print("=== IP-XACT Component Details ===")
        vendor      = root.find('spirit:vendor', ns)
        library     = root.find('spirit:library', ns)
        name        = root.find('spirit:name', ns)
        version     = root.find('spirit:version', ns)
        description = root.find('spirit:description', ns)
        if vendor is not None:      print(f"Vendor: {vendor.text}")
        if library is not None:     print(f"Library: {library.text}")
        if name is not None:        print(f"Name: {name.text}")
        if version is not None:     print(f"Version: {version.text}")
        if description is not None: print(f"Description: {description.text}")
        print()

        # Print Memory Maps (if any)
        memory_maps = root.find('spirit:memoryMaps', ns)
        if memory_maps is not None:
            print("=== Memory Maps ===")
            for mem_map in memory_maps.findall('spirit:memoryMap', ns):
                mem_map_name = mem_map.find('spirit:name', ns)
                if mem_map_name is not None: print(f"Memory Map: {mem_map_name.text}")
                for addr_block in mem_map.findall('spirit:addressBlock', ns):
                    addr_block_name = addr_block.find('spirit:name', ns)
                    display_name    = addr_block.find('spirit:displayName', ns)
                    base_address    = addr_block.find('spirit:baseAddress', ns)
                    range_elem      = addr_block.find('spirit:range', ns)
                    width           = addr_block.find('spirit:width', ns)
                    usage           = addr_block.find('spirit:usage', ns)
                    access          = addr_block.find('spirit:access', ns)
                    print(f"  Address Block: {addr_block_name.text if addr_block_name is not None else 'N/A'}")
                    if display_name is not None: print(f"    Name: {display_name.text}")
                    if base_address is not None: print(f"    Base Address: {base_address.text}")
                    if range_elem is not None:   print(f"    Range: {range_elem.text}")
                    if width is not None:        print(f"    Width: {width.text}")
                    if usage is not None:        print(f"    Usage: {usage.text}")
                    if access is not None:       print(f"    Access: {access.text}")

                    # Print Register Files (if any)
                    register_files = addr_block.findall('spirit:registerFile', ns)
                    if register_files:
                        print("    Register Files:")
                        for reg_file in register_files:
                            reg_file_name  = reg_file.find('spirit:name', ns)
                            display_name   = reg_file.find('spirit:displayName', ns)
                            address_offset = reg_file.find('spirit:addressOffset', ns)
                            range_elem     = reg_file.find('spirit:range', ns)
                            description    = reg_file.find('spirit:description', ns)
                            print(f"      Register File: {reg_file_name.text if reg_file_name is not None else 'N/A'}")
                            if display_name is not None:   print(f"        Name: {display_name.text}")
                            if address_offset is not None: print(f"        Address Offset: {address_offset.text}")
                            if range_elem is not None:     print(f"        Range: {range_elem.text}")
                            if description is not None:    print(f"        Description: {description.text}")

                            # Print Registers (if any)
                            registers = reg_file.findall('spirit:register', ns)
                            if registers:
                                print("        Registers:")
                                for reg in registers:
                                    reg_name        = reg.find('spirit:name', ns)
                                    reg_display     = reg.find('spirit:displayName', ns)
                                    reg_offset      = reg.find('spirit:addressOffset', ns)
                                    reg_size        = reg.find('spirit:size', ns)
                                    reg_access      = reg.find('spirit:access', ns)
                                    reg_description = reg.find('spirit:description', ns)
                                    print(f"          Register: {reg_name.text if reg_name is not None else 'N/A'}")
                                    if reg_display is not None:     print(f"            Name: {reg_display.text}")
                                    if reg_offset is not None:      print(f"            Offset: {reg_offset.text}")
                                    if reg_size is not None:        print(f"            Size: {reg_size.text}")
                                    if reg_access is not None:      print(f"            Access: {reg_access.text}")
                                    if reg_description is not None: print(f"            Description: {reg_description.text}")
                                print()
                        print()
                print()

        # Print File Sets (if any)
        file_sets = root.find('spirit:fileSets', ns)
        if file_sets is not None:
            print("=== File Sets ===")
            for file_set in file_sets.findall('spirit:fileSet', ns):
                file_set_name = file_set.find('spirit:name', ns)
                print(f"File Set: {file_set_name.text if file_set_name is not None else 'N/A'}")
                for file in file_set.findall('spirit:file', ns):
                    file_name = file.find('spirit:name', ns)
                    file_type = file.find('spirit:fileType', ns)
                    file_desc = file.find('spirit:description', ns)
                    print(f"  File: {file_name.text if file_name is not None else 'N/A'}")
                    if file_type is not None: print(f"    File Type: {file_type.text}")
                    if file_desc is not None: print(f"    Description: {file_desc.text}")
                print()

    except Exception as e:
        print(f"Error parsing IP-XACT file: {str(e)}")
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: show_registers.py ipxact.xml")
        sys.exit(1)

    ipxact_file = sys.argv[1]
    print_ipxact_contents(ipxact_file)
