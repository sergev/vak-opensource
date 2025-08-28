#!/usr/bin/env python3
#
# This script processes an IEEE 1685-2009 IP-XACT XML file to remove specified
# register files or registers and saves the result to a new XML file.
#
#   remove_registers.py input_file output_file --remove block:file[:register]
#
# Arguments:
#   input_file                      Path to the input IP-XACT XML file (required).
#   output_file                     Path to the output IP-XACT XML file (required).
#   --remove block:file[:register]  Specifies the address block, register file,
#                                   and optionally a register to remove. Can be used
#                                   multiple times to remove multiple items.
#       block                       Name of the address block.
#       file                        Name of the register file.
#       register                    Name of the register (optional). If omitted,
#                                   the entire register file is removed.
#
# Examples:
# 1. Remove an entire register file:
#       remove_registers.py input.xml output.xml --remove MyBlock:MyRegisterFile
#
# 2. Remove a specific register from a register file:
#       remove_registers.py input.xml output.xml --remove MyBlock:MyRegisterFile:MyRegister
#
# 3. Remove multiple items:
#       remove_registers.py input.xml output.xml --remove Block1:File1 --remove Block2:File2:Reg1
#
# Notes:
# - The input file must be a valid IP-XACT XML file compliant with the
#   http://www.spiritconsortium.org/XMLSchema/SPIRIT/1685-2009 namespace.
# - If the specified address block, register file, or register is not found, a message
#   is printed, and the script continues processing other `--remove` options.
# - The script exits with an error if the input file is not found, is invalid XML,
#   or if no `--remove` options are provided.
# - The output file will be overwritten if it already exists.
#
import xml.etree.ElementTree as ET
import sys
import argparse

ns = {'spirit': 'http://www.spiritconsortium.org/XMLSchema/SPIRIT/1685-2009'}

def parse_ipxact(input_file):
    ET.register_namespace('spirit', 'http://www.spiritconsortium.org/XMLSchema/SPIRIT/1685-2009')
    try:
        tree = ET.parse(input_file)
        root = tree.getroot()
        return tree, root
    except FileNotFoundError:
        print(f"Error: Input file '{input_file}' not found.")
        sys.exit(1)
    except ET.ParseError:
        print(f"Error: Failed to parse '{input_file}'. Invalid XML format.")
        sys.exit(1)

def find_address_block(root, address_block_name):
    for memory_map in root.findall('.//spirit:memoryMap', ns):
        for address_block in memory_map.findall('spirit:addressBlock', ns):
            name_elem = address_block.find('spirit:name', ns)
            if name_elem is not None and name_elem.text == address_block_name:
                return address_block
    return None

def remove_register_file(root, address_block_name, register_file_name):
    address_block = find_address_block(root, address_block_name)
    if address_block is None:
        print(f"Address block '{address_block_name}' not found.")
        return False
    for register_file in address_block.findall('spirit:registerFile', ns):
        name_elem = register_file.find('spirit:name', ns)
        if name_elem is not None and name_elem.text == register_file_name:
            address_block.remove(register_file)
            print(f"Removed register file '{register_file_name}' from '{address_block_name}'.")
            return True
    print(f"Register file '{register_file_name}' not found in '{address_block_name}'.")
    return False

def remove_register(root, address_block_name, register_file_name, register_name):
    address_block = find_address_block(root, address_block_name)
    if address_block is None:
        print(f"Address block '{address_block_name}' not found.")
        return False
    if register_file_name:
        for register_file in address_block.findall('spirit:registerFile', ns):
            name_elem = register_file.find('spirit:name', ns)
            if name_elem is not None and name_elem.text == register_file_name:
                for register in register_file.findall('spirit:register', ns):
                    reg_name_elem = register.find('spirit:name', ns)
                    if reg_name_elem is not None and reg_name_elem.text == register_name:
                        register_file.remove(register)
                        print(f"Removed register '{register_name}' from register file '{register_file_name}' in '{address_block_name}'.")
                        return True
                print(f"Register '{register_name}' not found in register file '{register_file_name}' in '{address_block_name}'.")
                return False
    else:
        for register in address_block.findall('spirit:register', ns):
            reg_name_elem = register.find('spirit:name', ns)
            if reg_name_elem is not None and reg_name_elem.text == register_name:
                address_block.remove(register)
                print(f"Removed register '{register_name}' from '{address_block_name}'.")
                return True
        print(f"Register '{register_name}' not found in '{address_block_name}'.")
        return False

def save_ipxact(tree, output_file):
    try:
        tree.write(output_file, encoding='utf-8', xml_declaration=False)
        print(f"Saved modified IP-XACT to '{output_file}'.")
    except Exception as e:
        print(f"Error: Failed to save to '{output_file}': {str(e)}")
        sys.exit(1)

def parse_remove_option(remove_arg):
    parts = remove_arg.split(':')
    if len(parts) < 2 or len(parts) > 3:
        raise argparse.ArgumentTypeError("Remove option must be in format 'block:file[:register]'")
    address_block_name = parts[0]
    register_file_name = parts[1]
    register_name = parts[2] if len(parts) == 3 else None
    if not address_block_name or not register_file_name:
        raise argparse.ArgumentTypeError("Address block and register file names must not be empty")
    return address_block_name, register_file_name, register_name

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Remove register files or registers from IP-XACT file")
    parser.add_argument("input_file", help="Input IP-XACT XML file")
    parser.add_argument("output_file", help="Output IP-XACT XML file")
    parser.add_argument("--remove", action='append', type=parse_remove_option,
                       help="Remove register or register file in format 'block:file[:register]'")

    args = parser.parse_args()

    if not args.remove:
        print("Error: At least one --remove option must be provided")
        sys.exit(1)

    tree, root = parse_ipxact(args.input_file)

    for address_block_name, register_file_name, register_name in args.remove:
        if register_name:
            remove_register(root, address_block_name, register_file_name, register_name)
        else:
            remove_register_file(root, address_block_name, register_file_name)

    save_ipxact(tree, args.output_file)
