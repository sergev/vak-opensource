#!/usr/bin/env python3
#
# Load a saved Keras model.
# Convert it to YAML format.
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

# Serialize model to YAML.
model_yaml = model.to_yaml()
with open("model.yaml", "w") as yaml_file:
    yaml_file.write(model_yaml)

# Serialize weights to HDF5.
model.save_weights("model-weights.h5")
