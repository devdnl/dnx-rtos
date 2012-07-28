# Makefile for GNU make
####################################################################################################
#
# AUTHOR: DANIEL ZORYCHTA
#
# Version: 20120628
#--------------------------------------------------------------------------------------------------
#
#
#    Copyright (C) 2011  Daniel Zorychta (daniel.zorychta@gmail.com)
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the  Free Software  Foundation;  either version 2 of the License, or
#    any later version.
#
#    This  program  is  distributed  in the hope that  it will be useful,
#    but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
#    MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
#    GNU General Public License for more details.
#
#    You  should  have received a copy  of the GNU General Public License
#    along  with  this  program;  if not,  write  to  the  Free  Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#
####################################################################################################

####################################################################################################
# INSERT HERE C SOURCES WHICH MUST BE COMPILED (AUTOMATIC ADDS PATHS AS HEADER PATHS)
####################################################################################################
CSRC = $(sort \
	app/terminal/terminal.c \
   cpu/stm32/stm32f10x_vectors.c \
   drivers/src/gpio.c \
   drivers/src/pll.c \
   drivers/src/uart.c \
   kernel/croutine.c \
   kernel/list.c \
   kernel/queue.c \
   kernel/tasks.c \
   kernel/timers.c \
   kernel/portable/GCC/ARM_CM3/port.c \
   kernel/portable/MemMang/heap_2.c \
   system/main.c \
   system/src/appruntime.c \
   system/src/hooks.c \
   system/src/idle.c \
   system/src/printf.c \
   )

####################################################################################################
# INSERT HERE C++ SOURCES WHICH MUST BE COMPILED (AUTOMATIC ADDS PATHS AS HEADER PATHS)
####################################################################################################
CXXSRC = $(sort \
   )

####################################################################################################
# INSERT HERE ASSEMBLER SOURCES WHICH MUST BE COMPILED (AUTOMATIC ADDS PATHS AS HEADER PATHS)
####################################################################################################
ASRC = $(sort \
   cpu/startup/cm_startup.s \
   )

####################################################################################################
# INSERT HERE PATHS WITH HEADER FILES ONLY
####################################################################################################
HDRLOC = $(sort $(dir $(CSRC)) $(dir $(CXXSRC)) \
   cfg \
   cpu \
   cpu/CMSIS \
   drivers \
   drivers/cfg \
   kernel/include \
   )

####################################################################################################
# PROJECT CONFIGURATION
####################################################################################################
# project name
PROJECT = main

# cpu type
CPU = cortex-m3

# mcu type
MCU = STM32F10X_CL

# optymalization
OPT = s

# linker script
LD_SCRIPT_micro = cpu/stm32/stm32f107xx.ld
LD_SCRIPT_qemu  = cpu/qemu/stm32f107xx_qemu.ld


####################################################################################################
####################################################################################################
# --------------------------------------------------------------------------------------------------
# object extension
OBJ_EXT = o

# c source extension
C_EXT = c

# C++ source extension
CXX_EXT = cpp

# assembler extension
AS_EXT = s

# --------------------------------------------------------------------------------------------------
# defines project path with binaries
BIN_LOC = 01_binaries

# defines project path with build info
INFO_LOC = 02_info

# defines object folder name
OBJ_LOC = Objects

# dependencies file name
DEP_FILE = depends.d

# --------------------------------------------------------------------------------------------------
# C compiler
CC = arm-none-eabi-gcc

# C++ compiler
CXX = arm-none-eabi-g++

# linker
LINKER = arm-none-eabi-gcc

# assembler
AS = arm-none-eabi-gcc -x assembler-with-cpp

# object copy program
OBJCOPY = arm-none-eabi-objcopy

# object copy program
OBJDUMP = arm-none-eabi-objdump

# size program
SIZE = arm-none-eabi-size

# --------------------------------------------------------------------------------------------------
# C compiler flags
CFLAGS_micro = -c -mcpu=$(CPU) -mthumb -O$(OPT) -ffunction-sections -fdata-sections -Wall \
               -Wstrict-prototypes -Wextra -std=gnu99 -g -ggdb3 -fverbose-asm -Wparentheses\
               -D$(MCU) -D$(TARGET)

CFLAGS_qemu  = $(CFLAGS_micro)

# C++ compiler flags
CXXFLAGS_micro =

CXXFLAGS_quemu =

# linker flags
LFLAGS_micro = -mcpu=$(CPU) -mthumb -T$(LD_SCRIPT_$(TARGET)) -g -nostartfiles -Wl,--gc-sections -Wall \
               -Wl,-Map=$(BIN_LOC)/$(TARGET)/$(PROJECT).map,--cref,--no-warn-mismatch -D$(TARGET)

