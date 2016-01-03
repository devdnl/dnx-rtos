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

\section sec-intro Introduction
In this section are presented libraries that are accessible from users' programs
(applications).


\section sec-user-manual Libraries accessible from user space
\li \subpage dnx-misc-h The set of helpful macros and functions
\li \subpage dnx-net-h The set of networking functions
\li \subpage dnx-os-h The dnx RTOS specific functions
\li \subpage dnx-thread-h The set of functions for thread handling
\li \subpage dnx-vt100-h VT100 terminal handling
\li \subpage sys-endian-h Endianness
\li \subpage sys-ioctl-h IO control library

*/


/*==============================================================================
  End of file
==============================================================================*/
