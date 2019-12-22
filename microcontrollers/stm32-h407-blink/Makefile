# 	Makefile for compiling the Getting Started project

#-------------------------------------------------------------------------------
#		User-modifiable options
#-------------------------------------------------------------------------------

# Trace level used for compilation
# (can be overriden by adding TRACE_LEVEL=#number to the command-line)
# TRACE_LEVEL_DEBUG      5
# TRACE_LEVEL_INFO       4
# TRACE_LEVEL_WARNING    3
# TRACE_LEVEL_ERROR      2
# TRACE_LEVEL_FATAL      1
# TRACE_LEVEL_NO_TRACE   0
TRACE_LEVEL = 4

# Optimization level
OPTIMIZATION = -O1

# Output file basename
OUTPUT = main

# Output directories
BIN = .
OBJ = obj

# library dirs
LIBRARYSRC = ./lib/src

STARTUPFILE = ./lib/startup_stm32f4xx.s

#-------------------------------------------------------------------------------
#		Tools
#-------------------------------------------------------------------------------

# Tool suffix when cross-compiling
CROSS_COMPILE = arm-none-eabi-

CC = $(CROSS_COMPILE)gcc
SIZE = $(CROSS_COMPILE)size
STRIP = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
LD = $(CROSS_COMPILE)ld
AS = $(CROSS_COMPILE)as

#-------------------------------------------------------------------------------
#		Files
#-------------------------------------------------------------------------------

# include folders
INCLUDES = -I./
INCLUDES += -I./lib/
INCLUDES += -I./lib/inc/
INCLUDES += -I./lib/inc/../../

# Objects built from C source files
C_OBJECTS = $(OBJ)/main.o
C_OBJECTS += $(OBJ)/system_stm32f4xx.o
C_OBJECTS += $(OBJ)/stm32f4xx_gpio.o
C_OBJECTS += $(OBJ)/stm32f4xx_rcc.o
C_OBJECTS += $(OBJ)/stm32f4xx_it.o

# Objects built from Assembly source files
ASM_OBJECTS = $(OBJ)/startup_stm32f4xx.o

LINKER_SCRIPT = ./lib/stm32f4xx_flash.ld
#LINKER_SCRIPT = ./lib/stm32f4xx_flash_extsram.ld

# Append OBJ and BIN directories to output filename
OUTPUT := $(BIN)/$(OUTPUT)

#-------------------------------------------------------------------------------
#		Rules
#-------------------------------------------------------------------------------

# Flags
CFLAGS = -Wall -fno-common -c -g -mcpu=cortex-m3 -mthumb
CFLAGS += -g $(OPTIMIZATION) $(INCLUDES) -DTRACE_LEVEL=$(TRACE_LEVEL)
ASFLAGS = -g -mapcs-32
LDFLAGS = -g -v -nostartfiles
OBJCOPYFLAGS = -O binary
#OBJDUMPFLAGS = -x --syms -S
OBJDUMPFLAGS = -D

all: $(BIN) $(OBJ) $(OUTPUT).out

$(BIN) $(OBJ):
	mkdir $@

$(OUTPUT).out: $(C_OBJECTS) $(ASM_OBJECTS) $(LINKER_SCRIPT)
	@ echo "..linking"
	$(LD) $(LDFLAGS) -Map $(OUTPUT).map -T$(LINKER_SCRIPT) -o $(OUTPUT).out $(C_OBJECTS) $(ASM_OBJECTS)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(OUTPUT).out $(OUTPUT).bin
	$(OBJDUMP) $(OBJDUMPFLAGS) $(OUTPUT).out > $(OUTPUT).dis
	@ echo "...completed."

$(C_OBJECTS): main.c system_stm32f4xx.c
	@ echo ".compiling"
	$(CC) $(CFLAGS) -o $(OBJ)/main.o main.c
	$(CC) $(CFLAGS) -o $(OBJ)/system_stm32f4xx.o system_stm32f4xx.c
	$(CC) $(CFLAGS) -o $(OBJ)/stm32f4xx_it.o stm32f4xx_it.c
	@ echo ".compiling libraries"
	$(CC) $(CFLAGS) -o $(OBJ)/stm32f4xx_gpio.o $(LIBRARYSRC)/stm32f4xx_gpio.c
	$(CC) $(CFLAGS) -o $(OBJ)/stm32f4xx_rcc.o $(LIBRARYSRC)/stm32f4xx_rcc.c

$(ASM_OBJECTS): $(STARTUPFILE)
	@ echo ".assembling"
	$(AS) $(ASFLAGS) -o $(OBJ)/startup_stm32f4xx.o $(STARTUPFILE)

clean:
	-rm -f $(OBJ)/*.o $(BIN)/*.out $(BIN)/*.bin $(BIN)/*.dmp $(BIN)/*.map $(BIN)/*.dis
