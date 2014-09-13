/*=========================================================================*//**
@file    i2c.c

@author  Daniel Zorychta

@brief   This driver support I2C peripherals.

@note    Copyright (C) 2014  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <dnx/misc.h>
#include <dnx/thread.h>
#include "stm32f1/i2c_cfg.h"
#include "stm32f1/i2c_def.h"
#include "stm32f1/stm32f10x.h"
#include "lib/stm32f10x_rcc.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define I2C_DEV_CFG(_major, _minor)\
{\
        .addr          = _major##_DEV_##_minor##_ADDRESS,\
        .addr10bit     = _major##_DEV_##_minor##_10BIT_ADDR_MODE,\
        .sub_addr_mode = _major##_DEV_##_minor##_SEND_SUB_ADDRESS,\
        .major         = _major,\
        .minor         = _minor\
}

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef enum {
        I2C_SUB_ADDR_MODE__DISABLED,
        I2C_SUB_ADDR_MODE__1_BYTE,
        I2C_SUB_ADDR_MODE__2_BYTES,
        I2C_SUB_ADDR_MODE__3_BYTES
} I2C_sub_addr_mode_t;

typedef struct {
        const u16_t               addr:16;
        const bool                addr10bit:1;
        const I2C_sub_addr_mode_t sub_addr_mode:2;
        const u8_t                major:4;
        const u8_t                minor:4;
} I2C_dev_config_t;

typedef struct {
        const I2C_t            *const I2C;
        const DMA_Channel_t    *const DMA_tx;
        const DMA_Channel_t    *const DMA_rx;
        const u32_t             freq;
        const u32_t             I2C_clken;
        const bool              use_DMA;
        const u8_t              IRQ_prio;
        const IRQn_Type         IRQ_EV_n;
        const IRQn_Type         IRQ_ER_n;
        const u8_t              num_of_devs; // FIXME is needed?
        const I2C_dev_config_t *const devices;
} I2C_config_t;

typedef struct {
        const I2C_dev_config_t *config;
        dev_lock_t              lock;
} I2C_dev_t;

typedef struct {
        struct {
                mutex_t *lock;
                sem_t   *event;
                u8_t    *data;
                size_t   size;
                u8_t     dev_cnt;
                bool     error:1;
                bool     stop:1;
                bool     initialized:1;
        } periph[_I2C_NUMBER_OF_PERIPHERALS];
} I2C_mem_t;


/*==============================================================================
  Local function prototypes
==============================================================================*/
static void release_resources(u8_t major);
static I2C_t *get_I2C(I2C_dev_t *hdl);
static bool enable_I2C(u8_t major);
static void disable_I2C(u8_t major);
static bool I2C_start(I2C_dev_t *hdl);
static bool I2C_send_address(I2C_dev_t *hdl, bool wr);
static bool I2C_send_sub_address(I2C_dev_t *hdl, u32_t addr);
static size_t I2C_transmit(I2C_dev_t *hdl, const u8_t *src, size_t size, bool stop);
static size_t I2C_receive(I2C_dev_t *hdl, u8_t *dst, size_t size);
static void I2C_stop(I2C_dev_t *hdl);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(I2C);

static const uint timeout   = 2000;
static const u8_t header10b = 0xF0;

#if (_I2C1_ENABLE > 0) && (_I2C1_NUMBER_OF_DEVICES > 0)
static const I2C_dev_config_t I2C1_dev_cfg[_I2C1_NUMBER_OF_DEVICES] = {
        #if (_I2C1_NUMBER_OF_DEVICES >= 1)
        I2C_DEV_CFG(_I2C1, 0),
        #endif
        #if (_I2C1_NUMBER_OF_DEVICES >= 2)
        I2C_DEV_CFG(_I2C1, 1),
        #endif
        #if (_I2C1_NUMBER_OF_DEVICES >= 3)
        I2C_DEV_CFG(_I2C1, 2),
        #endif
        #if (_I2C1_NUMBER_OF_DEVICES >= 4)
        I2C_DEV_CFG(_I2C1, 3),
        #endif
        #if (_I2C1_NUMBER_OF_DEVICES >= 5)
        I2C_DEV_CFG(_I2C1, 4),
        #endif
        #if (_I2C1_NUMBER_OF_DEVICES >= 6)
        I2C_DEV_CFG(_I2C1, 5),
        #endif
        #if (_I2C1_NUMBER_OF_DEVICES >= 7)
        I2C_DEV_CFG(_I2C1, 6),
        #endif
        #if (_I2C1_NUMBER_OF_DEVICES >= 8)
        I2C_DEV_CFG(_I2C1, 7),
        #endif
};
#endif

