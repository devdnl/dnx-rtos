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

/*==============================================================================
  Include files
==============================================================================*/
#include "core/module.h"
#include <dnx/thread.h>
#include <dnx/misc.h>
#include "stm32f1/spi_cfg.h"
#include "stm32f1/spi_def.h"
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define MUTEX_TIMOUT                    MAX_DELAY_MS
#define SEMAPHORE_TIMEOUT               MAX_DELAY_MS

/* calculate max number of slaves for all SPI peripherals (calculate array size) */
#if (_SPI1_NUMBER_OF_SLAVES > _SPI2_NUMBER_OF_SLAVES)
#define _SPI_NUMBER_OF_SLAVES_A         _SPI1_NUMBER_OF_SLAVES
#else
#define _SPI_NUMBER_OF_SLAVES_A         _SPI2_NUMBER_OF_SLAVES
#endif

#if (_SPI2_NUMBER_OF_SLAVES > _SPI3_NUMBER_OF_SLAVES)
#define _SPI_NUMBER_OF_SLAVES_B         _SPI2_NUMBER_OF_SLAVES
#else
#define _SPI_NUMBER_OF_SLAVES_B         _SPI3_NUMBER_OF_SLAVES
#endif

#if (_SPI_NUMBER_OF_SLAVES_A > _SPI_NUMBER_OF_SLAVES_B)
#define MAX_NUMBER_OF_CS                _SPI_NUMBER_OF_SLAVES_A
#else
#define MAX_NUMBER_OF_CS                _SPI_NUMBER_OF_SLAVES_B
#endif

#if (MAX_NUMBER_OF_CS > 8)
#error SPI module: set up to 8 slaves per device!
#elif (MAX_NUMBER_OF_CS == 0)
#error SPI module: set more than 0 slaves per device!
#endif

/*==============================================================================
  Local object types
==============================================================================*/
MODULE_NAME("SPI");

/* configuration of single CS line (port and pin) */
struct cs_pin_cfg {
        GPIO_t *const           port;
        u16_t                   pin_mask;
};

