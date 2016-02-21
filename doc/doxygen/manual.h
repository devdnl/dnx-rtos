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
kernel, file systems, communication stack, and utility applications).

\section sec-dirs Project directories
Project contains several folders:
\arg \c BSP -- Board Support Package -- predefined configurations dla can be loaded from Configtool,
\arg \c build -- directory is created at build process and contains build results,
\arg \c config -- project configuration files. The best way is to use the Configtool
        program to adjust system. Configtool can be started by using
        Eclipse IDE or in terminal by using <tt><b>make config</b></tt> command,
\arg \c doc -- project documentation. Use <tt><b>make doxygen</b></tt> command in the
        command line to generate Doxygen documentation. PDF version is also
        possible to generate by using LaTeX system,
\arg \c extra -- contains additional files, e. g. templates,
\arg \c src -- all project sources with user directories: \c lib, \c programs.
        There are also system sources in folder: \c system,
\arg \c tools -- many scripts and programs that helps organize project.

\section sec-user_manual User Manual
\li \subpage page-application
\li \subpage page-file-systems
\li \subpage page-modules
*/

//------------------------------------------------------------------------------
/**
\page page-application Application development

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
\li \subpage sys-stat-h     Library contains functions for nodes create and information
\li \subpage sys-statfs-h   File systems information
\li \subpage sys-types-h    System types
\li \subpage assert-h       Program assertion macro
\li \subpage dirent-h       Directory handling
\li \subpage errno-h        Error code list
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
<i>./src/programs/program_registration.c</i> file. This file is created automatically
by the build script. No other actions are needed to add a new program to the
system. <b>To create a new program just create a new folder in the <i>./src/programs/</i>
and add program’s files and Makefile</b>. The only one note is that the program’s
name should be the same as folder name (<tt><b>int_main()</b></tt> macro).
Program’s Makefile is automatically added to the main system’s Makefile.

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
\arg ./src/programs/<b>example_program</b>/<i>example_program.c</i>
\arg ./src/programs/<b>example_program</b>/<i>Makefile</i>


\section sec-app-example Users' Libraries
Libraries are stored in the <b>./src/lib</b> folder and are automatically added to
the system by the script at build process. Libraries are added to the system when
contains the Makefile. To use a library in the program just include their header.

Example of Makefile script for library:
\code
    # Makefile for GNU make

    CSRC_LIB   += mbus/mbus.c mbus/mbus_garbage.c mbus/mbus_signal.c
    CXXSRC_LIB +=
    HDRLOC_LIB += mbus
\endcode

\section Examples
To obtain applications and libraries code examples please browse <b>./src/programs/</b>
and <b>./src/lib/</b> folders.
*/

//------------------------------------------------------------------------------
/**
\page page-file-systems File system development

\section sec-fs-intro Introduction
In this section are presented libraries and examples that helps user to create
own file system.


\section sec-fs-topics Topics
\li \subpage fs-h File system library and system functions
*/

//------------------------------------------------------------------------------
/**
\page page-modules Modules (drivers) development

\section sec-mod-intro Introduction
In this section are presented libraries and examples that helps user to create
modules (drivers).


\section sec-mod-topics Topics
\li \subpage
*/

/*==============================================================================
  End of file
==============================================================================*/
