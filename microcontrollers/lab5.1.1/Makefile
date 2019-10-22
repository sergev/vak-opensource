#
# MPIDE_DIR    - Where the chipKIT MPIDE software has been unpacked
# ARDUINO_PORT - The port where the Arduino can be found. Only needed
#                when uploading.
# ARDUINO_LIBS - A list of any libraries used by the sketch (we assume
#                these are in $(MPIDE_DIR)/hardware/libraries
# BOARD_TAG    - The tag for the board e.g. uno or mega
#                'make show_boards' shows a list
#
# Once this file has been created the typical workflow is just
#
#   $ make upload
#
# All of the object files are created in the build subdirectory.
# All sources should be in the current directory and can include
# any number of .c, .cpp, .s and .h files.
#
# Besides make upload you can also
#   make             - no upload
#   make clean       - remove all our dependencies
#   make depends     - update dependencies
#   make reset       - reset the Arduino by tickling DTR on the serial port
#   make raw_upload  - upload without first resetting
#   make show_boards - list all the boards defined in boards.txt
#
#MPIDE_DIR       = /opt/mpide-0022-linux32-20110822
MPIDE_DIR       = /opt/mpide-0023-linux-20120903

ARDUINO_PORT    = /dev/ttyUSB*

BOARD_TAG       = uno_pic32

########################################################################
#
# chipKIT extensions for Arduino Makefile
# System part (i.e. project independent)
#
# Copyright (C) 2011 Christopher Peplin <chris.peplin@rhubarbtech.com>,
# based on work that is Copyright Martin Oldfield
#
# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.
#
ARDUINO_CORE_PATH = $(MPIDE_DIR)/hardware/pic32/cores/pic32
ARDUINO_LIB_PATH = $(MPIDE_DIR)/hardware/pic32/libraries
BOARDS_TXT      = $(MPIDE_DIR)/hardware/pic32/boards.txt
VARIANTS_PATH   = $(MPIDE_DIR)/hardware/pic32/variants
ARDUINO_VERSION = 23

LDSCRIPT        = $(call PARSE_BOARD,$(BOARD_TAG),ldscript)
LDSCRIPT_FILE   = $(ARDUINO_CORE_PATH)/$(LDSCRIPT)

EXTRA_LDFLAGS   = -T$(ARDUINO_CORE_PATH)/$(LDSCRIPT)
EXTRA_CXXFLAGS  = -mno-smart-io -D$(BOARD)

# Compiler is distributed with the MPIDE
GCC_TOOLS_PATH  = $(MPIDE_DIR)/hardware/pic32/compiler/pic32-tools/bin/
CC              := $(addprefix $(GCC_TOOLS_PATH),pic32-gcc)
CXX             := $(addprefix $(GCC_TOOLS_PATH),pic32-g++)
OBJCOPY         := $(addprefix $(GCC_TOOLS_PATH),pic32-objcopy)
OBJDUMP         := $(addprefix $(GCC_TOOLS_PATH),pic32-objdump)
AR              := $(addprefix $(GCC_TOOLS_PATH),pic32-ar)
SIZE            := $(addprefix $(GCC_TOOLS_PATH),pic32-size)
NM              := $(addprefix $(GCC_TOOLS_PATH),pic32-nm)

########################################################################
#
# Arduino command line tools Makefile
# System part (i.e. project independent)
#
# Copyright (C) 2010 Martin Oldfield <m@mjo.tc>, based on work that is
# Copyright Nicholas Zambetti, David A. Mellis & Hernando Barragan
#
# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.
#
# Adapted from Arduino 0011 Makefile by M J Oldfield
#
# Original Arduino adaptation by mellis, eighthave, oli.keller
#
# Modified by Christopher Peplin for chipKIT.
#

########################################################################
# boards.txt parsing
#
ifndef BOARD_TAG
BOARD_TAG       = uno
endif

ifndef BOARDS_TXT
BOARDS_TXT      = $(MPIDE_DIR)/hardware/arduino/boards.txt
endif

ifndef PARSE_BOARD
# result = $(call READ_BOARD_TXT, 'boardname', 'parameter')
PARSE_BOARD     = $(shell grep $(1).$(2) $(BOARDS_TXT) | cut -d = -f 2 )
endif

# processor stuff
ifndef MCU
MCU             = $(call PARSE_BOARD,$(BOARD_TAG),build.mcu)
endif

ifndef F_CPU
F_CPU           = $(call PARSE_BOARD,$(BOARD_TAG),build.f_cpu)
endif

# normal programming info
ifndef AVRDUDE_PROTOCOL
AVRDUDE_PROTOCOL = $(call PARSE_BOARD,$(BOARD_TAG),upload.protocol)
endif

ifndef AVRDUDE_BAUDRATE
AVRDUDE_BAUDRATE = $(call PARSE_BOARD,$(BOARD_TAG),upload.speed)
endif

