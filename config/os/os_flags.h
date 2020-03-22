/*=========================================================================*//**
@file    os_flags.h

@author  Daniel Zorychta

@brief   Operating System configuration flags

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


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
this:AddWidget("Spinbox", 48, 8192, "Size of I/O threads stack [levels]")
this:SetToolTip("This value determines the size of stack that is used by the file systems, network, etc.")
--*/
#define __OS_IO_STACK_DEPTH__ 240

/*--
this:AddWidget("Spinbox", 16, 8192, "Size of interrupt stack [levels]")
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
#define __OS_TASK_MAX_PRIORITIES__ 3

/*--
this:AddWidget("Spinbox", 10, 1000, "Context switch frequency [Hz]")
this:SetToolTip("Context switch frequency has influence to system response for events.\n"..
                "If your system should response faster for events set this value to higher ones.\n"..
                "If your microcontroller works on slower frequency this value should be smaller to avoid wasting of CPU time for context switch.")
--*/
#define __OS_TASK_SCHED_FREQ__ 1000

/*--
this:AddWidget("Spinbox", 1, 32, "Maximum number of user process threads")
this:SetToolTip("Number of threads that can be started by user process.")
--*/
#define __OS_TASK_MAX_USER_THREADS__ 4

/*--
this:AddWidget("Combobox", "Mode of kworker syscall threads")
this:AddItem("Mode 0: Automatic syscall thread allocation", "0")
this:AddItem("Mode 1: Fixed number of syscall threads", "1")
this:AddItem("Mode 2: Flat syscalls handling", "2")
this:SetToolTip("When 'Mode 0: Automatic syscall thread allocation' is enabled then system creates\n"..
                "syscall threads on demand. This option is slow but if system usage is\n"..
                "not too intense then this option can limit RAM usage (peek usage can be high).\n\n"..
                "When 'Mode 1: Fixed number of syscall threads' is used then user define how many\n"..
                "syscall threads are created at system startup. This option provides\n"..
                "fast response for syscalls but may use a lot of RAM.\n\n"..
                "When 'Mode 2: Flat syscalls handling' is used then syscalls are handled directly "..
                "by client process/thread. This option provides the fastest response for syscalls\n"..
                "but each process must increase stack size for IO operations e.g. file system.")
--*/
#define __OS_TASK_KWORKER_MODE__ 2

/*--
this:AddWidget("Spinbox", 2, 32, "Maximum number of kworker threads")
this:SetToolTip("Number of threads that can be started by system process.")
--*/
#define __OS_TASK_MAX_SYSTEM_THREADS__ 6

/*--
this:AddWidget("Spinbox", 1, 32, "Number of fixed I/O kworker threads")
this:SetToolTip("Number of kworker threads for I/O syscall handling.\n"..
                "Option valid only for 'Mode 1: Fixed number of syscall threads'.")
--*/
#define __OS_TASK_KWORKER_IO_THREADS__ 4

/*--
this:AddWidget("Combobox", "Priority management of syscall threads")
this:AddItem("Equal priority for all", "0")
this:AddItem("The same as the application thread", "1")
this:SetToolTip("Number of kworker threads for I/O syscall handling.\n"..
                "Option valid only for 'Mode 1: Fixed number of syscall threads'.")
--*/
#define __OS_TASK_KWORKER_THREADS_PRIORITY__ 0

/*--
this:AddExtraWidget("Label", "LabelFeatures", "\nSystem features (advanced)", -1, "bold")
this:AddExtraWidget("Void", "VoidFeatures")
++*/
/*--
this:AddWidget("Checkbox", "Sleep on idle")
this:SetToolTip("If this option is selected then system is halted in the idle task (CPU goes to sleep mode). This option can prevent debugging.")
--*/
#define __OS_SLEEP_ON_IDLE__ _NO_

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
this:AddWidget("Checkbox", "Time management functions")
this:SetToolTip("This function enables time management (RTC).")
--*/
#define __OS_ENABLE_TIMEMAN__ _YES_

/*--
this:AddWidget("Checkbox", "printf() family functions")
this:SetToolTip("If this function is selected then printf() family function can be used by the application.")
--*/
#define __OS_PRINTF_ENABLE__ _YES_

/*--
this:AddWidget("Checkbox", "printf() float support (%f flag)")
this:SetToolTip("If this function is selected then printf() support float to string conversion.")
--*/
#define __OS_PRINTF_FLOAT_ENABLE__ _YES_

/*--
this:AddWidget("Checkbox", "scanf() family functions")
this:SetToolTip("If this function is selected then scanf() family function can be used by the application.")
--*/
#define __OS_SCANF_ENABLE__ _YES_

/*--
this:AddWidget("Checkbox", "mkdir() function")
this:SetToolTip("This option enables mkdir() function.")
--*/
#define __OS_ENABLE_MKDIR__ _YES_

/*--
this:AddWidget("Checkbox", "getcwd() function")
this:SetToolTip("This function is used to read current working directory.")
--*/
#define __OS_ENABLE_GETCWD__ _YES_

/*--
this:AddWidget("Checkbox", "mkfifo() function")
this:SetToolTip("This function is used to create pipe file.")
--*/
#define __OS_ENABLE_MKFIFO__ _YES_

/*--
this:AddWidget("Checkbox", "remove() function")
this:SetToolTip("This function enables file remove functionality.")
--*/
#define __OS_ENABLE_REMOVE__ _YES_

/*--
this:AddWidget("Checkbox", "rename() function")
this:SetToolTip("This function enables file rename functionality.")
--*/
#define __OS_ENABLE_RENAME__ _NO_

/*--
this:AddWidget("Checkbox", "chmod() function")
this:SetToolTip("This function enables changing of file mode.")
--*/
#define __OS_ENABLE_CHMOD__ _NO_

