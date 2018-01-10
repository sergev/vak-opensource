ARDUINO_DIR     = /Applications/Arduino.app/Contents/Java
ARDMK_DIR       = /usr/local/opt/arduino-mk
AVR_TOOLS_DIR   = /Applications/Arduino.app/Contents/Java/hardware/tools/avr
MONITOR_PORT    = /dev/tty.wchusbserialfd130
BOARD_TAG       = uno

include /usr/local/opt/arduino-mk/Arduino.mk

dis:
	$(AVR_TOOLS_DIR)/bin/avr-objdump -S build-uno/uno-gip.elf > result.dis
