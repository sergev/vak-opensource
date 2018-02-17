ifeq (,$(wildcard /Applications/Arduino.app/Contents/Java))
    # Linux: use apt install arduino-mk
    ARDUINO_DIR = /opt/arduino-1.8.5
    ARDMK_DIR   = /usr/share/arduino
    MONITOR_PORT= /dev/ttyUSB0
else
    # Mac OS: use brew install arduino-mk
    ARDUINO_DIR = /Applications/Arduino.app/Contents/Java
    ARDMK_DIR   = /usr/local/opt/arduino-mk
    MONITOR_PORT= /dev/tty.wchusbserialfd130
endif

AVR_TOOLS_DIR   = $(ARDUINO_DIR)/hardware/tools/avr
AVRDUDE_CONF    = $(AVR_TOOLS_DIR)/etc/avrdude.conf
BOARD_TAG       = mega
BOARD_SUB       = atmega2560
GITCOUNT        = $(shell git rev-list HEAD --count)
CPPFLAGS        = -Werror -DGITCOUNT='"$(GITCOUNT)"'
CXXFLAGS        += -std=gnu++14 -fno-threadsafe-statics

include $(ARDMK_DIR)/Arduino.mk