#if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0)
static const I2C_dev_config_t I2C2_dev_cfg[_I2C2_NUMBER_OF_DEVICES] = {
        #if (_I2C2_NUMBER_OF_DEVICES >= 1)
        I2C_DEV_CFG(_I2C2, 0),
        #endif
        #if (_I2C2_NUMBER_OF_DEVICES >= 2)
        I2C_DEV_CFG(_I2C2, 1),
        #endif
        #if (_I2C2_NUMBER_OF_DEVICES >= 3)
        I2C_DEV_CFG(_I2C2, 2),
        #endif
        #if (_I2C2_NUMBER_OF_DEVICES >= 4)
        I2C_DEV_CFG(_I2C2, 3),
        #endif
        #if (_I2C2_NUMBER_OF_DEVICES >= 5)
        I2C_DEV_CFG(_I2C2, 4),
        #endif
        #if (_I2C2_NUMBER_OF_DEVICES >= 6)
        I2C_DEV_CFG(_I2C2, 5),
        #endif
        #if (_I2C2_NUMBER_OF_DEVICES >= 7)
        I2C_DEV_CFG(_I2C2, 6),
        #endif
        #if (_I2C2_NUMBER_OF_DEVICES >= 8)
        I2C_DEV_CFG(_I2C2, 7),
        #endif
};
#endif

static const I2C_config_t I2C_cfg[_I2C_NUMBER_OF_PERIPHERALS] = {
        #if (_I2C1_ENABLE > 0) && (_I2C1_NUMBER_OF_DEVICES > 0)
        {
                .I2C         = I2C1,
                .freq        = _I2C1_FREQUENCY,
                .use_DMA     = _I2C1_USE_DMA,
                .DMA_tx      = DMA1_Channel6,
                .DMA_rx      = DMA1_Channel7,
                .I2C_clken   = RCC_APB1ENR_I2C1EN,
                .IRQ_prio    = _I2C1_IRQ_PRIO,
                .IRQ_EV_n    = I2C1_EV_IRQn,
                .IRQ_ER_n    = I2C1_ER_IRQn,
                .num_of_devs = _I2C1_NUMBER_OF_DEVICES,
                .devices     = I2C1_dev_cfg
        },
        #endif
        #if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0)
        {
                .I2C         = I2C2,
                .freq        = _I2C2_FREQUENCY,
                .use_DMA     = _I2C2_USE_DMA,
                .DMA_tx      = DMA1_Channel4,
                .DMA_rx      = DMA1_Channel5,
                .I2C_clken   = RCC_APB1ENR_I2C2EN,
                .IRQ_prio    = _I2C2_IRQ_PRIO,
                .IRQ_EV_n    = I2C2_EV_IRQn,
                .IRQ_ER_n    = I2C2_ER_IRQn,
                .num_of_devs = _I2C2_NUMBER_OF_DEVICES,
                .devices     = I2C2_dev_cfg
        },
        #endif
};

static I2C_mem_t *I2C;