LFLAGS_qemu  = $(LFLAGS_micro)

# assembler flags
AFLAGS_micro = -c -mcpu=$(CPU) -mthumb -g -ggdb3 -D$(TARGET)

AFLAGS_qemu  = $(AFLAGS_micro)

# --------------------------------------------------------------------------------------------------
# basic programs definitions
SHELL   = sh
RM      = rm -f
MKDIR   = mkdir -p
TEST    = test
DATE    = date
CAT     = cat
DEPAPP  = makedepend
WC      = wc
GREP    = grep
SIZEOF  = stat -c %s

# --------------------------------------------------------------------------------------------------
# defines this makefile name (do not edit)
THIS_MAKEFILE = $(firstword $(MAKEFILE_LIST))

# number of threads when objects are compiled (cpu count + 1) (do not edit)
THREAD = $(shell echo $$[ $$($(CAT) /proc/cpuinfo | $(GREP) processor | $(WC) -l) + 1 ])

# sets header search path (adds -I flags to paths) (do not edit)
SEARCHPATH = $(foreach var, $(HDRLOC),-I$(var))

# first make goal (do not edit)
FIRST_GOAL = $(firstword $(MAKECMDGOALS))

# main target without defined prefixes (do not edit)
TARGET = $(lastword $(subst _, ,$(MAKECMDGOALS)))

# target path (do not edit)
TARGET_PATH = $(BIN_LOC)/$(TARGET)

# object path (do not edit)
OBJ_PATH = $(BIN_LOC)/$(TARGET)/$(OBJ_LOC)

# defines sources (do not edit)
SRC = $(dir $(CSRC)) $(dir $(ASRC))

# defines objects names (do not edit)
OBJECTS = $(ASRC:.$(AS_EXT)=.$(OBJ_EXT)) \
          $(CSRC:.$(C_EXT)=.$(OBJ_EXT))  \
          $(CXXSRC:.$(CXX_EXT)=.$(OBJ_EXT))


####################################################################################################
####################################################################################################
# default target
####################################################################################################
.PHONY : all
all :
	@$(MAKE) -s -f$(THIS_MAKEFILE) micro

####################################################################################################
# help for this Makefile
####################################################################################################
.PHONY : help
help :
	@echo "This is help for this $(THIS_MAKEFILE)"
	@echo "Possible targets:"
	@echo "   all or no target    compile default target"
	@echo "   help                this help"
	@echo "   micro               compilation for ARM-Cortex-M3 arch"
	@echo "   qemu                compilation for ARM-Cortex-M3 arch for qemu"
	@echo "   clean               clean project"
	@echo "   cleanall            clean all non-project files"
	@echo ""
	@echo "Non-build targets:"
	@echo "   flash               flashing micro using openOCD debbuger"

####################################################################################################
# targets
####################################################################################################
.PHONY : micro qemu
micro : dependencies buildobjects linkobjects hex status
qemu  : dependencies buildobjects linkobjects hex status

####################################################################################################
# create basic output files like hex, bin, lst etc.
####################################################################################################
.PHONY : hex
hex :
	@echo 'Creating IHEX image...'
	@$(OBJCOPY) $(TARGET_PATH)/$(PROJECT).elf -O ihex $(TARGET_PATH)/$(PROJECT).hex

	@echo 'Creating binary image...'
	@$(OBJCOPY) $(TARGET_PATH)/$(PROJECT).elf -O binary $(TARGET_PATH)/$(PROJECT).bin

	@echo 'Creating memory dump...'
	@$(OBJDUMP) -x --syms $(TARGET_PATH)/$(PROJECT).elf > $(TARGET_PATH)/$(PROJECT).dmp

	@echo 'Creating extended listing....'
	@$(OBJDUMP) -S $(TARGET_PATH)/$(PROJECT).elf > $(TARGET_PATH)/$(PROJECT).lst

	@echo 'Size of modules:'
	@$(SIZE) -B -t --common $(foreach var,$(OBJECTS),$(OBJ_PATH)/$(var))

	@echo -e "Flash image size: $$($(SIZEOF) $(TARGET_PATH)/$(PROJECT).bin) B\n"

####################################################################################################
# show compile status
####################################################################################################
.PHONY : status
status :
	@$(MKDIR) $(INFO_LOC)
	@if ! $(TEST) -f $(INFO_LOC)/build; then echo "0" > $(INFO_LOC)/build; fi
	@echo $$(($$($(CAT) $(INFO_LOC)/build) + 1)) > $(INFO_LOC)/build

	@echo '/* application version created automatically by $(THIS_MAKEFILE) */' > $(INFO_LOC)/version.h
	@echo '#'ifndef DSYS_VERSION >> $(INFO_LOC)/version.h
	@echo '#'define DSYS_VERSION `$(DATE) "+%Y%m%d"`UL >> $(INFO_LOC)/version.h
	@echo '#'endif >> $(INFO_LOC)/version.h

	@echo "Build: `$(CAT) $(INFO_LOC)/build` `$(DATE) "+completed: %k:%M:%S"`"


