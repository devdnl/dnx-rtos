/*=========================================================================*//**
@file    i2c_lld.c

@author  Daniel Zorychta

@brief   This driver support I2C peripherals.

@note    Copyright (C) 2017  Daniel Zorychta <daniel.zorychta@gmail.com>

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

// NOTE: 10-bit addressing mode is experimental and not tested!

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "stm32f4/i2c_cfg.h"
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/dma_ddi.h"
#include "i2c_ioctl.h"
#include "i2c.h"
#include "lib/stm32f4xx_rcc.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define USE_DMA ((_I2C1_USE_DMA > 0) || (_I2C2_USE_DMA > 0) || (_I2C3_USE_DMA > 0))

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/// type defines configuration of single I2C peripheral
typedef struct {
    #if USE_DMA > 0
        const bool                use_DMA;              //!< peripheral uses DMA and IRQ (true), or only IRQ (false)
        const u8_t                DMA_tx_stream_pri;    //!< primary Tx stream number
        const u8_t                DMA_tx_stream_alt;    //!< alternative Tx stream number
        const u8_t                DMA_rx_stream_pri;    //!< primary Rx stream number
        const u8_t                DMA_rx_stream_alt;    //!< alternative Rx stream number
        const u8_t                DMA_channel;          //!< DMA peripheral request channel number
    #endif
        const I2C_TypeDef        *const I2C;            //!< pointer to the I2C peripheral
        const u32_t               freq;                 //!< peripheral SCL frequency [Hz]
        const u32_t               APB1ENR_clk_mask;     //!< mask used to enable I2C clock in the APB1ENR register
        const IRQn_Type           IRQ_EV_n;             //!< number of event IRQ vector
        const IRQn_Type           IRQ_ER_n;             //!< number of error IRQ vector
} I2C_info_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static inline I2C_TypeDef *get_I2C(I2C_dev_t *hdl);
static void clear_send_address_event(I2C_dev_t *hdl);
static void IRQ_EV_handler(u8_t major);
static void IRQ_ER_handler(u8_t major);

#if USE_DMA > 0
static bool DMA_callback(DMA_Stream_TypeDef *stream, u8_t SR, void *arg);
static bool wait_for_DMA_event(I2C_dev_t *hdl);
#endif

/*==============================================================================
  Local object definitions
==============================================================================*/
/// peripherals configuration
static const I2C_info_t I2C_HW[_I2C_NUMBER_OF_PERIPHERALS] = {
        #if defined(RCC_APB1ENR_I2C1EN)
        {
                #if USE_DMA > 0
                .use_DMA           = _I2C1_USE_DMA,
                .DMA_tx_stream_pri = 6,
                .DMA_tx_stream_alt = 7,
                .DMA_rx_stream_pri = 0,
                .DMA_rx_stream_alt = 5,
                .DMA_channel       = 1,
                #endif
                .I2C               = I2C1,
                .freq              = _I2C1_FREQUENCY,
                .APB1ENR_clk_mask  = RCC_APB1ENR_I2C1EN,
                .IRQ_EV_n          = I2C1_EV_IRQn,
                .IRQ_ER_n          = I2C1_ER_IRQn,
        },
        #endif
        #if defined(RCC_APB1ENR_I2C2EN)
        {
                #if USE_DMA > 0
                .use_DMA           = _I2C2_USE_DMA,
                .DMA_tx_stream_pri = 7,
                .DMA_tx_stream_alt = 7,
                .DMA_rx_stream_pri = 2,
                .DMA_rx_stream_alt = 3,
                .DMA_channel       = 7,
                #endif
                .I2C               = I2C2,
                .freq              = _I2C2_FREQUENCY,
                .APB1ENR_clk_mask  = RCC_APB1ENR_I2C2EN,
                .IRQ_EV_n          = I2C2_EV_IRQn,
                .IRQ_ER_n          = I2C2_ER_IRQn,
        },
        #endif
        #if defined(RCC_APB1ENR_I2C3EN)
        {
                #if USE_DMA > 0
                .use_DMA           = _I2C3_USE_DMA,
                .DMA_tx_stream_pri = 4,
                .DMA_tx_stream_alt = 4,
                .DMA_rx_stream_pri = 2,
                .DMA_rx_stream_alt = 2,
                .DMA_channel       = 3,
                #endif
                .I2C               = I2C3,
                .freq              = _I2C3_FREQUENCY,
                .APB1ENR_clk_mask  = RCC_APB1ENR_I2C3EN,
                .IRQ_EV_n          = I2C3_EV_IRQn,
                .IRQ_ER_n          = I2C3_ER_IRQn,
        },
        #endif
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief  Returns I2C address of current device
 * @param  hdl          device handle
 * @return I2C peripheral address
 */
//==============================================================================
static inline I2C_TypeDef *get_I2C(I2C_dev_t *hdl)
{
        return const_cast(I2C_TypeDef*, I2C_HW[hdl->major].I2C);
}

//==============================================================================
/**
 * @brief  Function handle error (try make the interface working)
 * @param  hdl          device handle
 */
//==============================================================================
static void reset(I2C_dev_t *hdl, bool reinit)
{
        I2C_TypeDef *i2c = get_I2C(hdl);

        CLEAR_BIT(i2c->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN);

        u8_t tmp = i2c->SR1;
             tmp = i2c->SR2;
             tmp = i2c->DR;
             tmp = i2c->DR;

        UNUSED_ARG1(tmp);

        i2c->SR1 = 0;

        sys_sleep_ms(1);

        if (reinit) {
                _I2C_LLD__init(hdl->major);
        }

        _I2C_LLD__stop(hdl);
}

//==============================================================================
/**
 * @brief  Function wait for selected event (IRQ)
 * @param  hdl                  device handle
 * @param  SR1_event_mask       event mask (bits from SR1 register)
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int wait_for_I2C_event(I2C_dev_t *hdl, u16_t SR1_event_mask)
{
        I2C_TypeDef *i2c = get_I2C(hdl);

        _I2C[hdl->major]->SR1_mask = SR1_event_mask;

        u16_t CR2 = I2C_CR2_ITEVTEN | I2C_CR2_ITERREN;

        if (SR1_event_mask & (I2C_SR1_RXNE | I2C_SR1_TXE)) {
                CR2 |= I2C_CR2_ITBUFEN;
        }

        SET_BIT(i2c->CR2, CR2);

        int ferr = EIO;
        int err  = sys_queue_receive(_I2C[hdl->major]->event, &ferr, _I2C_DEVICE_TIMEOUT);
        if (!err) {
                err = ferr;
        }

        if (err) {
                reset(hdl, true);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function wait for DMA event (IRQ)
 * @param  hdl                  device handle
 * @param  DMA                  DMA channel
 * @return One of errno value (errno.h)
 */
//==============================================================================
#if USE_DMA > 0
static bool wait_for_DMA_event(I2C_dev_t *hdl)
{
        I2C_TypeDef *i2c = get_I2C(hdl);
        SET_BIT(i2c->CR2, I2C_CR2_LAST | I2C_CR2_DMAEN);

        int ferr = EIO;
        int err  = sys_queue_receive(_I2C[hdl->major]->event, &ferr, _I2C_DEVICE_TIMEOUT);

        CLEAR_BIT(i2c->CR2, I2C_CR2_LAST | I2C_CR2_DMAEN);

        if (!err) {
                err = ferr;
        }

        if (err) {
                reset(hdl, true);
        }

        return false;
}
#endif

//==============================================================================
/**
 * @brief  Clear event of send address
 * @param  hdl                  device handle
 */
//==============================================================================
static void clear_send_address_event(I2C_dev_t *hdl)
{
        I2C_TypeDef *i2c = get_I2C(hdl);

        sys_critical_section_begin();
        {
                u16_t tmp;
                tmp = i2c->SR1;
                tmp = i2c->SR2;
                (void)tmp;
        }
        sys_critical_section_end();
}

//==============================================================================
/**
 * @brief  Enables selected I2C peripheral according with configuration
 * @param  major        peripheral number
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__init(u8_t major)
{
        const I2C_info_t *cfg = &I2C_HW[major];
        I2C_TypeDef      *i2c = const_cast(I2C_TypeDef*, I2C_HW[major].I2C);

        SET_BIT(RCC->APB1RSTR, cfg->APB1ENR_clk_mask);
        CLEAR_BIT(RCC->APB1RSTR, cfg->APB1ENR_clk_mask);

        CLEAR_BIT(RCC->APB1ENR, cfg->APB1ENR_clk_mask);
        SET_BIT(RCC->APB1ENR, cfg->APB1ENR_clk_mask);

        RCC_ClocksTypeDef clocks;
        memset(&clocks, 0, sizeof(RCC_ClocksTypeDef));
        RCC_GetClocksFreq(&clocks);
        clocks.PCLK1_Frequency /= 1000000;

        if (clocks.PCLK1_Frequency < 2) {
                return EIO;
        }

        NVIC_EnableIRQ(cfg->IRQ_EV_n);
        NVIC_EnableIRQ(cfg->IRQ_ER_n);
        NVIC_SetPriority(cfg->IRQ_EV_n, _CPU_IRQ_SAFE_PRIORITY_);
        NVIC_SetPriority(cfg->IRQ_ER_n, _CPU_IRQ_SAFE_PRIORITY_);

        u16_t CCR;
        if (cfg->freq <= 100000) {
                CCR = ((1000000/(2 * cfg->freq)) * clocks.PCLK1_Frequency) & I2C_CCR_CCR;
        } else if (cfg->freq < 400000){
                CCR  = ((10000000/(3 * cfg->freq)) * clocks.PCLK1_Frequency / 10) & I2C_CCR_CCR;
                CCR |= I2C_CCR_FS;
        } else {
                CCR  = ((100000000/(25 * cfg->freq)) * clocks.PCLK1_Frequency / 100 + 1) & I2C_CCR_CCR;
                CCR |= I2C_CCR_FS | I2C_CCR_DUTY;
        }

        i2c->CR1   = I2C_CR1_SWRST;
        i2c->CR1   = 0;
        i2c->CR2   = clocks.PCLK1_Frequency & I2C_CR2_FREQ;;
        i2c->CCR   = CCR;
        i2c->TRISE = clocks.PCLK1_Frequency + 1;
        i2c->CR1   = I2C_CR1_PE;

        _I2C[major]->initialized = true;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Disables selected I2C peripheral
 * @param  major        I2C peripheral number
 */
//==============================================================================
void _I2C_LLD__release(u8_t major)
{
        const I2C_info_t *cfg = &I2C_HW[major];
        I2C_TypeDef            *i2c = const_cast(I2C_TypeDef*, I2C_HW[major].I2C);

        NVIC_DisableIRQ(cfg->IRQ_EV_n);
        NVIC_DisableIRQ(cfg->IRQ_ER_n);

        WRITE_REG(i2c->CR1, 0);
        SET_BIT(RCC->APB1RSTR, cfg->APB1ENR_clk_mask);
        CLEAR_BIT(RCC->APB1RSTR, cfg->APB1ENR_clk_mask);
        CLEAR_BIT(RCC->APB1ENR, cfg->APB1ENR_clk_mask);

        _I2C[major]->initialized = false;
}

//==============================================================================
/**
 * @brief  Function generate START sequence on I2C bus
 * @param  hdl                  device handle
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__start(I2C_dev_t *hdl)
{
        I2C_TypeDef *i2c = get_I2C(hdl);

        CLEAR_BIT(i2c->CR1, I2C_CR1_STOP);
        SET_BIT(i2c->CR1, I2C_CR1_START | I2C_CR1_ACK);

        return wait_for_I2C_event(hdl, I2C_SR1_SB);
}

//==============================================================================
/**
 * @brief  Function generate REPEAT START sequence on I2C bus
 * @param  hdl                  device handle
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__repeat_start(I2C_dev_t *hdl)
{
        I2C_TypeDef *i2c = get_I2C(hdl);

        CLEAR_BIT(i2c->CR1, I2C_CR1_STOP);
        SET_BIT(i2c->CR1, I2C_CR1_START | I2C_CR1_ACK);

        u32_t tref = sys_time_get_reference();

        while (!(i2c->SR1 & I2C_SR1_SB)) {
                if (sys_time_is_expired(tref, _I2C_DEVICE_TIMEOUT)) {
                        return EIO;
                }

                if (i2c->SR1 & (I2C_SR1_ARLO | I2C_SR1_BERR)) {
                        return EIO;
                }
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function generate STOP sequence on I2C bus
 * @param  hdl                  device handle
 */
//==============================================================================
void _I2C_LLD__stop(I2C_dev_t *hdl)
{
        I2C_TypeDef *i2c = get_I2C(hdl);

        SET_BIT(i2c->CR1, I2C_CR1_STOP);
}

//==============================================================================
/**
 * @brief  Function send I2C address sequence
 * @param  hdl                  device handle
 * @param  write                true: compose write address
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__send_address(I2C_dev_t *hdl, bool write)
{
        int err = EIO;

        I2C_TypeDef *i2c = get_I2C(hdl);

        if (hdl->config.addr_10bit) {
                u8_t hdr = _I2C_HEADER_ADDR_10BIT | ((hdl->config.address >> 7) & 0x6);

                // send header + 2 most significant bits of 10-bit address
                i2c->DR = _I2C_HEADER_ADDR_10BIT | ((hdl->config.address & 0xFE) >> 7);
                err = wait_for_I2C_event(hdl, I2C_SR1_ADD10);
                if (err) goto finish;

                // send rest 8 bits of 10-bit address
                u8_t  addr = hdl->config.address & 0xFF;
                u16_t tmp  = i2c->SR1;
                (void)tmp;   i2c->DR = addr;
                err = wait_for_I2C_event(hdl, I2C_SR1_ADDR);
                if (err) goto finish;

                clear_send_address_event(hdl);

                // send repeat start
                err = _I2C_LLD__start(hdl);
                if (err) goto finish;

                // send header
                i2c->DR = write ? hdr : hdr | 0x01;
                err = wait_for_I2C_event(hdl, I2C_SR1_ADDR);

        } else {
                u16_t tmp = i2c->SR1;
                      tmp = i2c->SR2;
                (void)tmp;
                i2c->DR = write ? (hdl->config.address & 0xFE) : (hdl->config.address | 0x01);
                err = wait_for_I2C_event(hdl, I2C_SR1_ADDR);
        }

        finish:
        if (!err) {
                clear_send_address_event(hdl);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function receive bytes from I2C bus (master-receiver)
 * @param  hdl                  device handle
 * @param  dst                  destination buffer
 * @param  count                number of bytes to receive
 * @param  rdcnt                number of read bytes
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__receive(I2C_dev_t *hdl, u8_t *dst, size_t count, size_t *rdcnt)
{
        int          err = EIO;
        ssize_t      n   = 0;
        I2C_TypeDef *i2c = get_I2C(hdl);

        if (count == 2) {
                CLEAR_BIT(i2c->CR1, I2C_CR1_ACK);
                SET_BIT(i2c->CR1, I2C_CR1_POS);
        } else {
                CLEAR_BIT(i2c->CR1, I2C_CR1_POS);
                SET_BIT(i2c->CR1, I2C_CR1_ACK);
        }

        if (count >= 3) {
                clear_send_address_event(hdl);

#if USE_DMA > 0
                if (I2C_HW[hdl->major].use_DMA) {
                        u32_t dmad = _DMA_DDI_reserve(0, I2C_HW[hdl->major].DMA_rx_stream_pri);
                        if (dmad == 0) {
                                dmad = _DMA_DDI_reserve(0, I2C_HW[hdl->major].DMA_rx_stream_alt);
                        }

                        if (dmad) {
                                CLEAR_BIT(i2c->CR2, I2C_CR2_ITBUFEN | I2C_CR2_ITERREN | I2C_CR2_ITEVTEN);

                                _DMA_DDI_config_t config;
                                config.arg      = _I2C[hdl->major];
                                config.callback = DMA_callback;
                                config.release  = true;
                                config.PA       = cast(u32_t, &i2c->DR);
                                config.MA[0]    = cast(u32_t, dst);
                                config.NDT      = count;
                                config.FC       = DMA_SxFCR_FTH_0 | DMA_SxFCR_FS_2;
                                config.CR       = ((I2C_HW[hdl->major].DMA_channel & 7) << DMA_SxCR_CHSEL_Pos)
                                                | DMA_SxCR_MINC;

                                if (_DMA_DDI_transfer(dmad, &config) == ESUCC) {
                                        err = wait_for_DMA_event(hdl);
                                        if (!err) {
                                                n = count;
                                                goto finish;
                                        }
                                }

                                _DMA_DDI_release(dmad);
                        }
                }
#endif
                {
                        while (count) {
                                if (count == 3) {
                                        err = wait_for_I2C_event(hdl, I2C_SR1_BTF);
                                        if (err) break;

                                        CLEAR_BIT(i2c->CR1, I2C_CR1_ACK);
                                        *dst++ = i2c->DR;
                                        n++;

                                        err = wait_for_I2C_event(hdl, I2C_SR1_RXNE);
                                        if (err) break;

                                        _I2C_LLD__stop(hdl);
                                        *dst++ = i2c->DR;
                                        n++;

                                        err = wait_for_I2C_event(hdl, I2C_SR1_RXNE);
                                        if (err) break;

                                        *dst++ = i2c->DR;
                                        n++;

                                        count = 0;
                                } else {
                                        err = wait_for_I2C_event(hdl, I2C_SR1_RXNE);
                                        if (!err) {
                                                *dst++ = i2c->DR;
                                                count--;
                                                n++;
                                        } else {
                                                break;
                                        }
                                }
                        }
                }

        } else if (count == 2) {
                sys_critical_section_begin();
                {
                        clear_send_address_event(hdl);
                        CLEAR_BIT(i2c->CR1, I2C_CR1_ACK);
                }
                sys_critical_section_end();

                err = wait_for_I2C_event(hdl, I2C_SR1_BTF);
                if (!err) {
                        _I2C_LLD__stop(hdl);

                        *dst++ = i2c->DR;
                        *dst++ = i2c->DR;
                        n     += 2;
                }

        } else if (count == 1) {
                CLEAR_BIT(i2c->CR1, I2C_CR1_ACK);
                clear_send_address_event(hdl);
                _I2C_LLD__stop(hdl);

                err = wait_for_I2C_event(hdl, I2C_SR1_RXNE);
                if (!err) {
                        *dst++ = i2c->DR;
                        n     += 1;
                }
        }

        finish:
        *rdcnt = n;

        return err;
}

//==============================================================================
/**
 * @brief  Function transmit selected amount bytes to I2C bus
 * @param  hdl                  device handle
 * @param  src                  data source
 * @param  count                number of bytes to transfer
 * @param  wrcnt                number of written bytes
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__transmit(I2C_dev_t *hdl, const u8_t *src, size_t count, size_t *wrcnt)
{
        int          err  = EIO;
        ssize_t      n    = 0;
        I2C_TypeDef *i2c  = get_I2C(hdl);

        clear_send_address_event(hdl);

#if USE_DMA > 0
        if (count >= 3 && I2C_HW[hdl->major].use_DMA) {
                u32_t dmad = _DMA_DDI_reserve(0, I2C_HW[hdl->major].DMA_tx_stream_pri);
                if (dmad == 0) {
                        dmad = _DMA_DDI_reserve(0, I2C_HW[hdl->major].DMA_tx_stream_alt);
                }

                if (dmad) {
                        CLEAR_BIT(i2c->CR2, I2C_CR2_ITBUFEN | I2C_CR2_ITERREN | I2C_CR2_ITEVTEN);

                        _DMA_DDI_config_t config;
                        config.arg      = _I2C[hdl->major];
                        config.callback = DMA_callback;
                        config.release  = true;
                        config.PA       = cast(u32_t, &i2c->DR);
                        config.MA[0]    = cast(u32_t, src);
                        config.NDT      = count;
                        config.FC       = DMA_SxFCR_FTH_0 | DMA_SxFCR_FS_2;
                        config.CR       = ((I2C_HW[hdl->major].DMA_channel & 7) << DMA_SxCR_CHSEL_Pos)
                                        | DMA_SxCR_MINC | DMA_SxCR_DIR_0;

                        if (_DMA_DDI_transfer(dmad, &config) == ESUCC) {
                                err = wait_for_DMA_event(hdl);
                                if (!err) {

                                        /*
                                         *  Master transmitter: In the interrupt routine after the EOT
                                         *  interrupt, disable DMA requests then wait for a BTF event
                                         *  before programming the Stop condition.
                                         */
                                        u32_t tref = sys_time_get_reference();

                                        while (!(i2c->SR1 & I2C_SR1_BTF)) {
                                                if (sys_time_is_expired(tref, _I2C_DEVICE_TIMEOUT)) {
                                                        err = EIO;
                                                        goto finish;
                                                }
                                        }

                                        n = count;
                                        goto finish;
                                }
                        }

                        _DMA_DDI_release(dmad);
                }
        }
