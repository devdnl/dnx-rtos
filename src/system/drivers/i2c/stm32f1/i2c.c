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

// NOTE: 10-bit addressing mode is experimental and not tested!

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
/// macro used to simplify configuration of particular device
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
/// type defines possible modes of sub-addressing sequence (used e.g. in EEPROM)
typedef enum {
        I2C_SUB_ADDR_MODE__DISABLED = 0,                //!< sub-addressing disabled
        I2C_SUB_ADDR_MODE__1_BYTE   = 1,                //!< sub-address is 1 byte long
        I2C_SUB_ADDR_MODE__2_BYTES  = 2,                //!< sub-address is 2 byte long
        I2C_SUB_ADDR_MODE__3_BYTES  = 3                 //!< sub-address is 3 byte long
} I2C_sub_addr_mode_t;

/// type defines state of state machine implemented in the IRQ that controls data flow
typedef enum {
        STATE_IDLE,                                     //!< state used when IRQ is disabled
        STATE_START,                                    //!< I2C start or repeated start sequence done
        STATE_ADDRESS_HEADER_SENT,                      //!< 10-bit address mode header sent
        STATE_ADDRESS_SENT,                             //!< 7- or 10-bit address sent
        STATE_SUB_ADDRESS,                              //!< sub-address send state
        STATE_TRANSFER                                  //!< data transfer (tx/rx and DMA)
} state_t;

/// type defines single device configuration connected to the specified I2C peripheral
typedef struct {
        const u16_t               addr:16;              //!< address of device (7- or 10-bit)
        const bool                addr10bit:1;          //!< 10-bit mode addressing (true)
        const I2C_sub_addr_mode_t sub_addr_mode:2;      //!< mode of sub-address (0, 1, 2, or 3 byte)
        const u8_t                major:4;              //!< major number of the device (I2C peripheral number)
        const u8_t                minor:4;              //!< minor number of the device (device identifier)
} I2C_dev_config_t;

/// type defines configuration of single I2C peripheral
typedef struct {
        const I2C_t            *const I2C;              //!< pointer to the I2C peripheral
        const DMA_Channel_t    *const DMA_tx;           //!< pointer to the DMA Tx channel peripheral
        const DMA_Channel_t    *const DMA_rx;           //!< pointer to the DMA Rx channel peripheral
        const u32_t             freq;                   //!< peripheral SCL frequency [Hz]
        const u32_t             APB1ENR_clk_mask;       //!< mask used to enable I2C clock in the APB1ENR register
        const bool              use_DMA;                //!< peripheral uses DMA and IRQ (true), or only IRQ (false)
        const u8_t              DMA_tx_number:4;        //!< number of channel of DMA Tx
        const u8_t              DMA_rx_number:4;        //!< number of channel of DMA Rx
        const u8_t              IRQ_prio;               //!< priority of IRQ (event, error, and DMA)
        const IRQn_Type         IRQ_EV_n;               //!< number of event IRQ vector
        const IRQn_Type         IRQ_ER_n;               //!< number of error IRQ vector
        const IRQn_Type         DMA_tx_IRQ_n;           //!< number of interrupt in the vector table
        const IRQn_Type         DMA_rx_IRQ_n;           //!< number of interrupt in the vector table
        const I2C_dev_config_t *const devices;          //!< pointer to devices' configuration table
} I2C_config_t;

/// type defines I2C device in the runtime environment
typedef struct {
        const I2C_dev_config_t *config;                 //!< pointer to the device configuration
        dev_lock_t              lock;                   //!< object used to lock access to opened device
        u16_t                   address;                //!< device address
} I2C_dev_t;

/// type defines main memory of this module
typedef struct {
        struct I2C_per {
                mutex_t            *lock;               //!< mutex used to lock access to the particular peripheral
                sem_t              *event;              //!< semaphore used to indicate event (operation finished)
                I2C_dev_t          *dev;                //!< pointer to the currently handling device
                u8_t               *data;               //!< pointer to the data input/output buffer
                u32_t               sub_addr;           //!< sub-address value
                size_t              data_size;          //!< input/output buffer data_size
                state_t             state;              //!< state of I2C peripheral
                u8_t                dev_cnt;            //!< number of initialized devices
                I2C_sub_addr_mode_t sub_addr_mode:2;    //!< sub-address mode and byte counter
                bool                addr10rd:1;         //!< enabled 10-bit addressing mode
                bool                write:1;            //!< indicates write transaction
                bool                error:1;            //!< indicates error event
                bool                stop:1;             //!< indicates stop sequence after current transaction
                bool                initialized:1;      //!< indicates that module for this peripheral is initialized
        } periph[_I2C_NUMBER_OF_PERIPHERALS];
} I2C_mem_t;


