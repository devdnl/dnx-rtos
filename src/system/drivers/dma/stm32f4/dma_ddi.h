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

#ifdef ARCH_stm32f4

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include "stm32f4/stm32f4xx.h"

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

#define DMA_SxCR_CHSEL_SEL(c)           (((c) &7) << DMA_SxCR_CHSEL_Pos)

#define DMA_SxCR_MBURST_SINGLE          ((0 * DMA_SxCR_MBURST_1) | (0 * DMA_SxCR_MBURST_0))
#define DMA_SxCR_MBURST_INCR4           ((0 * DMA_SxCR_MBURST_1) | (1 * DMA_SxCR_MBURST_0))
#define DMA_SxCR_MBURST_INCR8           ((1 * DMA_SxCR_MBURST_1) | (0 * DMA_SxCR_MBURST_0))
#define DMA_SxCR_MBURST_INCR16          ((1 * DMA_SxCR_MBURST_1) | (1 * DMA_SxCR_MBURST_0))

#define DMA_SxCR_PBURST_SINGLE          ((0 * DMA_SxCR_PBURST_1) | (0 * DMA_SxCR_PBURST_0))
#define DMA_SxCR_PBURST_INCR4           ((0 * DMA_SxCR_PBURST_1) | (1 * DMA_SxCR_PBURST_0))
#define DMA_SxCR_PBURST_INCR8           ((1 * DMA_SxCR_PBURST_1) | (0 * DMA_SxCR_PBURST_0))
#define DMA_SxCR_PBURST_INCR16          ((1 * DMA_SxCR_PBURST_1) | (1 * DMA_SxCR_PBURST_0))

#define DMA_SxCR_CT_M0AR                (0 * DMA_SxCR_CT)
#define DMA_SxCR_CT_M1AR                (1 * DMA_SxCR_CT)

#define DMA_SxCR_DBM_DISABLE            (0 * DMA_SxCR_DBM)
#define DMA_SxCR_DBM_ENABLE             (1 * DMA_SxCR_DBM)

#define DMA_SxCR_PL_LOW                 ((0 * DMA_SxCR_PL_1) | (0 * DMA_SxCR_PL_0))
#define DMA_SxCR_PL_MEDIUM              ((0 * DMA_SxCR_PL_1) | (1 * DMA_SxCR_PL_0))
#define DMA_SxCR_PL_HIGH                ((1 * DMA_SxCR_PL_1) | (0 * DMA_SxCR_PL_0))
#define DMA_SxCR_PL_VERY_HIGH           ((1 * DMA_SxCR_PL_1) | (1 * DMA_SxCR_PL_0))

#define DMA_SxCR_PINCOS_PSIZE           (0 * DMA_SxCR_PINCOS)
#define DMA_SxCR_PINCOS_FIXED           (1 * DMA_SxCR_PINCOS)

#define DMA_SxCR_MSIZE_BYTE             ((0 * DMA_SxCR_MSIZE_1) | (0 * DMA_SxCR_MSIZE_0))
#define DMA_SxCR_MSIZE_HALFWORD         ((0 * DMA_SxCR_MSIZE_1) | (1 * DMA_SxCR_MSIZE_0))
#define DMA_SxCR_MSIZE_WORD             ((1 * DMA_SxCR_MSIZE_1) | (0 * DMA_SxCR_MSIZE_0))

#define DMA_SxCR_PSIZE_BYTE             ((0 * DMA_SxCR_PSIZE_1) | (0 * DMA_SxCR_PSIZE_0))
#define DMA_SxCR_PSIZE_HALFWORD         ((0 * DMA_SxCR_PSIZE_1) | (1 * DMA_SxCR_PSIZE_0))
#define DMA_SxCR_PSIZE_WORD             ((1 * DMA_SxCR_PSIZE_1) | (0 * DMA_SxCR_PSIZE_0))

#define DMA_SxCR_MINC_FIXED             (0 * DMA_SxCR_MINC)
#define DMA_SxCR_MINC_ENABLE            (1 * DMA_SxCR_MINC)

#define DMA_SxCR_PINC_FIXED             (0 * DMA_SxCR_PINC)
#define DMA_SxCR_PINC_ENABLE            (1 * DMA_SxCR_PINC)

