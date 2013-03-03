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
# INSERT HERE C CORE SOURCES (AUTOMATIC ADDS PATHS AS HEADER PATHS)
####################################################################################################
CSRC_CORE = $(sort \
   system/kernel/FreeRTOS/Source/croutine.c \
   system/kernel/FreeRTOS/Source/list.c \
   system/kernel/FreeRTOS/Source/queue.c \
   system/kernel/FreeRTOS/Source/tasks.c \
   system/kernel/FreeRTOS/Source/timers.c \
   system/kernel/FreeRTOS/Source/portable/MemMang/heap_3.c \
   system/core/dnx.c \
   system/core/taskmoni.c \
   system/core/progman.c \
   system/core/process.c \
   system/core/dlist.c \
   system/core/main.c \
   system/core/memman.c \
   system/core/io.c \
   system/core/vfs.c \
   system/user/initd.c \
   system/user/regdrv.c \
   system/user/regfs.c \
   system/user/regprg.c \
   system/fs/procfs.c \
   system/fs/lfs.c \
   system/portable/rtos/oshooks.c \
   system/portable/rtos/oswrap.c \
   )
   
####################################################################################################
# INSERT HERE C PROGRAMS SOURCES ARCHITECTURE DEPENDED (AUTOMATIC ADDS PATHS AS HEADER PATHS)
####################################################################################################
CSRC_PROGRAMS = $(sort \
   programs/test/test.c \
   programs/top/top.c \
   )
   
####################################################################################################
# INSERT HERE C SOURCES ARCHITECTURE NOT DEPENDED (AUTOMATIC ADDS PATHS AS HEADER PATHS)
####################################################################################################
CSRC_noarch = $(sort \
   drivers/tty/arch/noarch/tty.c \
   )

####################################################################################################
# INSERT HERE C SOURCES ARCHITECTURE DEPENDED (AUTOMATIC ADDS PATHS AS HEADER PATHS)
####################################################################################################
CSRC_stm32 = $(sort \
   drivers/gpio/arch/stm32/gpio.c \
   drivers/uart/arch/stm32/uart.c \
   drivers/pll/arch/stm32/pll.c \
   system/kernel/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c \
   system/portable/stm32/cpuctl.c \
   system/portable/stm32/cpuhooks.c \
   system/portable/stm32/stm32f10x_vectors.c \
   system/portable/stm32/STM32F10x_StdPeriph_Driver/misc.c \
   )

CSRC_posix = $(sort \
   drivers/i2c/arch/posix/i2c.c \
   drivers/uart/arch/posix/uart.c \
   drivers/pll/arch/posix/pll.c \
   system/kernel/FreeRTOS/Source/portable/GCC/Posix/port.c \
   system/portable/posix/cpuctl.c \
   )

####################################################################################################
# INSERT HERE C++ SOURCES (AUTOMATIC ADDS PATHS AS HEADER PATHS)
####################################################################################################
CXXSRC = $(sort \
   )
   
####################################################################################################
# INSERT HERE C++ SOURCES ARCHITECTURE DEPENDED (AUTOMATIC ADDS PATHS AS HEADER PATHS)
####################################################################################################
CXXSRC_stm32 = $(sort \
   )
   
CXXSRC_posix = $(sort \
   )

####################################################################################################
# INSERT HERE ASSEMBLER SOURCES (AUTOMATIC ADDS PATHS AS HEADER PATHS)
####################################################################################################
ASRC = $(sort \
   )
   
####################################################################################################
# INSERT HERE ASSEMBLER SOURCES ARCHITECTURE DEPENDED (AUTOMATIC ADDS PATHS AS HEADER PATHS)
####################################################################################################
ASRC_stm32 = $(sort \
   system/portable/lib/cm_startup.s \
   )
   
ASCR_posix = $(sort \
   )

####################################################################################################
# INSERT HERE PATHS WITH HEADER FILES ONLY
####################################################################################################
HDRLOC = $(sort $(dir $(CSRC_CORE)) $(dir $(CSRC_PROGRAMS)) $(dir $(CSRC_noarch)) $(dir $(CXXSRC)) \
   drivers/tty \
   system/kernel/FreeRTOS/Source/include \
   system/config \
   system/core/include \
   system/fs/include \
   system/user/include \
   system/portable \
   )
   
####################################################################################################
# INSERT HERE PATHS WITH HEADER FILES ONLY WHICH ARE DEPENDING ON ARCHITECTURE
####################################################################################################
HDRLOC_stm32 = $(sort $(dir $(CSRC_stm32)) $(dir $(CXXSRC_stm32)) \
   drivers/gpio \
   drivers/pll \
   drivers/uart \
   system/portable/lib/CMSIS \
   lib/lwip \
   lib/lwip/include \
   lib/lwip/include/ipv4 \
   )
   
