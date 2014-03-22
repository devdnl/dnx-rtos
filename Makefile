# Makefile for GNU make
####################################################################################################
#
# AUTHOR: DANIEL ZORYCHTA
#
# Version: 20130516
#--------------------------------------------------------------------------------------------------
#
#    Copyright (C) 2011, 2012, 2013  Daniel Zorychta (daniel.zorychta@gmail.com)
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
# PROJECT CONFIGURATION
####################################################################################################
# project name
PROJECT = dnx

#---------------------------------------------------------------------------------------------------
# ARCHITECTURE CONFIG: stm32f1
#---------------------------------------------------------------------------------------------------
TOOLCHAIN_stm32f1 = arm-none-eabi-
LD_SCRIPT_stm32f1 = src/system/portable/stm32f1/STM32F107xCxx.ld
CPU_stm32f1       = cortex-m3
MCU_stm32f1       = STM32F10X_CL
DEFINE_stm32f1    = -D$(MCU_stm32f1) -DGCC_ARMCM3 -DARCH_$(TARGET)
CFLAGS_stm32f1    = -c -mcpu=$(CPU_stm32f1) -mthumb -mthumb-interwork -Os -ffunction-sections -Wall \
                    -Wextra -std=c99 -g -ggdb3 -Wparentheses $(DEFINE_stm32f1) -Werror=implicit-function-declaration
CXXFLAGS_stm32f1  = -c -mcpu=$(CPU_stm32f1) -mthumb -mthumb-interwork -Os -ffunction-sections -Wall \
                    -Wextra -std=c++0x -g -ggdb3 -Wparentheses $(DEFINE_stm32f1) -Werror=implicit-function-declaration \
                    -fno-rtti -fno-exceptions -fno-unwind-tables
LFLAGS_stm32f1    = -mcpu=$(CPU_stm32f1) -mthumb -mthumb-interwork -T$(LD_SCRIPT_stm32f1) -g -nostartfiles \
                    -Wl,--gc-sections -Wall -Wl,-Map=$(TARGET_DIR_NAME)/$(TARGET)/$(PROJECT).map,--cref,--no-warn-mismatch \
                    $(DEFINE_stm32f1) -lm
AFLAGS_stm32f1    = -c -mcpu=$(CPU_stm32f1) -mthumb -g -ggdb3 $(DEFINE_stm32f1)

#---------------------------------------------------------------------------------------------------
# FILE EXTENSIONS CONFIGURATION
#---------------------------------------------------------------------------------------------------
OBJ_EXT = o
C_EXT   = c
CXX_EXT = cpp
AS_EXT  = s

#---------------------------------------------------------------------------------------------------
# FILE AND DIRECTORY NAMES
#---------------------------------------------------------------------------------------------------
# defines project path with binaries
TARGET_DIR_NAME = build

# defines object folder name
OBJ_DIR_NAME    = obj

# dependencies file name
DEP_FILE_NAME   = $(PROJECT).d

# folder localizations
PROG_LOC = src/programs
SYS_LOC  = src/system
CORE_LOC = $(SYS_LOC)/core
FS_LOC   = $(SYS_LOC)/fs

#---------------------------------------------------------------------------------------------------
# BASIC PROGRAMS DEFINITIONS
#---------------------------------------------------------------------------------------------------
SHELL    = sh
RM       = rm -f
MKDIR    = mkdir -p
TEST     = test
DATE     = date
CAT      = cat
MKDEP    = makedepend
WC       = wc
GREP     = grep
SIZEOF   = stat -c %s
CC       = $(TOOLCHAIN_$(TARGET))gcc
CXX      = $(TOOLCHAIN_$(TARGET))g++
LD       = $(TOOLCHAIN_$(TARGET))g++
AS       = $(TOOLCHAIN_$(TARGET))gcc -x assembler-with-cpp
OBJCOPY  = $(TOOLCHAIN_$(TARGET))objcopy
OBJDUMP  = $(TOOLCHAIN_$(TARGET))objdump
SIZE     = $(TOOLCHAIN_$(TARGET))size

