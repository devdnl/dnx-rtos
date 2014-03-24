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

include ./config/project/Makefile
include ./config/$(PROJECT_CPU_ARCH)/Makefile

####################################################################################################
# PROJECT CONFIGURATION
####################################################################################################
# project name
PROJECT = $(PROJECT_NAME)

#---------------------------------------------------------------------------------------------------
# DEFAULT COMPILER FLAGS
#---------------------------------------------------------------------------------------------------
TOOLCHAIN = $(PROJECT_TOOLCHAIN)

CONFIG_DEF = 

AFLAGS   = -c \
           -g \
           -ggdb3 \
           -include ./config/project/flags.h \
           $(CPUCONFIG_AFLAGS) \
           $(CONFIG_DEF)

CFLAGS   = -c \
           -g \
           -ggdb3  \
           -Os \
           -std=c99 \
           -ffunction-sections \
           -Wall \
           -Wextra \
           -Wparentheses \
           -Werror=implicit-function-declaration \
           -include ./config/project/flags.h \
           $(CPUCONFIG_CFLAGS) \
           $(CONFIG_DEF)

CXXFLAGS = -c \
           -g \
           -ggdb3 \
           -Os \
           -std=c++0x \
           -ffunction-sections \
           -fno-rtti \
           -fno-exceptions \
           -fno-unwind-tables \
           -Wall \
           -Wextra \
           -Wparentheses \
           -Werror=implicit-function-declaration \
           -include ./config/project/flags.h \
           $(CPUCONFIG__CXXFLAGS) \
           $(CONFIG_DEF) \

LFLAGS   = -g \
           $(CPUCONFIG__LDFLAGS) \
           -nostartfiles \
           -T$(CPUCONFIG_LD) \
           -Wl,--gc-sections \
           -Wl,-Map=$(TARGET_DIR_NAME)/$(TARGET)/$(PROJECT).map,--cref,--no-warn-mismatch \
           -Wall \
           -lm

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
PROG_LOC   = src/programs
SYS_LOC    = src/system
CORE_LOC   = $(SYS_LOC)/core
FS_LOC     = $(SYS_LOC)/fs
KERNEL_LOC = $(SYS_LOC)/kernel
NET_LOC    = $(SYS_LOC)/net
DRV_LOC    = $(SYS_LOC)/drivers
PORT_LOC   = $(SYS_LOC)/portable

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
CC       = $(TOOLCHAIN)gcc
CXX      = $(TOOLCHAIN)g++
LD       = $(TOOLCHAIN)g++
AS       = $(TOOLCHAIN)gcc -x assembler-with-cpp
OBJCOPY  = $(TOOLCHAIN)objcopy
OBJDUMP  = $(TOOLCHAIN)objdump
SIZE     = $(TOOLCHAIN)size

#---------------------------------------------------------------------------------------------------
# MAKEFILE CORE (do not edit)
#---------------------------------------------------------------------------------------------------
# defines VALUES
ENABLE  = 1
DISABLE = 0
YES     = 1
NO      = 0
EMPTY   =

# defines this makefile name
THIS_MAKEFILE = $(firstword $(MAKEFILE_LIST))

# number of threads used in compilation (cpu count + 1)
THREAD = 1#$(shell echo $$($(CAT) /proc/cpuinfo | $(GREP) processor | $(WC) -l))

# sets header search path (adds -I flags to paths)
SEARCHPATH = $(foreach var, $(HDRLOC),-I$(var)) $(foreach var, $(HDRLOC_$(TARGET)),-I$(var))

# main target without defined prefixes
TARGET = $(PROJECT_CPU_ARCH)

# target path
TARGET_PATH = $(TARGET_DIR_NAME)/$(TARGET)

# object path
OBJ_PATH = $(TARGET_DIR_NAME)/$(TARGET)/$(OBJ_DIR_NAME)

# list of sources to compile
include $(PROG_LOC)/Makefile
include $(SYS_LOC)/Makefile

# defines objects localizations
HDRLOC  = $(foreach file, $(HDRLOC_PROGRAMS),$(PROG_LOC)/$(file)) \
          $(foreach file, $(HDRLOC_CORE),$(SYS_LOC)/$(file)) \
          $(foreach file, $(HDRLOC_NOARCH),$(SYS_LOC)/$(file)) \
          $(foreach file, $(HDRLOC_ARCH),$(SYS_LOC)/$(file)) \
          src/

# defines all C sources
CSRC    = $(foreach file, $(CSRC_PROGRAMS),$(PROG_LOC)/$(file)) \
          $(foreach file, $(CSRC_CORE),$(SYS_LOC)/$(file)) \
          $(foreach file, $(CSRC_NOARCH),$(SYS_LOC)/$(file)) \
          $(foreach file, $(CSRC_ARCH),$(SYS_LOC)/$(file))

