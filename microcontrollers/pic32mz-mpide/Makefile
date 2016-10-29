#
# Example of makefile to build and upload MPIDE sketches
# from the command line.
# Copyright (C) 2014 Serge Vakulenko.
# This software is distributed under the Apache License;
# http://www.apache.org/licenses/LICENSE-2.0
#
APP             = Blink

OBJS            = tmp/$(APP).cpp.o \
                  tmp/cpp-startup.S.o \
                  tmp/crti.S.o \
                  tmp/crtn.S.o \
                  tmp/pic32_software_reset.S.o \
                  tmp/vector_table.S.o \
                  tmp/exceptions.c.o \
                  tmp/HardwareSerial_cdcacm.c.o \
                  tmp/HardwareSerial_usb.c.o \
                  tmp/pins_arduino.c.o \
                  tmp/task_manager.c.o \
                  tmp/WInterrupts.c.o \
                  tmp/wiring.c.o \
                  tmp/wiring_analog.c.o \
                  tmp/wiring_digital.c.o \
                  tmp/wiring_pulse.c.o \
                  tmp/wiring_shift.c.o \
                  tmp/WSystem.c.o \
                  tmp/HardwareSerial.cpp.o \
                  tmp/main.cpp.o \
                  tmp/Print.cpp.o \
                  tmp/Tone.cpp.o \
                  tmp/WMath.cpp.o \
                  tmp/WString.cpp.o

ifndef MPIDE
ifneq (,$(wildcard /opt/mpide-0023-linux32-20140605-test))
    # Linux
    MPIDE       = /opt/mpide-0023-linux32-20140605-test/hardware
endif
endif
ifndef MPIDE
ifneq (,$(wildcard /Applications/mpide.app))
    # Mac OS X
    MPIDE       = /Applications/mpide.app/Contents/Resources/Java/hardware
endif
endif

GNU             = $(MPIDE)/pic32/compiler/pic32-tools/bin/pic32-

DEFS            = -mprocessor=32MZ2048ECG100 -DF_CPU=200000000UL -DARDUINO=23 \
                  -D_BOARD_WIFIRE_ -DMPIDEVER=16777996 -DMPIDE=23 \
                  -I$(MPIDE)/pic32/cores/pic32 -I$(MPIDE)/pic32/variants/WiFire

CFLAGS          = -O2 -mno-smart-io -w -ffunction-sections -fdata-sections \
                  -g3 -mdebugger -Wcast-align -fno-short-double -fframe-base-loclist \
                  $(DEFS)

CXXFLAGS        = -O2 -mno-smart-io -w -fno-exceptions -ffunction-sections \
                  -fdata-sections -g3 -mdebugger -Wcast-align -fno-short-double \
                  -fframe-base-loclist $(DEFS) -I.

ASFLAGS         = -O2 -g1 -Wa,--gdwarf-2 $(DEFS)

LDFLAGS         = -Wl,--gc-sections -mdebugger -mno-peripheral-libs \
                  -nostartfiles -mprocessor=32MZ2048ECG100 \
                  -T $(MPIDE)/pic32/variants/WiFire/MZ-application-32MZ2048ECX.ld \
                  -T $(MPIDE)/pic32/cores/pic32/chipKIT-application-COMMON-MZ.ld

LIBS            = -lm

COPYFLAGS       = -j .eeprom --set-section-flags=.eeprom=alloc,load \
                    --no-change-warnings --change-section-lma .eeprom=0

ifneq (,$(wildcard $(MPIDE)/tools/avrdude))
    # Linux
    PROGTOOL    = $(MPIDE)/tools/avrdude \
                  -C $(MPIDE)/tools/avrdude.conf -P /dev/ttyUSB0
endif
ifneq (,$(wildcard $(MPIDE)/tools/avr/bin/avrdude))
    # Mac OS X
    PROGTOOL    = $(MPIDE)/tools/avr/bin/avrdude \
                  -C $(MPIDE)/tools/avr/etc/avrdude.conf \
                  -P /dev/tty.usbserial-*
endif
PROGTOOL        += -c stk500v2 -p 32MZ2048ECG100 -b 115200

VPATH           = . $(MPIDE)/pic32/cores/pic32

all:            tmp tmp/$(APP).hex

load:           tmp/$(APP).hex
		$(PROGTOOL) -D -U flash:w:tmp/$(APP).hex:i

clean:
		rm -rf tmp

tmp:
		@mkdir tmp

tmp/$(APP).cpp.o: $(APP).pde
		(echo '#include "WProgram.h"'; cat $<) > tmp/$(APP).cpp
		$(GNU)g++ -c $(CXXFLAGS) tmp/$(APP).cpp -o $@

tmp/$(APP).hex: $(OBJS)
		$(GNU)ar -cr tmp/code.a $(OBJS)
		$(GNU)g++ $(LDFLAGS) tmp/code.a $(LIBS) -Wl,-Map=tmp/$(APP).map -o tmp/$(APP).elf
		$(GNU)objcopy -O ihex $(COPYFLAGS) tmp/$(APP).elf tmp/$(APP).eep
		$(GNU)objdump -d -S tmp/$(APP).elf > tmp/$(APP).dis
		$(GNU)bin2hex -a tmp/$(APP).elf

tmp/%.c.o:      %.c
		$(GNU)gcc -c $(CFLAGS) $< -o $@

tmp/%.cpp.o:    %.cpp
		$(GNU)g++ -c $(CXXFLAGS) $< -o $@

tmp/%.S.o:      %.S
		$(GNU)g++ -c $(ASFLAGS) $< -o $@
