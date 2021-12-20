/*=========================================================================*//**
@file    dma_ddi.h

@author  Daniel Zorychta

@brief   DMA Driver Direct Interface.

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

/**
 * @defgroup drv-dma-ddi DMA Driver Direct Interface.
 *
 * \section drv-dma-ddi-desc Description
 * DMA Driver Direct Interface. This interface can be used only from driver
 * level. Interface is created to provide common low-level functions that
 * handles basic microcontroller functionality. The interface of this driver
 * is not common for all architectures.
 *
 * \section drv-dma-sup-arch Supported architectures
 * \li stm32f4
 *
 * @todo Details
 *
 *
 * @{
 */

#ifndef _DMA_DDI_H_
#define _DMA_DDI_H_

#if defined(ARCH_stm32f1) || defined(ARCH_stm32f3)

/*==============================================================================
  Include files
==============================================================================*/
#include "lib/sys/types.h"

#if defined(ARCH_stm32f1)
#include "stm32f1/stm32f10x.h"
#elif defined(ARCH_stm32f3)
#include "stm32f3/stm32f3xx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define DMA_SR_GIF                      DMA_ISR_GIF1
#define DMA_SR_TCIF                     DMA_ISR_TCIF1
#define DMA_SR_HTIF                     DMA_ISR_HTIF1
#define DMA_SR_TEIF                     DMA_ISR_TEIF1

#define _DMA_DDI_DMA1                   0
#define _DMA_DDI_DMA2                   1

/*==============================================================================
  Exported object types
==============================================================================*/
typedef bool (*_DMA_cb_t)(DMA_Channel_TypeDef *channel, u8_t SR, void *arg);

typedef struct {
        void     *user_ctx;             /*! user context */
        _DMA_cb_t cb_finish;            /*! finish callback */
        _DMA_cb_t cb_half;              /*! half transfer callback */
        _DMA_cb_t cb_next;              /*! next callback */
        u32_t     data_number;          /*! data number */
        u32_t     peripheral_address;   /*! peripheral address */
        u32_t     memory_address;       /*! memory address */
        u32_t     IRQ_priority;         /*! IRQ priority */
        bool      release;              /*! automatically release stream */

        enum pl {
                _DMA_DDI_PRIORITY_LEVEL_LOW,
                _DMA_DDI_PRIORITY_LEVEL_MEDIUM,
                _DMA_DDI_PRIORITY_LEVEL_HIGH,
                _DMA_DDI_PRIORITY_LEVEL_VERY_HIGH,
        } priority_level:2;

        enum msize {
                _DMA_DDI_MEMORY_DATA_SIZE_BYTE,
                _DMA_DDI_MEMORY_DATA_SIZE_HALF_WORD,
                _DMA_DDI_MEMORY_DATA_SIZE_WORD,
        } memory_data_size:2;

        enum psize {
                _DMA_DDI_PERIPHERAL_DATA_SIZE_BYTE,
                _DMA_DDI_PERIPHERAL_DATA_SIZE_HALF_WORD,
                _DMA_DDI_PERIPHERAL_DATA_SIZE_WORD,
        } peripheral_data_size:2;

        enum minc {
                _DMA_DDI_MEMORY_ADDRESS_POINTER_IS_FIXED,
                _DMA_DDI_MEMORY_ADDRESS_POINTER_INCREMENTED,
        } memory_address_increment:1;

        enum pinc {
                _DMA_DDI_PERIPHERAL_ADDRESS_POINTER_IS_FIXED,
                _DMA_DDI_PERIPHERAL_ADDRESS_POINTER_INCREMENTED,
        } peripheral_address_increment:1;

        enum circ {
                _DMA_DDI_CIRCULAR_MODE_DISABLED,
                _DMA_DDI_CIRCULAR_MODE_ENABLED,
        } circular_mode:1;

        enum dir {
                _DMA_DDI_TRANSFER_DIRECTION_PERIPHERAL_TO_MEMORY,
                _DMA_DDI_TRANSFER_DIRECTION_MEMORY_TO_PERIPHERAL,
                _DMA_DDI_TRANSFER_DIRECTION_MEMORY_TO_MEMORY,
        } transfer_direction:2;
} _DMA_DDI_config_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function allocate selected stream.
 *
 * @param [in]  major         DMA peripheral number.
 * @param [in]  channel       channel number [1..7].
 *
 * @return On success DMA descriptor number, otherwise 0.
 */
//==============================================================================
extern u32_t _DMA_DDI_reserve(u8_t major, u8_t channel);

//==============================================================================
/**
 * @brief Function free allocated stream.
 *
 * @param dmad                  DMA descriptor.
 */
//==============================================================================
extern void _DMA_DDI_release(u32_t dmad);

//==============================================================================
/**
 * @brief Function start transfer. The IRQ flags (TCIE, TEIE) are added
 *        automatically.
 *
 * @param dmad                  DMA descriptor.
 * @param config                DMA configuration.
 *
 * @return One of errno value.
 */
//==============================================================================
extern int _DMA_DDI_transfer(u32_t dmad, _DMA_DDI_config_t *config);

//==============================================================================
/**
 * @brief Function return DMA channel according to DMA descriptor.
 *
 * @param dmad                  DMA descriptor.
 * @param channel               stream
 *
 * @return One of errno value.
 */
//==============================================================================
extern int _DMA_DDI_get_channel(u32_t dmad, DMA_Channel_TypeDef **channel);

//==============================================================================
/**
 * @brief Function start memory-to-memory transfer by using free channel.
 *
 * @param dst                   destination address.
 * @param src                   source address.
 * @param size                  block size.
 *
 * @return One of errno value.
 */
//==============================================================================
extern int _DMA_DDI_memcpy(void *dst, const void *src, size_t size);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* ARCH_stm32f1 */
#endif /* _DMA_DDI_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