/*==============================================================================
  Exported object definitions
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
API_MOD_INIT(I2C, void **device_handle, u8_t major, u8_t minor)
{
        if (major >= _I2C_NUMBER_OF_PERIPHERALS) {
                errno = ENXIO;
                return STD_RET_ERROR;
        }

        #if (_I2C1_ENABLE > 0) && (_I2C1_NUMBER_OF_DEVICES > 0)
        if (major == _I2C1 && minor >= _I2C1_NUMBER_OF_DEVICES) {
                errno = ENXIO;
                return STD_RET_ERROR;
        }
        #endif

        #if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0)
        if (major == _I2C2 && minor >= _I2C2_NUMBER_OF_DEVICES) {
                errno = ENXIO;
                return STD_RET_ERROR;
        }
        #endif


        /* create basic module structures */
        if (I2C == NULL) {
                I2C = calloc(1, sizeof(I2C_mem_t));
                if (!I2C) {
                        goto error;
                }
        }

        if (I2C->periph[major].lock == NULL) {
                I2C->periph[major].lock  = mutex_new(MUTEX_NORMAL);
                if (!I2C->periph[major].lock) {
                        goto error;
                }
        }

        if (I2C->periph[major].event == NULL) {
                I2C->periph[major].event = semaphore_new(1, 0);
                if (!I2C->periph[major].event) {
                        goto error;
                }
        }

        if (I2C->periph[major].initialized == false) {
                if (!enable_I2C(major)) {
                        goto error;
                }
        }


        /* create device structure */
        I2C_dev_t *hdl = calloc(1, sizeof(I2C_dev_t));
        if (hdl) {
                hdl->config = &I2C_cfg[major].devices[minor];
                I2C->periph[major].dev_cnt++;
                *device_handle = hdl;
                return STD_RET_OK;
        }


        /* error handling */
        error:
        release_resources(major);
        return STD_RET_ERROR;
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
API_MOD_RELEASE(I2C, void *device_handle)
{
        I2C_dev_t *hdl = device_handle;
        stdret_t status;

        critical_section_begin();

        if (device_is_unlocked(&hdl->lock)) {

                I2C->periph[hdl->config->major].dev_cnt--;
                release_resources(hdl->config->major);
                free(hdl);

                status = STD_RET_OK;
        } else {
                errno  = EBUSY;
                status = STD_RET_ERROR;
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
API_MOD_OPEN(I2C, void *device_handle, vfs_open_flags_t flags)
{
        UNUSED_ARG(flags);

        I2C_dev_t *hdl = device_handle;

        return device_lock(&hdl->lock) ? STD_RET_OK : STD_RET_ERROR;
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
API_MOD_CLOSE(I2C, void *device_handle, bool force)
{
        I2C_dev_t *hdl = device_handle;

        if (device_is_access_granted(&hdl->lock) || force) {
                device_unlock(&hdl->lock, force);
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
API_MOD_WRITE(I2C, void *device_handle, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fattr);

        I2C_dev_t *hdl = device_handle;

        ssize_t n = -1;

        if (device_is_access_granted(&hdl->lock)) {
                if (mutex_lock(I2C->periph[hdl->config->major].lock, timeout)) {
                        if (!I2C_start(hdl))
                                goto exit;

                        if (!I2C_send_address(hdl, true))
                                goto exit;

                        if (!I2C_send_sub_address(hdl, *fpos))
                                goto exit;

                        n = I2C_transmit(hdl, src, count, true);

                        exit:
                        mutex_unlock(I2C->periph[hdl->config->major].lock);
                } else {
                        errno = ETIME;
                }
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
API_MOD_READ(I2C, void *device_handle, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fattr);

        I2C_dev_t *hdl = device_handle;

        ssize_t n = -1;

        if (device_is_access_granted(&hdl->lock)) {
                if (mutex_lock(I2C->periph[hdl->config->major].lock, timeout)) {
                        if (!I2C_start(hdl))
                                goto exit;

                        if (hdl->config->sub_addr_mode != I2C_SUB_ADDR_MODE__DISABLED) {
                                if (!I2C_send_address(hdl, true))
                                        goto exit;

                                if (!I2C_send_sub_address(hdl, *fpos))
                                        goto exit;

                                if (!I2C_start(hdl))
                                        goto exit;
                        }

                        if (!I2C_send_address(hdl, false))
                                goto exit;

                        n = I2C_receive(hdl, dst, count);

                        exit:
                        I2C_stop(hdl);

                        mutex_unlock(I2C->periph[hdl->config->major].lock);
                } else {
                        errno = ETIME;
                }
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
 * @return On success return 0 or 1. On error, -1 is returned, and errno set
 *         appropriately.
 */
//==============================================================================
API_MOD_IOCTL(I2C, void *device_handle, int request, void *arg)
{
        UNUSED_ARG(device_handle);

        if (arg) {
                switch (request) {
                default:
                        errno = EBADRQC;
                        return -1;
                }
        }

        errno = EINVAL;
        return -1;
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
API_MOD_FLUSH(I2C, void *device_handle)
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
API_MOD_STAT(I2C, void *device_handle, struct vfs_dev_stat *device_stat)
{
        I2C_dev_t *hdl = device_handle;

        device_stat->st_size  = 0;
        device_stat->st_major = hdl->config->major;
        device_stat->st_minor = hdl->config->minor;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static void release_resources(u8_t major)
{
        if (I2C->periph[major].dev_cnt == 0 && I2C->periph[major].lock) {
                mutex_delete(I2C->periph[major].lock);
                I2C->periph[major].lock = NULL;
        }

        if (I2C->periph[major].dev_cnt == 0 && I2C->periph[major].event) {
                semaphore_delete(I2C->periph[major].event);
                I2C->periph[major].event = NULL;
        }

        if (I2C->periph[major].dev_cnt == 0 && I2C->periph[major].initialized) {
                disable_I2C(major);
        }

        bool mem_used = false;
        for (int i = 0; i < _I2C_NUMBER_OF_PERIPHERALS && !mem_used; i++) {
                mem_used = I2C->periph[i].dev_cnt > 0;
        }

        if (!mem_used && I2C) {
                free(I2C);
                I2C = NULL;
        }
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static I2C_t *get_I2C(I2C_dev_t *hdl)
{
        return (I2C_t *)I2C_cfg[hdl->config->major].I2C;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static bool enable_I2C(u8_t major)
{
        const I2C_config_t *cfg = &I2C_cfg[major];
        I2C_t              *i2c = (I2C_t *)I2C_cfg[major].I2C;

        SET_BIT(RCC->APB1RSTR, cfg->I2C_clken);
        CLEAR_BIT(RCC->APB1RSTR, cfg->I2C_clken);
        SET_BIT(RCC->APB1ENR, cfg->I2C_clken);

        RCC_ClocksTypeDef clocks = {0};
        RCC_GetClocksFreq(&clocks);
        clocks.PCLK1_Frequency /= 1000000;

        if (clocks.PCLK1_Frequency < 2) {
                errno = EIO;
                return false;
        }

        u16_t CR2 = (clocks.PCLK1_Frequency) & I2C_CR2_FREQ;

        if (cfg->use_DMA) {
                if ((cfg->DMA_rx->CCR & DMA_CCR1_EN) || (cfg->DMA_tx->CCR & DMA_CCR1_EN)) {
                        errno = EADDRINUSE;
                        return false;
                } else {
                        SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA1EN);
                }
        } else {
                SET_BIT(CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);
                NVIC_EnableIRQ(cfg->IRQ_EV_n);
                NVIC_EnableIRQ(cfg->IRQ_ER_n);
                NVIC_SetPriority(cfg->IRQ_EV_n, cfg->IRQ_prio);
                NVIC_SetPriority(cfg->IRQ_ER_n, cfg->IRQ_prio);
        }

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
        i2c->CR2   = CR2;
        i2c->CCR   = CCR;
        i2c->TRISE = clocks.PCLK1_Frequency + 1;
        i2c->CR1   = I2C_CR1_PE;

        I2C->periph[major].initialized = true;

        return true;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static void disable_I2C(u8_t major)
{
        const I2C_config_t *cfg = &I2C_cfg[major];
        I2C_t              *i2c = (I2C_t *)I2C_cfg[major].I2C;

        NVIC_DisableIRQ(I2C1_EV_IRQn);
        NVIC_DisableIRQ(I2C1_ER_IRQn);

        WRITE_REG(i2c->CR1, 0);
        SET_BIT(RCC->APB1RSTR, cfg->I2C_clken);
        CLEAR_BIT(RCC->APB1RSTR, cfg->I2C_clken);
        CLEAR_BIT(RCC->APB1ENR, cfg->I2C_clken);

        I2C->periph[major].initialized = false;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static bool wait_for_event(I2C_dev_t *hdl)
{
        I2C->periph[hdl->config->major].error = false;
        SET_BIT(get_I2C(hdl)->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);

        if (!semaphore_wait(I2C->periph[hdl->config->major].event, timeout)) {
                CLEAR_BIT(get_I2C(hdl)->CR2, I2C_CR2_ITERREN | I2C_CR2_ITBUFEN | I2C_CR2_ITEVTEN);
                I2C->periph[hdl->config->major].error = true;
                errno = ETIME;
                return false;
        }

        if (I2C->periph[hdl->config->major].error) {
                errno = EIO;
                return false;
        }

        return true;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static bool I2C_start(I2C_dev_t *hdl)
{
        printk("start\n");

        SET_BIT(get_I2C(hdl)->CR1, I2C_CR1_START);
        return wait_for_event(hdl);
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static bool I2C_send_address(I2C_dev_t *hdl, bool wr)
{
        printk("address\n");

        I2C_t *i2c = get_I2C(hdl);
        volatile u16_t  tmp;
        (void) tmp;

        if (hdl->config->addr10bit) {
                tmp = i2c->SR1;
                i2c->DR = header10b | ((hdl->config->addr & 0x300) >> 7);
                if (!wait_for_event(hdl))
                        return false;

                tmp = i2c->SR1;
                i2c->DR = hdl->config->addr;
                if (!wait_for_event(hdl))
                        return false;

                if (!wr) {
                        if (!I2C_start(hdl))
                                return false;

                        tmp = i2c->SR1;
                        i2c->DR = header10b | ((hdl->config->addr & 0x300) >> 7) | 0x01;
                        if (!wait_for_event(hdl))
                                return false;
                }
        } else {
                u8_t addr = hdl->config->addr & 0xFE;

                if (!wr)
                        addr |= 0x01;

                tmp = i2c->SR1;
                i2c->DR = addr;
                if (!wait_for_event(hdl))
                        return false;
        }

        return true;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static bool I2C_send_sub_address(I2C_dev_t *hdl, u32_t addr)
{
        printk("sub-address\n");

        int  i = 0;
        u8_t bfr[4];

        switch (hdl->config->sub_addr_mode) {
        default:
                return true;

        case I2C_SUB_ADDR_MODE__3_BYTES:
                bfr[i++] = addr >> 16;
                // flow through

        case I2C_SUB_ADDR_MODE__2_BYTES:
                bfr[i++] = addr >> 8;
                // flow through

        case I2C_SUB_ADDR_MODE__1_BYTE:
                bfr[i++] = addr;
                break;
        }

        size_t len = static_cast(size_t, hdl->config->sub_addr_mode);
        return I2C_transmit(hdl, bfr, len, false) == len;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static size_t I2C_transmit(I2C_dev_t *hdl, const u8_t *src, size_t size, bool stop)
{
        printk("transmit\n");

        I2C_t *i2c = get_I2C(hdl);

        if (I2C_cfg[hdl->config->major].use_DMA) {
                // TODO
        } else {
                I2C->periph[hdl->config->major].stop = stop;
                I2C->periph[hdl->config->major].data = const_cast(u8_t*, src);
                I2C->periph[hdl->config->major].size = size;

                SET_BIT(i2c->CR2, I2C_CR2_ITERREN | I2C_CR2_ITBUFEN | I2C_CR2_ITEVTEN);

                if (!wait_for_event(hdl)) {
                        return 0;
                }
        }

        return size - I2C->periph[hdl->config->major].size;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static size_t I2C_receive(I2C_dev_t *hdl, u8_t *dst, size_t size)
{

}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static void I2C_stop(I2C_dev_t *hdl)
{
        printk("stop\n");

        SET_BIT(get_I2C(hdl)->CR1, I2C_CR1_STOP);
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static bool IRQ_EV_handler(u8_t major)
{
        I2C_t *i2c  = const_cast(I2C_t*, I2C_cfg[major].I2C);
        bool  woken = false;

        if ((i2c->SR1 & I2C_SR1_TXE) && (i2c->SR2 & I2C_SR2_TRA) && (i2c->CR2 & I2C_CR2_ITBUFEN)) {
                if (I2C->periph[major].size) {
                        i2c->DR = *(I2C->periph[major].data++);
                }

                CLEAR_BIT(i2c->CR2, I2C_CR2_ITBUFEN);

        } else if ((i2c->SR1 & I2C_SR1_BTF) && (i2c->SR2 & I2C_SR2_TRA)) {
                if (I2C->periph[major].size == 1 && I2C->periph[major].stop) {
                        SET_BIT(i2c->CR1, I2C_CR1_STOP);
                        I2C->periph[major].stop = false;
                }

                if (I2C->periph[major].size == 0) {
                        CLEAR_BIT(i2c->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);
                        semaphore_signal_from_ISR(I2C->periph[major].event, &woken);
                } else {
                        I2C->periph[major].size--;
                        SET_BIT(i2c->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);
                }

        } else if ((i2c->SR1 & I2C_SR1_RXNE) && !(i2c->SR2 & I2C_SR2_TRA) && (i2c->CR2 & I2C_CR2_ITBUFEN)) {
                if (I2C->periph[major].size) {
                        if (I2C->periph[major].size == 1)
                                CLEAR_BIT(i2c->CR1, I2C_CR1_ACK);
                        else
                                SET_BIT(i2c->CR1, I2C_CR1_ACK);

                        *(I2C->periph[major].data++) = i2c->DR;
                        I2C->periph[major].size--;
                } else {
                        CLEAR_BIT(i2c->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);
                        semaphore_signal_from_ISR(I2C->periph[major].event, &woken);
                }

        } else {
                CLEAR_BIT(i2c->CR2, I2C_CR2_ITEVTEN);
                semaphore_signal_from_ISR(I2C->periph[major].event, &woken);
        }

        return woken;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static bool IRQ_ER_handler(u8_t major)
{
        I2C_t *i2c = const_cast(I2C_t*, I2C_cfg[major].I2C);

        I2C->periph[major].error = true;

        WRITE_REG(i2c->SR1, 0);
        CLEAR_BIT(i2c->CR2, I2C_CR2_ITEVTEN);
        CLEAR_BIT(i2c->CR2, I2C_CR2_ITBUFEN);

        bool woken = false;
        semaphore_signal_from_ISR(I2C->periph[major].event, &woken);

        return woken;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
#if (_I2C1_ENABLE > 0) && (_I2C1_NUMBER_OF_DEVICES > 0)
void I2C1_EV_IRQHandler(void)
{
        IRQ_EV_handler(_I2C1);
}
#endif

//==============================================================================
/**
 * @brief
 */
//==============================================================================
#if (_I2C1_ENABLE > 0) && (_I2C1_NUMBER_OF_DEVICES > 0)
void I2C1_ER_IRQHandler(void)
{
        IRQ_ER_handler(_I2C1);
}
#endif

//==============================================================================
/**
 * @brief
 */
//==============================================================================
#if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0)
void I2C2_EV_IRQHandler(void)
{
        IRQ_EV_handler(_I2C2);
}
#endif

//==============================================================================
/**
 * @brief
 */
//==============================================================================
#if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0)
void I2C2_ER_IRQHandler(void)
{
        IRQ_ER_handler(_I2C2);
}
#endif

//==============================================================================
/**
 * @brief
 */
//==============================================================================
#if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0) && (_I2C2_USE_DMA > 0)
void DMA1_Channel4_IRQHandler(void)
{

}
#endif

//==============================================================================
/**
 * @brief
 */
//==============================================================================
#if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0) && (_I2C2_USE_DMA > 0)
void DMA1_Channel5_IRQHandler(void)
{

}
#endif

//==============================================================================
/**
 * @brief
 */
//==============================================================================
#if (_I2C1_ENABLE > 0) && (_I2C1_NUMBER_OF_DEVICES > 0) && (_I2C1_USE_DMA > 0)
void DMA1_Channel6_IRQHandler(void)
{

}
#endif

//==============================================================================
/**
 * @brief
 */
//==============================================================================
#if (_I2C1_ENABLE > 0) && (_I2C1_NUMBER_OF_DEVICES > 0) && (_I2C1_USE_DMA > 0)
void DMA1_Channel7_IRQHandler(void)
{

}
#endif

/*==============================================================================
  End of file
==============================================================================*/