#---------------------------------------------------------------------------------------------------
# MAKEFILE CORE (do not edit)
#---------------------------------------------------------------------------------------------------
# defines this makefile name
THIS_MAKEFILE = $(firstword $(MAKEFILE_LIST))

# number of threads used in compilation (cpu count + 1)
THREAD = $(shell echo $$[ $$($(CAT) /proc/cpuinfo | $(GREP) processor | $(WC) -l) + 1 ])

# sets header search path (adds -I flags to paths)
SEARCHPATH = $(foreach var, $(HDRLOC),-I$(var)) $(foreach var, $(HDRLOC_$(TARGET)),-I$(var))

# main target without defined prefixes
TARGET = $(lastword $(subst _, ,$(MAKECMDGOALS)))

# target path
TARGET_PATH = $(TARGET_DIR_NAME)/$(TARGET)

# object path
OBJ_PATH = $(TARGET_DIR_NAME)/$(TARGET)/$(OBJ_DIR_NAME)

include $(PROG_LOC)/Makefile
include $(SYS_LOC)/Makefile

# defines objects localizations
HDRLOC  = $(foreach file, $(HDRLOC_noarch),$(SYS_LOC)/$(file)) \
          $(foreach file, $(HDRLOC_$(TARGET)),$(SYS_LOC)/$(file)) \
          $(foreach file, $(HDRLOC_PROGRAMS),$(PROG_LOC)/$(file)) \
          $(foreach file, $(HDRLOC_CORE),$(SYS_LOC)/$(file)) \
          src/

# defines all C sources
CSRC    = $(foreach file, $(CSRC_CORE),$(SYS_LOC)/$(file)) \
          $(foreach file, $(CSRC_$(TARGET)),$(SYS_LOC)/$(file)) \
          $(foreach file, $(CSRC_PROGRAMS),$(PROG_LOC)/$(file)) \
          $(foreach file, $(CSRC_noarch),$(SYS_LOC)/$(file))

# defines all C++ sources
CXXSRC  = $(foreach file, $(CXXSRC_$(TARGET)),$(SYS_LOC)/$(file)) \
          $(foreach file, $(CXXSRC_PROGRAMS),$(PROG_LOC)/$(file)) \
          $(foreach file, $(CXXSRC_noarch),$(SYS_LOC)/$(file))

# defines all assembler sources
ASRC    = $(foreach file, $(ASRC_$(TARGET)),$(SYS_LOC)/$(file))

# defines objects names
OBJECTS = $(ASRC:.$(AS_EXT)=.$(OBJ_EXT)) $(CSRC:.$(C_EXT)=.$(OBJ_EXT)) $(CXXSRC:.$(CXX_EXT)=.$(OBJ_EXT))

####################################################################################################
# default target
####################################################################################################
.PHONY : all
all :
	@echo "This is help for this $(THIS_MAKEFILE)"
	@echo "Possible targets:"
	@echo "   help                this help"
	@echo "   stm32f1             compilation for ARM-Cortex-M3 STM32F1XX microcontroller family"
	@echo "   clean               clean project"
	@echo "   cleanall            clean all non-project files"
	@echo ""
	@echo "Non-build targets:"
	@echo "   check               static code analyze for stm32f1 target"

####################################################################################################
# analisis
####################################################################################################
.PHONY : check
check :
	@cppcheck -j $(THREAD) --std=c99 --enable=all --inconclusive $(DEFINE_stm32f1) $(SEARCHPATH) $(foreach file,$(OBJECTS),$(subst $(OBJ_PATH)/,,$(file:.$(OBJ_EXT)=.$(C_EXT))))

####################################################################################################
# targets
####################################################################################################
.PHONY : stm32f1
stm32f1 : dependencies buildobjects linkobjects hex status

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

	@echo 'Creating size of modules file...'
	@$(SIZE) -B -t --common $(foreach var,$(OBJECTS),$(OBJ_PATH)/$(var)) > $(TARGET_PATH)/$(PROJECT).size

	@echo -e "Flash image size: $$($(SIZEOF) $(TARGET_PATH)/$(PROJECT).bin) bytes\n"