/*==============================================================================
  Local function prototypes
==============================================================================*/
static void release_resources(u8_t major);
static inline I2C_t *get_I2C(I2C_dev_t *hdl);
static bool enable_I2C(u8_t major);
static void disable_I2C(u8_t major);
static ssize_t I2C_transmit(I2C_dev_t *hdl, bool sub_addr_mode, u32_t sub_addr, const u8_t *src, size_t size, bool stop);
static ssize_t I2C_receive(I2C_dev_t *hdl, u8_t *dst, size_t size, bool stop);
static void clear_DMA_IRQ_flags(u8_t major);
static bool IRQ_EV_handler(u8_t major);
static bool IRQ_ER_handler(u8_t major);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(I2C);

static const uint access_timeout = 30000;
static const uint device_timeout = 2000;
static const u8_t header10b      = 0xF0;

/// configuration of devices of I2C1 peripheral
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

/// configuration of devices of I2C2 peripheral
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

/// peripherals configuration
static const I2C_config_t I2C_cfg[_I2C_NUMBER_OF_PERIPHERALS] = {
        #if (_I2C1_ENABLE > 0) && (_I2C1_NUMBER_OF_DEVICES > 0)
        {
                .I2C              = I2C1,
                .freq             = _I2C1_FREQUENCY,
                .use_DMA          = _I2C1_USE_DMA,
                .DMA_tx           = DMA1_Channel6,
                .DMA_rx           = DMA1_Channel7,
                .DMA_tx_number    = 6,
                .DMA_rx_number    = 7,
                .DMA_tx_IRQ_n     = DMA1_Channel6_IRQn,
                .DMA_rx_IRQ_n     = DMA1_Channel7_IRQn,
                .APB1ENR_clk_mask = RCC_APB1ENR_I2C1EN,
                .IRQ_prio         = _I2C1_IRQ_PRIO,
                .IRQ_EV_n         = I2C1_EV_IRQn,
                .IRQ_ER_n         = I2C1_ER_IRQn,
                .devices          = I2C1_dev_cfg
        },
        #endif
        #if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0)
        {
                .I2C              = I2C2,
                .freq             = _I2C2_FREQUENCY,
                .use_DMA          = _I2C2_USE_DMA,
                .DMA_tx           = DMA1_Channel4,
                .DMA_rx           = DMA1_Channel5,
                .DMA_tx_number    = 4,
                .DMA_rx_number    = 5,
                .DMA_tx_IRQ_n     = DMA1_Channel4_IRQn,
                .DMA_rx_IRQ_n     = DMA1_Channel5_IRQn,
                .APB1ENR_clk_mask = RCC_APB1ENR_I2C2EN,
                .IRQ_prio         = _I2C2_IRQ_PRIO,
                .IRQ_EV_n         = I2C2_EV_IRQn,
                .IRQ_ER_n         = I2C2_ER_IRQn,
                .devices          = I2C2_dev_cfg
        },
        #endif
};