HDRLOC_posix = $(sort $(dir $(CSRC_posix)) $(dir $(CXXSRC_posix)) \
   drivers/i2c \
   drivers/uart \
   drivers/pll \
   )


####################################################################################################
####################################################################################################
####################################################################################################
# PROJECT CONFIGURATION
####################################################################################################
# project name
PROJECT = main

# optymalization
OPT = s

#---------------------------------------------------------------------------------------------------
# ARCHITECTURE CONFIG: STM32
#---------------------------------------------------------------------------------------------------
CC_stm32        = arm-none-eabi-gcc
CXX_stm32       = arm-none-eabi-g++
LINKER_stm32    = arm-none-eabi-gcc
AS_stm32        = arm-none-eabi-gcc -x assembler-with-cpp
OBJCOPY_stm32   = arm-none-eabi-objcopy
OBJDUMP_stm32   = arm-none-eabi-objdump
SIZE_stm32      = arm-none-eabi-size
LD_SCRIPT_stm32 = system/portable/stm32/stm32f107xx.ld
CPU_stm32       = cortex-m3
MCU_stm32       = STM32F10X_CL

CFLAGS_stm32    = -c -mcpu=$(CPU_stm32) -mthumb -O$(OPT) -ffunction-sections -fdata-sections -Wall \
                  -Wstrict-prototypes -Wextra -std=gnu99 -g -ggdb3 -fverbose-asm -Wparentheses\
                  -D$(MCU_stm32) -DGCC_ARMCM3 -DARCH_$(TARGET)

CXXFLAGS_stm32  =

LFLAGS_stm32    = -mcpu=$(CPU_stm32) -mthumb -T$(LD_SCRIPT_stm32) -g -nostartfiles -Wl,--gc-sections -Wall \
                  -Wl,-Map=$(BIN_LOC)/$(TARGET)/$(PROJECT).map,--cref,--no-warn-mismatch \
                  -DGCC_ARMCM3 -DARCH_$(TARGET)

AFLAGS_stm32    = -c -mcpu=$(CPU_stm32) -mthumb -g -ggdb3 -DARCH_$(TARGET)

#---------------------------------------------------------------------------------------------------
# ARCHITECTURE CONFIG: POSIX
#---------------------------------------------------------------------------------------------------
CC_posix        = gcc
CXX_posix       = g++
LINKER_posix    = gcc
AS_posix        = gcc -x assembler-with-cpp
OBJCOPY_posix   = 
OBJDUMP_posix   = 
SIZE_posix      = 
LD_SCRIPT_posix = 
CPU_posix       = 
MCU_posix       = 

CFLAGS_posix    = -m32 -c -O0 -Wall -Wstrict-prototypes -Wextra -std=gnu99 -g -Wparentheses\
                  -DARCH_$(TARGET) -Wno-pointer-sign \
                  -D__GCC_POSIX__=1 -DDEBUG_BUILD=1 -DUSE_STDIO=1 -fmessage-length=0 -pthread -lrt

CXXFLAGS_posix  =

LFLAGS_posix    = -m32 -g -Wall -pthread -lrt \
                  -Wl,-Map=$(BIN_LOC)/$(TARGET)/$(PROJECT).map,--cref,--no-warn-mismatch \
                  -DARCH_$(TARGET)

AFLAGS_posix    = -m32 -c -g -DARCH_$(TARGET)

#---------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------
# object extension
OBJ_EXT = o

# c source extension
C_EXT = c

# C++ source extension
CXX_EXT = cpp

# assembler extension
AS_EXT = s

#---------------------------------------------------------------------------------------------------
# defines project path with binaries
BIN_LOC = target

# defines project path with build info
INFO_LOC = $(BIN_LOC)/info

# defines object folder name
OBJ_LOC = obj

# dependencies file name
DEP_FILE = dep.d

#---------------------------------------------------------------------------------------------------
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

#---------------------------------------------------------------------------------------------------
# defines this makefile name (do not edit)
THIS_MAKEFILE = $(firstword $(MAKEFILE_LIST))

# number of threads when objects are compiled (cpu count + 1) (do not edit)
THREAD = $(shell echo $$[ $$($(CAT) /proc/cpuinfo | $(GREP) processor | $(WC) -l) + 1 ])

# sets header search path (adds -I flags to paths) (do not edit)
SEARCHPATH = $(foreach var, $(HDRLOC),-I$(var)) $(foreach var, $(HDRLOC_$(TARGET)),-I$(var))

# main target without defined prefixes (do not edit)
TARGET = $(lastword $(subst _, ,$(MAKECMDGOALS)))

# target path (do not edit)
TARGET_PATH = $(BIN_LOC)/$(TARGET)

# object path (do not edit)
OBJ_PATH = $(BIN_LOC)/$(TARGET)/$(OBJ_LOC)

