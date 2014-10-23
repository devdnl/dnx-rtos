/*=========================================================================*//**
@file    gpio_ioctl.h

@author  Daniel Zorychta

@brief   This driver support GPIO ioctl request codes.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _GPIO_IOCTL_H_
#define _GPIO_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "core/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
enum {
        #if __GPIO_PA_ENABLE__
        _GPIO_PA,
        #endif
        #if __GPIO_PB_ENABLE__
        _GPIO_PB,
        #endif
        #if __GPIO_PC_ENABLE__
        _GPIO_PC,
        #endif
        #if __GPIO_PD_ENABLE__
        _GPIO_PD,
        #endif
        #if __GPIO_PE_ENABLE__
        _GPIO_PE,
        #endif
        #if __GPIO_PF_ENABLE__
        _GPIO_PF,
        #endif
        #if __GPIO_PG_ENABLE__
        _GPIO_PG
        #endif
};

/**
 * Type represent pin number. Use GPIO_PIN() macro to easly set this type
 */
typedef struct {
        u8_t port_index : 4;
        u8_t pin_number : 4;
} GPIO_pin_t;

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 *  @brief  Set selected pin (pin state 1)
 *  @param  GPIO_pin_t *
 *  @return On success 0, on error -1
 */
#define IOCTL_GPIO__SET_PIN             _IOW(GPIO, 0, GPIO_pin_t*)

/**
 *  @brief  Clear selected pin (pin state 0)
 *  @param  GPIO_pin_t *
 *  @return On success 0, on error -1
 */
#define IOCTL_GPIO__CLEAR_PIN           _IOW(GPIO, 1, GPIO_pin_t*)

/**
 *  @brief  Toggle pin state
 *  @param  GPIO_pin_t *
 *  @return On success 0, on error -1
 */
#define IOCTL_GPIO__TOGGLE_PIN          _IOW(GPIO, 2, GPIO_pin_t*)

/**
 *  @brief  Gets pin state
 *  @param  GPIO_pin_t *
 *  @return On success 0 or 1 (pin state), on error -1
 */
#define IOCTL_GPIO__GET_PIN             _IOR(GPIO, 3, GPIO_pin_t*)


/** macro load GPIO_pin_t structure using 1 argument only */
#define GPIO_PIN(_pin_name)             {.port_index = _PORT_##_pin_name, .pin_number = _pin_name}


/* PRIVATE MACROS ------------------------------------------------------------*/
/* helper macros */
#define _CAT(x, y) x ## y
#define _PIN_NAME(_port, _pin, _name)   enum _port##_pin {_CAT(GPIO_PIN__, _name) = _pin, _CAT(_PORT_GPIO_PIN__, _name) = _port, _CAT(GPIO_PORT_OF__, _name) = _port}

/** GPIO pin NONE definition */
#define _GPIO_NONE     0xF
#define _GPIO_PIN_NONE 0
_PIN_NAME(_GPIO_NONE, 0, NONE);

/** GPIOA pins configuration */
#if __GPIO_PA_ENABLE__
_PIN_NAME(_GPIO_PA,  0, __GPIO_PA_PIN_0_NAME__ );
_PIN_NAME(_GPIO_PA,  1, __GPIO_PA_PIN_1_NAME__ );
_PIN_NAME(_GPIO_PA,  2, __GPIO_PA_PIN_2_NAME__ );
_PIN_NAME(_GPIO_PA,  3, __GPIO_PA_PIN_3_NAME__ );
_PIN_NAME(_GPIO_PA,  4, __GPIO_PA_PIN_4_NAME__ );
_PIN_NAME(_GPIO_PA,  5, __GPIO_PA_PIN_5_NAME__ );
_PIN_NAME(_GPIO_PA,  6, __GPIO_PA_PIN_6_NAME__ );
_PIN_NAME(_GPIO_PA,  7, __GPIO_PA_PIN_7_NAME__ );
_PIN_NAME(_GPIO_PA,  8, __GPIO_PA_PIN_8_NAME__ );
_PIN_NAME(_GPIO_PA,  9, __GPIO_PA_PIN_9_NAME__ );
_PIN_NAME(_GPIO_PA, 10, __GPIO_PA_PIN_10_NAME__);
_PIN_NAME(_GPIO_PA, 11, __GPIO_PA_PIN_11_NAME__);
_PIN_NAME(_GPIO_PA, 12, __GPIO_PA_PIN_12_NAME__);
_PIN_NAME(_GPIO_PA, 13, __GPIO_PA_PIN_13_NAME__);
_PIN_NAME(_GPIO_PA, 14, __GPIO_PA_PIN_14_NAME__);
_PIN_NAME(_GPIO_PA, 15, __GPIO_PA_PIN_15_NAME__);
#endif

