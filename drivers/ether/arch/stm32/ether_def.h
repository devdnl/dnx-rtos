#ifndef ETHER_DEF_H_
#define ETHER_DEF_H_
/*=============================================================================================*//**
@file    ether_def.h

@author  Daniel Zorychta

@brief

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
#include "basic_types.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
enum ETHER_IORQ {
      ETHER_IORQ_GET_RX_FLAG,                         /* [out] bool_t */
      ETHER_IORQ_CLEAR_RX_FLAG,                       /* -- */
      ETHER_IORQ_SET_MAC_ADR,                         /* [in]  ch_t[] */
      ETHER_IORQ_GET_RX_PACKET_SIZE,                  /* [out] u32_t  */
      ETHER_IORQ_GET_RX_PACKET_CHAIN_MODE,            /* [out] struct ether_frame */
      ETHER_IORQ_GET_RX_BUFFER_UNAVAILABLE_STATUS,    /* [out] boot_t */
      ETHER_IORQ_CLEAR_RX_BUFFER_UNAVAILABLE_STATUS,  /* --- */
      ETHER_IORQ_RESUME_DMA_RECEPTION,                /* --- */
      ETHER_IORQ_SET_TX_FRAME_LENGTH_CHAIN_MODE,      /* [in]  u16_t */
      ETHER_IORQ_GET_CURRENT_TX_BUFFER,               /* [out] u8_t* */
};


/* ethernet frame data */
struct ether_frame {
      u32_t length;
      u32_t buffer;
};


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* ETHER_CFG_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
