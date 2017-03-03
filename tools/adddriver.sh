#!/usr/bin/env bash

set -e

cd $(dirname $0)

DRVNAME=$(echo "$1" | sed 's/[-+/*,."!\@#$%^&*()+-]/_/g')
DRVNAME=${DRVNAME,,}
ARCH=$2
ARCH=${ARCH,,}
BRIEF=$3
AUTHOR=$4
EMAIL=$5

if [[ "$*" < "3" ]]; then
    echo "Usage: $(basename $0) <driver-name> <architecture> [brief] [author] [email]"
    exit 1
else
    cd ../src/system/drivers
    mkdir $DRVNAME
    cd $DRVNAME
    mkdir $ARCH


#-------------------------------------------------------------------------------
cat << EOF > ${DRVNAME}_ioctl.h
/*==============================================================================
File     ${DRVNAME}_ioctl.h

Author   $AUTHOR

Brief    $BRIEF

         Copyright (C) $(date "+%Y") $AUTHOR <$EMAIL>

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


==============================================================================*/

/**
@defgroup drv-${DRVNAME} ${DRVNAME^^} Driver

\section drv-${DRVNAME}-desc Description
Driver handles ...

\section drv-${DRVNAME}-sup-arch Supported architectures
\li $ARCH

\section drv-${DRVNAME}-ddesc Details
\subsection drv-${DRVNAME}-ddesc-num Meaning of major and minor numbers
\todo Meaning of major and minor numbers

\subsubsection drv-${DRVNAME}-ddesc-numres Numeration restrictions
\todo Numeration restrictions

\subsection drv-${DRVNAME}-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("${DRVNAME^^}", 0, 0, "/dev/${DRVNAME^^}0-0");
@endcode
@code
driver_init("${DRVNAME^^}", 0, 1, "/dev/${DRVNAME^^}0-1");
@endcode

\subsection drv-${DRVNAME}-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("${DRVNAME^^}", 0, 0);
@endcode
@code
driver_release("${DRVNAME^^}", 0, 1);
@endcode

\subsection drv-${DRVNAME}-ddesc-cfg Driver configuration
\todo Driver configuration

\subsection drv-${DRVNAME}-ddesc-write Data write
\todo Data write

\subsection drv-${DRVNAME}-ddesc-read Data read
\todo Data read

@{
*/

#ifndef _${DRVNAME^^}_IOCTL_H_
#define _${DRVNAME^^}_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 *  @brief  Example IOCTL request.
 *  @param  [WR,RD] ioctl() params...
 *  @return On success 0 is returned, otherwise -1.
 *
 *  @b Example
 *  @code
    #include <sys/ioctl.h>

    //...



    //...
    @endcode
 */
#define IOCTL_${DRVNAME^^}__EXAMPLE     _IO(${DRVNAME^^}, 0x00)

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _${DRVNAME^^}_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
EOF

#-------------------------------------------------------------------------------
if [[ "$ARCH" == "noarch" ]]; then
cat << EOF > Makefile
# Makefile for GNU make
HDRLOC_NOARCH += drivers/${DRVNAME}

ifeq (\$(__ENABLE_${DRVNAME^^}__), _YES_)
   CSRC_NOARCH   += drivers/${DRVNAME}/noarch/${DRVNAME}.c
   CXXSRC_NOARCH +=
endif
EOF
else
cat << EOF > Makefile
# Makefile for GNU make
HDRLOC_ARCH += drivers/${DRVNAME}

ifeq (\$(__ENABLE_${DRVNAME^^}__), _YES_)
   ifeq (\$(TARGET), ${ARCH})
      CSRC_ARCH   += drivers/${DRVNAME}/\$(TARGET)/${DRVNAME}.c
      CXXSRC_ARCH +=
   endif
endif
EOF
fi

#-------------------------------------------------------------------------------
cd $ARCH
cat << EOF > ${DRVNAME}_cfg.h
/*==============================================================================
File     ${DRVNAME}_cfg.h

Author   $AUTHOR

Brief    $BRIEF

         Copyright (C) $(date "+%Y") $AUTHOR <$EMAIL>

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


==============================================================================*/

#ifndef _${DRVNAME^^}_CFG_H_
#define _${DRVNAME^^}_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#define _${DRVNAME^^}_CFG__EXAMPLE          __${DRVNAME^^}_CFG_EXAMPLE__

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _${DRVNAME^^}_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
EOF

