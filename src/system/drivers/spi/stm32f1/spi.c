/*=========================================================================*//**
@file    spi.c

@author  Daniel Zorychta

@brief   SPI driver

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "system/dnxmodule.h"
#include "system/thread.h"
#include "stm32f1/spi_cfg.h"
#include "stm32f1/spi_def.h"
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define MAX_NUMBER_OF_CS                8


#define MTX_BLOCK_TIME                  10
#define force_lock_mutex(mtx)           while (lock_mutex(mtx, 10) != MUTEX_LOCKED)
#define force_lock_recursive_mutex(mtx) while (lock_recursive_mutex(mtx, 10) != MUTEX_LOCKED)

#define SET_REG_BIT(reg, bits)          reg |=  (bits)
#define CLR_REG_BIT(reg, bits)          reg &= ~(bits)

#define SPI_CR1_BR_CALC(div)            (((div) << 3) & SPI_CR1_BR)

#define is_16_bit_frame(spi)            ((spi)->CR1 & SPI_CR1_DFF)
#define is_8_bit_frame(spi)             (!is_16_bit_frame(spi))
#define is_tx_buffer_empty(spi)         ((spi)->SR & SPI_SR_TXE)
#define is_rx_buffer_not_empty(spi)     ((spi)->SR & SPI_SR_RXNE)

#define DMA_CHAIN_NUMBER                3

/*==============================================================================
  Local object types
==============================================================================*/
/* configuration of single CS line (port and pin) */
struct cs_pin_cfg {
        GPIO_t *const           port;
        u16_t                   pin_mask;
};

/* independent SPI instance */
struct spi_virtual {
        u8_t                    major;
        u8_t                    minor;
        struct SPI_config       config;
        dev_lock_t              file_lock;
};

/* general module data */
struct module {
        mutex_t                *device_protect_mtx[_SPI_DEV_NUMBER];
        u8_t                    number_of_virtual_spi[_SPI_DEV_NUMBER];
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t spi_turn_on     (SPI_t *spi);
static stdret_t spi_turn_off    (SPI_t *spi);

/*==============================================================================
  Local objects
==============================================================================*/
/* SPI peripherals address */
static SPI_t *const spi[_SPI_DEV_NUMBER] = {
#if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        SPI1,
#endif
#if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        SPI2,
#endif
#if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        SPI3,
#endif
};

/* CS port configuration */
static const struct cs_pin_cfg spi_cs_pin_cfg[_SPI_DEV_NUMBER][MAX_NUMBER_OF_CS] = {
#if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        {{port: _SPI1_CS0_PORT, pin_mask: (1 << _SPI1_CS0_PIN)},
         {port: _SPI1_CS1_PORT, pin_mask: (1 << _SPI1_CS1_PIN)},
         {port: _SPI1_CS2_PORT, pin_mask: (1 << _SPI1_CS2_PIN)},
         {port: _SPI1_CS3_PORT, pin_mask: (1 << _SPI1_CS3_PIN)},
         {port: _SPI1_CS4_PORT, pin_mask: (1 << _SPI1_CS4_PIN)},
         {port: _SPI1_CS5_PORT, pin_mask: (1 << _SPI1_CS5_PIN)},
         {port: _SPI1_CS6_PORT, pin_mask: (1 << _SPI1_CS6_PIN)},
         {port: _SPI1_CS7_PORT, pin_mask: (1 << _SPI1_CS7_PIN)}},
#endif
#if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        {{port: _SPI2_CS0_PORT, pin_mask: (1 << _SPI2_CS0_PIN)},
         {port: _SPI2_CS1_PORT, pin_mask: (1 << _SPI2_CS1_PIN)},
         {port: _SPI2_CS2_PORT, pin_mask: (1 << _SPI2_CS2_PIN)},
         {port: _SPI2_CS3_PORT, pin_mask: (1 << _SPI2_CS3_PIN)},
         {port: _SPI2_CS4_PORT, pin_mask: (1 << _SPI2_CS4_PIN)},
         {port: _SPI2_CS5_PORT, pin_mask: (1 << _SPI2_CS5_PIN)},
         {port: _SPI2_CS6_PORT, pin_mask: (1 << _SPI2_CS6_PIN)},
         {port: _SPI2_CS7_PORT, pin_mask: (1 << _SPI2_CS7_PIN)}},
