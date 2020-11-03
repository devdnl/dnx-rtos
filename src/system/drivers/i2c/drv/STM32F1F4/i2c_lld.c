/*=========================================================================*//**
@file    i2c_lld.c

@author  Daniel Zorychta

@brief   This driver support I2C peripherals.

@note    Copyright (C) 2020  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "../STM32F1F4/i2c_cfg.h"
#include "../STM32F1F4/i2c_regs.h"
#include "drivers/driver.h"
#include "gpio_ddi.h"
#include "i2c_ioctl.h"
#include "i2c.h"

#if defined(ARCH_stm32f1)
#include "lib/stm32f10x_rcc.h"
#include "stm32f1/dma_ddi.h"
#elif defined(ARCH_stm32f4)
#include "lib/stm32f4xx_rcc.h"
#include "stm32f4/dma_ddi.h"
#endif

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define USE_DMA (((_I2C1_USE_DMA > 0) || (_I2C2_USE_DMA > 0) || (_I2C3_USE_DMA > 0)) && __ENABLE_DMA__)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/// type defines configuration of single I2C peripheral
typedef struct {
  #if USE_DMA > 0
        const bool              use_DMA;              //!< peripheral uses DMA and IRQ (true), or only IRQ (false)
        const u8_t              DMA_channel;          //!< DMA peripheral request channel number
        const u8_t              DMA_tx_stream_pri;    //!< primary Tx stream number
        const u8_t              DMA_rx_stream_pri;    //!< primary Rx stream number
        const u8_t              DMA_tx_stream_alt;    //!< alternative Tx stream number
        const u8_t              DMA_rx_stream_alt;    //!< alternative Rx stream number
  #endif
        const I2C_periph_t     *const I2C;            //!< pointer to the I2C peripheral
        const u32_t             freq;                 //!< peripheral SCL frequency [Hz]
        const u32_t             APB1ENR_clk_mask;     //!< mask used to enable I2C clock in the APB1ENR register
        const IRQn_Type         IRQ_EV_n;             //!< number of event IRQ vector
        const IRQn_Type         IRQ_ER_n;             //!< number of error IRQ vector
        const bool              duty_cycle_16_9;      //!< duty cycle
} I2C_info_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static inline I2C_periph_t *get_I2C(I2C_dev_t *hdl);
static void clear_address_event(I2C_dev_t *hdl);
static void IRQ_EV_handler(u8_t major);
static void IRQ_ER_handler(u8_t major);

#if USE_DMA > 0
#if defined(ARCH_stm32f1)
static bool DMA_callback(DMA_Channel_t *channel, u8_t SR, void *arg);
#elif defined(ARCH_stm32f4)
static bool DMA_callback(DMA_Stream_TypeDef *stream, u8_t SR, void *arg);
#endif
static int  wait_for_event_DMA(I2C_dev_t *hdl);
#endif

/*==============================================================================
  Local object definitions
==============================================================================*/
// peripherals configuration
static const I2C_info_t I2C_HW[_I2C_NUMBER_OF_PERIPHERALS] = {
#if defined(ARCH_stm32f1)
        #if defined(RCC_APB1ENR_I2C1EN)
        {
                #if USE_DMA > 0
                .use_DMA           = _I2C1_USE_DMA,
                .DMA_tx_stream_pri = 6,
                .DMA_tx_stream_alt = UINT8_MAX,
                .DMA_rx_stream_pri = 7,
                .DMA_rx_stream_alt = UINT8_MAX,
                #endif
                .I2C               = (I2C_periph_t*)I2C1,
                .freq              = _I2C1_FREQUENCY,
                .APB1ENR_clk_mask  = RCC_APB1ENR_I2C1EN,
                .IRQ_EV_n          = I2C1_EV_IRQn,
                .IRQ_ER_n          = I2C1_ER_IRQn,
                .duty_cycle_16_9   = _I2C1_DUTY_CYCLE
        },
        #endif
        #if defined(RCC_APB1ENR_I2C2EN)
        {
                #if USE_DMA > 0
                .use_DMA           = _I2C2_USE_DMA,
                .DMA_tx_stream_pri = 4,
                .DMA_tx_stream_alt = UINT8_MAX,
                .DMA_rx_stream_pri = 5,
                .DMA_rx_stream_alt = UINT8_MAX,
                #endif
                .I2C               = (I2C_periph_t*)I2C2,
                .freq              = _I2C2_FREQUENCY,
                .APB1ENR_clk_mask  = RCC_APB1ENR_I2C2EN,
                .IRQ_EV_n          = I2C2_EV_IRQn,
                .IRQ_ER_n          = I2C2_ER_IRQn,
                .duty_cycle_16_9   = _I2C2_DUTY_CYCLE
        },
        #endif
#elif defined(ARCH_stm32f4)
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
                .I2C               = (I2C_periph_t*)I2C1,
                .freq              = _I2C1_FREQUENCY,
                .APB1ENR_clk_mask  = RCC_APB1ENR_I2C1EN,
                .IRQ_EV_n          = I2C1_EV_IRQn,
                .IRQ_ER_n          = I2C1_ER_IRQn,
                .duty_cycle_16_9   = _I2C1_DUTY_CYCLE
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
                .I2C               = (I2C_periph_t*)I2C2,
                .freq              = _I2C2_FREQUENCY,
                .APB1ENR_clk_mask  = RCC_APB1ENR_I2C2EN,
                .IRQ_EV_n          = I2C2_EV_IRQn,
                .IRQ_ER_n          = I2C2_ER_IRQn,
                .duty_cycle_16_9   = _I2C2_DUTY_CYCLE
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
                .I2C               = (I2C_periph_t*)I2C3,
                .freq              = _I2C3_FREQUENCY,
                .APB1ENR_clk_mask  = RCC_APB1ENR_I2C3EN,
                .IRQ_EV_n          = I2C3_EV_IRQn,
                .IRQ_ER_n          = I2C3_ER_IRQn,
                .duty_cycle_16_9   = _I2C3_DUTY_CYCLE
        },
        #endif
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
static inline I2C_periph_t *get_I2C(I2C_dev_t *hdl)
{
        return const_cast(I2C_periph_t*, I2C_HW[hdl->major].I2C);
}

