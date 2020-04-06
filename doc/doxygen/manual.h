/*==============================================================================
file:    manual.h
purpose: Main page of the Doxygen manuals
author:  Daniel Zorychta
==============================================================================*/
/**
\mainpage dnx RTOS documentation

<b>Project homepage</b><br>
<a href="http://www.dnx-rtos.org">www.dnx-rtos.org</a>

\author Daniel Zorychta

\line line

\section sec-intro Introduction
The dnx RTOS is a small general purpose real-time operating system based on
the FreeRTOS kernel. The dnx is an unix-like layer which provides an unified
implementation of drivers and file systems (VFS) for embedded systems. We can
say that dnx RTOS is a kind of the FreeRTOS kernel distribution (dnx/FreeRTOS).
This software contains all features of the FreeRTOS and the unix-like
architecture. The dnx is a compilation of miscellaneous software (e.g. drivers,
kernel, file systems, communication stack, and utility applications), giving
users the complete development environment.

\section sec-dirs Project directories
Project contains several folders:
\arg \c BSP -- Board Support Package -- predefined configurations dla can be loaded from Configtool,
\arg \c build -- directory is created at build process and contains build results,
\arg \c config -- project configuration files. The best way is to use the Configtool
        program to adjust system. Configtool can be started by using
        Eclipse IDE or in terminal by using <tt><b>make config</b></tt> command,
\arg \c doc -- project documentation. Use <tt><b>make doc</b></tt> command in the
        command line to generate Doxygen documentation. PDF version is also
        possible to generate by using LaTeX system,
\arg \c res -- add here own files to transfer it to the romfs file system.
        Files can be read by dnx RTOS from ROM memory,
\arg \c src -- all project sources with user directories: \c lib, \c programs.
        There are also system sources in folder: \c system,
\arg \c tools -- many scripts and programs that helps organize project.

\section sec-compilation Project compilation
When all program dependencies are meet (compiler and standard utils e.g. Cygwin
are installed) then user can compile project. The first step is to start project
configuration. Configuration can be started by typing
\code make config \endcode
command or by using <b>./configure</b> script (in UNIX terminal).
Project configuration is realized by graphical tool (Configtool) that interprets
special scripts in configuration files. When project is configured (especially
GPIOs) then one can start project compilation. To do this the following command
should be typed:
\code make \endcode
When operation is finished successfully then created image can be installed in
a microcontroller. This operation is triggered by typing following command:
\code make install \endcode
or
\code make flash \endcode

\note In the project is several scripts (./tools folder) that should be adjusted
      to project requirements. In this case the following scripts should
      be modified by user: <b>flash.sh</b>, <b>reset.sh</b>, <b>runocd.sh</b>,
      and <b>terminal.sh</b>.

\note There is several example programs and libraries that can be removed from
      project (by removing library/program folder). When all programs and
      libraries are used then there is big possibility that project exceeds
      available free flash memory.

\section sec-sysstartup System startup
There is no specified configuration for system startup procedure. This should be
implemented by user because one have a knowledge how the project should works.
In default settings system starts the first application called <b>initd</b>.
The first application name can be changed by Configtool in <b>Operating System</b>
section by changing <b>Initialization program</b> value. There is example
implementation of startup program in <b>./src/application/programs/initd/initd.c</b>
file. There is no restrictions to start any other program as first application but
one should take in account that first program is responsible for creating entire
system environment like mounting file systems (at least ramfs), drivers initialization,
drivers configuration, and starting needed applications/daemons.

\section sec-user_manual User Manual
\li \subpage page-application
\li \subpage page-file-systems
\li \subpage page-drivers

\section sec-drivers Drivers
\li \subpage drv-snd
\li \subpage drv-spiee
\li \subpage drv-pwm
\li \subpage drv-nvm
\li \subpage drv-can
\li \subpage drv-sdio
\li \subpage drv-dci
\li \subpage drv-dma
\li \subpage drv-fmc
\li \subpage drv-afm
\li \subpage drv-crc
\li \subpage drv-dht11
\li \subpage drv-ethmac
\li \subpage drv-gpio
\li \subpage drv-i2c
\li \subpage drv-i2cee
\li \subpage drv-irq
\li \subpage drv-loop
\li \subpage drv-clk
\li \subpage drv-rtc
\li \subpage drv-sdspi
\li \subpage drv-spi
\li \subpage drv-tty
\li \subpage drv-uart
\li \subpage drv-usbd
\li \subpage drv-wdg
*/