#endif
#if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        {{port: _SPI3_CS0_PORT, pin_mask: (1 << _SPI3_CS0_PIN)},
         {port: _SPI3_CS1_PORT, pin_mask: (1 << _SPI3_CS1_PIN)},
         {port: _SPI3_CS2_PORT, pin_mask: (1 << _SPI3_CS2_PIN)},
         {port: _SPI3_CS3_PORT, pin_mask: (1 << _SPI3_CS3_PIN)},
         {port: _SPI3_CS4_PORT, pin_mask: (1 << _SPI3_CS4_PIN)},
         {port: _SPI3_CS5_PORT, pin_mask: (1 << _SPI3_CS5_PIN)},
         {port: _SPI3_CS6_PORT, pin_mask: (1 << _SPI3_CS6_PIN)},
         {port: _SPI3_CS7_PORT, pin_mask: (1 << _SPI3_CS7_PIN)}}
#endif
};

/* default SPI config */
static const struct SPI_config spi_default_cfg = {
        dummy_byte : _SPI_DEFAULT_CFG_DUMMY_BYTE,
        clk_divider: _SPI_DEFAULT_CFG_CLK_DIVIDER,
        mode       : _SPI_DEFAULT_CFG_MODE,
        master_mode: _SPI_DEFAULT_CFG_MASTER_MODE,
        msb_first  : _SPI_DEFAULT_CFG_MSB_FIRST
};

/* pointers to memory of specified device */
static struct module *spi_module;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_INIT(SPI, void **device_handle, u8_t major, u8_t minor)
{
        if (major >= _SPI_DEV_NUMBER)
                return STD_RET_ERROR;

#if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        if (major == _SPI_DEV_1 && minor >= _SPI1_NUMBER_OF_SLAVES)
                return STD_RET_ERROR;
#endif

#if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        if (major == _SPI_DEV_2 && minor >= _SPI2_NUMBER_OF_SLAVES)
                return STD_RET_ERROR;
#endif

#if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        if (major == _SPI_DEV_3 && minor >= _SPI3_NUMBER_OF_SLAVES)
                return STD_RET_ERROR;
#endif

        /* allocate module general data if initialized first time */
        if (!spi_module) {
                spi_module = calloc(1, sizeof(struct module));
                if (!spi_module)
                        return STD_RET_ERROR;
        }

        /* create protection mutex and start device if initialized first time */
        if (!spi_module->device_protect_mtx[major]) {
                spi_module->device_protect_mtx[major] = mutex_new(MUTEX_NORMAL);
                if (!spi_module->device_protect_mtx[major]) {
                        return STD_RET_ERROR;
                } else {
                        /* TODO start SPI clock etc */
                }
        }

        /* create new instance for specified major-minor number (virtual spi) */
        struct spi_virtual *hdl = calloc(1, sizeof(struct spi_virtual));
        if (!hdl) {
                return STD_RET_ERROR;
        }

        hdl->config     = spi_default_cfg;
        hdl->major      = major;
        hdl->minor      = minor;
        *device_handle  = hdl;

