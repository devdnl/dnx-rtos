# Makefile for GNU make
####################################################################################################
#
# AUTHOR: DANIEL ZORYCHTA
#
# Version: 20130516
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

include src/programs/Makefile.mk
include src/system/Makefile.mk

####################################################################################################
# PROJECT CONFIGURATION
####################################################################################################
# project name
PROJECT = dnx

#---------------------------------------------------------------------------------------------------
# ARCHITECTURE CONFIG: stm32f1
#---------------------------------------------------------------------------------------------------
       CC_stm32f1 = arm-none-eabi-gcc
      CXX_stm32f1 = arm-none-eabi-g++
   LINKER_stm32f1 = arm-none-eabi-gcc
       AS_stm32f1 = arm-none-eabi-gcc -x assembler-with-cpp
  OBJCOPY_stm32f1 = arm-none-eabi-objcopy
  OBJDUMP_stm32f1 = arm-none-eabi-objdump
     SIZE_stm32f1 = arm-none-eabi-size
LD_SCRIPT_stm32f1 = src/system/portable/stm32f1/stm32f107xx.ld
      CPU_stm32f1 = cortex-m3
      MCU_stm32f1 = STM32F10X_CL
  DEFINES_stm32f1 = -D$(MCU_stm32f1) -DGCC_ARMCM3 -DARCH_$(TARGET)
   CFLAGS_stm32f1 = -c -mcpu=$(CPU_stm32f1) -mthumb -mthumb-interwork -Os -ffunction-sections -Wall \
                    -Wstrict-prototypes -Wextra -std=gnu99 -g -ggdb3 -Wparentheses $(DEFINES_stm32f1)
 CXXFLAGS_stm32f1 =
   LFLAGS_stm32f1 = -mcpu=$(CPU_stm32f1) -mthumb -mthumb-interwork -T$(LD_SCRIPT_stm32f1) -g -nostartfiles \
                    -Wl,--gc-sections -Wall -Wl,-Map=$(TARGET_DIR_NAME)/$(TARGET)/$(PROJECT).map,--cref,--no-warn-mismatch \
                    $(DEFINES_stm32f1) -lm
   AFLAGS_stm32f1 = -c -mcpu=$(CPU_stm32f1) -mthumb -g -ggdb3 $(DEFINES_stm32f1)

#---------------------------------------------------------------------------------------------------
# ARCHITECTURE CONFIG: POSIX
#---------------------------------------------------------------------------------------------------
         CC_posix = gcc
        CXX_posix = g++
     LINKER_posix = gcc
         AS_posix = gcc -x assembler-with-cpp
    DEFINES_posix = -D__GCC_POSIX__=1 -DDEBUG_BUILD=1 -DUSE_STDIO=1 -DARCH_$(TARGET)
     CFLAGS_posix = -m32 -c -O0 -Wall -Wstrict-prototypes -Wextra -std=gnu99 -g -Wparentheses\
                    -Wno-pointer-sign -fmessage-length=0 -pthread -lrt $(DEFINES_posix)
   CXXFLAGS_posix =
     LFLAGS_posix = -m32 -g -Wall -lrt -Wl,-Map=$(TARGET_DIR_NAME)/$(TARGET)/$(PROJECT).map,--cref,--no-warn-mismatch \
                    -pthread $(DEFINES_posix) -lm
     AFLAGS_posix = -m32 -c -g $(DEFINES_posix)

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
TARGET_DIR_NAME = target

# defines object folder name
OBJ_DIR_NAME    = obj

# dependencies file name
DEP_FILE_NAME   = $(PROJECT).d

#---------------------------------------------------------------------------------------------------
# BASIC PROGRAMS DEFINITIONS
#---------------------------------------------------------------------------------------------------
SHELL    = sh
RM       = rm -f
MKDIR    = mkdir -p
TEST     = test
DATE     = date
CAT      = cat
DEPAPP   = makedepend
WC       = wc
GREP     = grep
SIZEOF   = stat -c %s

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

# defines objects localizations
HDRLOC  = $(foreach file, $(HDRLOC_noarch),src/system/$(file)) \
          $(foreach file, $(HDRLOC_$(TARGET)),src/system/$(file)) \
          $(foreach file, $(HDRLOC_PROGRAMS),src/programs/$(file)) \
          $(foreach file, $(HDRLOC_CORE),src/system/$(file)) \
          src/

# defines all C/C++ sources
CSRC    = $(foreach file, $(CSRC_CORE),src/system/$(file)) \
          $(foreach file, $(CSRC_$(TARGET)),src/system/$(file)) \
          $(foreach file, $(CSRC_PROGRAMS),src/programs/$(file)) \
          $(foreach file, $(CSRC_noarch),src/system/$(file)) \
          $(foreach file, $(CXXSRC_$(TARGET)),src/system/$(file)) \
          $(foreach file, $(CXXSRC_PROGRAMS),src/programs/$(file)) \
          $(foreach file, $(CXXSRC_noarch),src/system/$(file))
          
