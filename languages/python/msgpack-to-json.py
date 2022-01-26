#!/usr/bin/env python3
#
# Convert MessagePack file to JSON format.
#
import sys, msgpack, json, pathlib

if len(sys.argv) != 2:
    print("Usage: msgpack-to-json input.msgpack")
    sys.exit(1)
input_name = sys.argv[1]
output_name = pathlib.Path(input_name).stem + ".json"

#
# Load the MessageMack file.
#
print('Read from {}'.format(input_name))
with open(input_name, 'rb') as file:
    data = msgpack.unpackb(file.read())

#
# Save as JSON file.
#
print('Write to {}'.format(output_name))
with open(output_name, "w") as file:
    file.write(json.dumps(data, indent=2, sort_keys=False))
