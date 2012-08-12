#ifndef ETH_H_
#define ETH_H_
/*=============================================================================================*//**
@file    eth.h

@author  Daniel Zorychta

@brief   This file support ethernet peripheral

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
#include "system.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t ETHER_Init(void);


extern stdRet_t ETHER_Open(dev_t);


extern stdRet_t ETHER_Close(dev_t);


extern stdRet_t ETHER_Write(dev_t, void*, size_t, size_t);


extern stdRet_t ETHER_Read(dev_t, void*, size_t, size_t);


extern stdRet_t ETHER_IOCtl(dev_t, IORq_t, void*);


#ifdef __cplusplus
      }
#endif

#endif /* ETH_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
