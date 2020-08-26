#!/usr/bin/env python3
#
# Convert ONNX model to JSON format.
#
import sys

if len(sys.argv) < 2 or len(sys.argv) > 3:
    print("Usage: onnx-to-json.py input.onnx [output.json]")
    sys.exit(1)
input_name = sys.argv[1]
output_name = sys.argv[2] if len(sys.argv) >= 3 else "output.json"

# Load the ONNX file
import onnx
print("Read", input_name)
onnx_model = onnx.load(input_name)

#with open(output_name, 'w') as out_file:
#    out_file.write(str(onnx_model))

from google.protobuf.json_format import MessageToJson
import json
print("Write", output_name)
with open(output_name, 'w') as out_file:
#    json.dump(MessageToJson(onnx_model), out_file)
    out_file.write(json.dumps(json.loads(MessageToJson(onnx_model)), indent=4, sort_keys=True))