#define DMA_SxCR_CIRC_DISABLE           (0 * DMA_SxCR_CIRC)
#define DMA_SxCR_CIRC_ENABLE            (1 * DMA_SxCR_CIRC)

#define DMA_SxCR_DIR_P2M                ((0 * DMA_SxCR_DIR_1) | (0 * DMA_SxCR_DIR_0))
#define DMA_SxCR_DIR_M2P                ((0 * DMA_SxCR_DIR_1) | (1 * DMA_SxCR_DIR_0))
#define DMA_SxCR_DIR_M2M                ((1 * DMA_SxCR_DIR_1) | (0 * DMA_SxCR_DIR_0))

#define DMA_SxCR_PFCTRL_DMA             (0 * DMA_SxCR_PFCTRL)
#define DMA_SxCR_PFCTRL_PER             (1 * DMA_SxCR_PFCTRL)

#define DMA_SxFCR_FEIE_DISABLE          (0 * DMA_SxFCR_FEIE)
#define DMA_SxFCR_FEIE_ENABLE           (1 * DMA_SxFCR_FEIE)

#define DMA_SxFCR_FS_0_FL_1_4           ((0 * DMA_SxFCR_FS_2) | (0 * DMA_SxFCR_FS_1) | (0 * DMA_SxFCR_FS_0))
#define DMA_SxFCR_FS_1_4_FL_1_2         ((0 * DMA_SxFCR_FS_2) | (0 * DMA_SxFCR_FS_1) | (1 * DMA_SxFCR_FS_0))
#define DMA_SxFCR_FS_1_2_FL_3_4         ((0 * DMA_SxFCR_FS_2) | (1 * DMA_SxFCR_FS_1) | (0 * DMA_SxFCR_FS_0))
#define DMA_SxFCR_FS_3_4_FL_FULL        ((0 * DMA_SxFCR_FS_2) | (1 * DMA_SxFCR_FS_1) | (1 * DMA_SxFCR_FS_0))
#define DMA_SxFCR_FS_EMPTY              ((1 * DMA_SxFCR_FS_2) | (0 * DMA_SxFCR_FS_1) | (0 * DMA_SxFCR_FS_0))
#define DMA_SxFCR_FS_FULL               ((1 * DMA_SxFCR_FS_2) | (0 * DMA_SxFCR_FS_1) | (1 * DMA_SxFCR_FS_0))

#define DMA_SxFCR_DMDIS_NO              (0  * DMA_SxFCR_DMDIS)
#define DMA_SxFCR_DMDIS_YES             (1  * DMA_SxFCR_DMDIS)

#define DMA_SxFCR_FTH_1_4               ((0 * DMA_SxFCR_FTH_1) | (0 * DMA_SxFCR_FTH_0))
#define DMA_SxFCR_FTH_1_2               ((0 * DMA_SxFCR_FTH_1) | (1 * DMA_SxFCR_FTH_0))
#define DMA_SxFCR_FTH_3_4               ((1 * DMA_SxFCR_FTH_1) | (0 * DMA_SxFCR_FTH_0))
#define DMA_SxFCR_FTH_FULL              ((1 * DMA_SxFCR_FTH_1) | (1 * DMA_SxFCR_FTH_0))

#define DMA_SxFCR_RESET_VALUE           0x21

/*==============================================================================
  Exported object types
==============================================================================*/
typedef bool (*_DMA_cb_t)(DMA_Stream_TypeDef *stream, u8_t SR, void *arg);

typedef struct {
        void     *arg;          /*! user configuration: callback argument */
        _DMA_cb_t callback;     /*! user configuration: finish callback */
        _DMA_cb_t cb_next;      /*! user configuration: next callback */
        u32_t     CR;           /*! user configuration: control register */
        u32_t     NDT;          /*! user configuration: data number */
        u32_t     PA;           /*! user configuration: peripheral address */
        u32_t     MA[2];        /*! user configuration: memory address */
        u32_t     FC;           /*! user configuration: FIFO control */
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
 * @param [in]  stream        stream number.
 *
 * @return On success DMA descriptor number, otherwise 0.
 */
//==============================================================================
extern u32_t _DMA_DDI_reserve(u8_t major, u8_t stream);

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

#endif /* ARCH_stm32f4 */
#endif /* _DMA_DDI_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