        spi_module->number_of_virtual_spi[major]++;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Device release
 */
//==============================================================================
MODULE__DEVICE_RELEASE(SPI)
{
        STOP_IF(!device_handle);

        SPI_handle *hdl = device_handle;

        force_lock_recursive_mutex(hdl->file_lock);

        enter_critical_section();

        unlock_recursive_mutex(hdl->file_lock);

        delete_recursive_mutex(hdl->file_lock);

        spi_turn_off(hdl->SPI);
        free(hdl);
        exit_critical_section();

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Device open
 */
//==============================================================================
MODULE__DEVICE_OPEN(SPI)
{
        STOP_IF(!device_handle);

        SPI_handle *hdl = device_handle;

        if (lock_recursive_mutex(hdl->file_lock, MTX_BLOCK_TIME) == MUTEX_LOCKED) {

                /* program take control on this file, no mutex unlocking */

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Device close
 */
//==============================================================================
MODULE__DEVICE_CLOSE(SPI)
{
        STOP_IF(!device_handle);

        SPI_handle *hdl = device_handle;

        if (lock_recursive_mutex(hdl->file_lock, MTX_BLOCK_TIME) == MUTEX_LOCKED) {

                unlock_recursive_mutex(hdl->file_lock);         /* close */
                unlock_recursive_mutex(hdl->file_lock);         /* open  */

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Device write
 */
//==============================================================================
MODULE__DEVICE_WRITE(SPI)
{
        UNUSED_ARG(lseek);

        STOP_IF(device_handle == NULL);
        STOP_IF(src == NULL);
        STOP_IF(item_size == 0);
        STOP_IF(n_items == 0);

        SPI_handle *hdl = device_handle;

        size_t n = 0;
        if (lock_recursive_mutex(hdl->file_lock, MTX_BLOCK_TIME) == MUTEX_LOCKED) {

                if (hdl->I2S_mode) {
                        if (item_size == sizeof(u16_t)) {
                                if (take_counting_semaphore(hdl->I2S_chain.semaphore, MAX_DELAY) == SEMAPHORE_TAKEN) {
                                        enter_critical_section();
                                        if (write_I2S_FIFO(&hdl->I2S_chain, src, n_items) == 0) {

                                                if (hdl->I2S_chain.DMA_started == false) {
                                                        const u16_t *bfr;
                                                        uint         size;
                                                        if (read_I2S_FIFO(&hdl->I2S_chain, &bfr, &size) == 0) {
                                                                DMA1_Channel5->CMAR  = (u32_t) bfr;
                                                                DMA1_Channel5->CNDTR = size;
                                                                SET_BIT(DMA1_Channel5->CCR, DMA_CCR5_EN);
                                                                hdl->I2S_chain.DMA_started = true;
                                                        }
                                                }


                                        }
                                        exit_critical_section();
                                }
                        }
                } else {
#if   (SPI_HANDLER_MODE == 0)
                        uint         size    = n_items * item_size;
                        const u8_t  *data8b  = src;
                        const u16_t *data16b = src;

                        if (is_16_bit_frame(hdl->SPI)) {
                                size &= ~(1 << 0);
                        }

                        while (size) {
                                while (!is_tx_buffer_empty(hdl->SPI));

                                if (is_16_bit_frame(hdl->SPI)) {
                                        hdl->SPI->DR = *(data16b++);
                                } else {
                                        hdl->SPI->DR = *(data8b++);
                                }

                                while (!(hdl->SPI->SR & SPI_SR_RXNE));
                                volatile u16_t tmp = hdl->SPI->DR;
                                (void)tmp;

                                size--;
                                n++;
                        }
#elif (SPI_HANDLER_MODE == 1)
#elif (SPI_HANDLER_MODE == 2)
#else
#error "Unknown SPI handler mode! Check configuration."
#endif
                }

                unlock_recursive_mutex(hdl->file_lock);
        }

        return n;
}

//==============================================================================
/**
 * @brief Device read
 */
//==============================================================================
MODULE__DEVICE_READ(SPI)
{
        UNUSED_ARG(lseek);

        STOP_IF(device_handle == NULL);
        STOP_IF(dst == NULL);
        STOP_IF(item_size == 0);
        STOP_IF(n_items == 0);

        SPI_handle *hdl = device_handle;

        size_t n = 0;
        if (lock_recursive_mutex(hdl->file_lock, MTX_BLOCK_TIME) == MUTEX_LOCKED) {

#if   (SPI_HANDLER_MODE == 0)
                if (!hdl->I2S_mode) {
                        uint   size    = n_items * item_size;
                        u8_t  *data8b  = dst;
                        u16_t *data16b = dst;

                        if (is_16_bit_frame(hdl->SPI)) {
                                size &= ~(1 << 0);
                        }

                        while (size) {
                                while (!is_tx_buffer_empty(hdl->SPI));

                                hdl->SPI->DR = hdl->dummy_frame;

                                while (!(hdl->SPI->SR & SPI_SR_RXNE));
                                if (is_16_bit_frame(hdl->SPI)) {
                                        *(data16b++) = hdl->SPI->DR;
                                } else {
                                        *(data8b++) = hdl->SPI->DR;
                                }

                                size--;
                                n++;
                        }
#elif (SPI_HANDLER_MODE == 1)
#elif (SPI_HANDLER_MODE == 2)
#else
#error "Unknown SPI handler mode! Check configuration."
#endif
                }
                unlock_recursive_mutex(hdl->file_lock);
        }

        return n;
}

//==============================================================================
/**
 * @brief Device flush
 */
//==============================================================================
MODULE__DEVICE_FLUSH(SPI)
{
        STOP_IF(device_handle == NULL);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Device ioctl
 */
//==============================================================================
MODULE__DEVICE_IOCTL(SPI)
{
        STOP_IF(device_handle == NULL);

        SPI_handle *hdl    = device_handle;
        stdret_t    status = STD_RET_ERROR;

        if (lock_recursive_mutex(hdl->file_lock, MTX_BLOCK_TIME) == MUTEX_LOCKED) {
                status = STD_RET_OK;

                switch (iorq) {
                case SPI_IORQ_SET_DUMMY_FRAME:
                        hdl->dummy_frame = va_arg(args, int);
                        break;

                case SPI_IORQ_ENABLE_8_BIT_FRAME:
                        CLR_REG_BIT(hdl->SPI->CR1, SPI_CR1_DFF);
                        break;

                case SPI_IORQ_ENABLE_16_BIT_FRAME:
                        SET_REG_BIT(hdl->SPI->CR1, SPI_CR1_DFF);
                        break;

                case SPI_IORQ_LSB_FIRST:
                        SET_REG_BIT(hdl->SPI->CR1, SPI_CR1_LSBFIRST);
                        break;

                case SPI_IORQ_MSB_FIRST:
                        CLR_REG_BIT(hdl->SPI->CR1, SPI_CR1_LSBFIRST);
                        break;

                case SPI_IORQ_CAPTURE_ON_FIRST_EDGE:
                        CLR_REG_BIT(hdl->SPI->CR1, SPI_CR1_CPHA);
                        break;

                case SPI_IORQ_CAPTURE_ON_SECOND_EDGE:
                        SET_REG_BIT(hdl->SPI->CR1, SPI_CR1_CPHA);
                        break;

                case SPI_IORQ_CLOCK_LOW_WHEN_IDLE:
                        CLR_REG_BIT(hdl->SPI->CR1, SPI_CR1_CPOL);
                        break;

                case SPI_IORQ_CLOCK_HIGH_WHEN_IDLE:
                        SET_REG_BIT(hdl->SPI->CR1, SPI_CR1_CPOL);
                        break;

                case SPI_IORQ_ENABLE_MASTER_MODE:
                        SET_REG_BIT(hdl->SPI->CR1, SPI_CR1_MSTR);
                        break;

                case SPI_IORQ_ENABLE_SLAVE_MODE:
                        CLR_REG_BIT(hdl->SPI->CR1, SPI_CR1_MSTR);
                        break;

                case SPI_IORQ_SET_CLOCK_DIVIDER:
                        CLR_REG_BIT(hdl->SPI->CR1, SPI_CR1_BR);
                        SET_REG_BIT(hdl->SPI->CR1, SPI_CR1_BR_CALC(va_arg(args, int)));
                        break;

                case SPI_IORQ_ENABLE_SW_SLAVE_MANAGEMENT:
                        SET_REG_BIT(hdl->SPI->CR1, SPI_CR1_SSM);
                        break;

                case SPI_IORQ_DISABLE_SW_SLAVE_MANAGEMENT:
                        CLR_REG_BIT(hdl->SPI->CR1, SPI_CR1_SSM);
                        break;

                case SPI_IORQ_UNIDIRECTIONAL_DATA_MODE:
                        CLR_REG_BIT(hdl->SPI->CR1, SPI_CR1_BIDIMODE);
                        break;

                case SPI_IORQ_BIDIRECTIONAL_DATA_MODE:
                        SET_REG_BIT(hdl->SPI->CR1, SPI_CR1_BIDIMODE);
                        break;

                case SPI_IORQ_RECEIVE_ONLY_IN_BD_MODE:
                        CLR_REG_BIT(hdl->SPI->CR1, SPI_CR1_BIDIOE);
                        break;

                case SPI_IORQ_TRANSMIT_ONLY_IN_BD_MODE:
                        SET_REG_BIT(hdl->SPI->CR1, SPI_CR1_BIDIOE);
                        break;

                case SPI_IORQ_TURN_ON:
                        hdl->I2S_mode = false;
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SMOD | SPI_I2SCFGR_I2SE);
                        SET_REG_BIT(hdl->SPI->CR1, SPI_CR1_SPE);
                        break;

                case SPI_IORQ_TURN_OFF:
                        CLR_REG_BIT(hdl->SPI->CR1, SPI_CR1_SPE);
                        break;

                case SPI_IORQ_ENABLE_SS_OUTPUT:
                        SET_REG_BIT(hdl->SPI->CR2, SPI_CR2_SSOE);
                        break;

                case SPI_IORQ_DISABLE_SS_OUTPUT:
                        CLR_REG_BIT(hdl->SPI->CR2, SPI_CR2_SSOE);
                        break;

                case SPI_IORQ_SHIFT_WORD: {
                        u16_t *frame = va_arg(args, u16_t*);
                        if (!frame) {
                                status = STD_RET_ERROR;
                        } else {
                                while (!is_tx_buffer_empty(hdl->SPI));
                                hdl->SPI->DR = (*frame) & 0xFFFF;
                                while (!(hdl->SPI->SR & SPI_SR_RXNE));
                                *frame = hdl->SPI->DR;
                        }
                        break;
                }

                case I2S_IORQ_DISABLE_I2S:
                        RCC->APB1RSTR |=  RCC_APB1RSTR_SPI2RST;
                        RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;

                        CLEAR_BIT(hdl->SPI->CR1, SPI_CR1_SPE);
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SE);
                        break;

                case I2S_IORQ_ENABLE_I2S:
                        CLEAR_BIT(hdl->SPI->CR1, SPI_CR1_SPE);
                        SET_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SMOD);
                        SET_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SE);
                        hdl->I2S_mode = true;

                        NVIC_EnableIRQ(DMA1_Channel5_IRQn);
                        SET_BIT(hdl->SPI->CR2, SPI_CR2_TXDMAEN);
                        DMA1_Channel5->CPAR = (u32_t)&hdl->SPI->DR;
                        DMA1_Channel5->CCR   = DMA_CCR5_MINC
                                             | DMA_CCR5_DIR
                                             | DMA_CCR5_PSIZE_0
                                             | DMA_CCR5_MSIZE_0
                                             | DMA_CCR5_TCIE;
                        break;

                case I2S_IORQ_SLAVE_TRANSMIT:
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SCFG);
                        break;

                case I2S_IORQ_SLAVE_RECEIVE:
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SCFG);
                        SET_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SCFG_0);
                        break;

                case I2S_IORQ_MASTER_TRANSMIT:
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SCFG);
                        SET_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SCFG_1);
                        break;

