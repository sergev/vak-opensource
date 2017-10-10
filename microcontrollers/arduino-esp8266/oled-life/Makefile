#ARDUINO_HOME    = /Applications/Arduino.app/Contents/Java
ARDUINO_HOME    = /opt/arduino-1.8.4

#SKETCHBOOK      = $(HOME)/Library/Arduino15
SKETCHBOOK      = /home/sergev/.arduino15

PROJ_BASE       = $(HOME)/Project/Arduino
ESPTOOL         = $(SKETCHBOOK)/packages/esp8266/tools/esptool/0.4.9/esptool
SKETCH          = $(notdir $(CURDIR)).ino
TARGET_DIR      = $(CURDIR)/build-esp8266
#SERIAL_PORT     = /dev/tty.SLAB_USBtoUART
SERIAL_PORT     = /dev/ttyUSB1
BOARD           = esp8266:esp8266:nodemcuv2:CpuFrequency=80,FlashSize=4M3M

all:
	@mkdir -p $(TARGET_DIR)
	$(ARDUINO_HOME)/arduino-builder -compile -logger=machine \
            -hardware "$(ARDUINO_HOME)/hardware" \
            -hardware "$(SKETCHBOOK)/packages" \
            -tools "$(ARDUINO_HOME)/tools-builder" \
            -tools "$(ARDUINO_HOME)/hardware/tools/avr" \
            -tools "$(SKETCHBOOK)/packages" \
            -built-in-libraries "$(ARDUINO_HOME)/libraries" \
            -libraries "$(PROJ_BASE)/libraries" \
            -fqbn="$(BOARD)" \
            -build-path "$(TARGET_DIR)" \
            -warnings=none \
            -prefs=build.warn_data_percentage=75 \
            -verbose "$(SKETCH)"

upload: $(TARGET_DIR)/$(SKETCH).bin
	$(ESPTOOL) -v -cd nodemcu -cb 921600 -cp $(SERIAL_PORT) -ca 0x00000 -cf $(TARGET_DIR)/$(SKETCH).bin

clean:
	rm -rf $(TARGET_DIR)
