/*=========================================================================*//**
@file    pll.c

@author  Daniel Zorychta

@brief   File support PLL

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "pll.h"

MODULE_NAME(PLL);

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize clocks
 *
 * @param[out] **drvhdl         driver's memory handler
 * @param[in]  dev              device number
 * @param[in]  part             device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t PLL_init(void **drvhdl, uint dev, uint part)
{
        (void)drvhdl;
        (void)dev;
        (void)part;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Release PLL devices
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t PLL_release(void *drvhdl)
{
        (void)drvhdl;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t PLL_open(void *drvhdl)
{
        (void)drvhdl;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t PLL_close(void *drvhdl)
{
        (void)drvhdl;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Write to the device
 *
 * @param[in] *drvhdl           driver's memory handle
 * @param[in] *src              source
 * @param[in] size              size
 * @param[in] seek              seek
 *
 * @retval number of written nitems
 */
//==============================================================================
size_t PLL_write(void *drvhdl, void *src, size_t size, size_t nitems, size_t seek)
{
        (void)drvhdl;
        (void)src;
        (void)size;
        (void)seek;
        (void)nitems;

        return 0;
}

//==============================================================================
/**
 * @brief Read from device
 *
 * @param[in]  *drvhdl          driver's memory handle
 * @param[out] *dst             destination
 * @param[in]  size             size
 * @param[in]  seek             seek
 *
 * @retval number of read nitems
 */
//==============================================================================
size_t PLL_read(void *drvhdl, void *dst, size_t size, size_t nitems, size_t seek)
{
        (void)drvhdl;
        (void)dst;
        (void)size;
        (void)seek;
        (void)nitems;

        return 0;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in]     *drvhdl       driver's memory handle
 * @param[in]     iorq          IO reqest
 * @param[in,out] args          additional arguments
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t PLL_ioctl(void *drvhdl, iorq_t iorq, va_list args)
{
        (void)drvhdl;
        (void)iorq;
        (void)args;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function flush device
 *
 * @param[in] *drvhdl           driver's memory handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t PLL_flush(void *drvhdl)
{
        (void)drvhdl;

        return STD_RET_OK;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
