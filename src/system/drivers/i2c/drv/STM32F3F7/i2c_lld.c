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

/*==============================================================================
  Include files
==============================================================================*/
#include "i2c_cfg.h"
#include "drivers/driver.h"
#include "gpio_ddi.h"
#include "i2c.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define I2C_TRANSFER_TIMEOUT            2000
#define MAX_TRANSFER_SIZE               255
#define NSEC_PER_SEC                    1000000000ULL
#define PRESC_MAX                       16
#define SCLDEL_MAX                      16
#define SDADEL_MAX                      16
#define SCLH_MAX                        256
#define SCLL_MAX                        256

/*
 * Divide positive or negative dividend by positive or negative divisor
 * and round to closest integer. Result is undefined for negative
 * divisors if the dividend variable type is unsigned and for negative
 * dividends if the divisor variable type is unsigned.
 */
#define DIV_ROUND_CLOSEST(x, divisor)(                  \
{                                                       \
        __typeof__(x) __x = x;                          \
        __typeof__(divisor) __d = divisor;              \
        (((__typeof__(x))-1) > 0 ||                     \
         ((__typeof__(divisor))-1) > 0 ||               \
         (((__x) > 0) == ((__d) > 0))) ?                \
                (((__x) + ((__d) / 2)) / (__d)) :       \
                (((__x) - ((__d) / 2)) / (__d));        \
}                                                       \
)                                                       \


/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/// type defines configuration of single I2C peripheral
typedef struct {
        const I2C_TypeDef      *const I2C;            //!< pointer to the I2C peripheral
        const u32_t             scl_freq;             //!< peripheral SCL frequency [Hz]
        const u32_t             filter;               //!< filters mask
        const u16_t             scl_rise_time_ns;     //!< SCL rise time
        const u16_t             scl_fall_time_ns;     //!< SCL fall time
        const u16_t             data_hold_time_ns;    //!< Data hold time, (tHD:DAT), delay SCL low to data change
        const u16_t             data_setup_time_ns;   //!< Data setup time, (tSU:DAT), delay data setup to SCL high
        const u32_t             APB1ENR_I2CEN;        //!< mask used to enable I2C clock in the APB1ENR register
        const u32_t             APB1RSTR_I2CRST;      //!< mask used to reset I2C in the APB1RSTR register
        const IRQn_Type         IRQ_EV_n;             //!< number of event IRQ vector
        const IRQn_Type         IRQ_ER_n;             //!< number of error IRQ vector
        const u32_t             IRQ_priority;         //!< IRQ priority
        const u32_t             clk_src;              //!< I2C Clock source
} I2C_info_t;

/**
 * I2C timing results fields
 */
typedef struct {
        u32_t PRESC;
        u32_t SCLDEL;
        u32_t SDADEL;
        u32_t SCLH;
        u32_t SCLL;
} i2c_timing_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static inline I2C_TypeDef *get_I2C(I2C_dev_t *hdl);
static void IRQ_EV_handler(u8_t major);
static void IRQ_ER_handler(u8_t major);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(I2C);