//==============================================================================
/**
 * @brief  Function handle error (try make the interface working)
 * @param  hdl          device handle
 */
//==============================================================================
static void reset(I2C_dev_t *hdl, bool reinit)
{
        printk("I2C%d:%d interface reset", hdl->major, hdl->minor);

        I2C_periph_t *i2c = get_I2C(hdl);

        CLEAR_BIT(i2c->CR2, PI2C_CR2_ITEVTEN | PI2C_CR2_ITERREN | PI2C_CR2_ITBUFEN);

        sys_critical_section_begin();
        {
                volatile u8_t tmp = i2c->SR1;
                              tmp = i2c->SR2;
                              tmp = i2c->DR;
                              tmp = i2c->DR;

                UNUSED_ARG1(tmp);

                i2c->SR1 = 0;

                CLEAR_BIT(i2c->CR1, PI2C_CR1_PE);
        }
        sys_critical_section_end();

        sys_sleep_ms(1);

        if (reinit) {
                u16_t OAR1 = i2c->OAR1;
                _I2C_LLD__init(hdl->major);
                i2c->OAR1 = OAR1;
        }

        _I2C_LLD__master_stop(hdl);


        I2C_recovery_t *recovery = &_I2C[hdl->major]->recovery;

        if (recovery->enable) {
                i8_t state = _GPIO_DDI_get_pin(recovery->SDA.port_idx,
                                               recovery->SDA.pin_idx);
                int SCL_mode = 0;
                int err = _GPIO_DDI_get_pin_mode(recovery->SCL.port_idx,
                                                 recovery->SCL.pin_idx,
                                                 &SCL_mode);

                if (err) {
                        printk("I2C%d:%d invalid SCL pin - recovery skipped",
                               hdl->major, hdl->minor);
                }

                if ((state == 0) && (err == 0)) {
                        printk("I2C%d:%d detected SDA low - bus recovery",
                               hdl->major, hdl->minor);

                        #if defined(ARCH_stm32f1)
                        int mode = GPIO_MODE__OD_10MHz;
                        #elif defined(ARCH_stm32f4)
                        int mode = GPIO_MODE__OD;
                        #endif
                        _GPIO_DDI_set_pin_mode(recovery->SCL.port_idx,
                                               recovery->SCL.pin_idx,
                                               mode);

                        for (int i = 0; i < 10; i++) {
                                _GPIO_DDI_clear_pin(recovery->SCL.port_idx,
                                                    recovery->SCL.pin_idx);
                                sys_sleep_ms(1);

                                _GPIO_DDI_set_pin(recovery->SCL.port_idx,
                                                  recovery->SCL.pin_idx);
                                sys_sleep_ms(1);
                        }

                        _GPIO_DDI_set_pin_mode(recovery->SCL.port_idx,
                                               recovery->SCL.pin_idx,
                                               SCL_mode);

                        state = _GPIO_DDI_get_pin(recovery->SDA.port_idx,
                                                  recovery->SDA.pin_idx);

                        printk("I2C%d:%d recovery %s", hdl->major, hdl->minor,
                               (state == 1) ? "success" : "fail");
                }
        }
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
static int wait_for_event_DMA(I2C_dev_t *hdl)
{
        I2C_periph_t *i2c = get_I2C(hdl);
        SET_BIT(i2c->CR2, PI2C_CR2_LAST | PI2C_CR2_DMAEN);

        int ferr = EIO;
        int err  = sys_queue_receive(_I2C[hdl->major]->event, &ferr, _I2C_DEVICE_TIMEOUT);

        CLEAR_BIT(i2c->CR2, PI2C_CR2_LAST | PI2C_CR2_DMAEN);

        if (!err) {
                err = ferr;
        }

        if (err) {
                reset(hdl, true);
        }

        return err;
}
#endif

//==============================================================================
/**
 * @brief  Function wait for I2C event (by using polling).
 * @param  hdl                  device handle
 * @param  SR1_event_mask       event mask (bits from SR1 register)
 * @param  timeout_ms           timeout in milliseconds
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int wait_for_event_poll(I2C_dev_t *hdl, u32_t flags, u32_t timeout_ms)
{
        u32_t BTT = 1000 / (I2C_HW[hdl->major].freq / 10); // 1000/f = T in ms
        u32_t CST = 1000 / __OS_TASK_SCHED_FREQ__;         // 1000/f = T in ms

        I2C_periph_t *i2c = get_I2C(hdl);
        u32_t tref = sys_time_get_reference();

        while (!(i2c->SR1 & flags)) {

                if (sys_time_is_expired(tref, timeout_ms)) {
                        printk("I2C%d:%d event poll timeout (SR1 mask: %Xh)",
                               hdl->major, hdl->minor, flags);
                        reset(hdl, true);
                        return EIO;
                }

                if (i2c->SR1 & (PI2C_SR1_ARLO | PI2C_SR1_BERR | PI2C_SR1_AF)) {
                        printk("I2C%d:%d event poll error", hdl->major, hdl->minor);
                        reset(hdl, true);
                        return EIO;
                }

                if (BTT >= CST) {
                        sys_thread_yield();
                }
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Clear event of send address
 * @param  hdl                  device handle
 */
//==============================================================================
static void clear_address_event(I2C_dev_t *hdl)
{
        I2C_periph_t *i2c = get_I2C(hdl);

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
        I2C_periph_t            *i2c = const_cast(I2C_periph_t*, I2C_HW[major].I2C);

        CLEAR_BIT(RCC->APB1ENR, cfg->APB1ENR_clk_mask);
        SET_BIT(RCC->APB1ENR, cfg->APB1ENR_clk_mask);

        SET_BIT(RCC->APB1RSTR, cfg->APB1ENR_clk_mask);
        sys_sleep_ms(10);
        CLEAR_BIT(RCC->APB1RSTR, cfg->APB1ENR_clk_mask);

        LL_RCC_ClocksTypeDef clocks;
        memset(&clocks, 0, sizeof(LL_RCC_ClocksTypeDef));
        LL_RCC_GetSystemClocksFreq(&clocks);

        if (clocks.PCLK1_Frequency < 2000000) {
                printk("I2C: PCLK1 below recommended 2MHz");
                return EIO;
        }

        NVIC_EnableIRQ(cfg->IRQ_EV_n);
        NVIC_EnableIRQ(cfg->IRQ_ER_n);
        NVIC_SetPriority(cfg->IRQ_EV_n, _CPU_IRQ_SAFE_PRIORITY_);
        NVIC_SetPriority(cfg->IRQ_ER_n, _CPU_IRQ_SAFE_PRIORITY_);

        u16_t CCR;
        if (cfg->freq <= 100000) {
                CCR = max((clocks.PCLK1_Frequency / (2 * cfg->freq)), 0x04);
                i2c->TRISE = (clocks.PCLK1_Frequency / 1000000) + 1;

        } else {
                if (I2C_HW[major].duty_cycle_16_9) {
                        CCR  = (clocks.PCLK1_Frequency / (cfg->freq * 3));
                } else {
                        CCR  = (clocks.PCLK1_Frequency / (cfg->freq * 25));
                        CCR |= PI2C_CCR_DUTY;
                }

                if ((CCR & PI2C_CCR_CCR) == 0) {
                        CCR |= 1;
                }

                CCR |= PI2C_CCR_FS;

                i2c->TRISE = (((clocks.PCLK1_Frequency / 1000000) * 300) / 1000) + 1;
        }

        i2c->CR1 = PI2C_CR1_SWRST;
        i2c->CR1 = 0;
        i2c->CR2 = (clocks.PCLK1_Frequency / 1000000) & PI2C_CR2_FREQ;
        i2c->CCR = CCR;
        i2c->CR1 = PI2C_CR1_PE;

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
        I2C_periph_t     *i2c = const_cast(I2C_periph_t*, I2C_HW[major].I2C);

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
int _I2C_LLD__master_start(I2C_dev_t *hdl)
{
        I2C_periph_t *i2c = get_I2C(hdl);

        u8_t tmp = i2c->DR;
             tmp = i2c->DR;
        (void)tmp;

        WRITE_REG(i2c->SR1, 0);

        CLEAR_BIT(i2c->CR1, PI2C_CR1_STOP);
        SET_BIT(i2c->CR1, PI2C_CR1_START | PI2C_CR1_ACK);

        return wait_for_event_poll(hdl, PI2C_SR1_SB, _I2C_TIMEOUT_START);
}

//==============================================================================
/**
 * @brief  Function generate REPEAT START sequence on I2C bus
 * @param  hdl                  device handle
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__master_repeat_start(I2C_dev_t *hdl)
{
        I2C_periph_t *i2c = get_I2C(hdl);

        clear_address_event(hdl);

        CLEAR_BIT(i2c->CR1, PI2C_CR1_STOP);
        SET_BIT(i2c->CR1, PI2C_CR1_START | PI2C_CR1_ACK);

        return wait_for_event_poll(hdl, PI2C_SR1_SB, _I2C_TIMEOUT_START);
}

//==============================================================================
/**
 * @brief  Function generate STOP sequence on I2C bus
 * @param  hdl                  device handle
 */
//==============================================================================
void _I2C_LLD__master_stop(I2C_dev_t *hdl)
{
        I2C_periph_t *i2c = get_I2C(hdl);

        SET_BIT(i2c->CR1, PI2C_CR1_STOP);

        int loop = 0;
        while ((i2c->CR1 & PI2C_CR1_STOP) && (++loop < 2000));

        WRITE_REG(i2c->SR1, 0);
}

//==============================================================================
/**
 * @brief  Function send I2C address sequence
 * @param  hdl                  device handle
 * @param  write                true: compose write address
 * @param  count                number of bytes to be written/read (in transfer).
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__master_send_address(I2C_dev_t *hdl, bool write, size_t count)
{
        I2C_periph_t *i2c = get_I2C(hdl);

        if (hdl->config.addr_10bit) {

                int err = EIO;

                // send header + 2 most significant bits of 10-bit address
                sys_critical_section_begin();
                {
                        u16_t tmp = i2c->SR1;
                        i2c->DR   = _I2C_HEADER_ADDR_10BIT | ((hdl->config.address & 0xFE) >> 7);
                        (void)tmp;
                }
                sys_critical_section_end();

                err = wait_for_event_poll(hdl, PI2C_SR1_ADD10, _I2C_TIMEOUT_BYTE_TRANSFER);
                if (err) goto finish;

                // send rest of 8 bits of 10-bit address
                sys_critical_section_begin();
                {
                        u16_t tmp = i2c->SR1;
                        i2c->DR   = hdl->config.address;
                        (void)tmp;
                }
                sys_critical_section_end();

                err = wait_for_event_poll(hdl, PI2C_SR1_ADDR, _I2C_TIMEOUT_BYTE_TRANSFER);
                if (err) goto finish;

                // send repeat start
                err = _I2C_LLD__master_repeat_start(hdl);
                if (err) goto finish;

                // send header
                sys_critical_section_begin();
                {
                        if (!write && (count == 2)) {
                                SET_BIT(i2c->CR1, PI2C_CR1_POS | PI2C_CR1_ACK);
                        }

                        u8_t hdr  = _I2C_HEADER_ADDR_10BIT | ((hdl->config.address >> 7) & 0x6);
                        u16_t tmp = i2c->SR1;
                        i2c->DR   = write ? hdr : hdr | 0x01;
                        (void)tmp;
                }
                sys_critical_section_end();

                err = wait_for_event_poll(hdl, PI2C_SR1_ADDR, _I2C_TIMEOUT_BYTE_TRANSFER);

                finish:
                if (err) {
                        printk("I2C%d:%d send 10-bit address error", hdl->major, hdl->minor);
                }

                return err;

        } else {
                sys_critical_section_begin();
                {
                        if (!write && (count == 2)) {
                                SET_BIT(i2c->CR1, PI2C_CR1_POS | PI2C_CR1_ACK);
                        }

                        u16_t tmp = i2c->SR1;
                        i2c->DR   = write ? (hdl->config.address & 0xFE) : (hdl->config.address | 0x01);
                        (void)tmp;
                }
                sys_critical_section_end();

                int err =  wait_for_event_poll(hdl, PI2C_SR1_ADDR, _I2C_TIMEOUT_BYTE_TRANSFER);
                if (err) {
                        printk("I2C%d:%d send 7-bit address error", hdl->major, hdl->minor);
                }

                return err;
        }
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
int _I2C_LLD__master_receive(I2C_dev_t *hdl, u8_t *dst, size_t count, size_t *rdcnt)
{
        int      err = EIO;
        ssize_t  n   = 0;
        I2C_periph_t   *i2c = get_I2C(hdl);

        if (count >= 3) {
#if USE_DMA > 0
                if (I2C_HW[hdl->major].use_DMA) {
                        u32_t dmad = _DMA_DDI_reserve(0, I2C_HW[hdl->major].DMA_rx_stream_pri);
                        if (dmad == 0) {
                                dmad = _DMA_DDI_reserve(0, I2C_HW[hdl->major].DMA_rx_stream_alt);
                        }

                        if (dmad) {
                                CLEAR_BIT(i2c->CR2, PI2C_CR2_ITBUFEN | PI2C_CR2_ITERREN | PI2C_CR2_ITEVTEN);

                                _DMA_DDI_config_t config;
                                config.arg      = _I2C[hdl->major];
                                config.callback = DMA_callback;
                                config.cb_next  = NULL;
                                config.release  = false;
                                config.PA       = cast(u32_t, &i2c->DR);
                                config.NDT      = count;
                                #if defined(ARCH_stm32f1)
                                config.MA       = cast(u32_t, dst);
                                config.CR       = DMA_CCR1_MINC;
                                #elif defined(ARCH_stm32f4)
                                config.MA[0]    = cast(u32_t, dst);
                                config.FC       = DMA_SxFCR_FTH_0 | DMA_SxFCR_FS_2;
                                config.CR       = ((I2C_HW[hdl->major].DMA_channel & 7) << DMA_SxCR_CHSEL_Pos)
                                                | DMA_SxCR_MINC;
                                #endif
                                config.IRQ_priority = __CPU_DEFAULT_IRQ_PRIORITY__;

                                err = _DMA_DDI_transfer(dmad, &config);
                                if (!err) {
                                        SET_BIT(i2c->CR2, PI2C_CR2_LAST | PI2C_CR2_DMAEN);
                                        clear_address_event(hdl);

                                        err = wait_for_event_DMA(hdl);
                                        if (!err) {
                                                n = count;
                                        } else {
                                                printk("I2C: DMA Rx event error");
                                        }
                                } else {
                                        printk("I2C: DMA receive error");
                                }

                                _DMA_DDI_release(dmad);
                                goto finish;
                        } else {
                                printk("I2C: DMA read channel in use, using polling mode");
                        }
                }
#endif
                {
                        clear_address_event(hdl);

                        while (count) {
                                if (count == 3) {
                                        err = wait_for_event_poll(hdl, PI2C_SR1_RXNE, _I2C_TIMEOUT_BYTE_TRANSFER);
                                        if (err) break;

                                        err = wait_for_event_poll(hdl, PI2C_SR1_BTF, _I2C_TIMEOUT_BYTE_TRANSFER);
                                        if (err) break;

                                        sys_critical_section_begin();
                                        {
                                                CLEAR_BIT(i2c->CR1, PI2C_CR1_ACK);
                                                *dst++ = i2c->DR;
                                                n++;

                                                SET_BIT(i2c->CR1, PI2C_CR1_STOP);

                                                *dst++ = i2c->DR;
                                                n++;
                                        }
                                        sys_critical_section_end();

                                        err = wait_for_event_poll(hdl, PI2C_SR1_RXNE, _I2C_TIMEOUT_BYTE_TRANSFER);
                                        if (err) break;

                                        *dst++ = i2c->DR;
                                        n++;

                                        count = 0;
                                } else {
                                        err = wait_for_event_poll(hdl, PI2C_SR1_RXNE, _I2C_TIMEOUT_BYTE_TRANSFER);
                                        if (!err) {
                                                sys_critical_section_begin();
                                                {
                                                        int cnt = 0;

                                                        while ((i2c->SR1 & PI2C_SR1_RXNE) && (count > 3) && (cnt < 2)) {
                                                                *dst++ = i2c->DR;
                                                                count--;
                                                                n++;
                                                                cnt++;
                                                        }
                                                }
                                                sys_critical_section_end();
                                        } else {
                                                break;
                                        }
                                }
                        }
                }

        } else if (count == 2) {
                sys_critical_section_begin();
                {
                        clear_address_event(hdl);
                        CLEAR_BIT(i2c->CR1, PI2C_CR1_ACK);
                }
                sys_critical_section_end();

                err = wait_for_event_poll(hdl, PI2C_SR1_BTF, _I2C_TIMEOUT_BYTE_TRANSFER);
                if (!err) {
                        sys_critical_section_begin();
                        {
                                CLEAR_BIT(i2c->CR1, PI2C_CR1_POS);
                                SET_BIT(i2c->CR1, PI2C_CR1_STOP);
                        }
                        sys_critical_section_end();

                        *dst++ = i2c->DR;
                        *dst++ = i2c->DR;
                        n     += 2;
                        count  = 0;
                }

        } else if (count == 1) {
                sys_critical_section_begin();
                {
                        CLEAR_BIT(i2c->CR1, PI2C_CR1_ACK);
                        clear_address_event(hdl);
                        SET_BIT(i2c->CR1, PI2C_CR1_STOP);
                }
                sys_critical_section_end();

                err = wait_for_event_poll(hdl, PI2C_SR1_RXNE, _I2C_TIMEOUT_BYTE_TRANSFER);
                if (!err) {
                        *dst++ = i2c->DR;
                        count  = 0;
                        n++;
                }
        }

#if USE_DMA > 0
        finish:
#endif

        *rdcnt = n;

        if (err) {
                printk("I2C%d:%d receive error", hdl->major, hdl->minor);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function transmit selected amount bytes to I2C bus
 * @param  hdl                  device handle
 * @param  src                  data source
 * @param  count                number of bytes to transfer
 * @param  wrcnt                number of written bytes
 * @param  subaddr              subaddress transfer
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__master_transmit(I2C_dev_t *hdl, const u8_t *src, size_t count, size_t *wrcnt, bool subaddr)
{
        UNUSED_ARG1(subaddr);

        int     err  = EIO;
        ssize_t n    = 0;
        I2C_periph_t  *i2c  = get_I2C(hdl);

        clear_address_event(hdl);

#if USE_DMA > 0
        if (count >= 3 && I2C_HW[hdl->major].use_DMA) {
                u32_t dmad = _DMA_DDI_reserve(0, I2C_HW[hdl->major].DMA_tx_stream_pri);
                if (dmad == 0) {
                        dmad = _DMA_DDI_reserve(0, I2C_HW[hdl->major].DMA_tx_stream_alt);
                }

                if (dmad) {
                        CLEAR_BIT(i2c->CR2, PI2C_CR2_ITBUFEN | PI2C_CR2_ITERREN | PI2C_CR2_ITEVTEN);

                        _DMA_DDI_config_t config;
                        config.arg      = _I2C[hdl->major];
                        config.callback = DMA_callback;
                        config.cb_next  = NULL;
                        config.release  = false;
                        config.PA       = cast(u32_t, &i2c->DR);
                        config.NDT      = count;

#if defined(ARCH_stm32f1)
                        config.MA       = cast(u32_t, src);
                        config.CR       = DMA_CCR1_MINC | DMA_CCR1_DIR;
#elif defined(ARCH_stm32f4)
                        config.MA[0]    = cast(u32_t, src);
                        config.FC       = DMA_SxFCR_FTH_0 | DMA_SxFCR_FS_2;
                        config.CR       = ((I2C_HW[hdl->major].DMA_channel & 7) << DMA_SxCR_CHSEL_Pos)
                                        | DMA_SxCR_MINC | DMA_SxCR_DIR_0;
#endif
                        config.IRQ_priority = __CPU_DEFAULT_IRQ_PRIORITY__;

                        err = _DMA_DDI_transfer(dmad, &config);
                        if (!err) {
                                err = wait_for_event_DMA(hdl);
                                if (!err) {

                                        /*
                                         *  Master transmitter: In the interrupt routine after the EOT
                                         *  interrupt, disable DMA requests then wait for a BTF event
                                         *  before programming the Stop condition.
                                         */
                                        err = wait_for_event_poll(hdl, PI2C_SR1_BTF,
                                                                  _I2C_TIMEOUT_BYTE_TRANSFER);
                                        if (!err) {
                                                n = count;
                                        } else {
                                                printk("I2C: write not finished correctly");
                                        }
                                } else {
                                        printk("I2C: DMA Tx event error");
                                }
                        } else {
                                printk("I2C: DMA write error");
                        }

                        _DMA_DDI_release(dmad);
                        goto finish;

                } else {
                        printk("I2C: DMA write channel in use, using polling mode");
                }
        }