                case I2S_IORQ_MASTER_RECEIVE:
                        SET_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SCFG_1 | SPI_I2SCFGR_I2SCFG_0);
                        break;

                case I2S_IORQ_SET_I2S_DIVIDER: {
                        int divider = va_arg(args, int);
                        divider &= SPI_I2SPR_I2SDIV;
                        CLEAR_BIT(hdl->SPI->I2SPR, SPI_I2SPR_I2SDIV);
                        SET_BIT(hdl->SPI->I2SPR, divider);
                        break;
                }

                case I2S_IORQ_SET_ODD_FACTOR:
                        SET_BIT(hdl->SPI->I2SPR, SPI_I2SPR_ODD);
                        break;

                case I2S_IORQ_CLEAR_ODD_FACTOR:
                        CLEAR_BIT(hdl->SPI->I2SPR, SPI_I2SPR_ODD);
                        break;

                case I2S_IORQ_ENABLE_MASTER_CLOCK_OUTPUT:
                        SET_BIT(hdl->SPI->I2SPR, SPI_I2SPR_MCKOE);
                        break;

                case I2S_IORQ_DISABLE_MASTER_CLOCK_OUTPUT:
                        CLEAR_BIT(hdl->SPI->I2SPR, SPI_I2SPR_MCKOE);
                        break;

                case I2S_IORQ_PCMSYNC_SHORT_FRAME:
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_PCMSYNC);
                        break;

                case I2S_IORQ_PCMSYNC_LONG_FRAME:
                        SET_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_PCMSYNC);
                        break;

                case I2S_IORQ_PHILIPS_STANDARD:
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SSTD);
                        break;

                case I2S_IORQ_MSB_JUSTIFIED_STANDARD:
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SSTD);
                        SET_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SSTD_0);
                        break;

                case I2S_IORQ_LSB_JUSTIFIED_STANDARD:
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SSTD);
                        SET_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SSTD_1);
                        break;

                case I2S_IORQ_PCM_STANDARD:
                        SET_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_I2SSTD_1 | SPI_I2SCFGR_I2SSTD_0);
                        break;

                case I2S_IORQ_CLOCK_STEADY_LOW:
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_CKPOL);
                        break;

                case I2S_IORQ_CLOCK_STEADY_HIGH:
                        SET_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_CKPOL);
                        break;

                case I2S_IORQ_16_BIT_DATA_LENGTH:
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_DATLEN);
                        break;

                case I2S_IORQ_24_BIT_DATA_LENGTH:
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_DATLEN);
                        SET_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_DATLEN_0);
                        break;

                case I2S_IORQ_32_BIT_DATA_LENGTH:
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_DATLEN);
                        SET_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_DATLEN_1);
                        break;

                case I2S_IORQ_16_BIT_WIDE_CHANNEL:
                        CLEAR_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_CHLEN);
                        break;

                case I2S_IORQ_32_BIT_WIDE_CHANNEL:
                        SET_BIT(hdl->SPI->I2SCFGR, SPI_I2SCFGR_CHLEN);
                        break;

                default:
                        status = STD_RET_ERROR;
                        break;
                }

                unlock_recursive_mutex(hdl->file_lock);
        }

        return status;
}