/*--
this:AddWidget("Checkbox", "chown() function")
this:SetToolTip("This function enables changing of file owner.")
--*/
#define __OS_ENABLE_CHOWN__ _NO_

/*--
this:AddWidget("Checkbox", "statfs() and getmntentry() functions")
this:SetToolTip("This option enables reading of file system statistics.")
--*/
#define __OS_ENABLE_STATFS__ _YES_

/*--
this:AddWidget("Checkbox", "mknod() function")
this:SetToolTip("This option enables creating of device files.")
--*/
#define __OS_ENABLE_MKNOD__ _YES_

/*--
this:AddWidget("Checkbox", "stat() and fstat() functions")
this:SetToolTip("This option enables getting statistics of file.")
--*/
#define __OS_ENABLE_FSTAT__ _YES_

/*--
this:AddWidget("Checkbox", "System assert")
this:SetToolTip("This option enables assert system assert function. Use only for debug purposes!")
--*/
#define __OS_ENABLE_SYS_ASSERT__ _NO_

/*--
this:AddWidget("Checkbox", "IPC Shared memory")
this:SetToolTip("This option enables IPC shared memory.")
--*/
#define __OS_ENABLE_SHARED_MEMORY__ _NO_

/*--
this:AddWidget("Checkbox", "System log function")
this:SetToolTip("If this function is selected then system messages can be send to the terminal or file.")
--*/
#define __OS_SYSTEM_MSG_ENABLE__ _YES_

/*--
this:AddWidget("Checkbox", "Execute scripts")
this:SetToolTip("If this option is enabled then system is able to run scripts with shebang (#!).")
--*/
#define __OS_SYSTEM_SHEBANG_ENABLE__ _NO_

/*--
this:AddExtraWidget("Void", "VoidOption") -- uncomment if number of upper widgets is odd
this:AddExtraWidget("Label", "LabelSizes", "\nMemory parameters", -1, "bold")
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
#define __OS_PIPE_LENGTH__ 8

/*--
this:AddWidget("Spinbox", 4, 1024, "Memory allocation size [bytes]")
this:SetToolTip("The allocation block size is a minimal memory block that can be allocated by the Dynamic Memory Management (e.g. malloc function).")
--*/
#define __HEAP_BLOCK_SIZE__ 4

/*--
--this:AddExtraWidget("Void", "VoidOption")
this:AddWidget("Spinbox", 1, 250, "System log columns")
this:SetToolTip("This option determine how many characters in row can be stored. " ..
                "Option is active when system log function is enabled.")
--*/
#define __OS_SYSTEM_MSG_COLS__ 64

/*--
this:AddWidget("Spinbox", 1, 4096, "System log rows")
this:SetToolTip("This option determine how many rows is stored in buffer. " ..
                "Option is active when system log function is enabled.")
--*/
#define __OS_SYSTEM_MSG_ROWS__ 24

/*--
this:AddWidget("Spinbox", 10, 600, "Cache synchronization interval [s]")
this:SetToolTip("This option determine period of cache synchronization with storage.")
--*/
#define __OS_SYSTEM_CACHE_SYNC_PERIOD__ 30

/*--
this:AddWidget("Spinbox", 0, 16777216, "Network memory limit [bytes]")
this:SetToolTip("This option enables memory limit for network subsystem. Use 0 for no limit.")
--*/
#define __OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__ 0

/*--
this:AddWidget("Combobox", "Length of errno messages")
this:AddItem("Disabled (the lowest ROM usage)", "0")
this:AddItem("Only numbers (small ROM usage)", "1")
this:AddItem("Abbreviations (medium ROM usage)", "2")
this:AddItem("Full names (the highest ROM usage)", "3")
this:SetToolTip("The error messages are used to deliver users an information about error that occurred by using specified functions. "..
                "This option is used to translate errno error number to user friendly strings.")
--*/
#define __OS_ERRNO_STRING_LEN__ 3

/*--
this:AddWidget("Combobox", "Heap sanity check")
this:AddItem("Disable", "_NO_")
this:AddItem("Enable", "_YES_")
this:SetToolTip("Enable/Disable sanity check of heap memory.")
--*/
#define __OS_HEAP_SANITY_CHECK__ _NO_

/*--
this:AddWidget("Combobox", "Heap block overflow check")
this:AddItem("Disable", "_NO_")
this:AddItem("Enable", "_YES_")
this:SetToolTip("Enable/Disable block overflow check of heap memory.")
--*/
#define __OS_HEAP_OVERFLOW_CHECK__ _NO_


/*--
this:AddExtraWidget("Label", "LabelMisc", "\nMiscellaneous", -1, "bold")
this:AddExtraWidget("Void", "VoidMisc")
++*/
/*--
this:AddWidget("Editline", true, "Host name")
this:SetToolTip("This string configures host name in the e.g. network.")
--*/
#define __OS_HOSTNAME__ "dnx RTOS"

/*--
this:AddWidget("Editline", true, "RTC device path")
this:SetToolTip("This is RTC device path. By using this file (device) the system will read the time.")
--*/
#define __OS_RTC_FILE_PATH__ "/dev/rtc"

/*--
this:AddWidget("Editline", true, "Initialization program")
this:SetToolTip("This is the first program started after system startup.")
--*/
#define __OS_INIT_PROG__ "initd"

/*--
this:AddWidget("Editline", true, "system() shell program")
this:SetToolTip("This is a program that execute command passed to system() function.")
--*/
#define __OS_SYSTEM_PROG__ "dsh -e"

/* HEAP alignment depends on selected CPU */
#define  _HEAP_ALIGN_ _CPU_HEAP_ALIGN_

#endif /* _OS_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
