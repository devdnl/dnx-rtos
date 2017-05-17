/*=========================================================================*//**
@file    spi.c

@author  Daniel Zorychta

@brief   SPI driver

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "drivers/driver.h"
#include "stm32f1/spi_cfg.h"
#include "stm32f1/stm32f10x.h"
#include "gpio_ddi.h"
#include "../spi_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define MUTEX_TIMOUT             MAX_DELAY_MS
#define SEMAPHORE_TIMEOUT        MAX_DELAY_MS

/*==============================================================================
  Local object types
==============================================================================*/
MODULE_NAME(SPI);

/** major number */
enum {
    #if defined(RCC_APB2ENR_SPI1EN)
        _SPI1,
    #endif
    #if defined(RCC_APB1ENR_SPI2EN)
        _SPI2,
    #endif
    #if defined(RCC_APB1ENR_SPI3EN)
        _SPI3,
    #endif
        _NUMBER_OF_SPI_PERIPHERALS
};

/* SPI peripheral configuration */
struct SPI_info {
        SPI_t                   *SPI;                   //!< SPI peripheral address
        __IO u32_t              *APBRSTR;               //!< APB reset register address
        __IO u32_t              *APBENR;                //!< APB enable register
        u32_t                    APBRSTRENR;            //!< APB reset/enable bit
        IRQn_Type                IRQn;                  //!< SPI IRQ number
        DMA_t                   *DMA;                   //!< SPI DMA peripheral
    #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
        DMA_Channel_t           *DMA_Tx_channel;        //!< DMA Tx channel address
        DMA_Channel_t           *DMA_Rx_channel;        //!< DMA Rx channel address
        u8_t                     DMA_Rx_channel_number; //!< DMA Rx channel number
        IRQn_Type                DMA_Rx_IRQn;           //!< DMA Rx IRQ number
        u32_t                    DMAEN;                 //!< DMA enable bit
    #endif
};

/* independent SPI instance */
struct SPI_slave {
        dev_lock_t               lock;             //!< SPI slave lock
        u8_t                     major;                 //!< SPI major number
        u8_t                     minor;                 //!< SPI minor number
        SPI_config_t             config;                //!< SPI configuration
};

