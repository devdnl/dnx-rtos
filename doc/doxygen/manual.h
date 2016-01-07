/*==============================================================================
file:    manual.h
purpose: Main page of the Doxygen manuals
author:  Daniel Zorychta
==============================================================================*/
/**
\mainpage dnx RTOS documentation

<b>Project homepage</b><br>
<a href="www.dnx-rtos.org">www.dnx-rtos.org</a>

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

\section sec-user_manual User Manual
\li \subpage page-application
\li \subpage page-file-systems
\li \subpage page-modules
*/

//------------------------------------------------------------------------------
/**
\page page-application Application development

\section sec-app-intro Introduction
In this section are presented documentations that helps users to obtain information
about libraries accessible from user space (application), and examples how to
create own application.

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

\section sec-app-build Adding application to build process

\subsection subsec-app-build-path Application localization

\subsection subsec-app-makefile Application Makefile

\section sec-app-example Example application
\todo Example application
*/

//------------------------------------------------------------------------------
/**
\page page-file-systems File system development

\section sec-fs-intro Introduction
In this section are presented libraries and examples that helps user to create
own file system.


\section sec-fs-topics Topics
\li \subpage
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