//------------------------------------------------------------------------------
/**
\page page-application Application Development

\tableofcontents

\section sec-app-prg-intro Introduction
Programs can be developed similarly as in a standard PC. The dnx RTOS
provides standard C API functions that can be used in the same way as in PCs.
Besides standard libraries, special libraries are provided, e.g: network, thread,
time, and OS. For more details see @ref sec-app-topics.
Created programs are compiled in the project building time and are integral part
of the system, but have different functionality. Each program can be started in
many instances and at any time. The user decides which program will be started.
Programs integrated in the system are called built-in programs, and can be listed
by using procfs file system (typically localized in the /proc/bin directory).
The variables of each program are localized in the RAM and are dynamically
allocated at program setup. The program code is compiled and is stored in the
ROM memory of the microcontroller.

\section sec-app-topics Libraries accessible from user space
Libraries listed below are accessible from users' application and are directly
supported by dnx RTOS. Many libraries are not standard, some libraries are
modified for best usage for embedded applications. Other standard libraries can
be used but there is no warranty that library will work correctly. There are
severals libraries that are not officially supported e.g. <stdbool.h>, <stdint.h>
but there is not reasons to not use those libraries because are used internally
by system. In general, most of libc libraries can be used without any issues.
There are documented libraries that are modified for system purposes.
\li \subpage dnx-misc-h     The set of helpful macros and functions
\li \subpage dnx-net-h      The set of networking functions
\li \subpage dnx-os-h       The dnx RTOS specific functions
\li \subpage dnx-thread-h   The set of functions for thread handling
\li \subpage dnx-vt100-h    VT100 terminal handling
\li \subpage sys-endian-h   Endianness
\li \subpage sys-ioctl-h    IO control library
\li \subpage sys-mount-h    Library contains functions for file systems mount and drivers initialization
\li \subpage sys-shm-h      Library contains functions for shared memory management
\li \subpage sys-stat-h     Library contains functions for nodes create and information
\li \subpage sys-statfs-h   File systems information
\li \subpage sys-types-h    System types
\li \subpage sys-time-h     System set/get time
\li \subpage assert-h       Program assertion macro
\li \subpage ctype-h        Character classification routines
\li \subpage dirent-h       Directory handling
\li \subpage errno-h        Error code list
\li \subpage locale-h       Location specific settings
\li \subpage mntent-h       Information about file system entry
\li \subpage stdio-h        Standard IO library
\li \subpage stdlib-h       Standard library
\li \subpage string-h       String manipulation library
\li \subpage time-h         Time management library
\li \subpage unistd-h       Unix standard library

\section app-prg-limits Application Limitations
The dnx RTOS program implementation has some limitations:
\arg Global variables are provided by the main global structure and access to
     those variables are provided by the <tt><b>global</b></tt> pointer. This pointer is
     individually created for each program instance. To share global variables
     between the modules of the program, the user has to share the global variable
     structure in the header that is included in each module. The global variable
     structure is created by using <tt><b>GLOBAL_VARIABLES_SECTION {}</b></tt> macro.
\arg The global variables cannot be preloaded at program startup. The user has
     to initialize values of defined global variables if the value other than
     zero is required.
\arg If the user wants to create a constant then it should be inserted beyond
     the global variable section, because constants are stored in the ROM memory.
\arg The main function of the program has to be created by using the <tt><b>int_main()</b></tt>
     macro. Those macros create special hidden variables that are used by the system.
\arg Environment variables are not supported.

\section app-prg-reg Application Registration Details
As was mentioned above, each program in order to be visible should be registered
in the system register. The system register is localized in the
<i>./src/application/programs/program_registration.c</i> file. This file is created automatically
by the build script. No other actions are needed to add a new program to the
system. <b>To create a new program just create a new folder in the <i>./src/application/programs/</i>
and add program’s files and Makefile</b>. The only one note is that the program’s
name should be the same as folder name (<tt><b>int_main()</b></tt> macro).
Program’s Makefile is automatically added to the main system’s Makefile.

@note <b>One can use <i>./tools/addprogram.sh</i> script to create program template.
      It is the easiest way to create own program.</b>

\section app-prg-example Example Application
A simple program is presented in this section.
\code
    // FILE: example_program.c
    // BRIEF: This is simple program example

    #include <stdio.h>
    #include <stdlib.h>

    // global variables
    GLOBAL_VARIABLES_SECTION {
        int my_variable;
        int my_zero;
    };


    // constants
    static const int my_constant = 100;


    /// @brief Example program main function
    /// @param argc        number of arguments
    /// @param argv        list of arguments
    int_main(example_program, STACK_DEPTH_LOW, int argc, char *argv[])
    {
            // access to global variable
            global->my_variable = my_constant;

            // welcome message
            puts("Hello! This is example program!");

            printf("Program name: %s\n", argv[0]);

            puts("Program arguments:");
            for (int i = 1; i < argc; i++) {
                    printf("%d: %s\n", i, argv[i]);
            }

            return EXIT_SUCCESS;
    }
\endcode

Makefile script:
\code
    # Makefile for GNU make

    CSRC_PROGRAMS   += example_program/example_program.c
    CXXSRC_PROGRAMS +=
    HDRLOC_PROGRAMS +=
\endcode

File structure:
\arg ./src/application/programs/<b>example_program</b>/<i>example_program.c</i>
\arg ./src/application/programs/<b>example_program</b>/<i>Makefile</i>



\section sec-app-example Users' Libraries
Libraries are stored in the <b>./src/application/libs</b> folder and are
automatically added to the system by the script at build process. Libraries are
added to the system when contains the Makefile. To use a library in the program
just include their header. To remove library from project just remove library
folder (and remove dependencies from application).

Example of Makefile script for library:
\code
    # Makefile for GNU make

    CSRC_LIB   += mbus/mbus.c mbus/mbus_garbage.c mbus/mbus_signal.c
    CXXSRC_LIB +=
    HDRLOC_LIB += mbus
\endcode

Example how to use created library in application:
\code
    #include ...
    #include <mbus.h>
    #include ...

    ...

    int_main(...) {
        ...
    }
\endcode

\section Examples
To obtain applications and libraries code examples please browse <b>./src/application/programs/</b>
and <b>./src/application/libs/</b> folders.
*/

