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

#include "stm32f1/gpio_cfg.h" // TEST

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
        I2C_SUB_ADDR_MODE__DISABLED = 0,
        I2C_SUB_ADDR_MODE__1_BYTE   = 1,
        I2C_SUB_ADDR_MODE__2_BYTES  = 2,
        I2C_SUB_ADDR_MODE__3_BYTES  = 3
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

typedef enum {
        STATE_IDLE,
        STATE_START,
        STATE_ADDRESS_HEADER_SENT,
        STATE_ADDRESS_SENT,
        STATE_SUB_ADDRESS,
        STATE_TRANSFER
} state_t;

typedef struct {
        struct I2C_per {
                mutex_t            *lock;
                sem_t              *event;
                I2C_dev_t          *dev;
                u8_t               *data;
                u32_t               sub_addr;
                size_t              size;
                state_t             state;
                u8_t                dev_cnt;
                I2C_sub_addr_mode_t sub_addr_mode:2;
                bool                addr10rd:1;
                bool                write:1;
                bool                error:1;
                bool                stop:1;
                bool                initialized:1;
        } periph[_I2C_NUMBER_OF_PERIPHERALS];
} I2C_mem_t;


/*==============================================================================
  Local function prototypes
==============================================================================*/
static void release_resources(u8_t major);
static I2C_t *get_I2C(I2C_dev_t *hdl);
static bool enable_I2C(u8_t major);
static void disable_I2C(u8_t major);
static size_t I2C_transmit(I2C_dev_t *hdl, bool sub_addr_mode, u32_t sub_addr, const u8_t *src, size_t size, bool stop);
static size_t I2C_receive(I2C_dev_t *hdl, u8_t *dst, size_t size, bool stop);
static bool IRQ_EV_handler(u8_t major);
static bool IRQ_ER_handler(u8_t major);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(I2C);

