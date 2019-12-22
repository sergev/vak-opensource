ifeq (,$(wildcard /Applications/Arduino.app/Contents/Java))
    # Linux
    ARDUINO_HOME= /opt/arduino-1.8.10
    PKG_HOME    = $(HOME)/.arduino15/packages
    SERIAL_PORT = /dev/ttyUSB0
else
    # Mac OS X
    ARDUINO_HOME= /Applications/Arduino.app/Contents/Java
    SERIAL_PORT = /dev/tty.wchusbserialfa1320
endif

PROJ_BASE       = $(HOME)/Project/Arduino
ESPTOOL_PY      = $(PKG_HOME)/esp32/tools/esptool_py/2.6.1/esptool.py
ESP32_TOOLS     = $(PKG_HOME)/esp32/hardware/esp32/1.0.4/tools/

SKETCH          = $(notdir $(CURDIR)).ino
BUILD_DIR       = $(CURDIR)/build-esp32
UPLOAD_SPEED    = 460800
BOARD           = esp32:esp32:lolin32:FlashFreq=80

all:
	@mkdir -p $(BUILD_DIR)
	$(ARDUINO_HOME)/arduino-builder -compile -logger=machine \
            -hardware "$(ARDUINO_HOME)/hardware" \
            -hardware "$(PKG_HOME)" \
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
	python $(ESPTOOL_PY) \
            --chip esp32 \
            --port $(SERIAL_PORT) \
            --baud $(UPLOAD_SPEED) \
            --before default_reset \
            --after hard_reset write_flash -z \
            --flash_mode dio \
            --flash_freq 80m \
            --flash_size 4MB \
            0xe000 $(ESP32_TOOLS)/partitions/boot_app0.bin \
            0x1000 $(ESP32_TOOLS)/sdk/bin/bootloader_dio_80m.bin \
            0x10000 $(BUILD_DIR)/$(SKETCH).bin \
            0X8000 $(BUILD_DIR)/$(SKETCH).partitions.bin

probe:
	python $(ESPTOOL_PY) --port $(SERIAL_PORT) flash_id

clean:
	rm -rf $(BUILD_DIR)
