/*==============================================================================
File    spi_lld.c

Author  Daniel Zorychta

Brief   SPI driver.

        Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "spi_cfg.h"
#include "../spi_ioctl.h"
#include "../spi.h"

#if defined(ARCH_stm32f1)
#include "stm32f1/stm32f10x.h"
#include "stm32f1/dma_ddi.h"
#elif defined(ARCH_stm32f4)
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/dma_ddi.h"
#elif defined(ARCH_stm32f7)
#include "stm32f7/stm32f7xx.h"
#include "stm32f7/dma_ddi.h"
#define SPI_CR1_DFF SPI_CR1_CRCL
#endif

/*==============================================================================
  Local macros
==============================================================================*/
#define USE_DMA         (( (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0)\
                        || (_SPI3_USE_DMA > 0) || (_SPI4_USE_DMA > 0)\
                        || (_SPI5_USE_DMA > 0) || (_SPI6_USE_DMA > 0))\
                        && __ENABLE_DMA__)

/*==============================================================================
  Local object types
==============================================================================*/
/* SPI peripheral configuration */
struct SPI_info {
        SPI_TypeDef             *const SPI;             //!< SPI peripheral address
        __IO u32_t              *APBRSTR;               //!< APB reset register address
        __IO u32_t              *APBENR;                //!< APB enable register
        u32_t                    APBRSTRENR;            //!< APB reset/enable bit
        IRQn_Type                IRQn;                  //!< SPI IRQ number
    #if USE_DMA > 0
        bool                     use_DMA;               //!< peripheral uses DMA and IRQ (true), or only IRQ (false)
        u8_t                     DMA_channel;           //!< DMA peripheral request channel number
        u8_t                     DMA_major;             //!< DMA peripheral number
        u8_t                     DMA_tx_stream_pri;     //!< primary Tx stream number
        u8_t                     DMA_tx_stream_alt;     //!< alternative Tx stream number
        u8_t                     DMA_rx_stream_pri;     //!< primary Rx stream number
        u8_t                     DMA_rx_stream_alt;     //!< alternative Rx stream number
    #endif
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
#if USE_DMA > 0
#if defined(ARCH_stm32f1)
static bool DMA_callback(DMA_Channel_TypeDef *stream, u8_t SR, void *arg);
#elif defined(ARCH_stm32f4) || defined(ARCH_stm32f7)
static bool DMA_callback(DMA_Stream_TypeDef *stream, u8_t SR, void *arg);
#endif
#endif

/*==============================================================================
  Local objects
==============================================================================*/
/* SPI peripherals basic parameters */
static const struct SPI_info SPI_HW[_NUMBER_OF_SPI_PERIPHERALS] = {
#if defined(ARCH_stm32f1)
        #if defined(RCC_APB2ENR_SPI1EN)
        {
                .SPI                   = SPI1,
                .APBENR                = &RCC->APB2ENR,
                .APBRSTR               = &RCC->APB2RSTR,
                .APBRSTRENR            = RCC_APB2ENR_SPI1EN,
                .IRQn                  = SPI1_IRQn,
                #if USE_DMA > 0
                .use_DMA               = _SPI1_USE_DMA,
                .DMA_tx_stream_pri     = 3,
                .DMA_tx_stream_alt     = UINT8_MAX,
                .DMA_rx_stream_pri     = 2,
                .DMA_rx_stream_alt     = UINT8_MAX,
                .DMA_major             = 0,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_SPI2EN)
        {
                .SPI                   = SPI2,
                .APBENR                = &RCC->APB1ENR,
                .APBRSTR               = &RCC->APB1RSTR,
                .APBRSTRENR            = RCC_APB1ENR_SPI2EN,
                .IRQn                  = SPI2_IRQn,
                #if USE_DMA > 0
                .use_DMA               = _SPI2_USE_DMA,
                .DMA_tx_stream_pri     = 5,
                .DMA_tx_stream_alt     = UINT8_MAX,
                .DMA_rx_stream_pri     = 4,
                .DMA_rx_stream_alt     = UINT8_MAX,
                .DMA_major             = 0,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_SPI3EN)
        {
                .SPI                   = SPI3,
                .APBENR                = &RCC->APB1ENR,
                .APBRSTR               = &RCC->APB1RSTR,
                .APBRSTRENR            = RCC_APB1ENR_SPI3EN,
                .IRQn                  = SPI3_IRQn,
                #if USE_DMA > 0
                .use_DMA               = _SPI3_USE_DMA,
                .DMA_tx_stream_pri     = 2,
                .DMA_tx_stream_alt     = UINT8_MAX,
                .DMA_rx_stream_pri     = 1,
                .DMA_rx_stream_alt     = UINT8_MAX,
                .DMA_major             = 1,
                #endif
        }
        #endif
#elif defined(ARCH_stm32f4) || defined(ARCH_stm32f7)
        #if defined(RCC_APB2ENR_SPI1EN)
        {
                .SPI                    = SPI1,
                .APBENR                 = &RCC->APB2ENR,
                .APBRSTR                = &RCC->APB2RSTR,
                .APBRSTRENR             = RCC_APB2ENR_SPI1EN,
                .IRQn                   = SPI1_IRQn,
                #if USE_DMA > 0
                .use_DMA                = _SPI1_USE_DMA,
                .DMA_tx_stream_pri      = 3,
                .DMA_tx_stream_alt      = 5,
                .DMA_rx_stream_pri      = 0,
                .DMA_rx_stream_alt      = 2,
                .DMA_channel            = 3,
                .DMA_major              = _DMA_DDI_DMA2,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_SPI2EN)
        {
                .SPI                    = SPI2,
                .APBENR                 = &RCC->APB1ENR,
                .APBRSTR                = &RCC->APB1RSTR,
                .APBRSTRENR             = RCC_APB1ENR_SPI2EN,
                .IRQn                   = SPI2_IRQn,
                #if USE_DMA > 0
                .use_DMA                = _SPI2_USE_DMA,
                .DMA_tx_stream_pri      = 4,
                .DMA_tx_stream_alt      = 4,
                .DMA_rx_stream_pri      = 3,
                .DMA_rx_stream_alt      = 3,
                .DMA_channel            = 0,
                .DMA_major              = _DMA_DDI_DMA1,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_SPI3EN)
        {
                .SPI                    = SPI3,
                .APBENR                 = &RCC->APB1ENR,
                .APBRSTR                = &RCC->APB1RSTR,
                .APBRSTRENR             = RCC_APB1ENR_SPI3EN,
                .IRQn                   = SPI3_IRQn,
                #if USE_DMA > 0
                .use_DMA                = _SPI3_USE_DMA,
                .DMA_tx_stream_pri      = 5,
                .DMA_tx_stream_alt      = 7,
                .DMA_rx_stream_pri      = 0,
                .DMA_rx_stream_alt      = 2,
                .DMA_channel            = 0,
                .DMA_major              = _DMA_DDI_DMA1,
                #endif
        },
        #endif
        #if defined(RCC_APB2ENR_SPI4EN)
        {
                .SPI                    = SPI4,
                .APBENR                 = &RCC->APB2ENR,
                .APBRSTR                = &RCC->APB2RSTR,
                .APBRSTRENR             = RCC_APB2ENR_SPI4EN,
                .IRQn                   = SPI4_IRQn,
                #if USE_DMA > 0
                .use_DMA                = _SPI4_USE_DMA,
                .DMA_tx_stream_pri      = 1,
                .DMA_tx_stream_alt      = 1,
                .DMA_rx_stream_pri      = 0,
                .DMA_rx_stream_alt      = 0,
                .DMA_channel            = 4,
                .DMA_major              = _DMA_DDI_DMA2,
                #endif
        },
        #endif
        #if defined(RCC_APB2ENR_SPI5EN)
        {
                .SPI                    = SPI5,
                .APBENR                 = &RCC->APB2ENR,
                .APBRSTR                = &RCC->APB2RSTR,
                .APBRSTRENR             = RCC_APB2ENR_SPI5EN,
                .IRQn                   = SPI5_IRQn,
                #if USE_DMA > 0
                .use_DMA                = _SPI5_USE_DMA,
                .DMA_tx_stream_pri      = 4,
                .DMA_tx_stream_alt      = 4,
                .DMA_rx_stream_pri      = 3,
                .DMA_rx_stream_alt      = 3,
                .DMA_channel            = 2,
                .DMA_major              = _DMA_DDI_DMA2,
                #endif
        },
        #endif
        #if defined(RCC_APB2ENR_SPI6EN)
        {
                .SPI                    = SPI6,
                .APBENR                 = &RCC->APB2ENR,
                .APBRSTR                = &RCC->APB2RSTR,
                .APBRSTRENR             = RCC_APB2ENR_SPI6EN,
                .IRQn                   = SPI6_IRQn,
                #if USE_DMA > 0
                .use_DMA                = _SPI6_USE_DMA,
                .DMA_tx_stream_pri      = 5,
                .DMA_tx_stream_alt      = 5,
                .DMA_rx_stream_pri      = 6,
                .DMA_rx_stream_alt      = 6,
                .DMA_channel            = 1,
                .DMA_major              = _DMA_DDI_DMA2,
                #endif
        },
        #endif
#endif
};

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function enable SPI interface
 * @param[in] major     SPI major number
 * @return One of errno value.
 */
//==============================================================================
int _SPI_LLD__turn_on(u8_t major)
{
        if (!(*SPI_HW[major].APBENR & SPI_HW[major].APBRSTRENR)) {

                SET_BIT(*SPI_HW[major].APBRSTR, SPI_HW[major].APBRSTRENR);
                CLEAR_BIT(*SPI_HW[major].APBRSTR, SPI_HW[major].APBRSTRENR);
                SET_BIT(*SPI_HW[major].APBENR, SPI_HW[major].APBRSTRENR);

                NVIC_EnableIRQ(SPI_HW[major].IRQn);
                NVIC_SetPriority(SPI_HW[major].IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                return ESUCC;
        } else {
                return EADDRINUSE;
        }
}

//==============================================================================
/**
 * @brief Function disable SPI interface
 * @param[in] major     SPI major number
 * @return None
 */
//==============================================================================
void _SPI_LLD__turn_off(u8_t major)
{
        if (*SPI_HW[major].APBENR & SPI_HW[major].APBRSTRENR) {
                SET_BIT(*SPI_HW[major].APBRSTR, SPI_HW[major].APBRSTRENR);
                CLEAR_BIT(*SPI_HW[major].APBRSTR, SPI_HW[major].APBRSTRENR);
                CLEAR_BIT(*SPI_HW[major].APBENR, SPI_HW[major].APBRSTRENR);

                NVIC_DisableIRQ(SPI_HW[major].IRQn);
        }
}

//==============================================================================
/**
 * @brief Function apply new configuration for selected SPI
 *
 * @param hdl           SPI slave
 */
//==============================================================================
void _SPI_LLD__apply_config(struct SPI_slave *hdl)
{
        static const u16_t divider_mask[] = {
                [SPI_CLK_DIV__2  ] = 0x00,
                [SPI_CLK_DIV__4  ] = SPI_CR1_BR_0,
                [SPI_CLK_DIV__8  ] = SPI_CR1_BR_1,
                [SPI_CLK_DIV__16 ] = SPI_CR1_BR_1 | SPI_CR1_BR_0,
                [SPI_CLK_DIV__32 ] = SPI_CR1_BR_2,
                [SPI_CLK_DIV__64 ] = SPI_CR1_BR_2 | SPI_CR1_BR_0,
                [SPI_CLK_DIV__128] = SPI_CR1_BR_2 | SPI_CR1_BR_1,
                [SPI_CLK_DIV__256] = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0,
        };

        static const u16_t spi_mode_mask[] = {
                [SPI_MODE__0] = 0x00,
                [SPI_MODE__1] = SPI_CR1_CPHA,
                [SPI_MODE__2] = SPI_CR1_CPOL,
                [SPI_MODE__3] = SPI_CR1_CPOL | SPI_CR1_CPHA
        };

        SPI_TypeDef *SPI = SPI_HW[hdl->major].SPI;

        /* clear register */
        WRITE_REG(SPI->CR1, 0);

        /* configure SPI divider */
        SET_BIT(SPI->CR1, divider_mask[hdl->config.clk_divider]);

        /* configure SPI mode */
        SET_BIT(SPI->CR1, spi_mode_mask[hdl->config.mode]);

        /* CRC 8-bit length */
        CLEAR_BIT(SPI->CR1, SPI_CR1_DFF);

        /* set MSB/LSB */
        if (hdl->config.msb_first) {
                CLEAR_BIT(SPI->CR1, SPI_CR1_LSBFIRST);
        } else {
                SET_BIT(SPI->CR1, SPI_CR1_LSBFIRST);
        }

        /* NSS software mode */
        SET_BIT(SPI->CR1, SPI_CR1_SSM | SPI_CR1_SSI);

        /* set SPI as master */
        SET_BIT(SPI->CR1, SPI_CR1_MSTR);

#if defined(ARCH_stm32f7)
        WRITE_REG(SPI->CR2, SPI_CR2_FRXTH | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0);
#endif

        /* enable peripheral */
        SET_BIT(SPI->CR1, SPI_CR1_SPE);
}

//==============================================================================
/**
 * @brief Function apply SPI device safe configuration
 *
 * @param major         SPI major number
 */
//==============================================================================
void _SPI_LLD__halt(u8_t major)
{
        SPI_TypeDef *SPI = SPI_HW[major].SPI;

        while (SPI->SR & SPI_SR_BSY);

        CLEAR_BIT(SPI->CR1, SPI_CR1_SPE);
        SET_BIT(SPI->CR1, SPI_CR1_MSTR);
}

//==============================================================================
/**
 * @brief  Transceive data by using IRQs or DMA
 * @param  hdl          virtual SPI handler
 * @param  txbuf        source buffer (can be NULL for RX only)
 * @param  rxbuf        destination buffer (can be NULL for TX only)
 * @param  count        number of bytes to transfer
 * @return One of errno value
 */
//==============================================================================
int _SPI_LLD__transceive(struct SPI_slave *hdl, const u8_t *txbuf, u8_t *rxbuf, size_t count)
{
        int err = EIO;
        _SPI[hdl->major]->slave = hdl;

#if USE_DMA > 0
        bool dma_capable = true;

        if (  (count < 16)
           || (txbuf && !sys_is_mem_dma_capable(txbuf))
           || (rxbuf && !sys_is_mem_dma_capable(rxbuf)) ) {
                dma_capable = false;
        }

        // try to send/receive buffers by using DMA
        if (SPI_HW[hdl->major].use_DMA && dma_capable) {
                // reserve TX stream
                u32_t dmadtx = _DMA_DDI_reserve(SPI_HW[hdl->major].DMA_major,
                                                SPI_HW[hdl->major].DMA_tx_stream_pri);
                if (dmadtx == 0) {
                        dmadtx = _DMA_DDI_reserve(SPI_HW[hdl->major].DMA_major,
                                                  SPI_HW[hdl->major].DMA_tx_stream_alt);
                }

                // reserve RX stream
                u32_t dmadrx = _DMA_DDI_reserve(SPI_HW[hdl->major].DMA_major,
                                                SPI_HW[hdl->major].DMA_rx_stream_pri);
                if (dmadrx == 0) {
                        dmadrx = _DMA_DDI_reserve(SPI_HW[hdl->major].DMA_major,
                                                  SPI_HW[hdl->major].DMA_rx_stream_alt);
                }

                // configure channels
                if (dmadtx && dmadrx) {
                        CLEAR_BIT(SPI_HW[hdl->major].SPI->CR2, SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);

                        _SPI[hdl->major]->flush_byte = hdl->config.flush_byte;
                        _SPI[hdl->major]->count      = count;

                        _DMA_DDI_config_t config_tx = {0};
                        config_tx.user_ctx                             = NULL;
                        config_tx.cb_finish                            = NULL;
                        config_tx.cb_half                              = NULL;
                        config_tx.cb_next                              = NULL;
                        config_tx.data_number                          = count;
                        config_tx.peripheral_address                   = cast(u32_t, &SPI_HW[hdl->major].SPI->DR);
                        config_tx.memory_address[0]                    = cast(u32_t, txbuf ? txbuf : &_SPI[hdl->major]->flush_byte);
                        config_tx.memory_address[1]                    = 0;
                        config_tx.IRQ_priority                         = __CPU_DEFAULT_IRQ_PRIORITY__;
                        config_tx.channel                              = SPI_HW[hdl->major].DMA_channel;
                        config_tx.release                              = true;
                        config_tx.fifo.direct_mode                     = _DMA_DDI_DIRECT_MODE_ENABLED;
                        config_tx.control.memory_burst                 = _DMA_DDI_MEMORY_BURST_SINGLE_TRANSFER;
                        config_tx.control.peripheral_burst             = _DMA_DDI_PERIPHERAL_BURST_SINGLE_TRANSFER;
                        config_tx.control.double_buffer_mode           = _DMA_DDI_DOUBLE_BUFFER_MODE_DISABLED;
                        config_tx.control.priority_level               = _DMA_DDI_PRIORITY_LEVEL_LOW;
                        config_tx.control.peripheral_increment_offset  = _DMA_DDI_PERIPHERAL_INCREMENT_OFFSET_ACCORDING_TO_PERIPHERAL_SIZE;
                        config_tx.control.memory_data_size             = _DMA_DDI_MEMORY_DATA_SIZE_BYTE;
                        config_tx.control.peripheral_data_size         = _DMA_DDI_PERIPHERAL_DATA_SIZE_BYTE;
                        config_tx.control.memory_increment             = (txbuf ? _DMA_DDI_MEMORY_ADDRESS_POINTER_INCREMENTED_ACCORDING_TO_MEMORY_SIZE : _DMA_DDI_MEMORY_ADDRESS_POINTER_IS_FIXED);
                        config_tx.control.peripheral_address_increment = _DMA_DDI_PERIPHERAL_ADDRESS_POINTER_IS_FIXED;
                        config_tx.control.circular_mode                = _DMA_DDI_CIRCULAR_MODE_DISABLED;
                        config_tx.control.transfer_direction           = _DMA_DDI_TRANSFER_DIRECTION_MEMORY_TO_PERIPHERAL;
                        config_tx.control.flow_controller              = _DMA_DDI_FLOW_CONTROLLER_DMA;

                        _DMA_DDI_config_t config_rx = {0};
                        config_rx.user_ctx                             = hdl;
                        config_rx.cb_finish                            = DMA_callback;
                        config_rx.cb_half                              = NULL;
                        config_rx.cb_next                              = NULL;
                        config_rx.data_number                          = count;
                        config_rx.peripheral_address                   = cast(u32_t, &SPI_HW[hdl->major].SPI->DR);
                        config_rx.memory_address[0]                    = cast(u32_t, rxbuf ? rxbuf : &_SPI[hdl->major]->flush_byte);
                        config_rx.memory_address[1]                    = 0;
                        config_rx.IRQ_priority                         = __CPU_DEFAULT_IRQ_PRIORITY__;
                        config_rx.channel                              = SPI_HW[hdl->major].DMA_channel;
                        config_rx.release                              = true;
                        config_rx.fifo.direct_mode                     = _DMA_DDI_DIRECT_MODE_ENABLED;
                        config_rx.control.memory_burst                 = _DMA_DDI_MEMORY_BURST_SINGLE_TRANSFER;
                        config_rx.control.peripheral_burst             = _DMA_DDI_PERIPHERAL_BURST_SINGLE_TRANSFER;
                        config_rx.control.double_buffer_mode           = _DMA_DDI_DOUBLE_BUFFER_MODE_DISABLED;
                        config_rx.control.priority_level               = _DMA_DDI_PRIORITY_LEVEL_LOW;
                        config_rx.control.peripheral_increment_offset  = _DMA_DDI_PERIPHERAL_INCREMENT_OFFSET_ACCORDING_TO_PERIPHERAL_SIZE;
                        config_rx.control.memory_data_size             = _DMA_DDI_MEMORY_DATA_SIZE_BYTE;
                        config_rx.control.peripheral_data_size         = _DMA_DDI_PERIPHERAL_DATA_SIZE_BYTE;
                        config_rx.control.memory_increment             = (rxbuf ? _DMA_DDI_MEMORY_ADDRESS_POINTER_INCREMENTED_ACCORDING_TO_MEMORY_SIZE : _DMA_DDI_MEMORY_ADDRESS_POINTER_IS_FIXED);
                        config_rx.control.peripheral_address_increment = _DMA_DDI_PERIPHERAL_ADDRESS_POINTER_IS_FIXED;
                        config_rx.control.circular_mode                = _DMA_DDI_CIRCULAR_MODE_DISABLED;
                        config_rx.control.transfer_direction           = _DMA_DDI_TRANSFER_DIRECTION_PERIPHERAL_TO_MEMORY;
                        config_rx.control.flow_controller              = _DMA_DDI_FLOW_CONTROLLER_DMA;

                        err = _DMA_DDI_transfer(dmadrx, &config_rx);
                        if (!err) {
                                err = _DMA_DDI_transfer(dmadtx, &config_tx);
                                if (!err) {
                                        SET_BIT(SPI_HW[hdl->major].SPI->CR2, SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);

                                        err = sys_semaphore_wait(_SPI[hdl->major]->wait_irq_sem,
                                                                 _SPI_IRQ_WAIT_TIMEOUT);

                                        _DMA_DDI_release(dmadtx);
                                        _DMA_DDI_release(dmadrx);
                                }
                        }

                        return err;
                } else {
                        _DMA_DDI_release(dmadtx);
                        _DMA_DDI_release(dmadrx);
                }
        }
#endif

        // send/receive buffers by using IRQs
        _SPI[hdl->major]->tx_buffer  = txbuf;
        _SPI[hdl->major]->rx_buffer  = rxbuf;
        _SPI[hdl->major]->count      = count;
        _SPI[hdl->major]->flush_byte = hdl->config.flush_byte;

        SET_BIT(SPI_HW[hdl->major].SPI->CR2, SPI_CR2_TXEIE);

        err = sys_semaphore_wait(_SPI[hdl->major]->wait_irq_sem, _SPI_IRQ_WAIT_TIMEOUT);

        return err;
}

//==============================================================================
/**
 * @brief  Function handle SPI IRQ
 * @param  major        SPI device number
 * @return If task was woken then true is returned, otherwise false
 */
//==============================================================================
static void handle_SPI_IRQ(u8_t major)
{
        bool         woken = false;
        SPI_TypeDef *spi   = SPI_HW[major].SPI;

        /* transmit data by using Tx register */
        if ((spi->SR & SPI_SR_TXE) && (spi->CR2 & SPI_CR2_TXEIE)) {

                if (_SPI[major]->count > 0) {
                        if (_SPI[major]->tx_buffer) {
                                u8_t byte = *(_SPI[major]->tx_buffer++);
#if defined(ARCH_stm32f7)
                                uint8_t *DR = (uint8_t*)&spi->DR;
                                *DR = byte;
#else
                                spi->DR = byte;
#endif
                        } else {
                                u8_t byte = _SPI[major]->flush_byte;
#if defined(ARCH_stm32f7)
                                uint8_t *DR = (uint8_t*)&spi->DR;
                                *DR = byte;
#else
                                spi->DR = byte;
#endif
                        }
                }

                SET_BIT(spi->CR2, SPI_CR2_RXNEIE);
                CLEAR_BIT(spi->CR2, SPI_CR2_TXEIE);
        }

        /* receive data from RX register */
        if ((spi->SR & SPI_SR_RXNE) && (spi->CR2 & SPI_CR2_RXNEIE)) {
                u8_t byte = spi->DR;

                if (_SPI[major]->count > 0) {
                        if (_SPI[major]->rx_buffer) {
                                *(_SPI[major]->rx_buffer++) = byte;
                        }

                        _SPI[major]->count--;
                }

                SET_BIT(spi->CR2, SPI_CR2_TXEIE);
        }

        /* finish transmission if all frames are received and transmitted */
        if (_SPI[major]->count == 0) {
                CLEAR_BIT(spi->CR2, SPI_CR2_RXNEIE);
                CLEAR_BIT(spi->CR2, SPI_CR2_TXEIE);
                sys_semaphore_signal_from_ISR(_SPI[major]->wait_irq_sem, &woken);
        }

        sys_thread_yield_from_ISR(woken);
}

#if USE_DMA > 0
//==============================================================================
/**
 * @brief  DMA IRQ handler
 * @param  stream       DMA stream/channel
 * @param  major        SPI major number
 * @return If task was woken then true is returned, otherwise false
 */
//==============================================================================
#if defined(ARCH_stm32f1)
static bool DMA_callback(DMA_Channel_TypeDef *stream, u8_t SR, void *arg)
#elif defined(ARCH_stm32f4) || defined(ARCH_stm32f7)
static bool DMA_callback(DMA_Stream_TypeDef *stream, u8_t SR, void *arg)
#endif
{
        UNUSED_ARG2(stream, SR);

        struct SPI_slave *hdl = arg;

        CLEAR_BIT(SPI_HW[hdl->major].SPI->CR2, SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);

        bool woken = false;
        sys_semaphore_signal_from_ISR(_SPI[hdl->major]->wait_irq_sem, &woken);

        return woken;
}
#endif

#if defined(RCC_APB2ENR_SPI1EN)
//==============================================================================
/**
 * @brief SPI1 IRQ handler
 */
//==============================================================================
void SPI1_IRQHandler(void)
{
        handle_SPI_IRQ(_SPI1);
}
#endif

//==============================================================================

#if defined(RCC_APB1ENR_SPI2EN)
//==============================================================================
/**
 * @brief SPI2 IRQ handler
 */
//==============================================================================
void SPI2_IRQHandler(void)
{
        handle_SPI_IRQ(_SPI2);
}
#endif

#if defined(RCC_APB1ENR_SPI3EN)
//==============================================================================
/**
 * @brief SPI3 IRQ handler
 */
//==============================================================================
void SPI3_IRQHandler(void)
{
        handle_SPI_IRQ(_SPI3);
}
#endif

#if defined(RCC_APB2ENR_SPI4EN)
//==============================================================================
/**
 * @brief SPI4 IRQ handler
 */
//==============================================================================
void SPI4_IRQHandler(void)
{
        handle_SPI_IRQ(_SPI4);
}
#endif

#if defined(RCC_APB2ENR_SPI5EN)
//==============================================================================
/**
 * @brief SPI5 IRQ handler
 */
//==============================================================================
void SPI5_IRQHandler(void)
{
        handle_SPI_IRQ(_SPI5);
}
#endif

#if defined(RCC_APB2ENR_SPI6EN)
//==============================================================================
/**
 * @brief SPI6 IRQ handler
 */
//==============================================================================
void SPI6_IRQHandler(void)
{
        handle_SPI_IRQ(_SPI6);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
