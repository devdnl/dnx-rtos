/*=============================================================================================*//**
@file    utils.c

@author  Daniel Zorychta

@brief   Software tools

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
#include "utils.h"


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


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Convert byte to BCD in range up to 99
 *
 * @param value
 *
 * @return BCD value
 */
//================================================================================================//
u8_t UTL_Byte2BCD(u8_t value)
{
      value %= 100;

      u8_t highBCD = value / 10;
      u8_t lowBCD  = value % 10;

      return (highBCD << 4) | (lowBCD);
}


//================================================================================================//
/**
 * @brief Convert BCD to byte
 *
 * @param BCD
 *
 * @return byte
 */
//================================================================================================//
u8_t UTL_BCD2Byte(u8_t BCD)
{
      return ((BCD >> 4) * 10) + (BCD & 0x0F);
}


//================================================================================================//
/**
 * @brief Generate random value
 *
 * @return random value
 */
//================================================================================================//
u32_t UTL_GetRandom(void)
{
     static u32_t m_w = 156416;     /* must not be zero */
     static u32_t m_z = 12415641;   /* must not be zero */

     m_z = 36969 * (m_z & 65535) + (m_z >> 16);
     m_w = 18000 * (m_w & 65535) + (m_w >> 16);
     return (m_z << 16) + m_w;  /* 32-bit result */
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
