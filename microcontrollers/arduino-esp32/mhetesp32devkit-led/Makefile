ifeq (,$(wildcard /Applications/Arduino.app/Contents/Java))
    # Linux
    ARDUINO_HOME= /opt/arduino-1.8.5
    SERIAL_PORT = /dev/ttyUSB1
else
    # Mac OS X
    ARDUINO_HOME= /Applications/Arduino.app/Contents/Java
    SERIAL_PORT = /dev/tty.SLAB_USBtoUART
endif

ESP32_TOOLS     = $(ARDUINO_HOME)/hardware/espressif/esp32/tools
PROJ_BASE       = $(HOME)/Project/Arduino

SKETCH          = $(notdir $(CURDIR)).ino
BUILD_DIR       = $(CURDIR)/build-esp32
UPLOAD_SPEED    = 460800
BOARD           = espressif:esp32:mhetesp32devkit:FlashFreq=80

all:
	@mkdir -p $(BUILD_DIR)
	$(ARDUINO_HOME)/arduino-builder -compile -logger=machine \
            -hardware "$(ARDUINO_HOME)/hardware" \
            -tools "$(ARDUINO_HOME)/tools-builder" \
            -tools "$(ARDUINO_HOME)/hardware/tools/avr" \
            -built-in-libraries "$(ARDUINO_HOME)/libraries" \
            -libraries "$(PROJ_BASE)/libraries" \
            -fqbn="$(BOARD)" \
            -build-path "$(BUILD_DIR)" \
            -warnings=none \
            -prefs=build.warn_data_percentage=75 \
            -verbose "$(SKETCH)"

upload: $(BUILD_DIR)/$(SKETCH).bin $(BUILD_DIR)/$(SKETCH).partitions.bin
	$(ESP32_TOOLS)/esptool.py \
            --chip esp32 \
            --port $(SERIAL_PORT) \
            --baud $(UPLOAD_SPEED) \
            --before default_reset \
            --after hard_reset write_flash -z \
            --flash_mode dio \
            --flash_freq 80m \
            --flash_size 4MB \
            0xe000 $(ESP32_TOOLS)/partitions/boot_app0.bin \
            0x1000 $(ESP32_TOOLS)/sdk/bin/bootloader.bin \
            0x10000 $(BUILD_DIR)/$(SKETCH).bin \
            0X8000 $(BUILD_DIR)/$(SKETCH).partitions.bin

clean:
	rm -rf $(BUILD_DIR)