//------------------------------------------------------------------------------
/**
\page page-file-systems File Systems Development

\tableofcontents

\section sec-fs-intro Introduction
The file systems are used to store regular, device-like, and FIFO files. Some
file systems do not support special files, e.g. pipes and devices, because is
not possible to handle this kind of files (e.g. on the FAT) in the particular
file system. In the system there are few file systems:

\arg \c ramfs – the file system is a general usage file system that store files in
        the RAM. This file system is used by the system by default,
\arg \c devfs – the file system is used only to handle devices, it is much faster
        than \c ramfs, but does not provide many features,
\arg \c procfs – the file system is a special file system that provides the
        system information in form of the files,
\arg \c fatfs – the file system is used to read all storages that contains FAT12,
        FAT16, and FAT32 file systems.
\arg \c eefs – the file system is used for small devices e.g. 24CXX memory.
        File system requires small amount of memory and have size limitation
        (up to around 16 MiB).
\arg \c romfs - file system is used to store read-only files and directories
        directly from \c res directory to microcontroller memory. This file
        system uses small amount of memory and can be used to easily copy files
        to microcontroller at build procedure.

As we can see, each file system has special role in the system. In this case,
manage of the file systems must be the same for file that can comes from different
sources. To achieve this requirement the system has implemented core component
called VFS. The VFS is a kind of router that translates the user’s path to the
specified file system. The usage of files for the user is transparent; the user
do not care about real file position.

As mentioned above, each file system must be connected to the VFS. To do this,
the file systems must have the same interfaces. Thus, any file system can be
mounted easily by the VFS. File systems are similar to driver modules, because
single file system can handle many source files that contains specified file
system format (e.g. FAT, ext2, etc).

\section sec-fs-lib Library
All functions supported by operating system accessible from file systems are
stored in the \subpage fs-h library.

\section sec-fs-interface Interface
The interface functions are created by macros, thanks this the name of file
system functions are hidden and known only by the system.

\arg API_FS_INIT()
\arg API_FS_RELEASE()
\arg API_FS_OPEN()
\arg API_FS_CLOSE()
\arg API_FS_WRITE()
\arg API_FS_READ()
\arg API_FS_IOCTL()
\arg API_FS_FLUSH()
\arg API_FS_MKDIR()
\arg API_FS_MKFIFO()
\arg API_FS_MKNOD()
\arg API_FS_OPENDIR()
\arg API_FS_READDIR()
\arg API_FS_CLOSEDIR()
\arg API_FS_REMOVE()
\arg API_FS_RENAME()
\arg API_FS_CHMOD()
\arg API_FS_CHOWN()
\arg API_FS_FSTAT()
\arg API_FS_STAT()
\arg API_FS_STATFS()
\arg API_FS_SYNC()

\section sec-fs-reg File System Registration
A new file system is automatically added to the system if simple requirements
are met:
\arg folder with file system is created in the <tt>./src/system/fs directory</tt>,
\arg at least one source file is created in folder and contain file system API functions,
\arg Makefile script is added to created folder.

@note <b>One can use <i>./tools/addfilesystem.sh</i> script to create file system
      template. It is the easiest way to create own file system.</b>

\subsection subsec-fs-ex File System Source File
@code
#include "fs/fs.h"

API_FS_INIT(...)
{
        // ..
}

API_FS_...(...)
{
        // ...
}

...
@endcode

\subsection subsec-fs-mk Makefile
@code
# Makefile for GNU make
CSRC_CORE += fs/examplefs/examplefs.c
CSRC_CORE += fs/examplefs/examplefs_x.c
@endcode

\section sec-fs-cfg File System Configuration
For created file system the configuration can be added. In this case user
should add new entries in <tt>./config/filesystems</tt> directory. Existing
configurations are good example how to do this.

*/

