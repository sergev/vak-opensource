# Ubuntu: use package arduino.mk
ARDUINO_DIR     = /opt/arduino-1.8.5
ARDMK_DIR       = /usr/share/arduino

AVR_TOOLS_DIR   = $(ARDUINO_DIR)/hardware/tools/avr
AVRDUDE_CONF    = $(AVR_TOOLS_DIR)/etc/avrdude.conf
BOARD_TAG       = mega
BOARD_SUB       = atmega2560
MONITOR_PORT    = /dev/ttyUSB0

include $(ARDMK_DIR)/Arduino.mk
