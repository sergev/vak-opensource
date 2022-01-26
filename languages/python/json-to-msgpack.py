#!/usr/bin/env python3
#
# Convert JSON file to MessagePack format.
#
import sys, msgpack, json, pathlib

if len(sys.argv) != 2:
    print("Usage: json-to-msgpack.py filename.json")
    sys.exit(1)
input_name = sys.argv[1]
output_name = pathlib.Path(input_name).stem + ".msgpack"

#
# Load the JSON file.
#
print('Read from {}'.format(input_name))
with open(input_name, 'rb') as file:
    data = json.loads(file.read())

#
# Save as MessagePack file.
#
print('Write to {}'.format(output_name))
with open(output_name, 'wb') as file:
    file.write(msgpack.packb(data, use_bin_type=True))