/* general module data */
struct SPI {
        sem_t                   *wait_irq_sem;          //!< IRQ detect semaphore
        mutex_t                 *periph_protect_mtx;    //!< SPI protection mutex
        struct SPI_slave        *slave;                 //!< current handled slave
        const u8_t              *tx_buffer;             //!< Tx buffer
        u8_t                    *rx_buffer;             //!< Rx buffer
        size_t                   count;                 //!< transaction length
        bool                     RAW;                   //!< RAW mode
        u8_t                     flush_byte;            //!< flush byte (read transaction)
        u8_t                     slave_count;           //!< number of slaves
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void     release_resources    (u8_t major);
static int      turn_on_SPI          (u8_t major);
static void     turn_off_SPI         (u8_t major);
static void     apply_SPI_config     (struct SPI_slave *hdl);
static void     apply_SPI_safe_config(u8_t major);
static void     select_slave         (struct SPI_slave *hdl);
static void     deselect_slave       (struct SPI_slave *hdl);
static int      transceive           (struct SPI_slave *hdl, const u8_t *tx, u8_t *rx, size_t count);

/*==============================================================================
  Local objects
==============================================================================*/
/* SPI peripherals basic parameters */
static const struct SPI_info SPI_INFO[_NUMBER_OF_SPI_PERIPHERALS] = {
        #if defined(RCC_APB2ENR_SPI1EN)
        {
                .APBENR                = &RCC->APB2ENR,
                .APBRSTR               = &RCC->APB2RSTR,
                .APBRSTRENR            = RCC_APB2ENR_SPI1EN,
                .IRQn                  = SPI1_IRQn,
                .SPI                   = SPI1,
                .DMA                   = _SPI1_USE_DMA ? DMA1 : NULL,
                #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
                .DMA_Tx_channel        = DMA1_Channel3,
                .DMA_Rx_channel        = DMA1_Channel2,
                .DMA_Rx_IRQn           = DMA1_Channel2_IRQn,
                .DMA_Rx_channel_number = 2,
                .DMAEN                 = RCC_AHBENR_DMA1EN,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_SPI2EN)
        {
                .APBENR                = &RCC->APB1ENR,
                .APBRSTR               = &RCC->APB1RSTR,
                .APBRSTRENR            = RCC_APB1ENR_SPI2EN,
                .IRQn                  = SPI2_IRQn,
                .SPI                   = SPI2,
                .DMA                   = _SPI2_USE_DMA ? DMA1 : NULL,
                #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
                .DMA_Tx_channel        = DMA1_Channel5,
                .DMA_Rx_channel        = DMA1_Channel4,
                .DMA_Rx_IRQn           = DMA1_Channel4_IRQn,
                .DMA_Rx_channel_number = 4,
                .DMAEN                 = RCC_AHBENR_DMA1EN,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_SPI3EN)
        {
                .APBENR                = &RCC->APB1ENR,
                .APBRSTR               = &RCC->APB1RSTR,
                .APBRSTRENR            = RCC_APB1ENR_SPI3EN,
                .IRQn                  = SPI3_IRQn,
                .SPI                   = SPI3,
                .DMA                   = _SPI3_USE_DMA ? DMA2 : NULL,
                #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
                .DMA_Tx_channel        = DMA2_Channel2,
                .DMA_Rx_channel        = DMA2_Channel1,
                .DMA_Rx_IRQn           = DMA2_Channel1_IRQn,
                .DMA_Rx_channel_number = 1,
                .DMAEN                 = RCC_AHBENR_DMA2EN,
                #endif
        }
        #endif
};

/* default SPI config */
static const SPI_config_t SPI_DEFAULT_CFG = {
        .flush_byte  = _SPI_DEFAULT_CFG_FLUSH_BYTE,
        .clk_divider = _SPI_DEFAULT_CFG_CLK_DIVIDER,
        .mode        = _SPI_DEFAULT_CFG_MODE,
        .msb_first   = _SPI_DEFAULT_CFG_MSB_FIRST,
        .CS_port_idx = 255,     // CS deactivated
        .CS_pin_idx  = 255      // CS deactivated
};

/* pointers to memory of specified device */
static struct SPI *SPI[_NUMBER_OF_SPI_PERIPHERALS];

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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(SPI, void **device_handle, u8_t major, u8_t minor)
{
        int err = ENXIO;

        if (major >= _NUMBER_OF_SPI_PERIPHERALS) {
                return err;
        }

        /* initialize SPI peripheral */
        if (SPI[major] == NULL) {
                err = sys_zalloc(sizeof(struct SPI), cast(void**, &SPI[major]));
                if (err != ESUCC) {
                        goto finish;
                }

                err = sys_semaphore_create(1, 0, &SPI[major]->wait_irq_sem);
                if (err != ESUCC) {
                        goto finish;
                }

                err = sys_mutex_create(MUTEX_TYPE_RECURSIVE, &SPI[major]->periph_protect_mtx);
                if (err != ESUCC) {
                        goto finish;
                }

                err = turn_on_SPI(major);
                if (err != ESUCC) {
                        goto finish;
                }
        }

        /* create SPI slave instance */
        err = sys_zalloc(sizeof(struct SPI_slave), device_handle);
        if (err == ESUCC) {
                struct SPI_slave *hdl = *device_handle;
                hdl->config           = SPI_DEFAULT_CFG;
                hdl->major            = major;
                hdl->minor            = minor;

                sys_device_unlock(&hdl->lock, true);

                SPI[major]->slave_count++;
        }

        finish:
        if (err != ESUCC) {
                release_resources(major);
        }

        return err;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_RELEASE(SPI, void *device_handle)
{
        struct SPI_slave *hdl = device_handle;

        int err = sys_device_lock(&hdl->lock);
        if (!err) {
                SPI[hdl->major]->slave_count--;
                release_resources(hdl->major);
                sys_free(device_handle);
        }

        return err;
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_OPEN(SPI, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(flags);

        struct SPI_slave *hdl = device_handle;

        return sys_device_lock(&hdl->lock);
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_CLOSE(SPI, void *device_handle, bool force)
{
        struct SPI_slave *hdl = device_handle;

        return sys_device_unlock(&hdl->lock, force);
}

//==============================================================================
/**
 * @brief Write data to device. Function does not check that current task is
 *        allowed to write. This is filtered by the open() function. This design
 *        helps to handle other modules that uses directly files to transfer
 *        data. This allows modules to access file from other tasks
 *        (application tasks that not opened SPI file).
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_WRITE(SPI,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG2(fpos, fattr);

        struct SPI_slave *hdl = device_handle;

        int err = sys_mutex_lock(SPI[hdl->major]->periph_protect_mtx, MUTEX_TIMOUT);
        if (err == ESUCC) {
                if (SPI[hdl->major]->RAW == false) {
                        deselect_slave(hdl);
                        apply_SPI_config(hdl);
                        select_slave(hdl);
                }

                err = transceive(hdl, src, NULL, count);
                if (err == ESUCC) {
                        *wrcnt = count;
                }

                sys_mutex_unlock(SPI[hdl->major]->periph_protect_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_READ(SPI,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG2(fpos, fattr);

        struct SPI_slave *hdl = device_handle;

        int err = sys_mutex_lock(SPI[hdl->major]->periph_protect_mtx, MUTEX_TIMOUT);
        if (err == ESUCC) {
                if (SPI[hdl->major]->RAW == false) {
                        deselect_slave(hdl);
                        apply_SPI_config(hdl);
                        select_slave(hdl);
                }

                err = transceive(hdl, NULL, dst, count);
                if (err == ESUCC) {
                        *rdcnt = count;
                }

                sys_mutex_unlock(SPI[hdl->major]->periph_protect_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief IO control.  Function does not check that current task is
 *        allowed to write. This is filtered by the open() function. This design
 *        helps to handle other modules that uses directly files to transfer
 *        data. This allows modules to access file from other tasks
 *        (application tasks that not opened SPI file).
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_IOCTL(SPI, void *device_handle, int request, void *arg)
{
        struct SPI_slave *hdl = device_handle;
        int               err = EIO;

        switch (request) {
        case IOCTL_SPI__SET_CONFIGURATION:
                if (arg) {
                        hdl->config = *cast(SPI_config_t*, arg);
                        err = ESUCC;
                } else {
                        err = EINVAL;
                }
                break;

        case IOCTL_SPI__GET_CONFIGURATION:
                if (arg) {
                        *cast(SPI_config_t*, arg) = hdl->config;
                        err = ESUCC;
                } else {
                        err = EINVAL;
                }
                break;

        case IOCTL_SPI__SELECT:
                if (sys_mutex_trylock(SPI[hdl->major]->periph_protect_mtx) == ESUCC) {
                        SPI[hdl->major]->RAW = true;
                        apply_SPI_config(hdl);
                        deselect_slave(hdl);
                        select_slave(hdl);
                        err = ESUCC;
                } else {
                        err = EBUSY;
                }
                break;

        case IOCTL_SPI__DESELECT:
                if (sys_mutex_trylock(SPI[hdl->major]->periph_protect_mtx) == ESUCC) {
                        deselect_slave(hdl);
                        apply_SPI_safe_config(hdl->major);
                        SPI[hdl->major]->RAW = false;
                        sys_mutex_unlock(SPI[hdl->major]->periph_protect_mtx); // DESELECT unlock
                        sys_mutex_unlock(SPI[hdl->major]->periph_protect_mtx); // SELECT unlock
                        err = ESUCC;
                } else {
                        err = EBUSY;
                }
                break;

        case IOCTL_SPI__TRANSCEIVE:
                if (arg) {
                        SPI_transceive_t *tr = cast(SPI_transceive_t*, arg);
                        if (tr->count) {
                                if (sys_mutex_trylock(SPI[hdl->major]->periph_protect_mtx) == ESUCC) {
                                        if (SPI[hdl->major]->RAW == false) {
                                                deselect_slave(hdl);
                                                apply_SPI_config(hdl);
                                                select_slave(hdl);
                                        }

                                        err = transceive(hdl, tr->tx_buffer, tr->rx_buffer, tr->count);

                                        sys_mutex_unlock(SPI[hdl->major]->periph_protect_mtx);
                                }
                        } else {
                                err = EINVAL;
                        }
                } else {
                        err = EINVAL;
                }
                break;

        case IOCTL_SPI__TRANSMIT_NO_SELECT:
                if (arg) {
                        const u8_t *byte = arg;

                        if (sys_mutex_trylock(SPI[hdl->major]->periph_protect_mtx) == ESUCC) {
                                deselect_slave(hdl);
                                apply_SPI_config(hdl);

                                if (transceive(hdl, byte, NULL, 1)) {
                                        err = ESUCC;
                                }

                                sys_mutex_unlock(SPI[hdl->major]->periph_protect_mtx);
                        }
                } else {
                        err = EINVAL;
                }
                break;

        default:
                err = EBADRQC;
                break;
        }

        return err;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_FLUSH(SPI, void *device_handle)
{
        UNUSED_ARG1(device_handle);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_STAT(SPI, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_size = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Release allocated resources
 * @param  major        SPI major number
 * @return None
 */
//==============================================================================
static void release_resources(u8_t major)
{
        if (SPI[major] && SPI[major]->slave_count == 0) {
                if (SPI[major]->wait_irq_sem) {
                        sys_semaphore_destroy(SPI[major]->wait_irq_sem);
                        SPI[major]->wait_irq_sem = NULL;
                }

                if (SPI[major]->periph_protect_mtx) {
                        sys_mutex_destroy(SPI[major]->periph_protect_mtx);
                        SPI[major]->periph_protect_mtx = NULL;
                }

                turn_off_SPI(major);

                sys_free(cast(void**, &SPI[major]));
        }
}

//==============================================================================
/**
 * @brief Function enable SPI interface
 * @param[in] major     SPI major number
 * @return One of errno value.
 */
//==============================================================================
static int turn_on_SPI(u8_t major)
{
        if (!(*SPI_INFO[major].APBENR & SPI_INFO[major].APBRSTRENR)) {

                SET_BIT(*SPI_INFO[major].APBRSTR, SPI_INFO[major].APBRSTRENR);
                CLEAR_BIT(*SPI_INFO[major].APBRSTR, SPI_INFO[major].APBRSTRENR);
                SET_BIT(*SPI_INFO[major].APBENR, SPI_INFO[major].APBRSTRENR);

                if (SPI_INFO[major].DMA) {
                        #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
                        SET_BIT(RCC->AHBENR, SPI_INFO[major].DMAEN);

                        u32_t DMA_IRQ_mask = (DMA_ISR_GIF1 | DMA_ISR_TCIF1 | DMA_ISR_TEIF1);
                        u8_t  ch_position  = (SPI_INFO[major].DMA_Rx_channel_number - 1) * 4;
                        SPI_INFO[major].DMA->IFCR = (DMA_IRQ_mask << ch_position);
                        SPI_INFO[major].DMA->ISR |= (DMA_IRQ_mask << ch_position);

                        NVIC_EnableIRQ(SPI_INFO[major].DMA_Rx_IRQn);
                        NVIC_SetPriority(SPI_INFO[major].DMA_Rx_IRQn, _CPU_IRQ_SAFE_PRIORITY_);
                        #endif
                } else {
                        #if (_SPI1_USE_DMA == 0) || (_SPI2_USE_DMA == 0) || (_SPI3_USE_DMA == 0)
                        NVIC_EnableIRQ(SPI_INFO[major].IRQn);
                        NVIC_SetPriority(SPI_INFO[major].IRQn, _CPU_IRQ_SAFE_PRIORITY_);
                        #endif
                }

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
static void turn_off_SPI(u8_t major)
{
        if (*SPI_INFO[major].APBENR & SPI_INFO[major].APBRSTRENR) {
                SET_BIT(*SPI_INFO[major].APBRSTR, SPI_INFO[major].APBRSTRENR);
                CLEAR_BIT(*SPI_INFO[major].APBRSTR, SPI_INFO[major].APBRSTRENR);
                CLEAR_BIT(*SPI_INFO[major].APBENR, SPI_INFO[major].APBRSTRENR);

                if (SPI_INFO[major].DMA) {
                        #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
                        NVIC_DisableIRQ(SPI_INFO[major].DMA_Rx_IRQn);
                        #endif
                } else {
                        #if (_SPI1_USE_DMA == 0) || (_SPI2_USE_DMA == 0) || (_SPI3_USE_DMA == 0)
                        NVIC_DisableIRQ(SPI_INFO[major].IRQn);
                        #endif
                }
        }
}

//==============================================================================
/**
 * @brief Function apply new configuration for selected SPI
 *
 * @param hdl           SPI slave
 */
//==============================================================================
static void apply_SPI_config(struct SPI_slave *hdl)
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

        SPI_t *SPI = SPI_INFO[hdl->major].SPI;

        /* clear register */
        WRITE_REG(SPI->CR1, 0);

        /* configure SPI divider */
        SET_BIT(SPI->CR1, divider_mask[hdl->config.clk_divider]);

        /* configure SPI mode */
        SET_BIT(SPI->CR1, spi_mode_mask[hdl->config.mode]);

        /* 8-bit mode */
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
static void apply_SPI_safe_config(u8_t major)
{
        SPI_t *SPI = SPI_INFO[major].SPI;

        while (SPI->SR & SPI_SR_BSY);

        CLEAR_BIT(SPI->CR1, SPI_CR1_SPE);
        SET_BIT(SPI->CR1, SPI_CR1_MSTR);
}

//==============================================================================
/**
 * @brief Function select slave device
 *
 * @param hdl           SPI slave
 */
//==============================================================================
static void select_slave(struct SPI_slave *hdl)
{
        _GPIO_DDI_clear_pin(hdl->config.CS_port_idx, hdl->config.CS_pin_idx);
}

//==============================================================================
/**
 * @brief Function deselect current slave device
 *
 * @param hdl           SPI slave
 */
//==============================================================================
static void deselect_slave(struct SPI_slave *hdl)
{
        _GPIO_DDI_set_pin(hdl->config.CS_port_idx, hdl->config.CS_pin_idx);
}

//==============================================================================
/**
 * @brief  Transceive data by using IRQs or DMA
 * @param  hdl          virtual SPI handler
 * @param  tx           source buffer
 * @param  rx           destination buffer
 * @param  count        number of bytes to transfer
 * @return One of errno value
 */
//==============================================================================
static int transceive(struct SPI_slave *hdl, const u8_t *tx, u8_t *rx, size_t count)
{
        SPI[hdl->major]->slave = hdl;

        if (SPI_INFO[hdl->major].DMA) {
                #if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
                SPI[hdl->major]->flush_byte = hdl->config.flush_byte;
                SPI[hdl->major]->count      = count;

                SPI_INFO[hdl->major].DMA_Tx_channel->CPAR  = cast(u32_t, &SPI_INFO[hdl->major].SPI->DR);
                SPI_INFO[hdl->major].DMA_Tx_channel->CMAR  = tx ? cast(u32_t, tx) : cast(u32_t, &SPI[hdl->major]->flush_byte);
                SPI_INFO[hdl->major].DMA_Tx_channel->CNDTR = count;
                SPI_INFO[hdl->major].DMA_Tx_channel->CCR   = (tx ? DMA_CCR1_MINC : 0) | DMA_CCR1_DIR;

                SPI_INFO[hdl->major].DMA_Rx_channel->CPAR  = cast(u32_t, &SPI_INFO[hdl->major].SPI->DR);
                SPI_INFO[hdl->major].DMA_Rx_channel->CMAR  = rx ? cast(u32_t, rx) : cast(u32_t, &SPI[hdl->major]->flush_byte);
                SPI_INFO[hdl->major].DMA_Rx_channel->CNDTR = count;
                SPI_INFO[hdl->major].DMA_Rx_channel->CCR   = (rx ? DMA_CCR1_MINC : 0) | DMA_CCR1_TEIE | DMA_CCR1_TCIE;

                SET_BIT(SPI_INFO[hdl->major].SPI->CR2, SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);
                SET_BIT(SPI_INFO[hdl->major].DMA_Rx_channel->CCR, DMA_CCR1_EN);
                SET_BIT(SPI_INFO[hdl->major].DMA_Tx_channel->CCR, DMA_CCR1_EN);
                #endif
        } else {
                #if (_SPI1_USE_DMA == 0) || (_SPI2_USE_DMA == 0) || (_SPI3_USE_DMA == 0)
                SPI[hdl->major]->tx_buffer  = tx;
                SPI[hdl->major]->rx_buffer  = rx;
                SPI[hdl->major]->count      = count;
                SPI[hdl->major]->flush_byte = hdl->config.flush_byte;

                SET_BIT(SPI_INFO[hdl->major].SPI->CR2, SPI_CR2_TXEIE);
                #endif
        }

        return sys_semaphore_wait(SPI[hdl->major]->wait_irq_sem, SEMAPHORE_TIMEOUT);
}

//==============================================================================
/**
 * @brief  Function handle SPI IRQ
 * @param  major        SPI device number
 * @return If task was woken then true is returned, otherwise false
 */
//==============================================================================
#if (_SPI1_USE_DMA == 0) || (_SPI2_USE_DMA == 0) || (_SPI3_USE_DMA == 0)
static bool handle_SPI_IRQ(u8_t major)
{
        bool   woken = false;
        SPI_t *spi   = SPI_INFO[major].SPI;

        /* receive data from RX register */
        void receive()
        {
                if ((spi->SR & SPI_SR_RXNE) && (spi->CR2 & SPI_CR2_RXNEIE)) {
                        u8_t byte = spi->DR;

                        if (SPI[major]->count > 0) {
                                if (SPI[major]->rx_buffer) {
                                        *(SPI[major]->rx_buffer++) = byte;
                                }

                                SPI[major]->count--;
                        }

                        SET_BIT(spi->CR2, SPI_CR2_TXEIE);
                }
        }

        /* transmit data by using Tx register */
        void transmit()
        {
                if ((spi->SR & SPI_SR_TXE) && (spi->CR2 & SPI_CR2_TXEIE)) {

                        if (SPI[major]->count > 0) {
                                if (SPI[major]->tx_buffer) {
                                        spi->DR = *(SPI[major]->tx_buffer++);
                                } else {
                                        spi->DR = SPI[major]->flush_byte;
                                }
                        }

                        SET_BIT(spi->CR2, SPI_CR2_RXNEIE);
                        CLEAR_BIT(spi->CR2, SPI_CR2_TXEIE);
                }
        }

        /* finish transmission if all frames are received and transmitted */
        void check_finish()
        {
                if (SPI[major]->count == 0) {
                        if (SPI[major]->RAW == false) {
                                deselect_slave(SPI[major]->slave);
                        }

                        CLEAR_BIT(spi->CR2, SPI_CR2_RXNEIE);
                        CLEAR_BIT(spi->CR2, SPI_CR2_TXEIE);
                        sys_semaphore_signal_from_ISR(SPI[major]->wait_irq_sem, &woken);
                }
        }

        /* IRQ operations */
        transmit();
        receive();
        check_finish();

        return woken;
}
#endif

//==============================================================================
/**
 * @brief  DMA IRQ handler
 * @param  major        SPI major number
 * @return If task was woken then true is returned, otherwise false
 */
//==============================================================================
#if  (_SPI1_USE_DMA > 0) || (_SPI2_USE_DMA > 0) || (_SPI3_USE_DMA > 0)
static bool handle_DMA_IRQ(u8_t major)
{
        if (SPI[major]->RAW == false) {
                deselect_slave(SPI[major]->slave);
        }

        u32_t DMA_IRQ_mask        = (DMA_ISR_GIF1 | DMA_ISR_TCIF1 | DMA_ISR_TEIF1);
        u8_t  ch_position         = (SPI_INFO[major].DMA_Rx_channel_number - 1) * 4;
        SPI_INFO[major].DMA->IFCR = (DMA_IRQ_mask << ch_position);

        CLEAR_BIT(SPI_INFO[major].DMA_Rx_channel->CCR, DMA_CCR1_EN);
        CLEAR_BIT(SPI_INFO[major].DMA_Tx_channel->CCR, DMA_CCR1_EN);
        CLEAR_BIT(SPI_INFO[major].SPI->CR2, SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);

        bool woken = false;
        sys_semaphore_signal_from_ISR(SPI[major]->wait_irq_sem, &woken);
        woken = woken || SPI[major]->count > 10;

        return woken;
}
#endif

//==============================================================================
/**
 * @brief SPI1 IRQ handler
 */
//==============================================================================
#if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_USE_DMA == 0)
void SPI1_IRQHandler(void)
{
        sys_thread_yield_from_ISR(handle_SPI_IRQ(_SPI1));
}
#endif

//==============================================================================
/**
 * @brief SPI2 IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_USE_DMA == 0)
void SPI2_IRQHandler(void)
{
        sys_thread_yield_from_ISR(handle_SPI_IRQ(_SPI2));
}
#endif

//==============================================================================
/**
 * @brief SPI3 IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_USE_DMA == 0)
void SPI3_IRQHandler(void)
{
        sys_thread_yield_from_ISR(handle_SPI_IRQ(_SPI3));
}
#endif

//==============================================================================
/**
 * @brief DMA Rx transfer complete IRQ for SPI1
 */
//==============================================================================
#if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_USE_DMA > 0)
void DMA1_Channel2_IRQHandler(void)
{
        sys_thread_yield_from_ISR(handle_DMA_IRQ(_SPI1));
}
#endif

//==============================================================================
/**
 * @brief DMA Rx transfer complete IRQ for SPI2
 */
//==============================================================================
#if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_USE_DMA > 0)
void DMA1_Channel4_IRQHandler(void)
{
        sys_thread_yield_from_ISR(handle_DMA_IRQ(_SPI2));
}
#endif

//==============================================================================
/**
 * @brief DMA Rx transfer complete IRQ for SPI3
 */
//==============================================================================
#if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_USE_DMA > 0)
void DMA2_Channel1_IRQHandler(void)
{
        sys_thread_yield_from_ISR(handle_DMA_IRQ(_SPI3));
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