/** GPIOB pins configuration */
#if __GPIO_PB_ENABLE__
_PIN_NAME(_GPIO_PB,  0, __GPIO_PB_PIN_0_NAME__ );
_PIN_NAME(_GPIO_PB,  1, __GPIO_PB_PIN_1_NAME__ );
_PIN_NAME(_GPIO_PB,  2, __GPIO_PB_PIN_2_NAME__ );
_PIN_NAME(_GPIO_PB,  3, __GPIO_PB_PIN_3_NAME__ );
_PIN_NAME(_GPIO_PB,  4, __GPIO_PB_PIN_4_NAME__ );
_PIN_NAME(_GPIO_PB,  5, __GPIO_PB_PIN_5_NAME__ );
_PIN_NAME(_GPIO_PB,  6, __GPIO_PB_PIN_6_NAME__ );
_PIN_NAME(_GPIO_PB,  7, __GPIO_PB_PIN_7_NAME__ );
_PIN_NAME(_GPIO_PB,  8, __GPIO_PB_PIN_8_NAME__ );
_PIN_NAME(_GPIO_PB,  9, __GPIO_PB_PIN_9_NAME__ );
_PIN_NAME(_GPIO_PB, 10, __GPIO_PB_PIN_10_NAME__);
_PIN_NAME(_GPIO_PB, 11, __GPIO_PB_PIN_11_NAME__);
_PIN_NAME(_GPIO_PB, 12, __GPIO_PB_PIN_12_NAME__);
_PIN_NAME(_GPIO_PB, 13, __GPIO_PB_PIN_13_NAME__);
_PIN_NAME(_GPIO_PB, 14, __GPIO_PB_PIN_14_NAME__);
_PIN_NAME(_GPIO_PB, 15, __GPIO_PB_PIN_15_NAME__);
#endif

/** GPIOC pins configuration */
#if __GPIO_PC_ENABLE__
_PIN_NAME(_GPIO_PC,  0, __GPIO_PC_PIN_0_NAME__ );
_PIN_NAME(_GPIO_PC,  1, __GPIO_PC_PIN_1_NAME__ );
_PIN_NAME(_GPIO_PC,  2, __GPIO_PC_PIN_2_NAME__ );
_PIN_NAME(_GPIO_PC,  3, __GPIO_PC_PIN_3_NAME__ );
_PIN_NAME(_GPIO_PC,  4, __GPIO_PC_PIN_4_NAME__ );
_PIN_NAME(_GPIO_PC,  5, __GPIO_PC_PIN_5_NAME__ );
_PIN_NAME(_GPIO_PC,  6, __GPIO_PC_PIN_6_NAME__ );
_PIN_NAME(_GPIO_PC,  7, __GPIO_PC_PIN_7_NAME__ );
_PIN_NAME(_GPIO_PC,  8, __GPIO_PC_PIN_8_NAME__ );
_PIN_NAME(_GPIO_PC,  9, __GPIO_PC_PIN_9_NAME__ );
_PIN_NAME(_GPIO_PC, 10, __GPIO_PC_PIN_10_NAME__);
_PIN_NAME(_GPIO_PC, 11, __GPIO_PC_PIN_11_NAME__);
_PIN_NAME(_GPIO_PC, 12, __GPIO_PC_PIN_12_NAME__);
_PIN_NAME(_GPIO_PC, 13, __GPIO_PC_PIN_13_NAME__);
_PIN_NAME(_GPIO_PC, 14, __GPIO_PC_PIN_14_NAME__);
_PIN_NAME(_GPIO_PC, 15, __GPIO_PC_PIN_15_NAME__);
#endif

