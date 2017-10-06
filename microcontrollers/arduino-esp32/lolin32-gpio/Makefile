#ARDUINO_PATH    = /Applications/Arduino.app/Contents/Java
ARDUINO_PATH    = /opt/arduino-1.8.4

PROJ_BASE       = $(HOME)/Project/Arduino

#SKETCHBOOK      = $(HOME)/Library/Arduino15
SKETCHBOOK      = /home/sergev/.arduino15

ESPTOOL         = $(SKETCHBOOK)/packages/esp32/tools/esptool/0.4.9/esptool
SKETCH          = $(notdir $(CURDIR)).ino
TARGET_DIR      = $(CURDIR)/build-esp32
MONITOR_PORT    = /dev/ttyUSB1
BOARD           = espressif:esp32:lolin32:FlashFreq=80

all:
	@mkdir -p $(TARGET_DIR)
	$(ARDUINO_PATH)/arduino-builder -compile -logger=machine \
            -hardware "$(ARDUINO_PATH)/hardware" \
            -hardware "$(SKETCHBOOK)/packages" \
            -tools "$(ARDUINO_PATH)/tools-builder" \
            -tools "$(ARDUINO_PATH)/hardware/tools/avr" \
            -tools "$(SKETCHBOOK)/packages" \
            -built-in-libraries "$(ARDUINO_PATH)/libraries" \
            -libraries "$(PROJ_BASE)/libraries" \
            -fqbn="$(BOARD)" \
            -build-path "$(TARGET_DIR)" \
            -warnings=none \
            -prefs=build.warn_data_percentage=75 \
            -verbose "$(SKETCH)"

flash:
#TODO
	$(ESPTOOL) -v -cd nodemcu -cb 921600 -cp $(MONITOR_PORT) -ca 0x00000 -cf $(TARGET_DIR)/$(SKETCH).bin

upload: all flash

clean:
	rm -rf $(TARGET_DIR)
