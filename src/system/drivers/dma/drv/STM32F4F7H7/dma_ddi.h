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

        enum {
                _DMA_DDI_MEMORY_BURST_SINGLE_TRANSFER,
                _DMA_DDI_MEMORY_BURST_4_BEATS,
                _DMA_DDI_MEMORY_BURST_8_BEATS,
                _DMA_DDI_MEMORY_BURRT_16_BEATS,
        } memory_burst:2;

        enum {
                _DMA_DDI_PERIPHERAL_BURST_SINGLE_TRANSFER,
                _DMA_DDI_PERIPHERAL_BURST_4_BEATS,
                _DMA_DDI_PERIPHERAL_BURST_8_BEATS,
                _DMA_DDI_PERIPHERAL_BURST_16_BEATS,
        } peripheral_burst:2;

        enum {
                _DMA_DDI_BUFFERABLE_TRANSFER_DISABLED,
                _DMA_DDI_BUFFERABLE_TRANSFER_ENABLED,
        } bufferable_transfer:1;

        enum {
                _DMA_DDI_DOUBLE_BUFFER_MODE_DISABLED,
                _DMA_DDI_DOUBLE_BUFFER_MODE_ENABLED,
        } double_buffer_mode:1;

        enum {
                _DMA_DDI_PRIORITY_LEVEL_LOW,
                _DMA_DDI_PRIORITY_LEVEL_MEDIUM,
                _DMA_DDI_PRIORITY_LEVEL_HIGH,
                _DMA_DDI_PRIORITY_LEVEL_VERY_HIGH,
        } priority_level:2;

        enum {
                _DMA_DDI_PERIPHERAL_INCREMENT_OFFSET_ACCORDING_TO_PERIPHERAL_SIZE,
                _DMA_DDI_PERIPHERAL_INCREMENT_OFFSET_FIXED_TO_4,
        } peripheral_increment_offset:1;

        enum {
                _DMA_DDI_MEMORY_DATA_SIZE_BYTE,
                _DMA_DDI_MEMORY_DATA_SIZE_HALF_WORD,
                _DMA_DDI_MEMORY_DATA_SIZE_WORD,
        } memory_data_size:2;

        enum {
                _DMA_DDI_PERIPHERAL_DATA_SIZE_BYTE,
                _DMA_DDI_PERIPHERAL_DATA_SIZE_HALF_WORD,
                _DMA_DDI_PERIPHERAL_DATA_SIZE_WORD,
        } peripheral_data_size:2;

        enum {
                _DMA_DDI_MEMORY_ADDRESS_POINTER_IS_FIXED,
                _DMA_DDI_MEMORY_ADDRESS_POINTER_INCREMENTED,
        } memory_address_increment:1;

        enum {
                _DMA_DDI_PERIPHERAL_ADDRESS_POINTER_IS_FIXED,
                _DMA_DDI_PERIPHERAL_ADDRESS_POINTER_INCREMENTED,
        } peripheral_address_increment:1;

        enum {
                _DMA_DDI_CIRCULAR_MODE_DISABLED,
                _DMA_DDI_CIRCULAR_MODE_ENABLED,
        } circular_mode:1;

        enum {
                _DMA_DDI_TRANSFER_DIRECTION_PERIPHERAL_TO_MEMORY,
                _DMA_DDI_TRANSFER_DIRECTION_MEMORY_TO_PERIPHERAL,
                _DMA_DDI_TRANSFER_DIRECTION_MEMORY_TO_MEMORY,
        } transfer_direction:2;

        enum {
                _DMA_DDI_FLOW_CONTROLLER_DMA,
                _DMA_DDI_FLOW_CONTROLLER_PERIPHERAL,
        } flow_controller:1;

        enum {
                _DMA_DDI_MODE_DIRECT,
                _DMA_DDI_MODE_FIFO,
        } mode:1;

        enum {
                _DMA_DDI_FIFO_THRESHOLD_1_4,
                _DMA_DDI_FIFO_THRESHOLD_1_2,
                _DMA_DDI_FIFO_THRESHOLD_3_4,
                _DMA_DDI_FIFO_THRESHOLD_FULL,
        } fifo_threshold:2;
} _DMA_DDI_config_t;

#if defined(ARCH_stm32h7)
/*------------------------------------------------------------------------------
 * MDMA
 -----------------------------------------------------------------------------*/
typedef bool (*_MDMA_cb_t)(MDMA_Channel_TypeDef *stream, void *arg);

/*
 * MDMA configuration structure
 */
