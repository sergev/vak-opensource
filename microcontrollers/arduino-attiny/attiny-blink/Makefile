ifeq (,$(wildcard /Applications/Arduino.app/Contents/Java))
    # Linux: use apt install arduino-mk
    ARDUINO_DIR = /opt/arduino-1.8.5
    ARDMK_DIR   = /usr/share/arduino
else
    # Mac OS: use brew install arduino-mk
    ARDUINO_DIR = /Applications/Arduino.app/Contents/Java
    ARDMK_DIR   = /usr/local/opt/arduino-mk
endif

AVR_TOOLS_DIR   = $(ARDUINO_DIR)/hardware/tools/avr
AVRDUDE_CONF    = $(AVR_TOOLS_DIR)/etc/avrdude.conf
CORE            = microcore
BOARD_TAG       = attiny13
MCU             = attiny13a
AVRDUDE_MCU     = attiny13
#AVRDUDE_MCU     = attiny85
F_CPU           = 9600000
HEX_MAXIMUM_SIZE= 1024
CPPFLAGS        = -Werror
#CXXFLAGS        += -std=gnu++14 -fno-threadsafe-statics

#
# Use: make ispload
#
ISP_PROG        = avrisp2
#ISP_PROG        = usbasp
ISP_PORT        = usb
AVRDUDE_OPTS    = -v

include $(ARDMK_DIR)/Arduino.mk