####################################################################################################
# show compile status
####################################################################################################
.PHONY : status
status :
	@echo "-----------------------------------"
	@echo "| `$(DATE) "+Compilation completed: %k:%M:%S"` |"
	@echo "-----------------------------------"

####################################################################################################
####################################################################################################
# makes dependences
####################################################################################################
.PHONY : dependencies
dependencies :
	@echo "Creating dependencies for '$(TARGET)' target..."
	@$(MKDIR) $(TARGET_PATH)
	@$(RM) $(TARGET_PATH)/*.*
	@echo "" > $(TARGET_PATH)/$(DEP_FILE_NAME)
	@$(MKDEP) -f $(TARGET_PATH)/$(DEP_FILE_NAME) -p $(OBJ_PATH)/ -o .$(OBJ_EXT) $(SEARCHPATH) -Y -- $(CFLAGS_$(TARGET)) -- $(CSRC) $(CXXSRC) >& /dev/null
	@echo -e "$(foreach var,$(CSRC),\n$(OBJ_PATH)/$(var:.$(C_EXT )=.$(OBJ_EXT)) : $(var))" >> $(TARGET_PATH)/$(DEP_FILE_NAME)
	@echo -e "$(foreach var,$(ASRC),\n$(OBJ_PATH)/$(var:.$(AS_EXT)=.$(OBJ_EXT)) : $(var))" >> $(TARGET_PATH)/$(DEP_FILE_NAME)

####################################################################################################
# linking rules
####################################################################################################
.PHONY : linkobjects
linkobjects :
	@echo "Linking..."
	@$(LD) $(foreach var,$(OBJECTS),$(OBJ_PATH)/$(var)) $(LFLAGS_$(TARGET)) -o $(TARGET_PATH)/$(PROJECT).elf

####################################################################################################
# build objects
####################################################################################################
.PHONY : buildobjects buildobjects_$(TARGET)
buildobjects :
	@echo "Starting building objects on $(THREAD) threads..."
	@$(MAKE) -s -j$(THREAD) -f$(THIS_MAKEFILE) buildobjects_$(TARGET)

buildobjects_$(TARGET) :$(foreach var,$(OBJECTS),$(OBJ_PATH)/$(var))

####################################################################################################
# rule used to compile object files from c sources
####################################################################################################
$(OBJ_PATH)/%.$(OBJ_EXT) : %.$(C_EXT) $(THIS_MAKEFILE)
	@echo "Building: $@..."
	@$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS_$(TARGET)) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(C_EXT))) -o $@

####################################################################################################
# rule used to compile object files from C++ sources
####################################################################################################
$(OBJ_PATH)/%.$(OBJ_EXT) : %.$(CXX_EXT) $(THIS_MAKEFILE)
	@echo "Building: $@..."
	@$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS_$(TARGET)) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(CXX_EXT))) -o $@

####################################################################################################
# rule used to compile object files from assembler sources
####################################################################################################
$(OBJ_PATH)/%.$(OBJ_EXT) : %.$(AS_EXT) $(THIS_MAKEFILE)
	@echo "Building: $@..."
	@$(MKDIR) $(dir $@)
	$(AS) $(AFLAGS_$(TARGET)) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(AS_EXT))) -o $@

####################################################################################################
# clean target
####################################################################################################
.PHONY : cleantarget
cleantarget :
	@echo "Cleaning target..."
	-@$(RM) -r $(OBJ_PATH) $(LST_PATH)
	-@$(RM) $(TARGET_DIR_NAME)/*.*

####################################################################################################
# clean all targets
####################################################################################################
.PHONY : clean
clean :
	@echo "Deleting all build files..."
	-@$(RM) -r $(TARGET_DIR_NAME)/*

####################################################################################################
# clean up project (remove all files who arent project files!)
####################################################################################################
.PHONY : cleanall
cleanall:
	@echo "Cleaning up project..."
	-@$(RM) -r $(TARGET_DIR_NAME) $(INFO_LOC)

####################################################################################################
# include all dependencies
####################################################################################################
-include $(TARGET_PATH)/$(DEP_FILE_NAME)
