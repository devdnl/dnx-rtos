/*=========================================================================*//**
@file    dma_ddi.h

@author  Daniel Zorychta

@brief   DMA Driver Direct Interface.

@note    Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#if defined(ARCH_stm32f4) || defined(ARCH_stm32f7) || defined(ARCH_stm32h7)

#include <sys/types.h>
#if defined(ARCH_stm32f4)
#include "stm32f4/stm32f4xx.h"
#elif defined(ARCH_stm32f7)
#include "stm32f7/stm32f7xx.h"
#elif defined(ARCH_stm32h7)
#include "stm32h7/stm32h7xx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define DMA_SR_FEIF                     DMA_LISR_FEIF0
#define DMA_SR_DMEIF                    DMA_LISR_DMEIF0
#define DMA_SR_TEIF                     DMA_LISR_TEIF0
#define DMA_SR_HTIF                     DMA_LISR_HTIF0
#define DMA_SR_TCIF                     DMA_LISR_TCIF0

#if defined(ARCH_stm32f4) || defined(ARCH_stm32f7)
#define _DMA_DDI_DMA1                   0
#define _DMA_DDI_DMA2                   1
#elif defined(ARCH_stm32h7)
#define _DMA_DDI_DMA1                   (1 << 0)
#define _DMA_DDI_DMA2                   (1 << 1)
#define _DMA_DDI_BDMA                   (1 << 2)
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
typedef bool (*_DMA_cb_t)(DMA_Stream_TypeDef *stream, u8_t SR, void *arg);

struct _dma_ddi_control {
        enum mburst {
                _DMA_DDI_MEMORY_BURST_SINGLE_TRANSFER,
                _DMA_DDI_MEMORY_BURST_4_BEATS,
                _DMA_DDI_MEMORY_BURST_8_BEATS,
                _DMA_DDI_MEMORY_BURRT_16_BEATS,
        } memory_burst:2;

        enum pburst {
                _DMA_DDI_PERIPHERAL_BURST_SINGLE_TRANSFER,
                _DMA_DDI_PERIPHERAL_BURST_4_BEATS,
                _DMA_DDI_PERIPHERAL_BURST_8_BEATS,
                _DMA_DDI_PERIPHERAL_BURST_16_BEATS,
        } peripheral_burst:2;

        enum trbuff {
                _DMA_DDI_BUFFERABLE_TRANSFER_DISABLED,
                _DMA_DDI_BUFFERABLE_TRANSFER_ENABLED,
        } bufferable_transfer:1;

        enum dbm {
                _DMA_DDI_DOUBLE_BUFFER_MODE_DISABLED,
                _DMA_DDI_DOUBLE_BUFFER_MODE_ENABLED,
        } double_buffer_mode:1;

        enum pl {
                _DMA_DDI_PRIORITY_LEVEL_LOW,
                _DMA_DDI_PRIORITY_LEVEL_MEDIUM,
                _DMA_DDI_PRIORITY_LEVEL_HIGH,
                _DMA_DDI_PRIORITY_LEVEL_VERY_HIGH,
        } priority_level:2;

        enum pincos {
                _DMA_DDI_PERIPHERAL_INCREMENT_OFFSET_ACCORDING_TO_PERIPHERAL_SIZE,
                _DMA_DDI_PERIPHERAL_INCREMENT_OFFSET_FIXED_TO_4,
        } peripheral_increment_offset:1;

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

        enum pfctrl {
                _DMA_DDI_FLOW_CONTROLLER_DMA,
                _DMA_DDI_FLOW_CONTROLLER_PERIPHERAL,
        } flow_controller:1;
};

struct _dma_ddi_fifo {
        enum dmdis {
                _DMA_DDI_DIRECT_MODE_ENABLED,
                _DMA_DDI_DIRECT_MODE_DISABLED,
        } direct_mode:1;

        enum fth {
                _DMA_DDI_FIFO_THRESHOLD_1_4,
                _DMA_DDI_FIFO_THRESHOLD_1_2,
                _DMA_DDI_FIFO_THRESHOLD_3_4,
                _DMA_DDI_FIFO_THRESHOLD_FULL,
        } FIFO_threshold:2;
};

/*
 * DMA configuration structure
 */
typedef struct {
        void     *user_ctx;             /*! user context */
        _DMA_cb_t cb_finish;            /*! finish callback */
        _DMA_cb_t cb_half;              /*! half transfer callback */
        _DMA_cb_t cb_next;              /*! next callback */
        u32_t     data_number;          /*! data number */
        u32_t     peripheral_address;   /*! peripheral address */
        u32_t     memory_address[2];    /*! memory address */
        u32_t     IRQ_priority;         /*! IRQ priority */
        u32_t     channel;              /*! DMA channel */
        bool      release;              /*! automatically release stream */
        struct _dma_ddi_control control;/*! DMA control */
        struct _dma_ddi_fifo fifo;      /*! FIFO control */
} _DMA_DDI_config_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
#if defined(ARCH_stm32f4) || defined(ARCH_stm32f7)
//==============================================================================
/**
 * @brief Function allocate selected stream.
 *
 * @param [in]  major         DMA peripheral number.
 * @param [in]  stream        stream number.
 *
 * @return On success DMA descriptor number, otherwise 0.
 */
//==============================================================================
extern u32_t _DMA_DDI_reserve(u8_t major, u8_t stream);
#elif defined(ARCH_stm32h7)
//==============================================================================
/**
 * @brief Function allocate selected stream.
 *
 * @param [in]  major_mask    DMA peripheral mask.
 * @param [in]  stream        stream number.
 *
 * @return On success DMA descriptor number, otherwise 0.
 */
//==============================================================================
extern u32_t _DMA_DDI_reserve(u8_t major_mask);
#endif

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
 * @brief Function reaturn DMA steram according to DMA descriptor.
 *
 * @param dmad                  DMA descriptor.
 * @param stream                stream
 *
 * @return One of errno value.
 */
//==============================================================================
extern int _DMA_DDI_get_stream(u32_t dmad, DMA_Stream_TypeDef **stream);

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

#endif /* ARCH_stm32f4 || ARCH_stm32f7 */
#endif /* _DMA_DDI_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
