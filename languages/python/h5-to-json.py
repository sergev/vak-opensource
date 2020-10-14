#!/usr/bin/env python3
#
# Load a saved Keras model.
# Convert it to JSON format.
# Save weights to a separate H5 file.
#
import sys

if len(sys.argv) != 2:
    print("Usage: h5-dump.py filename.h5")
    sys.exit(1)
filename = sys.argv[1]

# Load model.
import tensorflow as tf
model = tf.keras.models.load_model(filename)

# Summarize model.
#model.summary()

# Serialize model to JSON.
import json
with open("model.json", "w") as file:
    file.write(json.dumps(json.loads(model.to_json()), indent=4, sort_keys=True))

# Serialize weights to HDF5.
model.save_weights("model-weights.h5")