# defines all C++ sources
CXXSRC  = $(foreach file, $(CXXSRC_PROGRAMS),$(PROG_LOC)/$(file)) \
          $(foreach file, $(CXXSRC_CORE),$(SYS_LOC)/$(file)) \
          $(foreach file, $(CXXSRC_NOARCH),$(SYS_LOC)/$(file)) \
          $(foreach file, $(CXXSRC_ARCH),$(SYS_LOC)/$(file))

# defines all assembler sources
ASRC    = $(foreach file, $(ASRC_ARCH),$(SYS_LOC)/$(file))

# defines objects names
OBJECTS = $(ASRC:.$(AS_EXT)=.$(OBJ_EXT)) $(CSRC:.$(C_EXT)=.$(OBJ_EXT)) $(CXXSRC:.$(CXX_EXT)=.$(OBJ_EXT))

####################################################################################################
# targets
####################################################################################################
.PHONY : all
all : dependencies buildobjects linkobjects hex status

####################################################################################################
# help
####################################################################################################
.PHONY : help
help :
	@echo "This is help for this $(THIS_MAKEFILE)"
	@echo "Possible targets:"
	@echo "   help                this help"
	@echo "   config              project configuration"
	@echo "   clean               clean project"
	@echo "   cleanall            clean all non-project files"
	@echo ""
	@echo "Non-build targets:"
	@echo "   check               static code analyze for stm32f1 target"

####################################################################################################
# project configuration
####################################################################################################
.PHONY : config
config : clean
ifeq ($(MAKECMDGOALS), config)
	@./tools/config.sh ./config/wizard/project
	@./tools/config.sh ./config/wizard/cpu
	@./tools/config.sh ./config/wizard/os
	@./tools/config.sh ./config/wizard/memory
	@./tools/config.sh ./config/wizard/fs
	@./tools/config.sh ./config/wizard/network
else
	@./tools/config.sh ./config/wizard/$(lastword $(MAKECMDGOALS))
endif

.PHONY : project cpu memory os network
project: _configdone
cpu    : _configdone
memory : _configdone
os     : _configdone
network: _configdone
fs     : _configdone
_configdone:
	@echo "Configuration done"

####################################################################################################
# analisis
####################################################################################################
.PHONY : check
check :
	@cppcheck -j $(THREAD) --std=c99 --enable=all --inconclusive $(DEFINE_stm32f1) $(SEARCHPATH) $(foreach file,$(OBJECTS),$(subst $(OBJ_PATH)/,,$(file:.$(OBJ_EXT)=.$(C_EXT))))

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
	@echo $(LD) $(foreach var,$(OBJECTS),$(OBJ_PATH)/$(var)) $(LFLAGS) -o $(TARGET_PATH)/$(PROJECT).elf
	@$(LD) $(foreach var,$(OBJECTS),$(OBJ_PATH)/$(var)) $(LFLAGS) -o $(TARGET_PATH)/$(PROJECT).elf

####################################################################################################
# build objects
####################################################################################################
.PHONY : buildobjects buildobjects_$(TARGET)
buildobjects :
	@echo "Starting building objects up to $(THREAD) threads..."
	@$(MAKE) -s -j$(THREAD) -f$(THIS_MAKEFILE) buildobjects_$(TARGET)

buildobjects_$(TARGET) :$(foreach var,$(OBJECTS),$(OBJ_PATH)/$(var))

####################################################################################################
# rule used to compile object files from c sources
####################################################################################################
$(OBJ_PATH)/%.$(OBJ_EXT) : %.$(C_EXT) $(THIS_MAKEFILE)
	@echo "Building: $@..."
	@$(MKDIR) $(dir $@)
	@echo $(CC) $(CFLAGS) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(C_EXT))) -o $@
	@$(CC) $(CFLAGS) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(C_EXT))) -o $@

####################################################################################################
# rule used to compile object files from C++ sources
####################################################################################################
$(OBJ_PATH)/%.$(OBJ_EXT) : %.$(CXX_EXT) $(THIS_MAKEFILE)
	@echo "Building: $@..."
	@$(MKDIR) $(dir $@)
	@$(CXX) $(CXXFLAGS) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(CXX_EXT))) -o $@

####################################################################################################
# rule used to compile object files from assembler sources
####################################################################################################
$(OBJ_PATH)/%.$(OBJ_EXT) : %.$(AS_EXT) $(THIS_MAKEFILE)
	@echo "Building: $@..."
	@$(MKDIR) $(dir $@)
	@echo $(AS) $(AFLAGS) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(AS_EXT))) -o $@
	@$(AS) $(AFLAGS) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(AS_EXT))) -o $@

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