static const uint access_timeout = 30000;
static const uint device_timeout = 2000;
static const u8_t header10b      = 0xF0;

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
                if (mutex_lock(I2C->periph[hdl->config->major].lock, access_timeout)) {
                        n = I2C_transmit(hdl, true, *fpos, src, count, true);
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
                if (mutex_lock(I2C->periph[hdl->config->major].lock, access_timeout)) {
                        if (hdl->config->sub_addr_mode != I2C_SUB_ADDR_MODE__DISABLED) {
                                I2C_transmit(hdl, true, *fpos, NULL, 0, false);
                                if (I2C->periph[hdl->config->major].error == false) {
                                        n = I2C_receive(hdl, dst, count, true);
                                }
                        }

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
state_t last_state; // TEST
static bool wait_for_event(I2C_dev_t *hdl)
{
        I2C->periph[hdl->config->major].error = false;
        SET_BIT(get_I2C(hdl)->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);

        if (!semaphore_wait(I2C->periph[hdl->config->major].event, device_timeout)) {
                CLEAR_BIT(get_I2C(hdl)->CR2, I2C_CR2_ITERREN | I2C_CR2_ITBUFEN | I2C_CR2_ITEVTEN);
                I2C->periph[hdl->config->major].error = true;
                errno = ETIME;

                printk("Timout %d\n", last_state); // TEST

                return false;
        }

        if (I2C->periph[hdl->config->major].error) {
                printk("Error %d\n", last_state); // TEST

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
static size_t I2C_transmit(I2C_dev_t *hdl, bool sub_addr_mode, u32_t sub_addr, const u8_t *src, size_t size, bool stop)
{
        printk("transmit\n"); // TEST

        struct I2C_per *per = &I2C->periph[hdl->config->major];

        per->data          = const_cast(u8_t*, src);
        per->size          = size;
        per->stop          = stop;
        per->write         = true;
        per->sub_addr      = sub_addr;
        per->sub_addr_mode = sub_addr_mode ? hdl->config->sub_addr_mode : I2C_SUB_ADDR_MODE__DISABLED;
        per->dev           = hdl;

        GPIO_SET_PIN(TP204); // TEST
        per->state = STATE_START;
        SET_BIT(get_I2C(hdl)->CR1, I2C_CR1_START);

        wait_for_event(hdl);

        GPIO_CLEAR_PIN(TP204); // TEST
        return size - per->size;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static size_t I2C_receive(I2C_dev_t *hdl, u8_t *dst, size_t size, bool stop)
{
        printk("receive\n"); // TEST

        struct I2C_per *per = &I2C->periph[hdl->config->major];

        per->data          = dst;
        per->size          = size;
        per->stop          = stop;
        per->write         = false;
        per->sub_addr      = 0;
        per->sub_addr_mode = I2C_SUB_ADDR_MODE__DISABLED;
        per->dev           = hdl;

        GPIO_SET_PIN(TP204); // TEST
        per->state = STATE_START;
        SET_BIT(get_I2C(hdl)->CR1, I2C_CR1_START);

        wait_for_event(hdl);

        GPIO_CLEAR_PIN(TP204); // TEST
        return size - per->size;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static bool IRQ_EV_handler(u8_t major)
{
        GPIO_SET_PIN(TP223); // TEST

        I2C_t          *i2c   = const_cast(I2C_t*, I2C_cfg[major].I2C);
        struct I2C_per *per   = &I2C->periph[major];
        bool            woken = false;


        void error()
        {
                last_state = per->state; //TEST
                woken = IRQ_ER_handler(major);
        }

        void finish()
        {
                per->state = STATE_IDLE;
                CLEAR_BIT(i2c->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN);

                if (per->stop)
                        SET_BIT(i2c->CR1, I2C_CR1_STOP);

                semaphore_signal_from_ISR(I2C->periph[major].event, &woken);
        }


        switch (per->state) {
        default:
        case STATE_IDLE:
                per->stop     = true;
                per->addr10rd = false;
                finish();
                break;

        case STATE_START: {
                if (i2c->SR1 & I2C_SR1_SB) {
                        GPIO_SET_PIN(TP224); // TEST

                        if (per->dev->config->addr10bit) {
                                if (per->addr10rd) {
                                        per->state    = STATE_ADDRESS_SENT;
                                        per->addr10rd = false;
                                } else {
                                        per->state = STATE_ADDRESS_HEADER_SENT;
                                }

                                i2c->DR = header10b | ((per->dev->config->addr & 0x300) >> 7) | (per->addr10rd ? 1 : 0);

                        } else {
                                per->state = STATE_ADDRESS_SENT;
                                if (per->write) {
                                        i2c->DR = per->dev->config->addr & 0xFE;
                                } else {
                                        if (per->size > 1) {
                                                SET_BIT(i2c->CR1, I2C_CR1_ACK);
                                        } else {
                                                CLEAR_BIT(i2c->CR1, I2C_CR1_ACK);
                                        }

                                        i2c->DR = per->dev->config->addr | 0x01;
                                }
                        }

                        GPIO_CLEAR_PIN(TP224); // TEST
                } else if (i2c->SR1 & I2C_SR1_BTF) {
                        // IRQ caught when repeated-start is generated after transmit sequence
                        // this operation prevents function from continuously IRQ
                        // generation at repeated start sequence
                        u8_t tmp = i2c->DR;
                        (void)tmp;
                } else {
                        error();
                }
                break;
        }

        case STATE_ADDRESS_HEADER_SENT:
                if (i2c->SR1 & I2C_SR1_ADD10) {
                        per->state    = STATE_ADDRESS_SENT;
                        per->addr10rd = !per->write;
                        i2c->DR       = per->dev->config->addr;

                } else {
                        error();
                }
                break;

        case STATE_ADDRESS_SENT:
                if (i2c->SR1 & I2C_SR1_ADDR) {
                        GPIO_SET_PIN(TP218); // TEST

                        int tmp = i2c->SR2;
                        (void)tmp;

                        if (per->write) {
                                per->state = STATE_SUB_ADDRESS;
                        } else {
                                if (per->dev->config->addr10bit && per->addr10rd) {
                                        per->state = STATE_START;
                                        SET_BIT(i2c->CR1, I2C_CR1_START);
                                        break;
                                }

                                if (per->size == 1) {
                                        SET_BIT(i2c->CR1, I2C_CR1_STOP);
                                }

                                per->state = STATE_TRANSFER;
                        }

                        SET_BIT(i2c->CR2, I2C_CR2_ITBUFEN);

                        GPIO_CLEAR_PIN(TP218); // TEST
                } else {
                        error();
                }
                break;

        case STATE_SUB_ADDRESS:
                if (i2c->SR1 & I2C_SR1_TXE) {
                        GPIO_SET_PIN(TP220); // TEST

                        switch (per->sub_addr_mode) {
                        case I2C_SUB_ADDR_MODE__3_BYTES:
                                i2c->DR = per->sub_addr >> 16;
                                per->sub_addr_mode = I2C_SUB_ADDR_MODE__2_BYTES;
                                break;

                        case I2C_SUB_ADDR_MODE__2_BYTES:
                                i2c->DR = per->sub_addr >> 8;
                                per->sub_addr_mode = I2C_SUB_ADDR_MODE__1_BYTE;
                                break;

                        case I2C_SUB_ADDR_MODE__1_BYTE:
                                i2c->DR = per->sub_addr;
                                per->sub_addr_mode = I2C_SUB_ADDR_MODE__DISABLED;
                                per->state = STATE_TRANSFER;
                                break;
                        default:
                                per->state = STATE_TRANSFER;
                        }

                        GPIO_CLEAR_PIN(TP220); // TEST
                } else {
                        error();
                }
                break;

        case STATE_TRANSFER:
                GPIO_SET_PIN(TP204); // TEST

                if (!per->data || !per->size) {
                        finish();

                } else if ((i2c->SR1 & I2C_SR1_TXE) && (i2c->SR2 & I2C_SR2_TRA)) {

                        i2c->DR = *(per->data++);

                        if (--per->size == 0) {
                                CLEAR_BIT(i2c->CR2, I2C_CR2_ITBUFEN);
                        }

                } else if ((i2c->SR1 & I2C_SR1_RXNE) && !(i2c->SR2 & I2C_SR2_TRA)) {

                        if (per->size == 2) {
                                CLEAR_BIT(i2c->CR1, I2C_CR1_ACK);
                                SET_BIT(i2c->CR1, I2C_CR1_STOP);
                        } else if (per->size <= 1) {
                                finish();
                        }

                        *(per->data++) = i2c->DR;
                        per->size--;

                } else {
                        error();
                }

                GPIO_CLEAR_PIN(TP204); // TEST

                break;
        }

        GPIO_CLEAR_PIN(TP223); // TEST

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
        I2C->periph[major].state = STATE_IDLE;

        WRITE_REG(i2c->SR1, 0);
        CLEAR_BIT(i2c->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
        SET_BIT(i2c->CR1, I2C_CR1_STOP);

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
