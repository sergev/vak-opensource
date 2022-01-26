#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
#
# Convert a YAML file to JSON format.
# To install it on Mac, use:
#   brew install yaml-cpp libyaml
#   pip3 --no-cache-dir install --verbose --force-reinstall -I pyyaml
#
import sys, os, yaml

if len(sys.argv) != 2:
    print("Usage: yaml-to-yaml input.yaml")
    sys.exit(1)

# Enable line buffering.
sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', buffering=1)

# Import YAML input.
file = open(sys.argv[1])
data = yaml.load(file, Loader=yaml.FullLoader)

# Export JSON output.
import json
with open("model.json", "w") as file:
    sys.stdout.write(json.dumps(data, indent=4, sort_keys=True))