#endif
        {
                while (count) {
                        err = wait_for_I2C_event(hdl, I2C_SR1_TXE);
                        if (!err) {
                                i2c->DR = *src++;
                        } else {
                                break;
                        }

                        n++;
                        count--;
                }
        }

        if (n && !err) {
                err = wait_for_I2C_event(hdl, I2C_SR1_BTF);
                if (err) {
                        n = n - 1;
                }
        }

        finish:
        *wrcnt = n;

        return err;
}

//==============================================================================
/**
 * @brief  Event IRQ handler (transaction state machine)
 * @param  major        number of peripheral
 */
//==============================================================================
static void IRQ_EV_handler(u8_t major)
{
        bool woken = false;

        I2C_TypeDef *i2c = const_cast(I2C_TypeDef*, I2C_HW[major].I2C);
        u16_t SR1 = i2c->SR1;
        u16_t SR2 = i2c->SR2;
        UNUSED_ARG1(SR2);

        if (SR1 & _I2C[major]->SR1_mask) {
                int err = ESUCC;
                sys_queue_send_from_ISR(_I2C[major]->event, &err, &woken);
                CLEAR_BIT(i2c->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN);
                _I2C[major]->unexp_event_cnt = 0;
        } else {
                /*
                 * This counter is used to check if there is no death loop of
                 * not handled IRQ. If counter reach specified value then
                 * the error flag is set.
                 */
                if (++_I2C[major]->unexp_event_cnt >= 16) {
                        int err = EIO;
                        sys_queue_send_from_ISR(_I2C[major]->event, &err, &woken);
                        CLEAR_BIT(I2C1->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN);
                }
        }

        sys_thread_yield_from_ISR(woken);
}