# defines all assembler sources
ASRC    = $(foreach file, $(ASRC_$(TARGET)),src/system/$(file))
         
# defines objects names
OBJECTS = $(ASRC:.$(AS_EXT)=.$(OBJ_EXT)) $(CSRC:.$(C_EXT)=.$(OBJ_EXT))

####################################################################################################
# default target
####################################################################################################
.PHONY : all
all :
	@echo "This is help for this $(THIS_MAKEFILE)"
	@echo "Possible targets:"
	@echo "   help                this help"
	@echo "   stm32f1             compilation for ARM-Cortex-M3 STM32F1XX microcontroller family"
	@echo "   posix               compilation for POSIX system"
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
	@cppcheck -j $(THREAD) --std=c99 --enable=all --inconclusive $(DEFINES_stm32f1) $(SEARCHPATH) $(foreach file,$(OBJECTS),$(subst $(OBJ_PATH)/,,$(file:.$(OBJ_EXT)=.$(C_EXT))))

####################################################################################################
# targets
####################################################################################################
.PHONY : stm32f1 posix
stm32f1 : dependencies buildobjects linkobjects hex status
posix   : dependencies buildobjects linkobjects status

####################################################################################################
# create basic output files like hex, bin, lst etc.
####################################################################################################
.PHONY : hex
hex :
	@echo 'Creating IHEX image...'
	@$(OBJCOPY_$(TARGET)) $(TARGET_PATH)/$(PROJECT).elf -O ihex $(TARGET_PATH)/$(PROJECT).hex

	@echo 'Creating binary image...'
	@$(OBJCOPY_$(TARGET)) $(TARGET_PATH)/$(PROJECT).elf -O binary $(TARGET_PATH)/$(PROJECT).bin

	@echo 'Creating memory dump...'
	@$(OBJDUMP_$(TARGET)) -x --syms $(TARGET_PATH)/$(PROJECT).elf > $(TARGET_PATH)/$(PROJECT).dmp

	@echo 'Creating extended listing....'
	@$(OBJDUMP_$(TARGET)) -S $(TARGET_PATH)/$(PROJECT).elf > $(TARGET_PATH)/$(PROJECT).lst

	@echo 'Size of modules:'
	@$(SIZE_$(TARGET)) -B -t --common $(foreach var,$(OBJECTS),$(OBJ_PATH)/$(var))

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
	@$(DEPAPP) -f $(TARGET_PATH)/$(DEP_FILE_NAME) -p $(OBJ_PATH)/ -o .$(OBJ_EXT) $(SEARCHPATH) -Y -- $(CFLAGS_$(TARGET)) -- $(CSRC) $(CXXSRC) >& /dev/null
	@echo -e "$(foreach var,$(CSRC),\n$(OBJ_PATH)/$(var:.$(C_EXT )=.$(OBJ_EXT)) : $(var))" >> $(TARGET_PATH)/$(DEP_FILE_NAME)
	@echo -e "$(foreach var,$(ASRC),\n$(OBJ_PATH)/$(var:.$(AS_EXT)=.$(OBJ_EXT)) : $(var))" >> $(TARGET_PATH)/$(DEP_FILE_NAME)

####################################################################################################
# linking rules
####################################################################################################
.PHONY : linkobjects
linkobjects :
	@echo "Linking..."
	@$(LINKER_$(TARGET)) $(foreach var,$(OBJECTS),$(OBJ_PATH)/$(var)) $(LFLAGS_$(TARGET)) -o $(TARGET_PATH)/$(PROJECT).elf

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
$(OBJ_PATH)/%.$(OBJ_EXT) : %.$(C_EXT)
	@echo "Building: $@..."
	@$(MKDIR) $(dir $@)
	$(CC_$(TARGET)) $(CFLAGS_$(TARGET)) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(C_EXT))) -o $@

####################################################################################################
# rule used to compile object files from C++ sources
####################################################################################################
$(OBJ_PATH)/%.$(OBJ_EXT) : %.$(CXX_EXT)
	@echo "Building: $@..."
	@$(MKDIR) $(dir $@)
	$(CXX_$(TARGET)) $(CXXFLAGS_$(TARGET)) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(CXX_EXT))) -o $@

####################################################################################################
# rule used to compile object files from assembler sources
####################################################################################################
$(OBJ_PATH)/%.$(OBJ_EXT) : %.$(AS_EXT)
	@echo "Building: $@..."
	@$(MKDIR) $(dir $@)
	$(AS_$(TARGET)) $(AFLAGS_$(TARGET)) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(AS_EXT))) -o $@

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