typedef struct {
        void      *user_ctx;
        _MDMA_cb_t cb_channel_transfer_completed;
        _MDMA_cb_t cb_block_transfer_completed;
        _MDMA_cb_t cb_block_transfer_repeat_completed;
        _MDMA_cb_t cb_buffer_transfer_completed;
        _MDMA_cb_t cb_transfer_error;
        u32_t      source_address;
        u32_t      destination_address;
        u32_t      buffer_transfer_length;
        u32_t      link_address;
        u16_t      block_repeat_destination_address_update;
        u16_t      block_repeat_source_address_update;
        u32_t      block_repeat_count;
        u32_t      block_size;
        u32_t      mask_address;
        u32_t      mask_data;
        bool       release;
        u8_t       trigger_selection;

        enum {
                _MDMA_DDI_SOFTWARE_REQUEST_DISABLE,
                _MDMA_DDI_SOFTWARE_REQUEST_ENABLE,
        } software_request:1;

        enum {
                _MDMA_DDI_WORD_ENDIANESS_EXCHANGE_DISABLE,
                _MDMA_DDI_WORD_ENDIANESS_EXCHANGE_ENABLE,
        } word_endianess_exchange:1;

        enum {
                _MDMA_DDI_HALF_WORD_ENDIANESS_EXCHANGE_DISABLE,
                _MDMA_DDI_HALF_WORD_ENDIANESS_EXCHANGE_ENABLE,
        } half_word_endianess_exchange:1;

        enum {
                _MDMA_DDI_BYTE_ENDIANESS_EXCHANGE_DISABLE,
                _MDMA_DDI_BYTE_ENDIANESS_EXCHANGE_ENABLE,
        } byte_endianess_exchange:1;

        enum {
                _MDMA_DDI_PRIORITY_LEVEL_LOW,
                _MDMA_DDI_PRIORITY_LEVEL_MEDIUM,
                _MDMA_DDI_PRIORITY_LEVEL_HIGH,
                _MDMA_DDI_PRIORITY_LEVEL_VERY_HIGH,
        } priority_level:2;

        enum {
                _MDMA_DDI_DESTINATION_WRITE_NON_BUFFERABLE,
                _MDMA_DDI_DESTINATION_WRITE_BUFFERABLE
        } bufferable_write_mode:1;

        enum {
                _MDMA_DDI_REQUEST_MODE_HARDWARE,
                _MDMA_DDI_REQUEST_MODE_SOFTWARE
        } request_mode:1;

        enum {
                _MDMA_DDI_TRIGGER_MODE_BUFFER_TRANSFER,
                _MDMA_DDI_TRIGGER_MODE_BLOCK_TRANSFER,
                _MDMA_DDI_TRIGGER_MODE_REPEATED_BLOCK_TRANSFER,
                _MDMA_DDI_TRIGGER_MODE_LINKED_LIST_TRANSFER,
        } trigger_mode:2;

        enum {
                _MDMA_DDI_PADDING_ALIGNMENT_RIGHT_ALIGNED_PADDED_0,
                _MDMA_DDI_PADDING_ALIGNMENT_RIGHT_ALIGNED_SIGN_EXTENDED,
                _MDMA_DDI_PADDING_ALIGNMENT_LEFT_ALIGNED_PADDED_0,
        } padding_alignment_mode:2;

        enum {
                _MDMA_DDI_PACK_MODE_DISABLED,
                _MDMA_DDI_PACK_MODE_ENABLED
        } pack_mode:1;

        enum {
                _MDMA_DDI_DESTINATION_BURST_SINGLE_TRANSFER,
                _MDMA_DDI_DESTINATION_BURST_2_BEATS,
                _MDMA_DDI_DESTINATION_BURST_4_BEATS,
                _MDMA_DDI_DESTINATION_BURST_8_BEATS,
                _MDMA_DDI_DESTINATION_BURST_16_BEATS,
                _MDMA_DDI_DESTINATION_BURST_32_BEATS,
                _MDMA_DDI_DESTINATION_BURST_64_BEATS,
                _MDMA_DDI_DESTINATION_BURST_128_BEATS,
        } destination_burst:3;

        enum {
                _MDMA_DDI_SOURCE_BURST_SINGLE_TRANSFER,
                _MDMA_DDI_SOURCE_BURST_2_BEATS,
                _MDMA_DDI_SOURCE_BURST_4_BEATS,
                _MDMA_DDI_SOURCE_BURST_8_BEATS,
                _MDMA_DDI_SOURCE_BURST_16_BEATS,
                _MDMA_DDI_SOURCE_BURST_32_BEATS,
                _MDMA_DDI_SOURCE_BURST_64_BEATS,
                _MDMA_DDI_SOURCE_BURST_128_BEATS,
        } source_burst:3;

        enum {
                _MDMA_DDI_DESTINATION_INCREMENT_OFFSET_BYTE,
                _MDMA_DDI_DESTINATION_INCREMENT_OFFSET_HALF_WORD,
                _MDMA_DDI_DESTINATION_INCREMENT_OFFSET_WORD,
                _MDMA_DDI_DESTINATION_INCREMENT_OFFSET_DOUBLE_WORD
        } destination_increment_offset_size:2;

        enum {
                _MDMA_DDI_SOURCE_INCREMENT_OFFSET_BYTE,
                _MDMA_DDI_SOURCE_INCREMENT_OFFSET_HALF_WORD,
                _MDMA_DDI_SOURCE_INCREMENT_OFFSET_WORD,
                _MDMA_DDI_SOURCE_INCREMENT_OFFSET_DOUBLE_WORD
        } source_increment_offset_size:2;

        enum {
                _MDMA_DDI_DESTINATION_DATA_SIZE_BYTE,
                _MDMA_DDI_DESTINATION_DATA_SIZE_HALF_WORD,
                _MDMA_DDI_DESTINATION_DATA_SIZE_WORD,
                _MDMA_DDI_DESTINATION_DATA_SIZE_DOUBLE_WORD
        } destination_data_size:2;

        enum {
                _MDMA_DDI_SOURCE_DATA_SIZE_BYTE,
                _MDMA_DDI_SOURCE_DATA_SIZE_HALF_WORD,
                _MDMA_DDI_SOURCE_DATA_SIZE_WORD,
                _MDMA_DDI_SOURCE_DATA_SIZE_DOUBLE_WORD
        } source_data_size:2;

        enum {
                _MDMA_DDI_DESTINATION_ADDRESS_POINTER_FIXED = 0,
                _MDMA_DDI_DESTINATION_ADDRESS_PONTER_INCREMENTED = 2,
                _MDMA_DDI_DESTINATION_ADDRESS_PONTER_DECREMENTED = 3,
        } destination_address_pointer_mode:2;

        enum {
                _MDMA_DDI_SOURCE_ADDRESS_POINTER_FIXED = 0,
                _MDMA_DDI_SOURCE_ADDRESS_PONTER_INCREMENTED = 2,
                _MDMA_DDI_SOURCE_ADDRESS_PONTER_DECREMENTED = 3,
        } source_address_pointer_mode:2;

        enum { // BRDUM
                _MDMA_DDI_BLOCK_REPEAT_DESTINATION_ADDRESS_UPDATE_ADD,
                _MDMA_DDI_BLOCK_REPEAT_DESTINATION_ADDRESS_UPDATE_SUB,
        } block_repeat_destination_address_update_mode:1;

        enum { // BRSUM
                _MDMA_DDI_BLOCK_REPEAT_SOURCE_ADDRESS_UPDATE_ADD,
                _MDMA_DDI_BLOCK_REPEAT_SOURCE_ADDRESS_UPDATE_SUB,
        } block_repeat_source_address_update_mode:1;

        enum {
                _MDMA_DDI_DESTINATION_BUS_SYSTEM_AXI,
                _MDMA_DDI_DESTINATION_BUS_AHB_TCM
        } destination_bus:1;

        enum {
                _MDMA_DDI_SOURCE_BUS_SYSTEM_AXI,
                _MDMA_DDI_SOURCE_BUS_AHB_TCM
        } source_bus:1;

} _MDMA_DDI_config_t;
#endif

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


#if defined(ARCH_stm32h7)
/*------------------------------------------------------------------------------
 * MDMA
 -----------------------------------------------------------------------------*/
//==============================================================================
/**
 * @brief Function allocate selected stream.
 *
 * @param [in]  major_mask    DMA peripheral mask.
 *
 * @return On success DMA descriptor number, otherwise 0.
 */
//==============================================================================
extern u32_t _MDMA_DDI_reserve(void);

//==============================================================================
/**
 * @brief Function free allocated stream.
 *
 * @param dmad                  DMA descriptor.
 */
//==============================================================================
extern void _MDMA_DDI_release(u32_t dmad);

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
extern int _MDMA_DDI_transfer(u32_t dmad, _MDMA_DDI_config_t *config);

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
extern int _MDMA_DDI_get_channel(u32_t dmad, MDMA_Channel_TypeDef **stream);

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
extern int _MDMA_DDI_memcpy(void *dst, const void *src, size_t size);
#endif

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* ARCH_stm32f4 || ARCH_stm32f7 || ARCH_stm32h7 */
#endif /* _DMA_DDI_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