//==============================================================================
/**
 * @brief  Error IRQ handler
 * @param  major        number of peripheral
 */
//==============================================================================
static void IRQ_ER_handler(u8_t major)
{
        I2C_TypeDef *i2c = const_cast(I2C_TypeDef*, I2C_HW[major].I2C);
        u16_t SR1 = i2c->SR1;
        u16_t SR2 = i2c->SR2;
        UNUSED_ARG1(SR2);

        int err = ESUCC;

        if (SR1 & I2C_SR1_ARLO) {
                err = EAGAIN;

        } else if (SR1 & I2C_SR1_AF) {
                if (_I2C[major]->SR1_mask & (I2C_SR1_ADDR | I2C_SR1_ADD10))
                        err = ENXIO;
                else
                        err = EIO;
        } else {
                err = EIO;
        }

        // clear error flags
        i2c->SR1 = 0;

        bool woken = false;
        sys_queue_send_from_ISR(_I2C[major]->event, &err, &woken);

        CLEAR_BIT(i2c->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN);
        sys_thread_yield_from_ISR(woken);
}

//==============================================================================
/**
 * @brief  DMA callback.
 * @param  SR           DMA transaction status (DMA_SR_*)
 * @param  arg          user's object
 * @return If IRQ should yield then true is returned, false otherwise.
 */
