ifeq (,$(wildcard /Applications/Arduino.app/Contents/Java))
    # Linux
    ARDUINO_HOME= /opt/arduino-1.8.5
    SERIAL_PORT = /dev/ttyUSB1
else
    # Mac OS X
    ARDUINO_HOME= /Applications/Arduino.app/Contents/Java
    SERIAL_PORT = /dev/cu.usbmodemFD131
endif

PACKAGES_HOME   = /Users/vak/Library/Arduino15/packages
PROJ_BASE       = $(HOME)/Project/Arduino

SKETCH          = $(notdir $(CURDIR)).ino
BUILD_DIR       = $(CURDIR)/build-pic32
BOARD           = chipKIT:pic32:lenny

all:
	@mkdir -p $(BUILD_DIR)
	$(ARDUINO_HOME)/arduino-builder -compile -logger=machine \
            -hardware "$(ARDUINO_HOME)/hardware" \
            -hardware /Users/vak/Library/Arduino15/packages \
            -tools "$(ARDUINO_HOME)/tools-builder" \
            -tools "$(ARDUINO_HOME)/hardware/tools/avr" \
            -built-in-libraries "$(ARDUINO_HOME)/libraries" \
            -libraries "$(PROJ_BASE)/libraries" \
            -fqbn="$(BOARD)" \
            -build-path "$(BUILD_DIR)" \
            -warnings=none \
            -prefs=build.warn_data_percentage=75 \
            -prefs=runtime.tools.pic32-tools.path="$(PACKAGES_HOME)/chipKIT/tools/pic32-tools/1.43-pic32gcc" \
            -verbose "$(SKETCH)"

upload: $(BUILD_DIR)/$(SKETCH).hex
	$(PACKAGES_HOME)/chipKIT/tools/pic32prog/v2.1.24/pic32prog \
            -d $(SERIAL_PORT) \
            $(BUILD_DIR)/$(SKETCH).hex

clean:
	rm -rf $(BUILD_DIR)