/* priority configuration */
struct spi_priority_cfg {
        IRQn_Type               IRQn;
        u32_t                   priority;
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
        sem_t                  *wait_irq_sem[_SPI_NUMBER];
        mutex_t                *device_protect_mtx[_SPI_NUMBER];
        u8_t                   *buffer[_SPI_NUMBER];
        size_t                  count[_SPI_NUMBER];
        bool                    write[_SPI_NUMBER];
        u8_t                    dummy_byte[_SPI_NUMBER];
        u8_t                    number_of_virtual_spi[_SPI_NUMBER];
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t spi_turn_on             (SPI_t *spi);
static void     spi_turn_off            (SPI_t *spi);
static void     config_apply            (struct spi_virtual *vspi);
static void     config_apply_safe       (u8_t major);
static void     slave_select            (u8_t major, u8_t minor);
static void     slave_deselect          (u8_t major);
static void     handle_irq              (u8_t major);

/*==============================================================================
  Local objects
==============================================================================*/
/* SPI peripherals address */
static SPI_t *const spi[_SPI_NUMBER] = {
        #if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        SPI1,
        #endif
        #if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        SPI2,
        #endif
        #if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        SPI3
        #endif
};

/* CS port configuration */
static const struct cs_pin_cfg spi_cs_pin_cfg[_SPI_NUMBER][MAX_NUMBER_OF_CS] = {
        #if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        {
                #if (MAX_NUMBER_OF_CS >= 1)
                {.port = (GPIO_t *)_SPI1_CS0_PORT, .pin_mask = _SPI1_CS0_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 2)
                {.port = (GPIO_t *)_SPI1_CS1_PORT, .pin_mask = _SPI1_CS1_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 3)
                {.port = (GPIO_t *)_SPI1_CS2_PORT, .pin_mask = _SPI1_CS2_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 4)
                {.port = (GPIO_t *)_SPI1_CS3_PORT, .pin_mask = _SPI1_CS3_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 5)
                {.port = (GPIO_t *)_SPI1_CS4_PORT, .pin_mask = _SPI1_CS4_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 6)
                {.port = (GPIO_t *)_SPI1_CS5_PORT, .pin_mask = _SPI1_CS5_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 7)
                {.port = (GPIO_t *)_SPI1_CS6_PORT, .pin_mask = _SPI1_CS6_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 8)
                {.port = (GPIO_t *)_SPI1_CS7_PORT, .pin_mask = _SPI1_CS7_PIN_BM}
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        {
                #if (MAX_NUMBER_OF_CS >= 1)
                {.port = (GPIO_t *)_SPI2_CS0_PORT, .pin_mask = _SPI2_CS0_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 2)
                {.port = (GPIO_t *)_SPI2_CS1_PORT, .pin_mask = _SPI2_CS1_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 3)
                {.port = (GPIO_t *)_SPI2_CS2_PORT, .pin_mask = _SPI2_CS2_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 4)
                {.port = (GPIO_t *)_SPI2_CS3_PORT, .pin_mask = _SPI2_CS3_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 5)
                {.port = (GPIO_t *)_SPI2_CS4_PORT, .pin_mask = _SPI2_CS4_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 6)
                {.port = (GPIO_t *)_SPI2_CS5_PORT, .pin_mask = _SPI2_CS5_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 7)
                {.port = (GPIO_t *)_SPI2_CS6_PORT, .pin_mask = _SPI2_CS6_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 8)
                {.port = (GPIO_t *)_SPI2_CS7_PORT, .pin_mask = _SPI2_CS7_PIN_BM}
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        {
                #if (MAX_NUMBER_OF_CS >= 1)
                {.port = (GPIO_t *)_SPI3_CS0_PORT, .pin_mask = _SPI3_CS0_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 2)
                {.port = (GPIO_t *)_SPI3_CS1_PORT, .pin_mask = _SPI3_CS1_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 3)
                {.port = (GPIO_t *)_SPI3_CS2_PORT, .pin_mask = _SPI3_CS2_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 4)
                {.port = (GPIO_t *)_SPI3_CS3_PORT, .pin_mask = _SPI3_CS3_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 5)
                {.port = (GPIO_t *)_SPI3_CS4_PORT, .pin_mask = _SPI3_CS4_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 6)
                {.port = (GPIO_t *)_SPI3_CS5_PORT, .pin_mask = _SPI3_CS5_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 7)
                {.port = (GPIO_t *)_SPI3_CS6_PORT, .pin_mask = _SPI3_CS6_PIN_BM},
                #endif
                #if (MAX_NUMBER_OF_CS >= 8)
                {.port = (GPIO_t *)_SPI3_CS7_PORT, .pin_mask = _SPI3_CS7_PIN_BM}
                #endif
        }
        #endif
};

/* number of SPI cs */
static const u8_t spi_number_of_slaves[_SPI_NUMBER] = {
        #if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        _SPI1_NUMBER_OF_SLAVES,
        #endif
        #if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        _SPI2_NUMBER_OF_SLAVES,
        #endif
        #if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        _SPI3_NUMBER_OF_SLAVES
        #endif
};

/* IRQ configurations */
static const struct spi_priority_cfg spi_irq[_SPI_NUMBER] = {
        #if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        {.IRQn = SPI1_IRQn, .priority = _SPI1_IRQ_PRIORITY},
        #endif
        #if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        {.IRQn = SPI2_IRQn, .priority = _SPI2_IRQ_PRIORITY},
        #endif
        #if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        {.IRQn = SPI3_IRQn, .priority = _SPI3_IRQ_PRIORITY}
        #endif
};

/* default SPI config */
static const struct SPI_config spi_default_cfg = {
        .dummy_byte  = _SPI_DEFAULT_CFG_DUMMY_BYTE,
        .clk_divider = _SPI_DEFAULT_CFG_CLK_DIVIDER,
        .mode        = _SPI_DEFAULT_CFG_MODE,
        .msb_first   = _SPI_DEFAULT_CFG_MSB_FIRST
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
        if (major >= _SPI_NUMBER) {
                errno = ENXIO;
                return STD_RET_ERROR;
        }

#if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        if (major == _SPI1 && minor >= _SPI1_NUMBER_OF_SLAVES) {
                errno = ENXIO;
                return STD_RET_ERROR;
        }
#endif

#if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        if (major == _SPI2 && minor >= _SPI2_NUMBER_OF_SLAVES) {
                errno = ENXIO;
                return STD_RET_ERROR;
        }
#endif

#if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        if (major == _SPI3 && minor >= _SPI3_NUMBER_OF_SLAVES) {
                errno = ENXIO;
                return STD_RET_ERROR;
        }