/** GPIOD pins configuration */
#if __GPIO_PD_ENABLE__
_PIN_NAME(_GPIO_PD,  0, __GPIO_PD_PIN_0_NAME__ );
_PIN_NAME(_GPIO_PD,  1, __GPIO_PD_PIN_1_NAME__ );
_PIN_NAME(_GPIO_PD,  2, __GPIO_PD_PIN_2_NAME__ );
_PIN_NAME(_GPIO_PD,  3, __GPIO_PD_PIN_3_NAME__ );
_PIN_NAME(_GPIO_PD,  4, __GPIO_PD_PIN_4_NAME__ );
_PIN_NAME(_GPIO_PD,  5, __GPIO_PD_PIN_5_NAME__ );
_PIN_NAME(_GPIO_PD,  6, __GPIO_PD_PIN_6_NAME__ );
_PIN_NAME(_GPIO_PD,  7, __GPIO_PD_PIN_7_NAME__ );
_PIN_NAME(_GPIO_PD,  8, __GPIO_PD_PIN_8_NAME__ );
_PIN_NAME(_GPIO_PD,  9, __GPIO_PD_PIN_9_NAME__ );
_PIN_NAME(_GPIO_PD, 10, __GPIO_PD_PIN_10_NAME__);
_PIN_NAME(_GPIO_PD, 11, __GPIO_PD_PIN_11_NAME__);
_PIN_NAME(_GPIO_PD, 12, __GPIO_PD_PIN_12_NAME__);
_PIN_NAME(_GPIO_PD, 13, __GPIO_PD_PIN_13_NAME__);
_PIN_NAME(_GPIO_PD, 14, __GPIO_PD_PIN_14_NAME__);
_PIN_NAME(_GPIO_PD, 15, __GPIO_PD_PIN_15_NAME__);
#endif

/** GPIOE pins configuration */
#if __GPIO_PE_ENABLE__
_PIN_NAME(_GPIO_PE,  0, __GPIO_PE_PIN_0_NAME__ );
_PIN_NAME(_GPIO_PE,  1, __GPIO_PE_PIN_1_NAME__ );
_PIN_NAME(_GPIO_PE,  2, __GPIO_PE_PIN_2_NAME__ );
_PIN_NAME(_GPIO_PE,  3, __GPIO_PE_PIN_3_NAME__ );
_PIN_NAME(_GPIO_PE,  4, __GPIO_PE_PIN_4_NAME__ );
_PIN_NAME(_GPIO_PE,  5, __GPIO_PE_PIN_5_NAME__ );
_PIN_NAME(_GPIO_PE,  6, __GPIO_PE_PIN_6_NAME__ );
_PIN_NAME(_GPIO_PE,  7, __GPIO_PE_PIN_7_NAME__ );
_PIN_NAME(_GPIO_PE,  8, __GPIO_PE_PIN_8_NAME__ );
_PIN_NAME(_GPIO_PE,  9, __GPIO_PE_PIN_9_NAME__ );
_PIN_NAME(_GPIO_PE, 10, __GPIO_PE_PIN_10_NAME__);
_PIN_NAME(_GPIO_PE, 11, __GPIO_PE_PIN_11_NAME__);
_PIN_NAME(_GPIO_PE, 12, __GPIO_PE_PIN_12_NAME__);
_PIN_NAME(_GPIO_PE, 13, __GPIO_PE_PIN_13_NAME__);
_PIN_NAME(_GPIO_PE, 14, __GPIO_PE_PIN_14_NAME__);
_PIN_NAME(_GPIO_PE, 15, __GPIO_PE_PIN_15_NAME__);
#endif

