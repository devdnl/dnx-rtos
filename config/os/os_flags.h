/*=========================================================================*//**
@file    os_flags.h

@author  Daniel Zorychta

@brief   Operating System configuration flags

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*==========================================================================*/

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _OS_FLAGS_H_
#define _OS_FLAGS_H_

/*--
# this:SetLayout("TitledGridBack", 2, "Home > Operating System",
#                function() this:LoadFile("config.h") end)
++*/


/*--
this:AddExtraWidget("Label", "LabelStack", "Stack configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidStack")
++*/
/*--
this:AddWidget("Spinbox", 48, 8192, "Minimal size of task stack [levels]")
this:SetToolTip("This value determines the lowest possible stack size that can be assigned for the task. "..
                "The value is a part of entire stack size.")
--*/
#define __OS_TASK_MIN_STACK_DEPTH__ 48

/*--
this:AddWidget("Spinbox", 48, 8192, "Size of file systems stack [levels]")
this:SetToolTip("This value determines the size of stack that is used by the file systems. This value depends on used file systems.")
--*/
#define __OS_FILE_SYSTEM_STACK_DEPTH__ 160

/*--
this:AddWidget("Spinbox", 48, 8192, "Size of network stack [levels]")
this:SetToolTip("This value determines the size of stack that is used by network subsystem.")
--*/
#define __OS_NETWORK_STACK_DEPTH__ 140

/*--
this:AddWidget("Spinbox", 48, 8192, "Size of system stack (syscall) [levels]")
this:SetToolTip("This value determines the size of stack that is used by syscalls.")
--*/
#define __OS_SYSCALL_STACK_DEPTH__ 140

/*--
this:AddWidget("Spinbox", 48, 8192, "Size of interrupt stack [levels]")
this:SetToolTip("This value determines the size of stack that is used by interrupts. The value is a part of entire stack size.")
--*/
#define __OS_IRQ_STACK_DEPTH__ 16


/*--
this:AddExtraWidget("Label", "LabelTask", "\nTask management", -1, "bold")
this:AddExtraWidget("Void", "VoidTask")
++*/
/*--
this:AddWidget("Spinbox", 3, 255, "Number of task priorities")
this:SetToolTip("This value determines total number of task priorities. By using this value task priorities are calculated. "..
                "If this value is set to e.g. 7, then system priorities are in range from -3 to 3.")
--*/
#define __OS_TASK_MAX_PRIORITIES__ 7

/*--
this:AddWidget("Spinbox", 10, 1000, "Context switch frequency [Hz]")
this:SetToolTip("Context switch frequency has influence to system response for events.\n"..
                "If your system should response faster for events set this value to higher ones.\n"..
                "If your microcontroller works on slower frequency this value should be smaller to avoid wasting of CPU time for context switch.")
--*/
#define __OS_TASK_SCHED_FREQ__ 1000


/*--
this:AddExtraWidget("Label", "LabelFeatures", "\nSystem features", -1, "bold")
this:AddExtraWidget("Void", "VoidFeatures")
++*/
/*--
this:AddWidget("Checkbox", "Sleep on idle")
this:SetToolTip("If this option is selected then system is halted in the idle task (CPU goes to sleep mode). This option can prevent debugging.")
--*/
#define __OS_SLEEP_ON_IDLE__ _NO_

/*--
this:AddWidget("Checkbox", "printf() family functions")
this:SetToolTip("If this function is selected then printf() family function can be used by the application.")
--*/
#define __OS_PRINTF_ENABLE__ _YES_

/*--
this:AddWidget("Checkbox", "scanf() family functions")
this:SetToolTip("If this function is selected then scanf() family function can be used by the application.")
--*/
#define __OS_SCANF_ENABLE__ _YES_

/*--
this:AddWidget("Checkbox", "System log function")
this:SetToolTip("If this function is selected then system messages can be send to the terminal or file.")
--*/
#define __OS_SYSTEM_MSG_ENABLE__ _YES_

/*--
this:AddWidget("Checkbox", "Color terminal")
this:SetToolTip("If this function is selected then terminal output can be colorized by using VT100 commands.")
--*/
#define __OS_COLOR_TERMINAL_ENABLE__ _YES_

/*--
this:AddWidget("Checkbox", "Measure CPU load")
this:SetToolTip("This function enables CPU load measurement.")
--*/
#define __OS_MONITOR_CPU_LOAD__ _YES_

/*--
this:AddWidget("Spinbox", 0, 16777216, "Network memory limit [Bytes]")
this:SetToolTip("This option enables memory limit for network subsystem. Use 0 for no limit.")
--*/
#define __OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__ 0


/*--
this:AddExtraWidget("Label", "LabelSizes", "\nBlock and buffer sizes", -1, "bold")
this:AddExtraWidget("Void", "VoidSizes")
++*/
/*--
this:AddWidget("Spinbox", 8, 1024, "Length of stream buffer [bytes]")
this:SetToolTip("This value determines a size of buffer used in the particular streams.")
--*/
#define __OS_STREAM_BUFFER_LENGTH__ 100

/*--
this:AddWidget("Spinbox", 8, 1024, "Length of pipe buffer [bytes]")
this:SetToolTip("This value determines a size of buffer used in the each pipe (FIFO file).")
--*/
#define __OS_PIPE_LENGTH__ 128

/*--
this:AddWidget("Spinbox", 4, 1024, "Memory allocation size [bytes]")
this:SetToolTip("The allocation block size is a minimal memory block that can be allocated by the Dynamic Memory Management (e.g. malloc function).")
--*/
#define __HEAP_BLOCK_SIZE__ 4

/*--
this:AddWidget("Combobox", "Length of errno messages")
this:AddItem("Disabled (the lowest memory usage)", "0")
this:AddItem("Only numbers (small memory usage)", "1")
this:AddItem("Abbreviations (medium memory usage)", "2")
this:AddItem("Full names (the highest memory usage)", "3")
this:SetToolTip("The error messages are used to deliver users an information about error that occurred by using specified functions. "..
                "This option is used to translate errno error number to user friendly strings.")
--*/
#define __OS_ERRNO_STRING_LEN__ 3


/*--
this:AddExtraWidget("Label", "LabelMisc", "\nMiscellaneous", -1, "bold")
this:AddExtraWidget("Void", "VoidMisc")
++*/
/*--
this:AddWidget("Editline", true, "Host name")
this:SetToolTip("This string configures host name in the e.g. network.")
--*/
#define __OS_HOSTNAME__ "localhost"

/*--
this:AddWidget("Editline", true, "RTC device path")
this:SetToolTip("This is RTC device path. By using this file (device) the system will read the time.")
--*/
#define __OS_RTC_FILE_PATH__ "/dev/rtc"


/* HEAP alignment depends on selected CPU */
#define  _HEAP_ALIGN_ _CPU_HEAP_ALIGN_

#endif /* _OS_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
