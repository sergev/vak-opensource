ARDUINO_PATH    = /Applications/Arduino.app/Contents/Java
PROJ_BASE       = $(HOME)/Project/Arduino
SKETCHBOOK      = $(HOME)/Library/Arduino15
ESPTOOL         = $(SKETCHBOOK)/packages/esp8266/tools/esptool/0.4.9/esptool
SKETCH          = $(notdir $(CURDIR)).ino
TARGET_DIR      = $(CURDIR)/build-esp8266
MONITOR_PORT    = /dev/tty.SLAB_USBtoUART
BOARD           = esp8266:esp8266:nodemcuv2:CpuFrequency=80,FlashSize=4M3M

all:
	@ mkdir -p $(TARGET_DIR)

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
	$(ESPTOOL) -v -cd nodemcu -cb 460800 -cp $(MONITOR_PORT) -ca 0x00000 -cf $(TARGET_DIR)/$(SKETCH).bin

upload: all flash

clean:
	rm -rf $(TARGET_DIR)
