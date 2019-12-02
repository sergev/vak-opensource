# Generic makefile for ESP32 IDF project.

# Build the project.
all:
	if [ ! -f sdkconfig ]; then cp -a defconfig sdkconfig; idf.py fullclean; fi
	idf.py build

# Delete the entire build directory contents.
clean:
	idf.py fullclean

# Flash the project.
flash:
	idf.py flash

# Print basic size information about the app.
size:
	idf.py size

# Run "menuconfig" project configuration tool.
menuconfig:
	idf.py menuconfig

# Re-run CMake.
reconfigure:
	idf.py reconfigure

# Display serial output.
monitor:
	idf.py monitor
