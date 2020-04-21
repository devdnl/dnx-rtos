/*=========================================================================*//**
@file    i2c.h

@author  Daniel Zorychta

@brief   This file support I2C peripheral.

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

#ifndef _I2C_H_
#define _I2C_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "i2c_ioctl.h"

#if defined(ARCH_stm32f1)
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"
#elif defined(ARCH_stm32f4)
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define _I2C_TIMEOUT_START              10
#define _I2C_TIMEOUT_BYTE_TRANSFER      100
#define _I2C_DEVICE_TIMEOUT             2000
#define _I2C_HEADER_ADDR_10BIT          0xF0

/*==============================================================================
  Exported object types
==============================================================================*/
/// enumerator defines number of I2C peripherals
#if defined(ARCH_stm32f1)
enum _I2C_major {
        #if defined(RCC_APB1ENR_I2C1EN)
        _I2C1,
        #endif
        #if defined(RCC_APB1ENR_I2C2EN)
        _I2C2,
        #endif
        _I2C_NUMBER_OF_PERIPHERALS
};
#elif defined(ARCH_stm32f4)
enum _I2C_major {
        #if defined(RCC_APB1ENR_I2C1EN)
        _I2C1,
        #endif
        #if defined(RCC_APB1ENR_I2C2EN)
        _I2C2,
        #endif
        #if defined(RCC_APB1ENR_I2C3EN)
        _I2C3,
        #endif
        _I2C_NUMBER_OF_PERIPHERALS
};
#endif

/// type defines I2C device in the runtime environment
typedef struct {
        I2C_config_t              config;               //!< device configuration
        dev_lock_t                lock_dev;             //!< object used to lock access to opened device
        u8_t                      major;                //!< major number of the device (I2C peripheral number)
        u8_t                      minor;                //!< minor number of the device (device identifier)
} I2C_dev_t;

/// type defines main memory of this module
typedef struct {
        mutex_t                  *lock_mtx;             //!< mutex used to lock access to the particular peripheral
        queue_t                  *event;                //!< queue used to indicate event (operation finished)
        I2C_recovery_t            recovery;             //!< recovery configuration
        u16_t                     SR1_mask;             //!< SR1 register mask (to catch specified event in IRQ)
        bool                      initialized:1;        //!< indicates that module for this peripheral is initialized
        u8_t                      dev_cnt;              //!< number of initialized devices
        u8_t                      unexp_event_cnt;      //!< number of unexpected events
        u8_t                      major;                //!< major number of the device (I2C peripheral number)
} I2C_mem_t;

/*==============================================================================
  Exported objects
==============================================================================*/
extern I2C_mem_t *_I2C[];

/*==============================================================================
  Exported functions
==============================================================================*/
extern int  _I2C_LLD__init(u8_t major);
extern void _I2C_LLD__release(u8_t major);
extern int  _I2C_LLD__start(I2C_dev_t *hdl);
extern int  _I2C_LLD__repeat_start(I2C_dev_t *hdl);
extern void _I2C_LLD__stop(I2C_dev_t *hdl);
extern int  _I2C_LLD__send_address(I2C_dev_t *hdl, bool write, size_t count);
extern int  _I2C_LLD__receive(I2C_dev_t *hdl, u8_t *dst, size_t count, size_t *rdcnt);
extern int  _I2C_LLD__transmit(I2C_dev_t *hdl, const u8_t *src, size_t count, size_t *wrcnt);
extern int  _I2C_LLD__slave_mode_setup(I2C_dev_t *hdl);
extern int  _I2C_LLD__slave_wait_for_selection(I2C_dev_t *hdl, I2C_selection_t *event);
extern int  _I2C_LLD__slave_transmit(I2C_dev_t *hdl, const u8_t *src, size_t count, size_t *wrcnt);
extern int  _I2C_LLD__slave_receive(I2C_dev_t *hdl, u8_t *dst, size_t count, size_t *rdcnt);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _I2C_H_ */
/*==============================================================================
  End of file
==============================================================================*/
