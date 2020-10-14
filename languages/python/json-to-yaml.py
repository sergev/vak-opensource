#!/usr/bin/env python3
#
# Convert JSON file to YAML format.
#
import sys, yaml, json, pathlib

if len(sys.argv) != 2:
    print("Usage: json-to-yaml.py filename.json")
    sys.exit(1)
input_name = sys.argv[1]
output_name = pathlib.Path(input_name).stem + ".yaml"

#
# Use fast YAML dumper on Python 3.6 and later.
#
try:
    from yaml import CDumper as Dumper
except ImportError:
    print('Using slow YAML loader')
    from yaml import Dumper

#
# Load the JSON file.
#
print('Read from {}'.format(input_name))
with open(input_name, 'rb') as file:
    data = json.loads(file.read())

#
# Save as YAML file.
#
print('Write to {}'.format(output_name))
with open(output_name, 'w') as file:
    yaml.dump(data, file, Dumper = Dumper, sort_keys = False)