// peripherals configuration
static const I2C_info_t I2C_HW[_I2C_NUMBER_OF_PERIPHERALS] = {
        #if defined(RCC_APB1ENR_I2C1EN)
        {
                .I2C               = (I2C_TypeDef*)I2C1,
                .scl_freq          = __I2C1_SCL_FREQ__,
                .filter            = __I2C1_ANALOG_FILTER_DISABLE__ | ((__I2C1_DIGITAL_FILTER__ << I2C_CR1_DNF_Pos) & I2C_CR1_DNF),
                .scl_rise_time_ns  = __I2C1_SCL_RISE_TIME_ns__,
                .scl_fall_time_ns  = __I2C1_SCL_FALL_TIME_ns__,
                .data_hold_time_ns = __I2C1_DATA_HOLD_TIME_ns__,
                .data_setup_time_ns= __I2C1_DATA_SETUP_TIME_ns__,
                .APB1ENR_I2CEN     = RCC_APB1ENR_I2C1EN,
                .APB1RSTR_I2CRST   = RCC_APB1RSTR_I2C1RST,
                .IRQ_EV_n          = I2C1_EV_IRQn,
                .IRQ_ER_n          = I2C1_ER_IRQn,
                .IRQ_priority      = __I2C1_IRQ_PRIORITY__,
                .clk_src           = LL_RCC_I2C1_CLKSOURCE
        },
        #endif
        #if defined(RCC_APB1ENR_I2C2EN)
        {
                .I2C               = (I2C_TypeDef*)I2C2,
                .scl_freq          = __I2C2_SCL_FREQ__,
                .filter            = __I2C2_ANALOG_FILTER_DISABLE__ | ((__I2C2_DIGITAL_FILTER__ << I2C_CR1_DNF_Pos) & I2C_CR1_DNF),
                .scl_rise_time_ns  = __I2C2_SCL_RISE_TIME_ns__,
                .scl_fall_time_ns  = __I2C2_SCL_FALL_TIME_ns__,
                .data_hold_time_ns = __I2C2_DATA_HOLD_TIME_ns__,
                .data_setup_time_ns= __I2C2_DATA_SETUP_TIME_ns__,
                .APB1ENR_I2CEN     = RCC_APB1ENR_I2C2EN,
                .APB1RSTR_I2CRST   = RCC_APB1RSTR_I2C2RST,
                .IRQ_EV_n          = I2C2_EV_IRQn,
                .IRQ_ER_n          = I2C2_ER_IRQn,
                .IRQ_priority      = __I2C2_IRQ_PRIORITY__,
                .clk_src           = LL_RCC_I2C2_CLKSOURCE
        },
        #endif
        #if defined(RCC_APB1ENR_I2C3EN)
        {
                .I2C               = (I2C_TypeDef*)I2C3,
                .scl_freq          = __I2C3_SCL_FREQ__,
                .filter            = __I2C3_ANALOG_FILTER_DISABLE__ | ((__I2C3_DIGITAL_FILTER__ << I2C_CR1_DNF_Pos) & I2C_CR1_DNF),
                .scl_rise_time_ns  = __I2C3_SCL_RISE_TIME_ns__,
                .scl_fall_time_ns  = __I2C3_SCL_FALL_TIME_ns__,
                .data_hold_time_ns = __I2C3_DATA_HOLD_TIME_ns__,
                .data_setup_time_ns= __I2C3_DATA_SETUP_TIME_ns__,
                .APB1ENR_I2CEN     = RCC_APB1ENR_I2C3EN,
                .APB1RSTR_I2CRST   = RCC_APB1RSTR_I2C3RST,
                .IRQ_EV_n          = I2C3_EV_IRQn,
                .IRQ_ER_n          = I2C3_ER_IRQn,
                .IRQ_priority      = __I2C3_IRQ_PRIORITY__,
                .clk_src           = LL_RCC_I2C3_CLKSOURCE
        },
        #endif
        #if defined(RCC_APB1ENR_I2C4EN)
        {
                .I2C               = (I2C_TypeDef*)I2C4,
                .scl_freq          = __I2C4_SCL_FREQ__,
                .filter            = __I2C4_ANALOG_FILTER_DISABLE__ | ((__I2C4_DIGITAL_FILTER__ << I2C_CR1_DNF_Pos) & I2C_CR1_DNF),
                .scl_rise_time_ns  = __I2C4_SCL_RISE_TIME_ns__,
                .scl_fall_time_ns  = __I2C4_SCL_FALL_TIME_ns__,
                .data_hold_time_ns = __I2C4_DATA_HOLD_TIME_ns__,
                .data_setup_time_ns= __I2C4_DATA_SETUP_TIME_ns__,
                .APB1ENR_I2CEN     = RCC_APB1ENR_I2C4EN,
                .APB1RSTR_I2CRST   = RCC_APB1RSTR_I2C4RST,
                .IRQ_EV_n          = I2C4_EV_IRQn,
                .IRQ_ER_n          = I2C4_ER_IRQn,
                .IRQ_priority      = __I2C4_IRQ_PRIORITY__,
                .clk_src           = LL_RCC_I2C4_CLKSOURCE
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
        return const_cast(I2C_HW[hdl->major].I2C);
}

//==============================================================================
/**
 * @brief  Function handle error (try make the interface working)
 * @param  hdl          device handle
 */
//==============================================================================
static void reset(I2C_dev_t *hdl, bool reinit)
{
        dev_dbg(hdl, "interface reset");

        I2C_TypeDef *i2c = get_I2C(hdl);

        CLEAR_BIT(i2c->CR1, I2C_CR1_PE);

        if (reinit) {
                u16_t OAR1 = i2c->OAR1;
                _I2C_LLD__init(hdl->major);
                i2c->OAR1 = OAR1;
        }

        sys_sleep_ms(10);

        I2C_recovery_t *recovery = &_I2C[hdl->major]->recovery;

        if (recovery->enable && (hdl->config.mode == I2C_MODE__MASTER)) {

                i8_t state = _GPIO_DDI_get_pin(recovery->SDA.port_idx,
                                               recovery->SDA.pin_idx);
                int SCL_mode = 0;
                int err = _GPIO_DDI_get_pin_mode(recovery->SCL.port_idx,
                                                 recovery->SCL.pin_idx,
                                                 &SCL_mode);

                if (err) {
                        dev_dbg(hdl, "invalid SCL pin - recovery skipped");
                }

                if ((state == 0) && (err == 0)) {
                        dev_dbg(hdl, "detected SDA low - bus recovery");

                        #if defined(ARCH_stm32f3) || defined(ARCH_stm32f7)
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

                        dev_dbg(hdl, "recovery %s", (state == 1) ? "success" : "fail");
                }
        }
}

//==============================================================================
/**
 * @brief  Function compute I2C timing.
 * @param  major        peripheral number
 * @param  timing       timing output
 */
//==============================================================================
static void compute_timing(u8_t major, i2c_timing_t *timing)
{
        u32_t fi2cclk = LL_RCC_GetI2CClockFreq(I2C_HW[major].clk_src);
        u32_t ti2cclk = DIV_ROUND_CLOSEST(NSEC_PER_SEC, fi2cclk);

        /*
         * Default configuration
         */
        timing->PRESC  = PRESC_MAX;
        timing->SCLL   = SCLL_MAX;
        timing->SCLH   = SCLH_MAX;
        timing->SCLDEL = 0;
        timing->SDADEL = 0;

        /*
         * Try calculate SCL timing value
         */
        bool found = false;
        for (u32_t presc = 1; (presc <= PRESC_MAX) && !found; presc++) {

                i32_t div = fi2cclk / presc / I2C_HW[major].scl_freq;

                if (div < 512) {

                        i32_t sync1_ticks = I2C_HW[major].scl_fall_time_ns / ti2cclk;
                        i32_t sync2_ticks = I2C_HW[major].scl_rise_time_ns / ti2cclk;

                        i32_t tsdadel = I2C_HW[major].data_hold_time_ns / ti2cclk;
                        i32_t tscldel = I2C_HW[major].data_setup_time_ns / ti2cclk;

                        timing->SCLDEL = min(15, tsdadel);
                        timing->SDADEL = min(15, tscldel);
                        timing->PRESC  = presc - 1;

                        if ((div / 2) - sync1_ticks - 1 >= 0) {
                                timing->SCLL = (div / 2) - 1 - sync1_ticks;
                        } else {
                                timing->SCLL = 0;
                        }

                        if ((div / 2) - 1 - sync2_ticks) {
                                timing->SCLH = (div / 2) - 1 - sync2_ticks;
                        } else {
                                timing->SCLH = 0;
                        }

                        printk("%s%u: target SCL frequency: %u Hz", GET_MODULE_NAME(), major,
                               fi2cclk / presc / div);

                        found = true;
                }
        }

        if (!found) {
                printk("%s%u: SCL frequency out of bound", GET_MODULE_NAME(), major);
        }
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
        I2C_TypeDef            *i2c = const_cast(I2C_HW[major].I2C);

        CLEAR_BIT(RCC->APB1ENR, cfg->APB1ENR_I2CEN);
        SET_BIT(RCC->APB1ENR, cfg->APB1ENR_I2CEN);

        SET_BIT(RCC->APB1RSTR, cfg->APB1RSTR_I2CRST);
        sys_sleep_ms(10);
        CLEAR_BIT(RCC->APB1RSTR, cfg->APB1RSTR_I2CRST);

        NVIC_EnableIRQ(cfg->IRQ_EV_n);
        NVIC_EnableIRQ(cfg->IRQ_ER_n);
        NVIC_SetPriority(cfg->IRQ_EV_n, I2C_HW[major].IRQ_priority);
        NVIC_SetPriority(cfg->IRQ_ER_n, I2C_HW[major].IRQ_priority);

        CLEAR_BIT(i2c->CR1, I2C_CR1_PE);

        // Analog/digital filter
        CLEAR_BIT(i2c->CR1, I2C_CR1_ANFOFF | I2C_CR1_DNF);
        SET_BIT(i2c->CR1, cfg->filter);

        // SCL timing
        i2c_timing_t timing;
        compute_timing(major, &timing);

        i2c->TIMINGR = ((timing.PRESC << I2C_TIMINGR_PRESC_Pos) & I2C_TIMINGR_PRESC)
                     | ((timing.SCLDEL << I2C_TIMINGR_SCLDEL_Pos) & I2C_TIMINGR_SCLDEL)
                     | ((timing.SDADEL << I2C_TIMINGR_SDADEL_Pos) & I2C_TIMINGR_SDADEL)
                     | ((timing.SCLH << I2C_TIMINGR_SCLH_Pos) & I2C_TIMINGR_SCLH)
                     | ((timing.SCLL << I2C_TIMINGR_SCLL_Pos) & I2C_TIMINGR_SCLL);

        i2c->CR2 = 0;
        i2c->CR1 = I2C_CR1_PE;

        _I2C[major]->initialized = true;

        return 0;
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
        I2C_TypeDef     *i2c = const_cast(I2C_HW[major].I2C);

        NVIC_DisableIRQ(cfg->IRQ_EV_n);
        NVIC_DisableIRQ(cfg->IRQ_ER_n);

        WRITE_REG(i2c->CR1, 0);
        SET_BIT(RCC->APB1RSTR, cfg->APB1ENR_I2CEN);
        CLEAR_BIT(RCC->APB1RSTR, cfg->APB1ENR_I2CEN);
        CLEAR_BIT(RCC->APB1ENR, cfg->APB1ENR_I2CEN);

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
        UNUSED_ARG1(hdl);

        /*
         * Not used in this port. Everything is done in _I2C_LLD__master_transmit()
         * and _I2C_LLD__master_receive() functions.
         */

        return ESUCC;
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
        UNUSED_ARG1(hdl);

        /*
         * Not used in this port. Everything is done in _I2C_LLD__master_transmit()
         * and _I2C_LLD__master_receive() functions.
         */

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function generate STOP sequence on I2C bus
 * @param  hdl                  device handle
 */
//==============================================================================
void _I2C_LLD__master_stop(I2C_dev_t *hdl)
{
        UNUSED_ARG1(hdl);

        /*
         * Not used in this port. Everything is done in _I2C_LLD__master_transmit()
         * and _I2C_LLD__master_receive() functions.
         */
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
        UNUSED_ARG3(hdl, write, count);

        /*
         * Not used in this port. Everything is done in _I2C_LLD__master_transmit()
         * and _I2C_LLD__master_receive() functions.
         */

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function receive bytes from I2C bus (master-receiver)
 * @param  hdl                  device handle
 * @param  dst                  destination buffer
 * @param  count                number of bytes to receive
 * @param  rdctr                number of read bytes
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__master_receive(I2C_dev_t *hdl, u8_t *dst, size_t count, size_t *rdctr)
{
        I2C_TypeDef *i2c = get_I2C(hdl);

        /*
         * Disable transmission auto STOP.
         */
        CLEAR_BIT(i2c->CR2, I2C_CR2_AUTOEND);

        /*
         * Set I2C device address
         */
        CLEAR_BIT(i2c->CR2, I2C_CR2_SADD);
        SET_BIT(i2c->CR2, ((hdl->config.address & 0xFFFE) << I2C_CR2_SADD_Pos) & I2C_CR2_SADD);

        CLEAR_BIT(i2c->CR2, I2C_CR2_ADD10);
        if (hdl->config.addr_10bit) {
                SET_BIT(i2c->CR2, I2C_CR2_ADD10);
        }

        /*
         * Calculate transfer size. If subaddress is enabled then write transfer
         * is started first, then automatically repeat start is used to receive
         * selected amount of data (done in EV ISR).
         */
        size_t nbytes = 0;
        _I2C[hdl->major]->buf = dst;
        _I2C[hdl->major]->buf_len = count;
        _I2C[hdl->major]->subaddr_len = hdl->config.sub_addr_mode;
        _I2C[hdl->major]->subaddr_buf = _I2C[hdl->major]->subaddr;

        WRITE_REG(i2c->ICR, I2C_ICR_NACKCF | I2C_ICR_STOPCF);

        if (_I2C[hdl->major]->subaddr_len > 0) {
                /*
                 * The first transfer is subaddress.
                 * Switch peripheral to transmit mode.
                 */
                SET_BIT(i2c->CR1, I2C_CR1_ERRIE  | I2C_CR1_TCIE | I2C_CR1_STOPIE
                                | I2C_CR1_NACKIE | I2C_CR1_TXIE | I2C_CR1_RXIE);

                CLEAR_BIT(i2c->CR2, I2C_CR2_RD_WRN);

                _I2C[hdl->major]->restart_receive = true;

                nbytes = _I2C[hdl->major]->subaddr_len;

        } else {
                /*
                 * Subaddress is disabled, I2C will receive data directly.
                 * Switch peripheral to receive mode.
                 */
                SET_BIT(i2c->CR1, I2C_CR1_ERRIE  | I2C_CR1_TCIE | I2C_CR1_STOPIE
                                | I2C_CR1_NACKIE | I2C_CR1_RXIE);

                SET_BIT(i2c->CR2, I2C_CR2_RD_WRN);

                _I2C[hdl->major]->restart_receive = false;

                nbytes = _I2C[hdl->major]->buf_len;
        }

        if (nbytes > MAX_TRANSFER_SIZE) {
                SET_BIT(i2c->CR2, I2C_CR2_RELOAD);
        } else {
                CLEAR_BIT(i2c->CR2, I2C_CR2_RELOAD);
        }

        CLEAR_BIT(i2c->CR2, I2C_CR2_NBYTES);
        SET_BIT(i2c->CR2, (min(MAX_TRANSFER_SIZE, nbytes) << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES);

        /*
         * Start I2C transmission
         */
        SET_BIT(i2c->CR2, I2C_CR2_START);

        /*
         * Wait for transaction to be finished.
         */
        u32_t event;
        int err = sys_queue_receive(_I2C[hdl->major]->event, &event, I2C_TRANSFER_TIMEOUT);
        if (!err) {
                if (event & I2C_ISR_BERR) {
                        err = EFAULT;

                } else if (event & I2C_ISR_ARLO) {
                        err = EAGAIN;

                } else if (event & I2C_ISR_NACKF) {
                        err = ENODEV;

                } else if (event & I2C_ISR_STOPF) {
                        err = ESUCC;
                }

                if (!err) {
                        *rdctr = count - _I2C[hdl->major]->buf_len;
                }
        } else {
                reset(hdl, false);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function transmit selected amount bytes to I2C bus
 * @param  hdl                  device handle
 * @param  src                  data source
 * @param  count                number of bytes to transfer
 * @param  wrctr                number of written bytes
 * @param  subaddr              subaddress transfer
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _I2C_LLD__master_transmit(I2C_dev_t *hdl, const u8_t *src, size_t count, size_t *wrctr, bool subaddr)
{
        I2C_TypeDef *i2c = get_I2C(hdl);

        /*
         * If transfer contains subaddress then remember it instead of transfer.
         */
        if (subaddr) {
                memset(_I2C[hdl->major]->subaddr, 0, sizeof(_I2C[hdl->major]->subaddr));
                memcpy(_I2C[hdl->major]->subaddr, src, count);
                return ESUCC;
        }

        _I2C[hdl->major]->restart_receive = false;

        /*
         * Set write mode and disable auto STOP.
         */
        CLEAR_BIT(i2c->CR2, I2C_CR2_RD_WRN | I2C_CR2_AUTOEND);

        /*
         * Set device address.
         */
        CLEAR_BIT(i2c->CR2, I2C_CR2_SADD);
        SET_BIT(i2c->CR2, ((hdl->config.address & 0xFFFE) << I2C_CR2_SADD_Pos) & I2C_CR2_SADD);

        CLEAR_BIT(i2c->CR2, I2C_CR2_ADD10);
        if (hdl->config.addr_10bit) {
                SET_BIT(i2c->CR2, I2C_CR2_ADD10);
        }

        /*
         * Configure transfer (number of bytes to send, IRQs, etc).
         */
        WRITE_REG(i2c->ICR, I2C_ICR_NACKCF | I2C_ICR_STOPCF);
        SET_BIT(i2c->CR1, I2C_CR1_ERRIE | I2C_CR1_TCIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE | I2C_CR1_TXIE);

        _I2C[hdl->major]->buf = const_cast(src);
        _I2C[hdl->major]->buf_len = count;
        _I2C[hdl->major]->subaddr_len = hdl->config.sub_addr_mode;
        _I2C[hdl->major]->subaddr_buf = _I2C[hdl->major]->subaddr;

        size_t nbytes = _I2C[hdl->major]->buf_len + _I2C[hdl->major]->subaddr_len;
        if (nbytes > MAX_TRANSFER_SIZE) {
                SET_BIT(i2c->CR2, I2C_CR2_RELOAD);
        } else {
                CLEAR_BIT(i2c->CR2, I2C_CR2_RELOAD);
        }

        CLEAR_BIT(i2c->CR2, I2C_CR2_NBYTES);
        SET_BIT(i2c->CR2, (min(MAX_TRANSFER_SIZE, nbytes) << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES);

        /*
         * Start transfer.
         */
        SET_BIT(i2c->CR2, I2C_CR2_START);

        /*
         * Wait for transaction to be finished.
         */
        u32_t event;
        int err = sys_queue_receive(_I2C[hdl->major]->event, &event, I2C_TRANSFER_TIMEOUT);
        if (!err) {
                if (event & I2C_ISR_BERR) {
                        err = EFAULT;

                } else if (event & I2C_ISR_ARLO) {
                        err = EAGAIN;

                } else if (event & I2C_ISR_NACKF) {
                        err = ENODEV;

                } else if (event & I2C_ISR_STOPF) {
                        err = ESUCC;
                }

                if (!err) {
                        *wrctr = count - _I2C[hdl->major]->buf_len;
                }
        } else {
                reset(hdl, false);
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
        I2C_TypeDef *i2c = get_I2C(hdl);

        if (hdl->config.mode == I2C_MODE__SLAVE) {

                i2c->OAR1 = (hdl->config.address & I2C_OAR1_OA1)
                          | (hdl->config.addr_10bit ? I2C_OAR1_OA1MODE : 0);

                i2c->OAR2 = 0;

                SET_BIT(i2c->OAR1, I2C_OAR1_OA1EN);
                SET_BIT(i2c->CR1, I2C_CR1_ADDRIE);

        } else {
                CLEAR_BIT(i2c->CR1, I2C_CR1_ADDRIE);
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
        I2C_TypeDef *i2c = get_I2C(hdl);

        SET_BIT(i2c->CR1, I2C_CR1_ADDRIE);

        u32_t ISR;
        int err = sys_queue_receive(_I2C[hdl->major]->event, &ISR, event->timeout_ms);
        if (!err) {
                if (ISR & I2C_ISR_ADDR) {
                        event->RD_addr = (ISR & I2C_ISR_DIR);
                } else {
                        err = EAGAIN;
                }
        } else {
                reset(hdl, false);
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
        I2C_TypeDef *i2c = get_I2C(hdl);

        *wrctr = 0;

        _I2C[hdl->major]->buf = const_cast(src);
        _I2C[hdl->major]->buf_len = count;
        _I2C[hdl->major]->subaddr_len = 0;
        _I2C[hdl->major]->subaddr_buf = NULL;
        _I2C[hdl->major]->restart_receive = false;

        WRITE_REG(i2c->ICR, I2C_ICR_ADDRCF | I2C_ICR_STOPCF);
        CLEAR_BIT(i2c->CR1, I2C_CR1_ERRIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE | I2C_CR1_TXIE);
        SET_BIT(i2c->CR1, I2C_CR1_ERRIE | I2C_CR1_STOPIE | I2C_CR1_TXIE);

        /*
         * Wait for transaction to be finished.
         */
        u32_t event;
        int err = sys_queue_receive(_I2C[hdl->major]->event, &event, I2C_TRANSFER_TIMEOUT);
        if (!err) {
                if (event & I2C_ISR_BERR) {
                        err = EFAULT;

                } else if (event & I2C_ISR_ARLO) {
                        err = EAGAIN;

                } else if (event & I2C_ISR_STOPF) {
                        err = ESUCC;
                }

                if (!err) {
                        *wrctr = count - _I2C[hdl->major]->buf_len;
                }
        } else {
                reset(hdl, false);
        }

        CLEAR_BIT(i2c->CR1, I2C_CR1_ERRIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE | I2C_CR1_TXIE);

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
        I2C_TypeDef *i2c = get_I2C(hdl);

        *rdctr = 0;

        _I2C[hdl->major]->buf = dst;
        _I2C[hdl->major]->buf_len = count;
        _I2C[hdl->major]->subaddr_len = 0;
        _I2C[hdl->major]->subaddr_buf = NULL;
        _I2C[hdl->major]->restart_receive = false;

        WRITE_REG(i2c->ICR, I2C_ICR_ADDRCF);
        CLEAR_BIT(i2c->CR1, I2C_CR1_ERRIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE | I2C_CR1_TXIE);
        SET_BIT(i2c->CR1, I2C_CR1_ERRIE | I2C_CR1_STOPIE | I2C_CR1_RXIE);

        /*
         * Wait for transaction to be finished.
         */
        u32_t event;
        int err = sys_queue_receive(_I2C[hdl->major]->event, &event, I2C_TRANSFER_TIMEOUT);
        if (!err) {
                if (event & I2C_ISR_BERR) {
                        err = EFAULT;

                } else if (event & I2C_ISR_ARLO) {
                        err = EAGAIN;

                } else if (event & I2C_ISR_STOPF) {
                        err = ESUCC;
                }

                if (!err) {
                        *rdctr = count - _I2C[hdl->major]->buf_len;
                }
        } else {
                reset(hdl, false);
        }

        CLEAR_BIT(i2c->CR1, I2C_CR1_ERRIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE | I2C_CR1_RXIE);

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

        I2C_TypeDef *i2c = const_cast(I2C_HW[major].I2C);
        u16_t  ISR = i2c->ISR;

        /*
         * Handle slave NACK response.
         */
        if (ISR & I2C_ISR_NACKF) {
                WRITE_REG(i2c->ICR, I2C_ICR_NACKCF);
                CLEAR_BIT(i2c->CR1, I2C_CR1_TXIE);

                if (i2c->CR1 & I2C_CR1_NACKIE) {
                        u32_t event = I2C_ISR_NACKF;
                        sys_queue_send_from_ISR(_I2C[major]->event, &event, &woken);
                }
        }

        /*
         * Handle Tx register empty.
         */
        if ((ISR & I2C_ISR_TXIS) && (i2c->CR1 & I2C_CR1_TXIE)) {
                u8_t byte;

                if (_I2C[major]->subaddr_len > 0) {
                        byte = *_I2C[major]->subaddr_buf++;
                        _I2C[major]->subaddr_len--;

                } else if (_I2C[major]->buf_len > 0) {
                        byte = *_I2C[major]->buf++;
                        _I2C[major]->buf_len--;

                } else {
                        byte = 0;
                }

                i2c->TXDR = byte;
        }

        /*
         * Handle Rx register not empty.
         */
        if ((ISR & I2C_ISR_RXNE) && (i2c->CR1 & I2C_CR1_RXIE)) {
                u8_t byte = i2c->RXDR;

                if (_I2C[major]->buf_len > 0) {
                        *_I2C[major]->buf++ = byte;
                        _I2C[major]->buf_len--;
                }
        }

        /*
         * Handle transfer complete reload. This flag is set when NBYTES field
         * is 0. Flag setup new amount of data to be send.
         */
        if ((ISR & I2C_ISR_TCR) && (i2c->CR1 & I2C_CR1_TCIE)) {

                u32_t nbytes = min(MAX_TRANSFER_SIZE, _I2C[major]->buf_len);
                CLEAR_BIT(i2c->CR2, I2C_CR2_NBYTES);
                SET_BIT(i2c->CR2, (nbytes << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES);

                if (_I2C[major]->buf_len > MAX_TRANSFER_SIZE) {
                        SET_BIT(i2c->CR2, I2C_CR2_RELOAD);
                } else {
                        CLEAR_BIT(i2c->CR2, I2C_CR2_RELOAD);
                }
        }

        /*
         * Handle transfer complete. Flag is set when transfer tx/rx is complete.
         */
        if ((ISR & I2C_ISR_TC) && (i2c->CR1 & I2C_CR1_TCIE)) {

                if (_I2C[major]->restart_receive) {
                        /*
                         * Restart-receive operation requested (after subaddress
                         * sent).
                         */
                        _I2C[major]->restart_receive = false;
                        CLEAR_BIT(i2c->CR1, I2C_CR1_TXIE);

                        SET_BIT(i2c->CR2, I2C_CR2_RD_WRN);

                        size_t nbytes = _I2C[major]->buf_len;
                        if (nbytes > MAX_TRANSFER_SIZE) {
                                SET_BIT(i2c->CR2, I2C_CR2_RELOAD);
                        } else {
                                CLEAR_BIT(i2c->CR2, I2C_CR2_RELOAD);
                        }

                        CLEAR_BIT(i2c->CR2, I2C_CR2_NBYTES);
                        SET_BIT(i2c->CR2, (min(MAX_TRANSFER_SIZE, nbytes) << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES);

                        SET_BIT(i2c->CR2, I2C_CR2_START);
                } else {
                        /*
                         * Transfer finished, stop request.
                         */
                        SET_BIT(i2c->CR2, I2C_CR2_STOP);
                }
        }

        /*
         * STOP signal generated. After this operation entire transfer is complete.
         */
        if ((ISR & I2C_ISR_STOPF) && (i2c->CR1 & I2C_CR1_STOPIE)) {
                WRITE_REG(i2c->ICR, I2C_ICR_STOPCF);

                CLEAR_BIT(i2c->CR1, I2C_CR1_ERRIE  | I2C_CR1_TCIE | I2C_CR1_STOPIE
                                  | I2C_CR1_NACKIE | I2C_CR1_TXIE | I2C_CR1_RXIE);

                u32_t event = I2C_ISR_STOPF;
                sys_queue_send_from_ISR(_I2C[major]->event, &event, &woken);
        }

        /*
         * Handle address selection flag.
         */
        if ((ISR & I2C_ISR_ADDR) && (i2c->CR1 & I2C_CR1_ADDRIE)) {
                SET_BIT(i2c->ISR, I2C_ISR_TXE);
                CLEAR_BIT(i2c->CR1, I2C_CR1_ADDRIE);

                u32_t event = i2c->ISR;
                sys_queue_send_from_ISR(_I2C[major]->event, &event, &woken);
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
        bool woken = false;

        I2C_TypeDef *i2c = const_cast(I2C_HW[major].I2C);
        u16_t ISR = i2c->ISR;

        if (ISR & ( I2C_ISR_BERR    | I2C_ISR_OVR   | I2C_ISR_ARLO
                  | I2C_ISR_TIMEOUT | I2C_ISR_ALERT | I2C_ISR_PECERR)) {

                u32_t event = ISR;
                sys_queue_send_from_ISR(_I2C[major]->event, &event, &woken);
        }

        WRITE_REG(i2c->ICR, I2C_ICR_BERRCF   | I2C_ICR_OVRCF    | I2C_ICR_ARLOCF
                          | I2C_ICR_TIMOUTCF | I2C_ICR_ALERTCF  | I2C_ICR_PECCF);

        sys_thread_yield_from_ISR(woken);
}

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

//==============================================================================
/**
 * @brief  I2C4 Event IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_I2C4EN)
void I2C4_EV_IRQHandler(void)
{
        IRQ_EV_handler(_I2C4);
}
#endif

//==============================================================================
/**
 * @brief  I2C4 Error IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_I2C4EN)
void I2C4_ER_IRQHandler(void)
{
        IRQ_ER_handler(_I2C4);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