//==============================================================================
#if USE_DMA > 0
static bool DMA_callback(DMA_Stream_TypeDef *stream, u8_t SR, void *arg)
{
        UNUSED_ARG1(stream);

        I2C_mem_t *I2C = arg;

        bool yield = false;
        int  err   = (SR & DMA_SR_TCIF) ? ESUCC : EIO;
        sys_queue_send_from_ISR(I2C->event, &err, &yield);

        return yield;
}
#endif

//==============================================================================
/**
 * @brief  I2C1 Event IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_I2C1EN)
void I2C1_EV_IRQHandler(void)
{
        IRQ_EV_handler(_I2C1);
}
#endif

//==============================================================================
/**
 * @brief  I2C1 Error IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_I2C1EN)
void I2C1_ER_IRQHandler(void)
{
        IRQ_ER_handler(_I2C1);
}
#endif

//==============================================================================
/**
 * @brief  I2C2 Event IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_I2C2EN)
void I2C2_EV_IRQHandler(void)
{
        IRQ_EV_handler(_I2C2);
}
#endif

//==============================================================================
/**
 * @brief  I2C2 Error IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_I2C2EN)
void I2C2_ER_IRQHandler(void)
{
        IRQ_ER_handler(_I2C2);
}
#endif

//==============================================================================
/**
 * @brief  I2C3 Event IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_I2C3EN)
void I2C3_EV_IRQHandler(void)
{
        IRQ_EV_handler(_I2C3);
}
#endif

//==============================================================================
/**
 * @brief  I2C3 Error IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_I2C3EN)
void I2C3_ER_IRQHandler(void)
{
        IRQ_ER_handler(_I2C3);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