#endif

        /* allocate module general data if initialized first time */
        if (!spi_module) {
                spi_module = calloc(1, sizeof(struct module));
                if (!spi_module)
                        return STD_RET_ERROR;
        }

        /* create irq semaphore */
        if (!spi_module->wait_irq_sem[major]) {
                spi_module->wait_irq_sem[major] = semaphore_new(1, 1);
                if (!spi_module->wait_irq_sem[major]) {
                        return STD_RET_ERROR;
                }
        }

        /* create protection mutex and start device if initialized first time */
        if (!spi_module->device_protect_mtx[major]) {
                spi_module->device_protect_mtx[major] = mutex_new(MUTEX_RECURSIVE);
                if (!spi_module->device_protect_mtx[major]) {
                        semaphore_delete(spi_module->wait_irq_sem[major]);
                        spi_module->wait_irq_sem[major] = NULL;
                        return STD_RET_ERROR;
                } else {
                        if (spi_turn_on(spi[major]) == STD_RET_OK) {
                                NVIC_EnableIRQ(spi_irq[major].IRQn);
                                NVIC_SetPriority(spi_irq[major].IRQn, spi_irq[major].priority);
                        } else {
                                semaphore_delete(spi_module->wait_irq_sem[major]);
                                spi_module->wait_irq_sem[major] = NULL;
                                mutex_delete(spi_module->device_protect_mtx[major]);
                                spi_module->device_protect_mtx[major] = NULL;
                                errno = EADDRINUSE;
                                return STD_RET_ERROR;
                        }
                }
        }

        /* create new instance for specified major-minor number (virtual spi) */
        struct spi_virtual *hdl = calloc(1, sizeof(struct spi_virtual));
        if (!hdl) {
                return STD_RET_ERROR;
        }

        hdl->config    = spi_default_cfg;
        hdl->major     = major;
        hdl->minor     = minor;
        *device_handle = hdl;

        spi_module->number_of_virtual_spi[major]++;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_RELEASE(SPI, void *device_handle)
{
        struct spi_virtual *hdl = device_handle;

        stdret_t status = STD_RET_ERROR;

        critical_section_begin();
        if (device_is_unlocked(hdl->file_lock)) {

                spi_module->number_of_virtual_spi[hdl->major]--;

                /* deinitialize major device if all minor devices are deinitialized */
                if (spi_module->number_of_virtual_spi == 0) {
                        mutex_delete(spi_module->device_protect_mtx[hdl->major]);
                        spi_module->device_protect_mtx[hdl->major] = NULL;
                        semaphore_delete(spi_module->wait_irq_sem[hdl->major]);
                        spi_module->wait_irq_sem[hdl->major] = NULL;
                        spi_turn_off(spi[hdl->major]);
                }

                /* free module memory if all devices are deinitialized */
                bool free_module_mem = true;
                for (int i = 0; i < _SPI_NUMBER && free_module_mem; i++) {
                        free_module_mem = spi_module->device_protect_mtx[i] == NULL;
                }

                if (free_module_mem) {
                        free(spi_module);
                        spi_module = NULL;
                }

                /* free virtual spi memory */
                free(hdl);

                status = STD_RET_OK;
        } else {
                errno = EBUSY;
        }
        critical_section_end();

        return status;
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_OPEN(SPI, void *device_handle, vfs_open_flags_t flags)
{
        UNUSED_ARG(flags);

        struct spi_virtual *hdl = device_handle;

        return device_lock(&hdl->file_lock) ? STD_RET_OK : STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_CLOSE(SPI, void *device_handle, bool force)
{
        struct spi_virtual *hdl = device_handle;

        if (device_is_access_granted(&hdl->file_lock) || force) {
                device_unlock(&hdl->file_lock, force);
                return STD_RET_OK;
        } else {
                errno = EBUSY;
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Write data to device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[in ]           fattr                  file attributes
 *
 * @return number of written bytes, -1 if error
 */
//==============================================================================
API_MOD_WRITE(SPI, void *device_handle, const u8_t *src, size_t count, u64_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        struct spi_virtual *hdl = device_handle;

        ssize_t n = -1;

        if (device_is_access_granted(&hdl->file_lock)) {
                if (mutex_lock(spi_module->device_protect_mtx[hdl->major], MUTEX_TIMOUT)) {
                        config_apply(hdl);
                        slave_deselect(hdl->major);
                        slave_select(hdl->major, hdl->minor);

                        spi_module->buffer[hdl->major] = (u8_t *)src;
                        spi_module->count[hdl->major]  = count;
                        spi_module->write[hdl->major]  = true;

                        SET_BIT(spi[hdl->major]->CR2, SPI_CR2_TXEIE);
                        semaphore_wait(spi_module->wait_irq_sem[hdl->major], SEMAPHORE_TIMEOUT);
                        while (spi[hdl->major]->SR & SPI_SR_BSY); /* flush buffer */

                        n = count - spi_module->count[hdl->major];

                        slave_deselect(hdl->major);
                        config_apply_safe(hdl->major);
                        mutex_unlock(spi_module->device_protect_mtx[hdl->major]);
                } else {
                        errno = ETIME;
                }
        } else {
                errno = EACCES;
        }

        return n;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[in ]           fattr                  file attributes
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
API_MOD_READ(SPI, void *device_handle, u8_t *dst, size_t count, u64_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        struct spi_virtual *hdl = device_handle;

        ssize_t n = -1;

        if (device_is_access_granted(&hdl->file_lock)) {
                if (mutex_lock(spi_module->device_protect_mtx[hdl->major], MUTEX_TIMOUT)) {
                        config_apply(hdl);
                        slave_deselect(hdl->major);
                        slave_select(hdl->major, hdl->minor);

                        spi_module->buffer[hdl->major] = dst;
                        spi_module->count[hdl->major]  = count;
                        spi_module->write[hdl->major]  = false;

                        u8_t tmp;
                        while (spi[hdl->major]->SR & SPI_SR_RXNE) {
                                tmp = spi[hdl->major]->DR;
                        }

                        SET_BIT(spi[hdl->major]->CR2, SPI_CR2_RXNEIE);
                        tmp = hdl->config.dummy_byte;
                        spi_module->dummy_byte[hdl->major]= tmp;
                        spi[hdl->major]->DR = tmp;
                        semaphore_wait(spi_module->wait_irq_sem[hdl->major], SEMAPHORE_TIMEOUT);
                        while (spi[hdl->major]->SR & SPI_SR_BSY); /* flush buffer */

                        n = count - spi_module->count[hdl->major];

                        slave_deselect(hdl->major);
                        config_apply_safe(hdl->major);
                        mutex_unlock(spi_module->device_protect_mtx[hdl->major]);
                } else {
                        errno = ETIME;
                }
        } else {
                errno = EACCES;
        }

        return n;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_IOCTL(SPI, void *device_handle, int request, void *arg)
{
        struct spi_virtual *hdl    = device_handle;
        stdret_t            status = STD_RET_ERROR;

        if (device_is_access_granted(&hdl->file_lock)) {
                switch (request) {
                case IOCTL_SPI__SET_CONFIGURATION:
                        if (arg) {
                                hdl->config = *(struct SPI_config *)arg;
                                status      = STD_RET_OK;
                        } else {
                                errno = EINVAL;
                        }
                        break;

                case IOCTL_SPI__GET_CONFIGURATION:
                        if (arg) {
                                *(struct SPI_config *)arg = hdl->config;
                                status                    = STD_RET_OK;
                        } else {
                                errno = EINVAL;
                        }
                        break;

                case IOCTL_SPI__LOCK:
                        if (mutex_lock(spi_module->device_protect_mtx[hdl->major], 0)) {
                                status = STD_RET_OK;
                        } else {
                                errno = EBUSY;
                        }
                        break;

                case IOCTL_SPI__UNLOCK:
                        if (mutex_unlock(spi_module->device_protect_mtx[hdl->major])) {
                                status = STD_RET_OK;
                        } else {
                                errno = EBUSY;
                        }
                        break;

                case IOCTL_SPI__SELECT:
                        if (mutex_lock(spi_module->device_protect_mtx[hdl->major], 0)) {
                                slave_select(hdl->major, hdl->minor);
                                mutex_unlock(spi_module->device_protect_mtx[hdl->major]);
                                status = STD_RET_OK;
                        } else {
                                errno = EBUSY;
                        }
                        break;

                case IOCTL_SPI__DESELECT:
                        if (mutex_lock(spi_module->device_protect_mtx[hdl->major], 0)) {
                                slave_deselect(hdl->major);
                                mutex_unlock(spi_module->device_protect_mtx[hdl->major]);
                                status = STD_RET_OK;
                        } else {
                                errno = EBUSY;
                        }
                        break;

                case IOCTL_SPI__TRANSMIT:
                        if (arg) {
                                if (mutex_lock(spi_module->device_protect_mtx[hdl->major], 0)) {
                                        SPI_t *SPI = spi[hdl->major];

                                        while (SPI->SR & SPI_SR_RXNE) {
                                                u16_t tmp = SPI->DR;
                                                (void)tmp;
                                        }

                                        while (!(SPI->SR & SPI_SR_TXE));
                                        SPI->DR = *(u8_t *)arg;

                                        while (!(SPI->SR & SPI_SR_RXNE));
                                        *(u8_t *)arg = SPI->DR;

                                        mutex_unlock(spi_module->device_protect_mtx[hdl->major]);
                                        status = STD_RET_OK;
                                }
                        } else {
                                errno = EINVAL;
                        }
                        break;

                default:
                        errno = EBADRQC;
                        break;
                }
        } else {
                errno = EACCES;
        }

        return status;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_FLUSH(SPI, void *device_handle)
{
        UNUSED_ARG(device_handle);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_STAT(SPI, void *device_handle, struct vfs_dev_stat *device_stat)
{
        struct spi_virtual *hdl = device_handle;

        device_stat->st_major = hdl->major;
        device_stat->st_minor = hdl->minor;
        device_stat->st_size  = 0;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function enable SPI interface
 *
 * @param[in] *spi      spi peripheral
 *
 * @return STD_RET_OK, STD_RET_ERROR
 */
//==============================================================================
static stdret_t spi_turn_on(SPI_t *spi)
{
        switch ((uint32_t)spi) {
        #if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        case SPI1_BASE:
                if (!(RCC->APB2ENR & RCC_APB2ENR_SPI1EN)) {
                        RCC->APB2RSTR |=  RCC_APB2RSTR_SPI1RST;
                        RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
                        RCC->APB2ENR  |=  RCC_APB2ENR_SPI1EN;
                        return STD_RET_OK;
                } else {
                        return STD_RET_ERROR;
                }
                break;
        #endif
        #if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        case SPI2_BASE:
                if (!(RCC->APB1ENR & RCC_APB1ENR_SPI2EN)) {
                        RCC->APB1RSTR |=  RCC_APB1RSTR_SPI2RST;
                        RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;
                        RCC->APB1ENR  |=  RCC_APB1ENR_SPI2EN;
                        return STD_RET_OK;
                } else {
                        return STD_RET_ERROR;
                }
                break;
        #endif
        #if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        case SPI3_BASE:
                if (!(RCC->APB1ENR & RCC_APB1ENR_SPI3EN)) {
                        RCC->APB1RSTR |=  RCC_APB1RSTR_SPI3RST;
                        RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI3RST;
                        RCC->APB1ENR  |=  RCC_APB1ENR_SPI3EN;
                        return STD_RET_OK;
                } else {
                        return STD_RET_ERROR;
                }
                break;
        #endif
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function disable SPI interface
 *
 * @param[in] *spi      spi peripheral
 */
//==============================================================================
static void spi_turn_off(SPI_t *spi)
{
        switch ((uint32_t)spi) {
        #if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        case SPI1_BASE:
                RCC->APB2RSTR |=  RCC_APB2RSTR_SPI1RST;
                RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
                RCC->APB2ENR  &= ~RCC_APB2ENR_SPI1EN;
                break;
        #endif
        #if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        case SPI2_BASE:
                RCC->APB1RSTR |=  RCC_APB1RSTR_SPI2RST;
                RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;
                RCC->APB1ENR  &= ~RCC_APB1ENR_SPI2EN;
                break;
        #endif
        #if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        case SPI3_BASE:
                RCC->APB1RSTR |=  RCC_APB1RSTR_SPI3RST;
                RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI3RST;
                RCC->APB1ENR  &= ~RCC_APB1ENR_SPI3EN;
                break;
        #endif
        }
}

//==============================================================================
/**
 * @brief Function apply new configuration for selected SPI
 *
 * @param vspi          virtual spi handler
 */
//==============================================================================
static void config_apply(struct spi_virtual *vspi)
{
        const u16_t divider_mask[SPI_CLK_DIV_256 + 1] = {
                [SPI_CLK_DIV_2  ] 0x00,
                [SPI_CLK_DIV_4  ] SPI_CR1_BR_0,
                [SPI_CLK_DIV_8  ] SPI_CR1_BR_1,
                [SPI_CLK_DIV_16 ] SPI_CR1_BR_1 | SPI_CR1_BR_0,
                [SPI_CLK_DIV_32 ] SPI_CR1_BR_2,
                [SPI_CLK_DIV_64 ] SPI_CR1_BR_2 | SPI_CR1_BR_0,
                [SPI_CLK_DIV_128] SPI_CR1_BR_2 | SPI_CR1_BR_1,
                [SPI_CLK_DIV_256] SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0,
        };

        const u16_t spi_mode_mask[SPI_MODE_3 + 1] = {
                [SPI_MODE_0] 0x00,
                [SPI_MODE_1] SPI_CR1_CPHA,
                [SPI_MODE_2] SPI_CR1_CPOL,
                [SPI_MODE_3] SPI_CR1_CPOL | SPI_CR1_CPHA
        };

        SPI_t *SPI = spi[vspi->major];

        /* configure SPI divider */
        CLEAR_BIT(SPI->CR1, SPI_CR1_BR);
        SET_BIT(SPI->CR1, divider_mask[vspi->config.clk_divider]);

        /* configure SPI mode */
        CLEAR_BIT(SPI->CR1, SPI_CR1_CPOL | SPI_CR1_CPHA);
        SET_BIT(SPI->CR1, spi_mode_mask[vspi->config.mode]);

        /* 8-bit mode */
        CLEAR_BIT(SPI->CR1, SPI_CR1_DFF);

        /* set MSB/LSB */
        if (vspi->config.msb_first)
                CLEAR_BIT(SPI->CR1, SPI_CR1_LSBFIRST);
        else
                SET_BIT(SPI->CR1, SPI_CR1_LSBFIRST);

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
static void config_apply_safe(u8_t major)
{
        SPI_t *SPI = spi[major];

        while (SPI->SR & SPI_SR_BSY)
        CLEAR_BIT(SPI->CR1, SPI_CR1_SPE);
        SET_BIT(SPI->CR1, SPI_CR1_MSTR);
}

//==============================================================================
/**
 * @brief Function select slave device
 *
 * @param major         major device number (SPI device)
 * @param minor         minor device number (CS number)
 */
//==============================================================================
static void slave_select(u8_t major, u8_t minor)
{
        GPIO_t *GPIO = spi_cs_pin_cfg[major][minor].port;
        u16_t   mask = spi_cs_pin_cfg[major][minor].pin_mask;

        GPIO->BRR = mask;
}

//==============================================================================
/**
 * @brief Function deselect current slave device
 *
 * @param major         major device number (SPI device)
 * @param minor         minor device number (CS number)
 */
//==============================================================================
static void slave_deselect(u8_t major)
{
        for (int minor = 0; minor < spi_number_of_slaves[major]; minor++) {
                GPIO_t *GPIO = spi_cs_pin_cfg[major][minor].port;
                u16_t   mask = spi_cs_pin_cfg[major][minor].pin_mask;
                GPIO->BSRR   = mask;
        }
}

//==============================================================================
/**
 * @brief Function handle SPI IRQ
 *
 * @param major        spi device number
 */
//==============================================================================
static void handle_irq(u8_t major)
{
        SPI_t *SPI = spi[major];

        if (spi_module->write[major]) {
                if ((SPI->SR & SPI_SR_TXE) && (SPI->CR2 & SPI_CR2_TXEIE)) {
                        if (spi_module->count[major] > 0) {
                                SPI->DR = *(spi_module->buffer[major]++);
                                spi_module->count[major]--;
                        } else {
                                CLEAR_BIT(SPI->CR2, SPI_CR2_TXEIE);

                                while (SPI->SR & SPI_SR_RXNE) {
                                        int tmp = SPI->DR;
                                        (void) tmp;
                                }

                                SET_BIT(SPI->CR2, SPI_CR2_RXNEIE);
                        }

                        return;
                }

                /* receive recently sent frame to fast disable selected slave */
                if ((SPI->SR & SPI_SR_RXNE) && (SPI->CR2 & SPI_CR2_RXNEIE)) {
                        CLEAR_BIT(SPI->CR2, SPI_CR2_RXNEIE);
                        slave_deselect(major);
                        semaphore_signal_from_ISR(spi_module->wait_irq_sem[major], NULL);
                }
        } else {
                if (SPI->SR & SPI_SR_RXNE) {
                        spi_module->count[major]--;
                        if (spi_module->count[major] > 0) {
                                *(spi_module->buffer[major]++) = SPI->DR;
                                SPI->DR = spi_module->dummy_byte[major];
                        } else {
                                *(spi_module->buffer[major]++) = SPI->DR;
                                slave_deselect(major);
                                semaphore_signal_from_ISR(spi_module->wait_irq_sem[major], NULL);
                                CLEAR_BIT(SPI->CR2, SPI_CR2_RXNEIE);
                        }
                }
        }
}

//==============================================================================
/**
 * @brief SPI1 IRQ handler
 */
//==============================================================================
#if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
void SPI1_IRQHandler(void)
{
        handle_irq(_SPI1);
}
#endif

//==============================================================================
/**
 * @brief SPI2 IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
void SPI2_IRQHandler(void)
{
        handle_irq(_SPI2);
}
#endif

//==============================================================================
/**
 * @brief SPI3 IRQ handler
 */
//==============================================================================
#if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
void SPI3_IRQHandler(void)
{
        handle_irq(_SPI3);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