#-------------------------------------------------------------------------------
cat << EOF > ${DRVNAME}.c
/*==============================================================================
File     ${DRVNAME}.c

Author   $AUTHOR

Brief    $BRIEF

         Copyright (C) $(date "+%Y") $AUTHOR <$EMAIL>

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


==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "$ARCH/${DRVNAME}_cfg.h"
#include "../${DRVNAME}_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        // ...
} ${DRVNAME^^}_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(${DRVNAME^^});

/*==============================================================================
  Exported object
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device.
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_INIT(${DRVNAME^^}, void **device_handle, u8_t major, u8_t minor)
{
        int err = EFAULT;

        if (major == 0 && minor == 0) {
                err = sys_zalloc(sizeof(${DRVNAME^^}_t), device_handle);
                if (!err) {
                        //...
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Release device.
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_RELEASE(${DRVNAME^^}, void *device_handle)
{
        int err = sys_free(&device_handle);
        return err;
}

//==============================================================================
/**
 * @brief Open device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_OPEN(${DRVNAME^^}, void *device_handle, u32_t flags)
{
        ${DRVNAME^^}_t *hdl = device_handle;

        int err = ESUCC;

        // ...

        return err;
}

//==============================================================================
/**
 * @brief Close device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_CLOSE(${DRVNAME^^}, void *device_handle, bool force)
{
        ${DRVNAME^^}_t *hdl = device_handle;

        int err = ESUCC;

        // ...

        return err;
}

//==============================================================================
/**
 * @brief Write data to device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_WRITE(${DRVNAME^^},
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        ${DRVNAME^^}_t *hdl = device_handle;

        int err = ESUCC;

        // ...

        return err;
}

//==============================================================================
/**
 * @brief Read data from device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_READ(${DRVNAME^^},
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        ${DRVNAME^^}_t *hdl = device_handle;

        int err = ESUCC;

        // ...

        return err;
}

//==============================================================================
/**
 * @brief IO control.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_IOCTL(${DRVNAME^^}, void *device_handle, int request, void *arg)
{
        ${DRVNAME^^}_t *hdl = device_handle;

        int err = ESUCC;

        // ...

        return err;
}

//==============================================================================
/**
 * @brief Flush device.
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_FLUSH(${DRVNAME^^}, void *device_handle)
{
        ${DRVNAME^^}_t *hdl = device_handle;

        int err = ESUCC;

        // ...

        return err;
}

//==============================================================================
/**
 * @brief Device information.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_STAT(${DRVNAME^^}, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_size = 0;

        return ESUCC;
}

/*==============================================================================
  End of file
==============================================================================*/
EOF


#-------------------------------------------------------------------------------
cd ../../../../../config/arch
mkdir -p $ARCH
cd $ARCH

cat << EOF > ${DRVNAME}_flags.h
/*==============================================================================
File     ${DRVNAME}_flags.h

Author   $AUTHOR

Brief    $BRIEF

         Copyright (C) $(date "+%Y") $AUTHOR <$EMAIL>

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


==============================================================================*/

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as prefix
 *       and suffix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _${DRVNAME^^}_FLAGS_H_
#define _${DRVNAME^^}_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > ${DRVNAME^^}",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/

/*--
this:AddWidget("Checkbox", "Example configuration")
--*/
#define __${DRVNAME^^}_CFG_EXAMPLE__ _NO_

#endif /* _${DRVNAME^^}_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
EOF

#-------------------------------------------------------------------------------
# ./config/arch/arch_flags.h
#-------------------------------------------------------------------------------
cd ..

    put_drv_include() {
        echo "#include \"$ARCH/${DRVNAME}_flags.h\"" >> arch_flags.h
    }

    put_drv_enable() {
        if [[ "$ARCH" == "noarch" ]]; then
            cat << EOF >> arch_flags.h
#/*--
# this:PutWidgets("${DRVNAME^^}", "arch/noarch/${DRVNAME}_flags.h")
# this:SetToolTip("$BRIEF")
#--*/
#define __ENABLE_${DRVNAME^^}__ _NO_
#/*
__ENABLE_${DRVNAME^^}__=_NO_
#*/

EOF
    else
        cat << EOF >> arch_flags.h
#/*--
# if uC.PERIPH[uC.NAME].${DRVNAME^^} ~= nil then
#     this:PutWidgets("${DRVNAME^^}", "arch/"..uC.ARCH.."/${DRVNAME}_flags.h")
#     this:SetToolTip("$BRIEF")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_${DRVNAME^^}__", "_NO_")
# end
#--*/
#define __ENABLE_${DRVNAME^^}__ _NO_
#/*
__ENABLE_${DRVNAME^^}__=_NO_
#*/

EOF
        fi
    }

    readarray file < arch_flags.h
    echo -n "" > arch_flags.h

    for line in "${file[@]}"; do
        if [[ "$ARCH" == "noarch" ]] && [[ "$line" =~ '#define ARCH_noarch' ]]; then
            echo -n "$line" >> arch_flags.h
            put_drv_include
            continue
        elif [[ "$line" =~ "#if (__CPU_ARCH__ == $ARCH)" ]]; then
            echo -n "$line" >> arch_flags.h
            put_drv_include
            continue
        elif [[ "$line" =~ "#// MODULE LIST END" ]]; then
            put_drv_enable
        fi

        echo -n "$line" >> arch_flags.h
    done

    if [[ "$ARCH" != "noarch" ]]; then
        echo "*************************************************************************"
        echo "INFO: Add module name to specified microcontrollers in table uC.PERIPH"
        echo "      in file ./config/arch/arch_flags.h. If module is not added to table"
        echo "      then may not be visible in configuration."
        echo "*************************************************************************"
    fi

    echo "Done"


#-------------------------------------------------------------------------------
# ./doc/doxygen/
#-------------------------------------------------------------------------------
cd ../../doc/doxygen

readarray file < manual.h
echo -n "" > manual.h

for line in "${file[@]}"; do
    if [[ "$line" =~ '\section sec-drivers Drivers' ]]; then
        echo -n "$line" >> manual.h
        echo "\li \subpage drv-${DRVNAME}" >> manual.h
        continue
    fi

    echo -n "$line" >> manual.h
done


readarray file < Doxyfile
echo -n "" > Doxyfile

for line in "${file[@]}"; do
    if [[ "$line" =~ 'manual.h' ]]; then
        echo -n "$line" >> Doxyfile
        echo "                         ../../src/system/drivers/${DRVNAME}/${DRVNAME}_ioctl.h \\" >> Doxyfile
        continue
    fi

    echo -n "$line" >> Doxyfile
done


fi