//------------------------------------------------------------------------------
/**
\page page-drivers Drivers Development

\tableofcontents

\section sec-drv-intro Introduction
he drivers (that are part of the modules) are based on the unified software
architecture; in other words, each driver has the same unified interface.
The dnx RTOS uses suitable interfaces to communicate with drivers (devices).
The interfaces are defined as functions and all of those functions must exist
in a particular module. Each module can operate on several devices of the same
kind, e.g. UART (UART1, UART2), SPI (SPI1, SPI2), etc. Each peripheral interface
has usually the same registers, interrupts, and functionality. Some parts of the
device can be different, but usually the difference is not significant.
In summary, one module can handle many devices of the same kind. To handle all
devices (of the same type) by one module, the user defines major and minor
numbers that precise which device should be initialized. The pair of those
numbers is called the interface or simply the driver. For example, the major
number in the UART devices means that the user selects a peripheral number:
UART0, UART1, UART2, etc. In this case, the minor number is not used because the
UART peripheral cannot be divided into next parts (sub-devices). This is
different for the SPI peripherals. The major number means that the user selects
a particular SPI device: SPI0, SPI1, etc; the minor number means that the user
selects a specified Chip Select: CS0, CS1, etc. As we can see, those numbers can
be easily adjusted to the user needs and the peripheral restrictions.

A module is created for the specified microcontroller architecture or for any
architecture when the code can work on any microcontroller, e.g. the TTY module.
The modules are localized in the <b>./src/system/drivers</b> folder. Each driver
has its own folder, Makefile, and source files. There are few files that are
required:

\arg\c *_cfg.h – the file contains module configuration flags. These flags are
        connected to the main configuration localized in the ./config folder;
\arg\c *_ioctl.h – the file contains IO control requests and description
        referring to its usage;
\arg\c *.c – the source files;
\arg\c Makefile – the file indicates source files for compilation.


\section sec-drv-topics Interface
The interface functions are created by macros, thanks this the name of file
system functions are hidden and known only by the system.
\arg API_MOD_INIT()
\arg API_MOD_RELEASE()
\arg API_MOD_OPEN()
\arg API_MOD_CLOSE()
\arg API_MOD_WRITE()
\arg API_MOD_READ()
\arg API_MOD_IOCTL()
\arg API_MOD_FLUSH()
\arg API_MOD_STAT()


\section sec-drv-reg  Registering Module in the System
When module folder is created and driver files are added (see @ref sec-drv-intro),
the build process automatically add driver to the system. No user action is needed.
For example if one create <i>abc</i> driver then it can be used in the system
as <i>ABC</i> module.

@note <b>One can use <i>./tools/adddriver.sh</i> script to create new driver
      template. It is the easiest way to create own driver. If driver is not for
      <i>noarch</i> architecture then uC.PERIPH[] table in file
      <i>./config/arch/arch_flags.h</i> should be updated for specified architecture
      to see driver in configuration of selected microcontroller.</b>


Example of driver source file (./src/system/drivers/abc/noarch/abc.c):
@code
#include "drivers/driver.h"
#include "abc_cfg.h"
#inclide "../abc_ioctl.h"

MODULE_NAME(ABC)

typedef struct {
        ...
} ABC_t

static ABC_t *ABC;

API_MOD_INIT(ABC, void **device_handle, u8_t major, u8_t minor)
{
        int err = sys_zalloc(sizeof(ABC_t), cast(void**, &ABC));

        ...

        return err;
}

API_MOD_RELEASE(ABC, void *device_handle)
{
        ABC_t *hdl = device_handle;

        int err = ...;

        // ...

        return err;
}

API_MOD_OPEN(ABC, void *device_handle, u32_t flags)
{
        ABC_t *hdl = device_handle;

        int err = ...;

        // ...

        return err;
}

API_MOD_CLOSE(ABC, void *device_handle, bool force)
{
        ABC_t *hdl = device_handle;

        int err = ...;

        // ...

        return err;
}

API_MOD_WRITE(ABC,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        ABC_t *hdl = device_handle;

        int err = ...;

        // ...

        return err;
}

API_MOD_READ(ABC,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        ABC_t *hdl = device_handle;

        int err = ...;

        // ...

        return err;
}

API_MOD_IOCTL(ABC, void *device_handle, int request, void *arg)
{
        ABC_t *hdl = device_handle;

        int err = ...;

        // ...

        return err;
}

API_MOD_FLUSH(ABC, void *device_handle)
{
        UNUSED_ARG1(device_handle);

        return ESUCC;
}

API_MOD_STAT(ABC, void *device_handle, struct vfs_dev_stat *device_stat)
{
        device_stat->st_size  = 0;

        return ESUCC;
}

@endcode


Example of driver configuration file (./src/system/drivers/abc/noarch/abc_cfg.h):
@code
#ifndef _ABC_CFG_H_
#define _ABC_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define _ABC_CONFIG_X1                  __ABC_CONFIG_X1__
#define _ABC_CONFIG_X2                  __ABC_CONFIG_X2__

#ifdef __cplusplus
}
#endif

#endif // _ABC_CFG_H_
@endcode


Example of IO control file (./src/system/drivers/abc/abc_ioctl.h):
@code
#ifndef _ABC_IOCTL_H_
#define _ABC_IOCTL_H_

#include "drivers/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IOCTL_ABC__GET_X        _IOR(ABC, 0x00, int*)
#define IOCTL_ABC__SET_X        _IOW(ABC, 0x01, int*)

#ifdef __cplusplus
}
#endif

#endif // _ABC_IOCTL_H_
@endcode


Example of driver makefile (./src/system/drivers/abc/Makefile):
@code
# Makefile for GNU make
HDRLOC_NOARCH += drivers/abc

ifeq ($(__ENABLE_ABC__), _YES_)
   CSRC_NOARCH   += drivers/abc/noarch/abc.c
   CXXSRC_NOARCH +=
endif

@endcode


\section sec-drv-init Initializing and Releasing Module
If our module is ready, then we can use it. The module can be initialized on
demand in any time by invoking the driver_init() function. Mostly, the modules
are initialized at system startup in the initd daemon. The module can be used
only when the file node representing the driver is created.
Module can be released by using driver_release() function.

\section sec-drv-sum Summary
The process of the implementation of module is not complicated, but there are
several steps that must be done. The presented example is easy, more advanced
modules can be implemented with more powerful features by using all system
features, e.g. mutexes, sempahores, etc. To see more modules, please study the
content of the ./src/system/drivers folder.

*/


/*==============================================================================
  End of file
==============================================================================*/