/// main memory of module
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


        /* creates basic module structures */
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


        /* creates device structure */
        I2C_dev_t *hdl = calloc(1, sizeof(I2C_dev_t));
        if (hdl) {
                hdl->config  = &I2C_cfg[major].devices[minor];
                hdl->address =  I2C_cfg[major].devices[minor].addr;
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
 * @return On success return 0. On error, -1 is returned, and errno set
 *         appropriately.
 */
//==============================================================================
API_MOD_IOCTL(I2C, void *device_handle, int request, void *arg)
{
        I2C_dev_t *hdl = device_handle;

        if (device_is_access_granted(&hdl->lock)) {
                switch (request) {
                case IOCTL_I2C__SET_ADDRESS:
                        hdl->address = reinterpret_cast(int, arg);
                        return 0;

                default:
                        errno = EBADRQC;
                        break;
                }
        } else {
                errno = EBUSY;
        }

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
 * @brief  Function release all resource allocated during initialization phase
 * @param  major         major device number
 * @return None
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
 * @brief  Returns I2C address of current device
 * @param  hdl          device handle
 * @return I2C peripheral address
 */
//==============================================================================
static inline I2C_t *get_I2C(I2C_dev_t *hdl)
{
        return const_cast(I2C_t*, I2C_cfg[hdl->config->major].I2C);
}

//==============================================================================
/**
 * @brief  Enables selected I2C peripheral according with configuration
 * @param  major        peripheral number
 * @return On success true is returned, otherwise false and appropriate error is set
 */
//==============================================================================
static bool enable_I2C(u8_t major)
{
        const I2C_config_t *cfg = &I2C_cfg[major];
        I2C_t              *i2c = const_cast(I2C_t*, I2C_cfg[major].I2C);

        SET_BIT(RCC->APB1RSTR, cfg->APB1ENR_clk_mask);
        CLEAR_BIT(RCC->APB1RSTR, cfg->APB1ENR_clk_mask);
        SET_BIT(RCC->APB1ENR, cfg->APB1ENR_clk_mask);

        RCC_ClocksTypeDef clocks = {0};
        RCC_GetClocksFreq(&clocks);
        clocks.PCLK1_Frequency /= 1000000;

        if (clocks.PCLK1_Frequency < 2) {
                errno = EIO;
                return false;
        }

        u16_t CR2 = (clocks.PCLK1_Frequency) & I2C_CR2_FREQ;

        if (cfg->use_DMA) {
                #if (_I2C1_USE_DMA > 0) || (_I2C2_USE_DMA > 0)
                if ((cfg->DMA_rx->CCR & DMA_CCR1_EN) || (cfg->DMA_tx->CCR & DMA_CCR1_EN)) {
                        errno = EADDRINUSE;
                        return false;
                } else {
                        SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA1EN);

                        clear_DMA_IRQ_flags(major);

                        NVIC_EnableIRQ(cfg->DMA_tx_IRQ_n);
                        NVIC_EnableIRQ(cfg->DMA_rx_IRQ_n);
                        NVIC_SetPriority(cfg->DMA_tx_IRQ_n, cfg->IRQ_prio);
                        NVIC_SetPriority(cfg->DMA_rx_IRQ_n, cfg->IRQ_prio);
                }
                #endif
        } else {
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
 * @brief  Disables selected I2C peripheral
 * @param  major        I2C peripheral number
 * @return None
 */
//==============================================================================
static void disable_I2C(u8_t major)
{
        const I2C_config_t *cfg = &I2C_cfg[major];
        I2C_t              *i2c = const_cast(I2C_t*, I2C_cfg[major].I2C);

        NVIC_DisableIRQ(cfg->DMA_tx_IRQ_n);
        NVIC_DisableIRQ(cfg->DMA_rx_IRQ_n);
        NVIC_DisableIRQ(cfg->IRQ_EV_n);
        NVIC_DisableIRQ(cfg->IRQ_ER_n);

        WRITE_REG(i2c->CR1, 0);
        SET_BIT(RCC->APB1RSTR, cfg->APB1ENR_clk_mask);
        CLEAR_BIT(RCC->APB1RSTR, cfg->APB1ENR_clk_mask);
        CLEAR_BIT(RCC->APB1ENR, cfg->APB1ENR_clk_mask);

        I2C->periph[major].initialized = false;
}

//==============================================================================
/**
 * @brief  Waits for IRQ event
 * @param  hdl          device handle
 * @return On success true is returned, otherwise false and appropriate error is set
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
 * @brief  Clear all DMA IRQ flags (of tx and rx)
 * @param  major        peripheral major number
 * @return None
 */
//==============================================================================
static void clear_DMA_IRQ_flags(u8_t major)
{
        I2C_config_t  *cfg    = const_cast(I2C_config_t*, &I2C_cfg[major]);
        DMA_Channel_t *DMA_tx = const_cast(DMA_Channel_t*, cfg->DMA_tx);
        DMA_Channel_t *DMA_rx = const_cast(DMA_Channel_t*, cfg->DMA_rx);

        WRITE_REG(DMA1->IFCR, (DMA_IFCR_CGIF1  << (cfg->DMA_tx_number - 1))
                            | (DMA_IFCR_CTCIF1 << (cfg->DMA_tx_number - 1))
                            | (DMA_IFCR_CHTIF1 << (cfg->DMA_tx_number - 1))
                            | (DMA_IFCR_CTEIF1 << (cfg->DMA_tx_number - 1))
                            | (DMA_IFCR_CGIF1  << (cfg->DMA_rx_number - 1))
                            | (DMA_IFCR_CTCIF1 << (cfg->DMA_rx_number - 1))
                            | (DMA_IFCR_CHTIF1 << (cfg->DMA_rx_number - 1))
                            | (DMA_IFCR_CTEIF1 << (cfg->DMA_rx_number - 1)) );

        CLEAR_BIT(DMA_tx->CCR, DMA_CCR1_EN);
        CLEAR_BIT(DMA_rx->CCR, DMA_CCR1_EN);
}

//==============================================================================
/**
 * @brief  Configure IRQ's state machine to perform data transmission
 * @param  hdl                  device's handle
 * @param  sub_addr_mode        sub-address send enabled (details read from configuration)
 * @param  sub_addr             sub-address value
 * @param  src                  data source
 * @param  size                 size of data
 * @param  stop                 send STOP sequence after transaction (true)
 * @return On success returns number of transfered bytes, otherwise -1 and
 *         appropriate error is set
 */
//==============================================================================
static ssize_t I2C_transmit(I2C_dev_t *hdl, bool sub_addr_mode, u32_t sub_addr, const u8_t *src, size_t size, bool stop)
{
        printk("transmit\n"); // TEST

        struct I2C_per *per = &I2C->periph[hdl->config->major];

        per->data          = const_cast(u8_t*, src);
        per->data_size     = size;
        per->stop          = stop;
        per->write         = true;
        per->sub_addr      = sub_addr;
        per->sub_addr_mode = sub_addr_mode ? hdl->config->sub_addr_mode : I2C_SUB_ADDR_MODE__DISABLED;
        per->dev           = hdl;
        per->state         = STATE_START;

        SET_BIT(get_I2C(hdl)->CR1, I2C_CR1_START);

        return wait_for_event(hdl) ? static_cast(ssize_t, size - per->data_size) : -1;
}

//==============================================================================
/**
 * @brief  Configre IRQ's state machine to perform data reception
 * @param  hdl                  device's handle
 * @param  dst                  data destination buffer
 * @param  size                 number of bytes to read
 * @param  stop                 send STOP sequence after transaction (true)
 * @param  On success returns number of received bytes, otherwise -1 and
 *         appropriate error is set
 */
//==============================================================================
static ssize_t I2C_receive(I2C_dev_t *hdl, u8_t *dst, size_t size, bool stop)
{
        printk("receive\n"); // TEST

        struct I2C_per *per = &I2C->periph[hdl->config->major];

        per->data          = dst;
        per->data_size     = size;
        per->stop          = stop;
        per->write         = false;
        per->sub_addr      = 0;
        per->sub_addr_mode = I2C_SUB_ADDR_MODE__DISABLED;
        per->dev           = hdl;
        per->state         = STATE_START;

        SET_BIT(get_I2C(hdl)->CR1, I2C_CR1_START);

        return wait_for_event(hdl) ? static_cast(ssize_t, size - per->data_size) : -1;
}

//==============================================================================
/**
 * @brief  Event IRQ handler (transaction state machine)
 * @param  major        number of peripheral
 * @return If IRQ was woken then true is returned, otherwise false
 */
//==============================================================================
static bool IRQ_EV_handler(u8_t major)
{
        // TEST check stack usage for this function

        GPIO_SET_PIN(TP223); // TEST

        I2C_config_t   *cfg   = const_cast(I2C_config_t*, &I2C_cfg[major]);
        I2C_t          *i2c   = const_cast(I2C_t*, cfg->I2C);
        struct I2C_per *per   = &I2C->periph[major];
        bool            woken = false;


        void error()
        {
                last_state = per->state; //TEST
                woken = IRQ_ER_handler(major);
        }

        void finish()
        {
                per->addr10rd = false;
                per->state    = STATE_IDLE;
                CLEAR_BIT(i2c->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN);

                if (per->stop)
                        SET_BIT(i2c->CR1, I2C_CR1_STOP);

                semaphore_signal_from_ISR(I2C->periph[major].event, &woken);
        }


        switch (per->state) {
        default:
        case STATE_IDLE:
                per->stop = true;
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

                                i2c->DR = header10b | ((per->dev->address & 0x300) >> 7) | (per->addr10rd ? 1 : 0);

                        } else {
                                per->state = STATE_ADDRESS_SENT;
                                if (per->write) {
                                        i2c->DR = per->dev->address & 0xFE;
                                } else {
                                        if (per->data_size > 1) {
                                                SET_BIT(i2c->CR1, I2C_CR1_ACK);
                                        } else {
                                                CLEAR_BIT(i2c->CR1, I2C_CR1_ACK);
                                        }

                                        i2c->DR = per->dev->address | 0x01;
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
                        i2c->DR       = per->dev->address;

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

                                if (per->data_size == 1) {
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

                if (!per->data || !per->data_size) {
                        finish();

                } else {
                        if (cfg->use_DMA) {
                                #if (_I2C1_USE_DMA > 0) || (_I2C2_USE_DMA > 0)
                                DMA_Channel_t *DMA = const_cast(DMA_Channel_t*, per->write ? cfg->DMA_tx : cfg->DMA_rx);

                                DMA->CPAR  = reinterpret_cast(u32_t, &i2c->DR);
                                DMA->CMAR  = reinterpret_cast(u32_t, per->data);
                                DMA->CNDTR = per->data_size;
                                DMA->CCR   = DMA_CCR1_DIR | DMA_CCR1_TCIE | DMA_CCR1_TEIE;

                                CLEAR_BIT(i2c->CR2, I2C_CR2_ITBUFEN | I2C_CR2_ITERREN | I2C_CR2_ITEVTEN);
                                SET_BIT(i2c->CR2, I2C_CR2_DMAEN);

                                if (per->stop) {
                                        SET_BIT(i2c->CR2, I2C_CR2_LAST);
                                } else {
                                        CLEAR_BIT(i2c->CR2, I2C_CR2_LAST);
                                }

                                SET_BIT(DMA->CCR, DMA_CCR1_EN);
                                #endif
                        } else {
                                if ((i2c->SR1 & I2C_SR1_TXE) && (i2c->SR2 & I2C_SR2_TRA)) {

                                        i2c->DR = *(per->data++);

                                        if (--per->data_size == 0) {
                                                CLEAR_BIT(i2c->CR2, I2C_CR2_ITBUFEN);
                                        }

                                } else if ((i2c->SR1 & I2C_SR1_RXNE) && !(i2c->SR2 & I2C_SR2_TRA)) {

                                        if (per->data_size == 2) {
                                                CLEAR_BIT(i2c->CR1, I2C_CR1_ACK);
                                                SET_BIT(i2c->CR1, I2C_CR1_STOP);
                                        } else if (per->data_size <= 1) {
                                                finish();
                                        }

                                        *(per->data++) = i2c->DR;
                                        per->data_size--;

                                } else {
                                        error();
                                }
                        }
                }

                GPIO_CLEAR_PIN(TP204); // TEST

                break;
        }

        GPIO_CLEAR_PIN(TP223); // TEST

        return woken;
}

//==============================================================================
/**
 * @brief  Error IRQ handler
 * @param  major        number of peripheral
 * @return If IRQ was woken then true is returned, otherwise false
 */
//==============================================================================
static bool IRQ_ER_handler(u8_t major)
{
        I2C_t *i2c = const_cast(I2C_t*, I2C_cfg[major].I2C);

        I2C->periph[major].addr10rd = false;
        I2C->periph[major].error    = true;
        I2C->periph[major].state    = STATE_IDLE;

        WRITE_REG(i2c->SR1, 0);
        CLEAR_BIT(i2c->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
        SET_BIT(i2c->CR1, I2C_CR1_STOP);

        bool woken = false;
        semaphore_signal_from_ISR(I2C->periph[major].event, &woken);

        return woken;
}

//==============================================================================
/**
 * @brief  DMA IRQ handler
 * @param  major        number of peripheral
 * @return If IRQ was woken then true is returned, otherwise false
 */
//==============================================================================
#if (_I2C1_USE_DMA > 0) || (_I2C2_USE_DMA > 0)
static bool IRQ_DMA_handler(u8_t major)
{
        GPIO_SET_PIN(TP204); // TEST

        bool            woken = false;
        I2C_config_t   *cfg   = const_cast(I2C_config_t*, &I2C_cfg[major]);
        int             txno  = cfg->DMA_tx_number - 1;
        int             rxno  = cfg->DMA_rx_number - 1;

        if (DMA1->ISR & ((DMA_ISR_TCIF1 << txno) | (DMA_ISR_TCIF1 << rxno))) {
                I2C->periph[major].data_size = 0;
                semaphore_signal_from_ISR(I2C->periph[major].event, &woken);

        } else if (DMA1->ISR & ((DMA_ISR_TEIF1 << txno) | (DMA_ISR_TEIF1 << rxno))) {
                woken = IRQ_ER_handler(major);
        }

        clear_DMA_IRQ_flags(major);

        GPIO_CLEAR_PIN(TP204); // TEST

        return woken;
}
#endif

//==============================================================================
/**
 * @brief  I2C1 Event IRQ handler
 * @param  None
 * @return None
 */
//==============================================================================
#if (_I2C1_ENABLE > 0) && (_I2C1_NUMBER_OF_DEVICES > 0)
void I2C1_EV_IRQHandler(void)
{
        if (IRQ_EV_handler(_I2C1)) {
                task_yield_from_ISR();
        }
}
#endif

//==============================================================================
/**
 * @brief  I2C1 Error IRQ handler
 * @param  None
 * @return None
 */
//==============================================================================
#if (_I2C1_ENABLE > 0) && (_I2C1_NUMBER_OF_DEVICES > 0)
void I2C1_ER_IRQHandler(void)
{
        if (IRQ_ER_handler(_I2C1)) {
                task_yield_from_ISR();
        }
}
#endif

//==============================================================================
/**
 * @brief  I2C2 Event IRQ handler
 * @param  None
 * @return None
 */
//==============================================================================
#if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0)
void I2C2_EV_IRQHandler(void)
{
        if (IRQ_EV_handler(_I2C2)) {
                task_yield_from_ISR();
        }
}
#endif

//==============================================================================
/**
 * @brief  I2C2 Error IRQ handler
 * @param  None
 * @return None
 */
//==============================================================================
#if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0)
void I2C2_ER_IRQHandler(void)
{
        if (IRQ_ER_handler(_I2C2)) {
                task_yield_from_ISR();
        }
}
#endif

//==============================================================================
/**
 * @brief  I2C1 DMA Tx Complete IRQ handler
 * @param  None
 * @return None
 */
//==============================================================================
#if (_I2C1_ENABLE > 0) && (_I2C1_NUMBER_OF_DEVICES > 0) && (_I2C1_USE_DMA > 0)
void DMA1_Channel6_IRQHandler(void)
{
        if (IRQ_DMA_handler(_I2C1)) {
                task_yield_from_ISR();
        }
}
#endif

//==============================================================================
/**
 * @brief  I2C1 DMA Rx Complete IRQ handler
 * @param  None
 * @return None
 */
//==============================================================================
#if (_I2C1_ENABLE > 0) && (_I2C1_NUMBER_OF_DEVICES > 0) && (_I2C1_USE_DMA > 0)
void DMA1_Channel7_IRQHandler(void)
{
        if (IRQ_DMA_handler(_I2C1)) {
                task_yield_from_ISR();
        }
}
#endif


//==============================================================================
/**
 * @brief  I2C2 DMA Tx Complete IRQ handler
 * @param  None
 * @return None
 */
//==============================================================================
#if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0) && (_I2C2_USE_DMA > 0)
void DMA1_Channel4_IRQHandler(void)
{
        if (IRQ_DMA_handler(_I2C2)) {
                task_yield_from_ISR();
        }
}
#endif

//==============================================================================
/**
 * @brief  I2C2 DMA Rx Complete IRQ handler
 * @param  None
 * @return None
 */
//==============================================================================
#if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0) && (_I2C2_USE_DMA > 0)
void DMA1_Channel5_IRQHandler(void)
{
        if (IRQ_DMA_handler(_I2C2)) {
                task_yield_from_ISR();
        }
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