ifndef VARIANT
VARIANT         = $(call PARSE_BOARD,$(BOARD_TAG),build.variant)
endif

ifndef BOARD
BOARD           = $(call PARSE_BOARD,$(BOARD_TAG),board)
endif

# Everything gets built in here
OBJDIR          = build

########################################################################
# Local sources
#
LOCAL_C_SRCS    = $(wildcard *.c)
LOCAL_CPP_SRCS  = $(wildcard *.cpp)
LOCAL_CC_SRCS   = $(wildcard *.cc)
LOCAL_AS_SRCS   = $(wildcard *.S)
LOCAL_OBJ_FILES = $(LOCAL_C_SRCS:.c=.o) $(LOCAL_CPP_SRCS:.cpp=.o) \
		  $(LOCAL_CC_SRCS:.cc=.o) $(LOCAL_AS_SRCS:.S=.o)
LOCAL_OBJS      = $(patsubst %,build/%,$(LOCAL_OBJ_FILES))

# Dependency files
DEPS            = $(LOCAL_OBJS:.o=.d)

# core sources
ifeq ($(strip $(NO_CORE)),)
ifdef ARDUINO_CORE_PATH
CORE_C_SRCS     = $(wildcard $(ARDUINO_CORE_PATH)/*.c)
CORE_CPP_SRCS   = $(wildcard $(ARDUINO_CORE_PATH)/*.cpp)

ifneq ($(strip $(NO_CORE_MAIN_FUNCTION)),)
CORE_CPP_SRCS   := $(filter-out %main.cpp, $(CORE_CPP_SRCS))
endif

CORE_OBJ_FILES  = $(CORE_C_SRCS:.c=.o) $(CORE_CPP_SRCS:.cpp=.o)
CORE_OBJS       = $(patsubst $(ARDUINO_CORE_PATH)/%, \
                  build/%,$(CORE_OBJ_FILES))
endif
endif

########################################################################
# Rules for making stuff
#

# The name of the main targets
TARGET_HEX      = build/target.hex
TARGET_ELF      = build/target.elf
TARGETS         = build/target.*
CORE_LIB        = build/libcore.a

# A list of dependencies
DEP_FILE        = build/depends.mk

# General arguments
SYS_LIBS        = $(patsubst %,$(ARDUINO_LIB_PATH)/%,$(ARDUINO_LIBS))
SYS_INCLUDES    = $(patsubst %,-I%,$(SYS_LIBS))
SYS_OBJS        = $(wildcard $(patsubst %,%/*.o,$(SYS_LIBS)))
LIB_CPP_SRC     = $(wildcard $(patsubst %,%/*.cpp,$(SYS_LIBS)))
LIB_C_SRC       = $(wildcard $(patsubst %,%/*.c,$(SYS_LIBS)))
LIB_OBJS        = $(patsubst $(ARDUINO_LIB_PATH)/%.cpp,build/libs/%.o,$(LIB_CPP_SRC))
LIB_OBJS        += $(patsubst $(ARDUINO_LIB_PATH)/%.c,build/libs/%.o,$(LIB_C_SRC))

CPPFLAGS        = -mprocessor=$(MCU) -DF_CPU=$(F_CPU) \
		  -DARDUINO=$(ARDUINO_VERSION) \
		  -I. -I$(ARDUINO_CORE_PATH) \
		  -I$(VARIANTS_PATH)/$(VARIANT) \
		  $(SYS_INCLUDES) -w -Wall -fno-exceptions\
		  -ffunction-sections -fdata-sections $(EXTRA_CXXFLAGS)

ifdef DEBUG
CPPFLAGS        += -O0 -g -mdebugger
else
CPPFLAGS        += -O2
endif

CXXFLAGS        = -fno-exceptions
ASFLAGS         = -mprocessor=$(MCU) -I. -x assembler-with-cpp
LDFLAGS         = -mprocessor=$(MCU) -lm -Wl,--gc-sections -Os $(EXTRA_LDFLAGS)

# Expand and pick the first port
ARD_PORT        = $(firstword $(wildcard $(ARDUINO_PORT)))

# Implicit rules for building everything (needed to get everything in
# the right directory)
#
# Rather than mess around with VPATH there are quasi-duplicate rules
# here for building e.g. a system C++ file and a local C++
# file. Besides making things simpler now, this would also make it
# easy to change the build options in future

# library sources
build/libs/%.o: $(ARDUINO_LIB_PATH)/%.cpp
		mkdir -p $(dir $@)
		$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@
build/libs/%.o: $(ARDUINO_LIB_PATH)/%.c
		mkdir -p $(dir $@)
		$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

# normal local sources
# .o rules are for objects, .d for dependency tracking
# there seems to be an awful lot of duplication here!!!
build/%.o:      %.c
		$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

build/%.o:      %.cc
		$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

build/%.o:      %.cpp
		$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

build/%.o:      %.S
		$(CC) -c $(CPPFLAGS) $(ASFLAGS) $< -o $@

build/%.o:      %.s
		$(CC) -c $(CPPFLAGS) $(ASFLAGS) $< -o $@

build/%.d:      %.c
		$(CC) -MM $(CPPFLAGS) $(CFLAGS) $< -MF $@ -MT $(@:.d=.o)

build/%.d:      %.cc
		$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< -MF $@ -MT $(@:.d=.o)

build/%.d:      %.cpp
		$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< -MF $@ -MT $(@:.d=.o)

build/%.d:      %.S
		$(CC) -MM $(CPPFLAGS) $(ASFLAGS) $< -MF $@ -MT $(@:.d=.o)

build/%.d:      %.s
		$(CC) -MM $(CPPFLAGS) $(ASFLAGS) $< -MF $@ -MT $(@:.d=.o)

build/%.o:      build/%.cpp
		$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

build/%.d:      build/%.cpp
		$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< -MF $@ -MT $(@:.d=.o)

# core files
build/%.o:      $(ARDUINO_CORE_PATH)/%.c
		$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

build/%.o:      $(ARDUINO_CORE_PATH)/%.cpp
		$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

# various object conversions
build/%.hex:    build/%.elf
		$(OBJCOPY) -O ihex -R .eeprom $< $@

build/%.eep:    build/%.elf
		-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
			--change-section-lma .eeprom=0 -O ihex $< $@

build/%.lss:    build/%.elf
		$(OBJDUMP) -h -S $< > $@

build/%.sym:    build/%.elf
		$(NM) -n $< > $@

########################################################################
#
# Avrdude
#
AVRDUDE_TOOLS_PATH = $(MPIDE_DIR)/hardware/tools/avr/bin
AVRDUDE_ETC_PATH = $(MPIDE_DIR)/hardware/tools/avr/etc

ifeq ($(wildcard $(AVRDUDE_ETC_PATH)),)
AVRDUDE_TOOLS_PATH = $(MPIDE_DIR)/hardware/tools
AVRDUDE_ETC_PATH = $(AVRDUDE_TOOLS_PATH)
endif

AVRDUDE         = $(AVRDUDE_TOOLS_PATH)/avrdude
AVRDUDE_OPTS    = -q -V -p $(MCU) -C $(AVRDUDE_ETC_PATH)/avrdude.conf
AVRDUDE_OPTS    += -c $(AVRDUDE_PROTOCOL) -b $(AVRDUDE_BAUDRATE) -P $(ARD_PORT)

########################################################################
#
# Explicit targets start here
#

all: 		build $(TARGET_HEX)

build:
		mkdir build

$(TARGET_ELF): 	$(LOCAL_OBJS) $(CORE_LIB) $(OTHER_OBJS)
		$(CC) $(LDFLAGS) -o $@ $(LOCAL_OBJS) $(CORE_LIB) $(OTHER_OBJS) -lc -lm

$(CORE_LIB):	$(CORE_OBJS) $(LIB_OBJS)
		$(AR) rcs $@ $(CORE_OBJS) $(LIB_OBJS)

$(DEP_FILE):	build $(DEPS)
		@cat $(DEPS) > $(DEP_FILE)

upload:		reset raw_upload

raw_upload:	$(TARGET_HEX)
		$(AVRDUDE) $(AVRDUDE_OPTS) -U flash:w:$(TARGET_HEX):i

# stty on MacOS likes -F, but on Debian it likes -f redirecting
# stdin/out appears to work but generates a spurious error on MacOS at
# least. Perhaps it would be better to just do it in perl ?
reset:
		@if [ -z "$(ARD_PORT)" ]; then \
			echo "No Arduino-compatible TTY device found -- exiting"; exit 2; \
			fi
		for STTYF in 'stty --file' 'stty -f' 'stty <'; do \
			$$STTYF /dev/tty >/dev/null 2>/dev/null && break; \
		done ;\
		$$STTYF $(ARD_PORT)  hupcl ;\
		(sleep 0.1 || sleep 1)     ;\
		$$STTYF $(ARD_PORT) -hupcl

serial:
		minicom -D $(ARD_PORT)

clean:
		rm -r build

depends:	$(DEPS)
		@cat $(DEPS) > $(DEP_FILE)

size:		build $(TARGET_HEX)
		$(SIZE) $(TARGET_HEX)

show_boards:
		@cat $(BOARDS_TXT) | grep -E "^[[:alnum:]]" | cut -d . -f 1 | uniq

.PHONY:         all clean depends upload raw_upload reset show_boards

ifneq ($(wildcard $(DEP_FILE)),)
include $(DEP_FILE)
endif