####################################################################################################
####################################################################################################
# makes dependences
####################################################################################################
.PHONY : dependencies
dependencies :
	@echo "Creating dependencies for $(TARGET) target..."
	@$(MKDIR) $(TARGET_PATH)
	@echo "" > $(BIN_LOC)/$(DEP_FILE)
	@$(DEPAPP) -f$(BIN_LOC)/$(DEP_FILE) -p$(OBJ_PATH)/ -o.$(OBJ_EXT) $(SEARCHPATH) -Y -- $(CFLAGS_$(TARGET)) -- $(CSRC) $(CXXSRC) >& /dev/null
	@echo -e "$(foreach var,$(CSRC),\n$(OBJ_PATH)/$(var:.$(C_EXT)=.$(OBJ_EXT)) : $(var))" >> $(BIN_LOC)/$(DEP_FILE)
	@echo -e "$(foreach var,$(CXXSRC),\n$(OBJ_PATH)/$(var:.$(CXX_EXT)=.$(OBJ_EXT)) : $(var))" >> $(BIN_LOC)/$(DEP_FILE)
	@echo -e "$(foreach var,$(ASRC),\n$(OBJ_PATH)/$(var:.$(AS_EXT)=.$(OBJ_EXT)) : $(var))" >> $(BIN_LOC)/$(DEP_FILE)

####################################################################################################
# linking rules
####################################################################################################
.PHONY : linkobjects
linkobjects :
	@echo "Linking..."
	@$(MKDIR) $(TARGET_PATH)
	@$(RM) $(TARGET_PATH)/*.*
	@$(LINKER) $(LFLAGS_$(TARGET)) $(foreach var,$(OBJECTS),$(OBJ_PATH)/$(var)) -o $(TARGET_PATH)/$(PROJECT).elf

####################################################################################################
# build objects
####################################################################################################
.PHONY : buildobjects buildobjects_$(TARGET)
buildobjects :
	@echo "Starting building objects on $(THREAD) threads..."
	@$(MAKE) -s -j$(THREAD) -f$(THIS_MAKEFILE) buildobjects_$(TARGET)

buildobjects_$(TARGET) : $(foreach var,$(OBJECTS),$(OBJ_PATH)/$(var))

####################################################################################################
# rule used to compile object files from c sources
####################################################################################################
$(OBJ_PATH)/%.$(OBJ_EXT) : %.$(C_EXT)
	@echo "Building: $@..."
	@$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS_$(TARGET)) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(C_EXT))) -o $@

####################################################################################################
# rule used to compile object files from C++ sources
####################################################################################################
$(OBJ_PATH)/%.$(OBJ_EXT) : %.$(CXX_EXT)
	@echo "Building: $@..."
	@$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS_$(TARGET)) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(CXX_EXT))) -o $@

####################################################################################################
# rule used to compile object files from assembler sources
####################################################################################################
$(OBJ_PATH)/%.$(OBJ_EXT) : %.$(AS_EXT)
	@echo "Building: $@..."
	@$(MKDIR) $(dir $@)
	@$(AS) $(AFLAGS_$(TARGET)) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(AS_EXT))) -o $@

####################################################################################################
# clean target
####################################################################################################
.PHONY : cleantarget
cleantarget :
	@echo "Cleaning target..."
	-@$(RM) -r $(OBJ_PATH) $(LST_PATH)
	-@$(RM) $(BIN_LOC)/*.*

####################################################################################################
# clean all targets
####################################################################################################
.PHONY : clean
clean :
	@echo "Deleting all build files..."
	-@$(RM) -r $(BIN_LOC)/*

####################################################################################################
# clean up project (remove all files who arent project files!)
####################################################################################################
.PHONY : cleanall
cleanall:
	@echo "Cleaning up project..."
	-@$(RM) -r $(BIN_LOC) $(INFO_LOC)

####################################################################################################
# include all dependencies
####################################################################################################
-include $(BIN_LOC)/$(DEP_FILE)


####################################################################################################
# program Flash memory using openOCD and gdb
####################################################################################################
.PHONY : flash
flash :
	@echo -e "target remote localhost:3333\nmonitor reset halt\nload\nmonitor reset halt\nq\ny\n" > $(BIN_LOC)/gdbcmd
	@arm-none-eabi-gdb $(BIN_LOC)/micro/main.elf -x $(BIN_LOC)/gdbcmd
	-@$(RM) $(BIN_LOC)/gdbcmd

