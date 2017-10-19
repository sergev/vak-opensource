ifeq (,$(wildcard /Applications/Arduino.app/Contents/Java))
    # Linux
    ARDUINO_HOME= /opt/arduino-1.8.5
    SERIAL_PORT = /dev/ttyUSB1
else
    # Mac OS X
    ARDUINO_HOME= /Applications/Arduino.app/Contents/Java
    SERIAL_PORT = /dev/tty.SLAB_USBtoUART
endif

SKETCH          = $(notdir $(CURDIR)).ino
TARGET_DIR      = $(CURDIR)/build-esp8266
LIBRARIES       = $(HOME)/Project/Arduino/libraries
BOARD           = esp8266com:esp8266:d1_mini:CpuFrequency=80,FlashSize=4M3M

all:
	@mkdir -p $(TARGET_DIR)
	$(ARDUINO_HOME)/arduino-builder -compile -logger=machine \
            -hardware "$(ARDUINO_HOME)/hardware" \
            -tools "$(ARDUINO_HOME)/tools-builder" \
            -tools "$(ARDUINO_HOME)/hardware/tools/avr" \
            -built-in-libraries "$(ARDUINO_HOME)/libraries" \
            -libraries "$(LIBRARIES)" \
            -libraries "$(ARDUINO_HOME)/hardware/esp8266com/esp8266/libraries" \
            -fqbn="$(BOARD)" \
            -build-path "$(TARGET_DIR)" \
            -warnings=none \
            -prefs=build.warn_data_percentage=75 \
            -verbose "$(SKETCH)"

upload: $(TARGET_DIR)/$(SKETCH).bin
	$(ARDUINO_HOME)/hardware/esp8266com/esp8266/tools/esptool/esptool \
            -v -cd nodemcu -cb 921600 -cp $(SERIAL_PORT) -ca 0x00000 -cf $(TARGET_DIR)/$(SKETCH).bin

clean:
	rm -rf $(TARGET_DIR)