#endif
        {
                while (count) {
                        err = wait_for_event_poll(hdl, PI2C_SR1_TXE, _I2C_TIMEOUT_BYTE_TRANSFER);
                        if (!err) {
                                i2c->DR = *src++;
                                n++;
                                count--;
                        } else {
                                break;
                        }
                }
        }

        if (n && !err) {
                err = wait_for_event_poll(hdl, PI2C_SR1_TXE, _I2C_TIMEOUT_BYTE_TRANSFER);
                if (!err) {
                        err = wait_for_event_poll(hdl, PI2C_SR1_BTF, _I2C_TIMEOUT_BYTE_TRANSFER);
                }
        }

#if USE_DMA > 0
        finish:
#endif

        *wrcnt = n;

        if (err) {
                printk("I2C%d:%d transmit error", hdl->major, hdl->minor);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function enable peripheral in slave mode.
 * @param  hdl                  device handle
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__slave_mode_setup(I2C_dev_t *hdl)
{
        I2C_periph_t *i2c = get_I2C(hdl);

        if (hdl->config.mode == I2C_MODE__SLAVE) {

                i2c->OAR1 = (1 << 14) // not used but documentation say to set this bit...
                          | (hdl->config.address & 0x3FF)
                          | (hdl->config.addr_10bit ? PI2C_OAR1_ADDMODE : 0);

                i2c->OAR2 = 0;

        } else {

                CLEAR_BIT(i2c->CR1, PI2C_CR1_ACK);

                i2c->OAR1 = 0;
                i2c->OAR2 = 0;
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function wait for I2C start/address condition event.
 * @param  hdl                  device handle
 * @param  event                event type and timeout
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__slave_wait_for_selection(I2C_dev_t *hdl, I2C_selection_t *event)
{
        I2C_periph_t *i2c = get_I2C(hdl);

        sys_critical_section_begin();
        {
                _I2C[hdl->major]->SR1_mask = PI2C_SR1_ADDR;

                SET_BIT(i2c->CR2, PI2C_CR2_ITEVTEN | PI2C_CR2_ITERREN);
                CLEAR_BIT(i2c->CR1, PI2C_CR1_STOP);
                SET_BIT(i2c->CR1, PI2C_CR1_ACK);
        }
        sys_critical_section_end();

        int ferr = EIO;
        int err  = sys_queue_receive(_I2C[hdl->major]->event, &ferr, event->timeout_ms);
        if (!err) {
                err = ferr;
        }

        if (err) {
                reset(hdl, true);
        } else {
                event->RD_addr = (i2c->SR2 & PI2C_SR2_TRA);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function transmit bytes to master device.
 * @param  hdl                  device handle
 * @param  src                  source buffer
 * @param  count                number of bytes to transfer
 * @param  wrctr                write counter
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__slave_transmit(I2C_dev_t *hdl, const u8_t *src, size_t count, size_t *wrctr)
{
        I2C_periph_t *i2c = get_I2C(hdl);
        int    err = ESUCC;

        *wrctr = 0;

        u32_t tref = sys_time_get_reference();

        clear_address_event(hdl);

        while (true) {
                if ((i2c->SR1 & PI2C_SR1_TXE)) {
                        if (count > 0) {
                                i2c->DR = *src++;
                                count--;
                                (*wrctr)++;
                        } else {
                                i2c->DR = 0;
                        }

                } else if (sys_time_is_expired(tref, _I2C_DEVICE_TIMEOUT)) {
                        printk("I2C%d: slave transmit timeout", hdl->major);
                        _I2C_LLD__master_stop(hdl);
                        err = ETIME;
                        break;

                } else {
                        if (i2c->SR1 & PI2C_SR1_AF) {
                                /*
                                 * TXE flag is set always when shift buffer is
                                 * empty. In this case CPU write next byte but
                                 * there is possibility that there is a stop
                                 * condition so written byte is not send to
                                 * master device. In this case the number of
                                 * sent bytes should be corrected by 1 byte.
                                 */
                                if (!(i2c->SR1 & PI2C_SR1_TXE)) {
                                        if (*wrctr > 1) {
                                                (*wrctr)--;
                                        }
                                }

                                break;
                        }
                }
        }

        // clear AF flag
        i2c->SR1 = 0;

        return err;
}

//==============================================================================
/**
 * @brief  Function receive bytes from master device.
 * @param  hdl                  device handle
 * @param  dst                  destination buffer
 * @param  count                number of bytes to transfer
 * @param  rdctr                read counter
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__slave_receive(I2C_dev_t *hdl, u8_t *dst, size_t count, size_t *rdctr)
{
        I2C_periph_t *i2c = get_I2C(hdl);
        int    err = ESUCC;

        *rdctr = 0;

        u32_t tref = sys_time_get_reference();

        while (true) {
                if (i2c->SR1 & PI2C_SR1_RXNE) {
                        if (count > 0) {
                                *dst++ = i2c->DR;
                                count--;
                                (*rdctr)++;
                        } else {
                                u8_t tmp = i2c->DR;
                                (void)tmp;
                        }

                } else if (sys_time_is_expired(tref, _I2C_DEVICE_TIMEOUT)) {
                        printk("I2C%d: slave receive timeout", hdl->major);
                        _I2C_LLD__master_stop(hdl);
                        err = ETIME;
                        break;

                } else {
                        if (i2c->SR1 & (PI2C_SR1_ADDR | PI2C_SR1_STOPF)) {
                                if (i2c->SR1 & PI2C_SR1_RXNE) {
                                       if (count > 0) {
                                               *dst++ = i2c->DR;
                                               count--;
                                               (*rdctr)++;
                                       } else {
                                               u8_t tmp = i2c->DR;
                                               (void)tmp;
                                       }

                                }
                                break;
                        }
                }
                sys_sleep_ms(1);
        }

        // clear STOPF flag
        sys_critical_section_begin();
        {
                u8_t tmp = i2c->SR1;
                (void)tmp;
                SET_BIT(i2c->CR1, PI2C_CR1_PE);
        }
        sys_critical_section_end();

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

        I2C_periph_t *i2c = const_cast(I2C_periph_t*, I2C_HW[major].I2C);
        u16_t  SR1 = i2c->SR1;

        if (SR1 & _I2C[major]->SR1_mask) {
                int err = ESUCC;
                sys_queue_send_from_ISR(_I2C[major]->event, &err, &woken);
                CLEAR_BIT(i2c->CR2, PI2C_CR2_ITEVTEN | PI2C_CR2_ITERREN | PI2C_CR2_ITBUFEN);
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
                        CLEAR_BIT(I2C1->CR2, PI2C_CR2_ITEVTEN | PI2C_CR2_ITERREN | PI2C_CR2_ITBUFEN);
                        i2c->SR1 = 0;
                        NVIC_DisableIRQ(I2C_HW[major].IRQ_ER_n);
                        NVIC_DisableIRQ(I2C_HW[major].IRQ_EV_n);
                        NVIC_ClearPendingIRQ(I2C_HW[major].IRQ_ER_n);
                        NVIC_ClearPendingIRQ(I2C_HW[major].IRQ_EV_n);
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
        I2C_periph_t *i2c = const_cast(I2C_periph_t*, I2C_HW[major].I2C);
        u16_t  SR1 = i2c->SR1;
        u16_t  SR2 = i2c->SR2;
        UNUSED_ARG1(SR2);

        int err = ESUCC;

        if (SR1 & PI2C_SR1_ARLO) {
                err = EAGAIN;

        } else if (SR1 & PI2C_SR1_AF) {
                if (_I2C[major]->SR1_mask & (PI2C_SR1_ADDR | PI2C_SR1_ADD10))
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

        CLEAR_BIT(i2c->CR2, PI2C_CR2_ITEVTEN | PI2C_CR2_ITERREN | PI2C_CR2_ITBUFEN);
        sys_thread_yield_from_ISR(woken);
}

//==============================================================================
/**
 * @brief  DMA callback.
 * @param  stream       DMA stream
 * @param  SR           DMA transaction status (DMA_SR_*)
 * @param  arg          user's object
 * @return If IRQ should yield then true is returned, false otherwise.
 */
//==============================================================================
#if USE_DMA > 0
#if defined(ARCH_stm32f1)
static bool DMA_callback(DMA_Channel_t *channel, u8_t SR, void *arg)
#elif defined(ARCH_stm32f4)
static bool DMA_callback(DMA_Stream_TypeDef *stream, u8_t SR, void *arg)
#endif
{
        I2C_mem_t    *I2C_mem = arg;
        I2C_periph_t *i2c     = const_cast(I2C_periph_t*, I2C_HW[I2C_mem->major].I2C);

        int err = (SR & DMA_SR_TEIF) ? EIO : ESUCC;

        if (!err) {
                err = (i2c->SR1 & (PI2C_SR1_ARLO | PI2C_SR1_BERR | PI2C_SR1_AF))
                    ? EIO : ESUCC;
        }

        if (!err) {
                err = (SR & DMA_SR_TCIF) ? ESUCC : EIO;
        }

        if (!err) {
                #if defined(ARCH_stm32f1)
                u32_t NDTR = channel->CNDTR;
                #elif defined(ARCH_stm32f4)
                u32_t NDTR = stream->NDTR;
                #endif
                err = (NDTR > 0) ? EIO : ESUCC;
        }

        bool yield = false;
        sys_queue_send_from_ISR(I2C_mem->event, &err, &yield);

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