//==============================================================================
/**
 * @brief Device info
 */
//==============================================================================
MODULE__DEVICE_INFO(SPI)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(device_info == NULL);

        device_info->st_size = 0;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function enable SPI interface
 *
 * @param[in] *spi      spi peripheral
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t spi_turn_on(SPI_t *spi)
{
        switch ((uint32_t)spi) {
#if defined(RCC_APB2ENR_SPI1EN) && (SPI_1_ENABLE > 0)
        case SPI1_BASE:
                RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
                break;
#endif
#if defined(RCC_APB1ENR_SPI2EN) && (SPI_2_ENABLE > 0)
        case SPI2_BASE:
                RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
                break;
#endif
#if defined(RCC_APB1ENR_SPI3EN) && (SPI_3_ENABLE > 0)
        case SPI3_BASE:
                RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
                break;
#endif
        default:
                return STD_RET_ERROR;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function disable SPI interface
 *
 * @param[in] *spi      spi peripheral
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t spi_turn_off(SPI_t *spi)
{
        switch ((uint32_t)spi) {
#if defined(RCC_APB2ENR_SPI1EN) && (SPI_1_ENABLE > 0)
        case SPI1_BASE:
                RCC->APB2RSTR |=  RCC_APB2RSTR_SPI1RST;
                RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
                RCC->APB2ENR  &= ~RCC_APB2ENR_SPI1EN;
                break;
#endif
#if defined(RCC_APB1ENR_SPI2EN) && (SPI_2_ENABLE > 0)
        case SPI2_BASE:
                RCC->APB1RSTR |=  RCC_APB1RSTR_SPI2RST;
                RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;
                RCC->APB1ENR  &= ~RCC_APB1ENR_SPI2EN;
                break;
#endif
#if defined(RCC_APB1ENR_SPI3EN) && (SPI_3_ENABLE > 0)
        case SPI3_BASE:
                RCC->APB1RSTR |=  RCC_APB1RSTR_SPI3RST;
                RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI3RST;
                RCC->APB1ENR  &= ~RCC_APB1ENR_SPI3EN;
                break;
#endif
        default:
                return STD_RET_ERROR;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function write data to FIFO
 */
//==============================================================================
static int write_I2S_FIFO(struct chain_fifo *fifo, const u16_t *bfr, uint size)
{
        if (!fifo || !bfr || !size)
                return 1;

        if(fifo->level < DMA_CHAIN_NUMBER) {
                fifo->buffer[fifo->wr_idx] = bfr;
                fifo->bsize[fifo->wr_idx]  = size;

                if (++fifo->wr_idx >= DMA_CHAIN_NUMBER)
                        fifo->wr_idx = 0;

                fifo->level++;

                return 0;
        }

        return 1;
}

//==============================================================================
/**
 * @brief Function read data from FIFO
 */
//==============================================================================
static int read_I2S_FIFO(struct chain_fifo *fifo, const u16_t **bfr, uint *size)
{
        if (!fifo || !bfr || !size)
                return 1;

        if (fifo->level > 0) {
                *bfr  = fifo->buffer[fifo->rd_idx];
                *size = fifo->bsize[fifo->rd_idx];

                fifo->buffer[fifo->rd_idx] = NULL;
                fifo->bsize[fifo->rd_idx]  = 0;

                if (++fifo->rd_idx >= DMA_CHAIN_NUMBER)
                        fifo->rd_idx = 0;

                fifo->level--;

                return 0;
        }

        return 1;
}

//==============================================================================
/**
 * @brief Function
 */
//==============================================================================
void DMA1_Channel5_IRQHandler(void)
{
        DMA1->IFCR = DMA_IFCR_CTCIF5;
        CLEAR_BIT(DMA1_Channel5->CCR, DMA_CCR5_EN);

        const u16_t *bfr  = NULL;
        uint         size = 0;
        if (read_I2S_FIFO(&SPI_data[SPI_DEV_2]->I2S_chain, &bfr, &size) == 0) {
                STOP_IF(bfr == NULL);
                STOP_IF(size == 0);

                DMA1_Channel5->CMAR  = (u32_t) bfr;
                DMA1_Channel5->CNDTR = (u32_t) size;
                SET_BIT(DMA1_Channel5->CCR, DMA_CCR5_EN);

                SPI_data[SPI_DEV_2]->I2S_chain.DMA_started = true;
        } else {
                SPI_data[SPI_DEV_2]->I2S_chain.DMA_started = false;
        }

        int woke = OS_FALSE;
        give_counting_semaphore_from_ISR(SPI_data[SPI_DEV_2]->I2S_chain.semaphore, &woke);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
