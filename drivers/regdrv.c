/*=============================================================================================*//**
@file    regdrv.c

@author  Daniel Zorychta

@brief   This file is used to registration drivers

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "regdrv.h"
#include "systypes.h"
#include <string.h>

/* include here drivers headers */
#include "uart.h"
#include "gpio.h"
#include "pll.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static const regDrv_t drvList[] =
{
      {"uart1", UART_Init, UART_Release, UART_DEV_1   },
      {"gpio",  GPIO_Init, GPIO_Release, GPIO_DEV_NONE},
      {"pll",   PLL_Init,  PLL_Release,  PLL_DEV_NONE },
};


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Function find driver name and then initialize device
 *
 * @param *drvName            driver name
 * @param *nodeName           file name in /dev/ directory
 *
 * @return driver depending value, all not equal to STD_RET_OK are errors
 */
//================================================================================================//
stdRet_t InitDrv(const ch_t *drvName, const ch_t *nodeName)
{
      stdRet_t status = STD_RET_ERROR;
      u32_t i;

      if (drvName)
      {
            for (i = 0; i < ARRAY_SIZE(drvList); i++)
            {
                  if (strcmp(drvList[i].drvName, drvName) == 0)
                  {
                        status = drvList[i].init(drvList[i].device);
                        break;
                  }
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function find driver name and then release device
 *
 * @param *drvName            driver name
 *
 * @return driver depending value, all not equal to STD_RET_OK are errors
 */
//================================================================================================//
stdRet_t ReleaseDrv(const ch_t *drvName)
{
      stdRet_t status = STD_RET_ERROR;
      u32_t i;

      if (drvName)
      {
            for (i = 0; i < ARRAY_SIZE(drvList); i++)
            {
                  if (strcmp(drvList[i].drvName, drvName) == 0)
                  {
                        status = drvList[i].release(drvList[i].device);
                        break;
                  }
            }
      }

      return status;
}


regDrv_t *GetDrvData(const ch_t *drvNode)
{
      return NULL; /* DNLTODO */
}



#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