/** GPIOF pins configuration */
#if __GPIO_PF_ENABLE__
_PIN_NAME(_GPIO_PF,  0, __GPIO_PF_PIN_0_NAME__ );
_PIN_NAME(_GPIO_PF,  1, __GPIO_PF_PIN_1_NAME__ );
_PIN_NAME(_GPIO_PF,  2, __GPIO_PF_PIN_2_NAME__ );
_PIN_NAME(_GPIO_PF,  3, __GPIO_PF_PIN_3_NAME__ );
_PIN_NAME(_GPIO_PF,  4, __GPIO_PF_PIN_4_NAME__ );
_PIN_NAME(_GPIO_PF,  5, __GPIO_PF_PIN_5_NAME__ );
_PIN_NAME(_GPIO_PF,  6, __GPIO_PF_PIN_6_NAME__ );
_PIN_NAME(_GPIO_PF,  7, __GPIO_PF_PIN_7_NAME__ );
_PIN_NAME(_GPIO_PF,  8, __GPIO_PF_PIN_8_NAME__ );
_PIN_NAME(_GPIO_PF,  9, __GPIO_PF_PIN_9_NAME__ );
_PIN_NAME(_GPIO_PF, 10, __GPIO_PF_PIN_10_NAME__);
_PIN_NAME(_GPIO_PF, 11, __GPIO_PF_PIN_11_NAME__);
_PIN_NAME(_GPIO_PF, 12, __GPIO_PF_PIN_12_NAME__);
_PIN_NAME(_GPIO_PF, 13, __GPIO_PF_PIN_13_NAME__);
_PIN_NAME(_GPIO_PF, 14, __GPIO_PF_PIN_14_NAME__);
_PIN_NAME(_GPIO_PF, 15, __GPIO_PF_PIN_15_NAME__);
#endif

/** GPIOG pins configuration */
#if __GPIO_PG_ENABLE__
_PIN_NAME(_GPIO_PG,  0, __GPIO_PG_PIN_0_NAME__ );
_PIN_NAME(_GPIO_PG,  1, __GPIO_PG_PIN_1_NAME__ );
_PIN_NAME(_GPIO_PG,  2, __GPIO_PG_PIN_2_NAME__ );
_PIN_NAME(_GPIO_PG,  3, __GPIO_PG_PIN_3_NAME__ );
_PIN_NAME(_GPIO_PG,  4, __GPIO_PG_PIN_4_NAME__ );
_PIN_NAME(_GPIO_PG,  5, __GPIO_PG_PIN_5_NAME__ );
_PIN_NAME(_GPIO_PG,  6, __GPIO_PG_PIN_6_NAME__ );
_PIN_NAME(_GPIO_PG,  7, __GPIO_PG_PIN_7_NAME__ );
_PIN_NAME(_GPIO_PG,  8, __GPIO_PG_PIN_8_NAME__ );
_PIN_NAME(_GPIO_PG,  9, __GPIO_PG_PIN_9_NAME__ );
_PIN_NAME(_GPIO_PG, 10, __GPIO_PG_PIN_10_NAME__);
_PIN_NAME(_GPIO_PG, 11, __GPIO_PG_PIN_11_NAME__);
_PIN_NAME(_GPIO_PG, 12, __GPIO_PG_PIN_12_NAME__);
_PIN_NAME(_GPIO_PG, 13, __GPIO_PG_PIN_13_NAME__);
_PIN_NAME(_GPIO_PG, 14, __GPIO_PG_PIN_14_NAME__);
_PIN_NAME(_GPIO_PG, 15, __GPIO_PG_PIN_15_NAME__);
#endif

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

#endif /* _GPIO_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