# defines objects names (do not edit)
OBJECTS = $(ASRC:.$(AS_EXT)=.$(OBJ_EXT)) \
          $(ASRC_$(TARGET):.$(AS_EXT)=.$(OBJ_EXT)) \
          $(CSRC_CORE:.$(C_EXT)=.$(OBJ_EXT)) \
          $(CSRC_PROGRAMS:.$(C_EXT)=.$(OBJ_EXT)) \
          $(CSRC_noarch:.$(C_EXT)=.$(OBJ_EXT)) \
          $(CSRC_$(TARGET):.$(C_EXT)=.$(OBJ_EXT)) \
          $(CXXSRC:.$(CXX_EXT)=.$(OBJ_EXT)) \
          $(CXXSRC_$(TARGET):.$(CXX_EXT)=.$(OBJ_EXT))


####################################################################################################
####################################################################################################
# default target
####################################################################################################
.PHONY : all
all :
	@echo "This is help for this $(THIS_MAKEFILE)"
	@echo "Possible targets:"
	@echo "   help                this help"
	@echo "   stm32               compilation for ARM-Cortex-M3 stm32"
	@echo "   posix               compilation for POSIX system"
	@echo "   clean               clean project"
	@echo "   cleanall            clean all non-project files"
	@echo ""
	@echo "Non-build targets:"
	@echo "   flash               flashing uC (stm32) using openOCD debbuger"

####################################################################################################
# targets
####################################################################################################
.PHONY : stm32 posix
stm32 : dependencies buildobjects linkobjects hex status
posix : dependencies buildobjects linkobjects status

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
	@echo '#'ifndef VERSION >> $(INFO_LOC)/version.h
	@echo '#'define VERSION `$(DATE) "+%Y%m%d"`UL >> $(INFO_LOC)/version.h
	@echo '#'endif >> $(INFO_LOC)/version.h

	@echo "Build: `$(CAT) $(INFO_LOC)/build` `$(DATE) "+completed: %k:%M:%S"`"


####################################################################################################
####################################################################################################
# makes dependences
####################################################################################################
.PHONY : dependencies
dependencies :
	@echo "Creating dependencies for '$(TARGET)' target..."
	@$(MKDIR) $(TARGET_PATH)
	@echo "" > $(BIN_LOC)/$(DEP_FILE)
	@$(DEPAPP) -f$(BIN_LOC)/$(DEP_FILE) -p$(OBJ_PATH)/ -o.$(OBJ_EXT) $(SEARCHPATH) -Y -- $(CFLAGS_$(TARGET)) -- $(CSRC) $(CXXSRC) >& /dev/null
	@echo -e "$(foreach var,$(CSRC),\n$(OBJ_PATH)/$(var:.$(C_EXT)=.$(OBJ_EXT)) : $(var))" >> $(BIN_LOC)/$(DEP_FILE)
	@echo -e "$(foreach var,$(CSRC_$(TARGET)),\n$(OBJ_PATH)/$(var:.$(C_EXT)=.$(OBJ_EXT)) : $(var))" >> $(BIN_LOC)/$(DEP_FILE)
	@echo -e "$(foreach var,$(CXXSRC),\n$(OBJ_PATH)/$(var:.$(CXX_EXT)=.$(OBJ_EXT)) : $(var))" >> $(BIN_LOC)/$(DEP_FILE)
	@echo -e "$(foreach var,$(CXXSRC_$(TARGET)),\n$(OBJ_PATH)/$(var:.$(CXX_EXT)=.$(OBJ_EXT)) : $(var))" >> $(BIN_LOC)/$(DEP_FILE)
	@echo -e "$(foreach var,$(ASRC),\n$(OBJ_PATH)/$(var:.$(AS_EXT)=.$(OBJ_EXT)) : $(var))" >> $(BIN_LOC)/$(DEP_FILE)
	@echo -e "$(foreach var,$(ASRC_$(TARGET)),\n$(OBJ_PATH)/$(var:.$(AS_EXT)=.$(OBJ_EXT)) : $(var))" >> $(BIN_LOC)/$(DEP_FILE)

####################################################################################################
# linking rules
####################################################################################################
.PHONY : linkobjects
linkobjects :
	@echo "Linking..."
	@$(MKDIR) $(TARGET_PATH)
	@$(RM) $(TARGET_PATH)/*.*
	@$(LINKER_$(TARGET)) $(LFLAGS_$(TARGET)) $(foreach var,$(OBJECTS),$(OBJ_PATH)/$(var)) -o $(TARGET_PATH)/$(PROJECT).elf

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
	@$(AS_$(TARGET)) $(AFLAGS_$(TARGET)) $(SEARCHPATH) $(subst $(OBJ_PATH)/,,$(@:.$(OBJ_EXT)=.$(AS_EXT))) -o $@

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
	@arm-none-eabi-gdb $(BIN_LOC)/stm32/main.elf -x $(BIN_LOC)/gdbcmd
	-@$(RM) $(BIN_LOC)/gdbcmd

