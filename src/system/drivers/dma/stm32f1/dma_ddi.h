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

#ifdef ARCH_stm32f1

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include "stm32f1/stm32f10x.h"

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

#define DMA_CCRx_PL_LOW                 ((0 * DMA_CCR1_PL_1) | (0 * DMA_CCR1_PL_0))
#define DMA_CCRx_PL_MEDIUM              ((0 * DMA_CCR1_PL_1) | (1 * DMA_CCR1_PL_0))
#define DMA_CCRx_PL_HIGH                ((1 * DMA_CCR1_PL_1) | (0 * DMA_CCR1_PL_0))
#define DMA_CCRx_PL_VERY_HIGH           ((1 * DMA_CCR1_PL_1) | (1 * DMA_CCR1_PL_0))

#define DMA_CCRx_MSIZE_BYTE             ((0 * DMA_CCR1_MSIZE_1) | (0 * DMA_CCR1_MSIZE_0))
#define DMA_CCRx_MSIZE_HALFWORD         ((0 * DMA_CCR1_MSIZE_1) | (1 * DMA_CCR1_MSIZE_0))
#define DMA_CCRx_MSIZE_WORD             ((1 * DMA_CCR1_MSIZE_1) | (0 * DMA_CCR1_MSIZE_0))

#define DMA_CCRx_PSIZE_BYTE             ((0 * DMA_CCR1_PSIZE_1) | (0 * DMA_CCR1_PSIZE_0))
#define DMA_CCRx_PSIZE_HALFWORD         ((0 * DMA_CCR1_PSIZE_1) | (1 * DMA_CCR1_PSIZE_0))
#define DMA_CCRx_PSIZE_WORD             ((1 * DMA_CCR1_PSIZE_1) | (0 * DMA_CCR1_PSIZE_0))

#define DMA_CCRx_MINC_FIXED             (0 * DMA_CCR1_MINC)
#define DMA_CCRx_MINC_ENABLE            (1 * DMA_CCR1_MINC)

#define DMA_CCRx_PINC_FIXED             (0 * DMA_CCR1_PINC)
#define DMA_CCRx_PINC_ENABLE            (1 * DMA_CCR1_PINC)

#define DMA_CCRx_CIRC_DISABLE           (0 * DMA_CCR1_CIRC)
#define DMA_CCRx_CIRC_ENABLE            (1 * DMA_CCR1_CIRC)

#define DMA_CCRx_DIR_P2M                ((0 * DMA_CCR1_MEM2MEM) | (0 * DMA_CCR1_DIR))
#define DMA_CCRx_DIR_M2P                ((0 * DMA_CCR1_MEM2MEM) | (1 * DMA_CCR1_DIR))
#define DMA_CCRx_DIR_M2M                ((1 * DMA_CCR1_MEM2MEM) | (0 * DMA_CCR1_DIR))

/*==============================================================================
  Exported object types
==============================================================================*/
typedef bool (*_DMA_cb_t)(DMA_Channel_t *channel, u8_t SR, void *arg);

typedef struct {
        void     *arg;          /*! user configuration: callback argument */
        _DMA_cb_t callback;     /*! user configuration: finish callback */
        u32_t     CR;           /*! user configuration: control register */
        u32_t     NDT;          /*! user configuration: data number */
        u32_t     PA;           /*! user configuration: peripheral address */
        u32_t     MA;           /*! user configuration: memory address */
        bool      release;      /*! user configuration: automatically release stream */
        uint32_t  IRQ_priority; /*! user configuration: IRQ priority */
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
