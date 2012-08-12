#ifndef I2C_H_
#define I2C_H_
/*=============================================================================================*//**
@file    i2c.h

@author  Daniel Zorychta

@brief   This driver support I2C

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


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t I2C_Init(void);


extern stdRet_t I2C_Open(dev_t dev);


extern stdRet_t I2C_Close(dev_t dev);


extern stdRet_t I2C_Write(dev_t dev, void *src, size_t size, size_t seek);


extern stdRet_t I2C_Read(dev_t dev , void *dst, size_t size, size_t seek);


extern stdRet_t I2C_IOCtl(dev_t dev, IORq_t iorq, void *data);


#ifdef __cplusplus
}
#endif

#endif /* I2C_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
