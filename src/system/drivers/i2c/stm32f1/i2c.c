/*=========================================================================*//**
@file    i2c.c

@author  Daniel Zorychta

@brief   This driver support I2C peripherals.

@note    Copyright (C) 2014-2015  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "drivers/driver.h"
#include "stm32f1/i2c_cfg.h"
#include "stm32f1/i2c_def.h"
#include "stm32f1/stm32f10x.h"
#include "lib/stm32f10x_rcc.h"

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
        #if ((_I2C1_ENABLE > 0) && (_I2C1_USE_DMA > 0)) || ((_I2C2_ENABLE > 0) && (_I2C2_USE_DMA > 0))
        const bool                use_DMA;              //!< peripheral uses DMA and IRQ (true), or only IRQ (false)
        const DMA_Channel_t      *const DMA_tx;         //!< pointer to the DMA Tx channel peripheral
        const DMA_Channel_t      *const DMA_rx;         //!< pointer to the DMA Rx channel peripheral
        const u8_t                DMA_tx_number:4;      //!< number of channel of DMA Tx
        const u8_t                DMA_rx_number:4;      //!< number of channel of DMA Rx
        const IRQn_Type           DMA_tx_IRQ_n;         //!< number of interrupt in the vector table
        const IRQn_Type           DMA_rx_IRQ_n;         //!< number of interrupt in the vector table
        #endif
        const I2C_t              *const I2C;            //!< pointer to the I2C peripheral
        const I2C_dev_config_t   *const devices;        //!< pointer to devices' configuration table
        const u32_t               freq;                 //!< peripheral SCL frequency [Hz]
        const u32_t               APB1ENR_clk_mask;     //!< mask used to enable I2C clock in the APB1ENR register
        const u8_t                IRQ_prio;             //!< priority of IRQ (event, error, and DMA)
        const IRQn_Type           IRQ_EV_n;             //!< number of event IRQ vector
        const IRQn_Type           IRQ_ER_n;             //!< number of error IRQ vector
} I2C_config_t;

/// type defines I2C device in the runtime environment
typedef struct {
        const I2C_dev_config_t   *config;               //!< pointer to the device configuration
        dev_lock_t                lock;                 //!< object used to lock access to opened device
        u16_t                     address;              //!< device address
} I2C_dev_t;

/// type defines main memory of this module
typedef struct {
        struct I2C_per {
                mutex_t          *lock;                 //!< mutex used to lock access to the particular peripheral
                sem_t            *event;                //!< semaphore used to indicate event (operation finished)
                u16_t             SR1_mask;             //!< SR1 register mask (to catch specified event in IRQ)
                bool              use_DMA:1;            //!< true if peripheral use DMA channels
                bool              initialized:1;        //!< indicates that module for this peripheral is initialized
                u8_t              dev_cnt;              //!< number of initialized devices
                u8_t              error;                //!< error number (errno)
                u8_t              unexp_event_cnt;      //!< number of unexpected events
        } periph[_I2C_NUMBER_OF_PERIPHERALS];
} I2C_mem_t;


/*==============================================================================
  Local function prototypes
==============================================================================*/
static void release_resources(u8_t major);
static inline I2C_t *get_I2C(I2C_dev_t *hdl);
static int enable_I2C(u8_t major);
static void disable_I2C(u8_t major);
static bool start(I2C_dev_t *hdl);
static void stop(I2C_dev_t *hdl);
static bool send_address(I2C_dev_t *hdl, bool write);
static void clear_send_address_event(I2C_dev_t *hdl);
static bool send_subaddress(I2C_dev_t *hdl, u32_t address, I2C_sub_addr_mode_t mode);
static void set_ACK_according_to_reception_size(I2C_dev_t *hdl, size_t count);
static ssize_t receive(I2C_dev_t *hdl, u8_t *dst, size_t count);
static ssize_t transmit(I2C_dev_t *hdl, const u8_t *src, size_t count);
static void IRQ_EV_handler(u8_t major);
static void IRQ_ER_handler(u8_t major);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(I2C);

static const uint access_timeout    = 30000;
static const uint device_timeout    = 2000;
static const u8_t header_addr_10bit = 0xF0;

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
                #if ((_I2C1_ENABLE > 0) && (_I2C1_USE_DMA > 0)) || ((_I2C2_ENABLE > 0) && (_I2C2_USE_DMA > 0))
                .use_DMA          = _I2C1_USE_DMA,
                .DMA_tx           = DMA1_Channel6,
                .DMA_rx           = DMA1_Channel7,
                .DMA_tx_number    = 6,
                .DMA_rx_number    = 7,
                .DMA_tx_IRQ_n     = DMA1_Channel6_IRQn,
                .DMA_rx_IRQ_n     = DMA1_Channel7_IRQn,
                #endif
                .I2C              = I2C1,
                .freq             = _I2C1_FREQUENCY,
                .APB1ENR_clk_mask = RCC_APB1ENR_I2C1EN,
                .IRQ_prio         = _I2C1_IRQ_PRIO,
                .IRQ_EV_n         = I2C1_EV_IRQn,
                .IRQ_ER_n         = I2C1_ER_IRQn,
                .devices          = I2C1_dev_cfg
        },
        #endif
        #if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0)
        {
                #if ((_I2C1_ENABLE > 0) && (_I2C1_USE_DMA > 0)) || ((_I2C2_ENABLE > 0) && (_I2C2_USE_DMA > 0))
                .use_DMA          = _I2C2_USE_DMA,
                .DMA_tx           = DMA1_Channel4,
                .DMA_rx           = DMA1_Channel5,
                .DMA_tx_number    = 4,
                .DMA_rx_number    = 5,
                .DMA_tx_IRQ_n     = DMA1_Channel4_IRQn,
                .DMA_rx_IRQ_n     = DMA1_Channel5_IRQn,
                #endif
                .I2C              = I2C2,
                .freq             = _I2C2_FREQUENCY,
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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(I2C, void **device_handle, u8_t major, u8_t minor)
{
        int result = ENODEV;

        if (major >= _I2C_NUMBER_OF_PERIPHERALS) {
                return result;
        }

#if (_I2C1_ENABLE > 0) && (_I2C1_NUMBER_OF_DEVICES > 0)
        if (major == _I2C1 && minor >= _I2C1_NUMBER_OF_DEVICES) {
                return result;
        }
#endif

#if (_I2C2_ENABLE > 0) && (_I2C2_NUMBER_OF_DEVICES > 0)
        if (major == _I2C2 && minor >= _I2C2_NUMBER_OF_DEVICES) {
                return result;
        }
#endif

        /* creates basic module structures */
        if (I2C == NULL) {
                result = _sys_zalloc(sizeof(I2C_mem_t), static_cast(void**, &I2C));
                if (result != ESUCC)
                        goto finish;
        }

        if (I2C->periph[major].lock == NULL) {
                result = _sys_mutex_create(MUTEX_TYPE_NORMAL, &I2C->periph[major].lock);
                if (result != ESUCC) {
                        goto finish;
                }
        }

        if (I2C->periph[major].event == NULL) {
                result = _sys_semaphore_create(1, 0, &I2C->periph[major].event);
                if (result != ESUCC)
                        goto finish;
        }

        if (I2C->periph[major].initialized == false) {
                result = enable_I2C(major);
                if (result != ESUCC)
                        goto finish;
        }

        /* creates device structure */
        result = _sys_zalloc(sizeof(I2C_dev_t), device_handle);
        if (result == ESUCC) {
                I2C_dev_t *hdl = *device_handle;
                hdl->config    = &I2C_cfg[major].devices[minor];
                hdl->address   =  I2C_cfg[major].devices[minor].addr;
                I2C->periph[major].dev_cnt++;
        }

        finish:
        if (result != ESUCC) {
                release_resources(major);
        }

        return result;
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
API_MOD_RELEASE(I2C, void *device_handle)
{
        I2C_dev_t *hdl = device_handle;
        int        result;

        _sys_critical_section_begin();

        if (_sys_device_is_unlocked(&hdl->lock)) {

                I2C->periph[hdl->config->major].dev_cnt--;
                release_resources(hdl->config->major);
                _sys_free(device_handle);

                result = ESUCC;
        } else {
                result = EBUSY;
        }

        _sys_critical_section_end();

        return result;
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
API_MOD_OPEN(I2C, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(flags);

        I2C_dev_t *hdl = device_handle;

        return _sys_device_lock(&hdl->lock) ? ESUCC : EBUSY;
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
API_MOD_CLOSE(I2C, void *device_handle, bool force)
{
        I2C_dev_t *hdl = device_handle;

        if (_sys_device_is_access_granted(&hdl->lock) || force) {
                _sys_device_unlock(&hdl->lock, force);
                return ESUCC;
        } else {
                return EBUSY;
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
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_WRITE(I2C,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              struct vfs_fattr  fattr,
              size_t           *wrcnt)
{
        UNUSED_ARG1(fattr);

        I2C_dev_t *hdl    = device_handle;

        int status = _sys_mutex_lock(I2C->periph[hdl->config->major].lock, access_timeout);
        if (status == ESUCC) {

                if (!start(hdl))
                        goto error;

                if (!send_address(hdl, true))
                        goto error;
                else
                        clear_send_address_event(hdl);

                if (hdl->config->sub_addr_mode != I2C_SUB_ADDR_MODE__DISABLED) {
                        if (!send_subaddress(hdl, *fpos, hdl->config->sub_addr_mode))
                                goto error;
                }

                *wrcnt = transmit(hdl, src, count);
                status = ESUCC;

                error:
                _sys_mutex_unlock(I2C->periph[hdl->config->major].lock);
        }

        return status;
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
API_MOD_READ(I2C,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        I2C_dev_t *hdl    = device_handle;

        int status = _sys_mutex_lock(I2C->periph[hdl->config->major].lock, access_timeout);
        if (status == ESUCC) {

                if (hdl->config->sub_addr_mode != I2C_SUB_ADDR_MODE__DISABLED) {
                        if (!start(hdl))
                                goto error;

                        if (!send_address(hdl, true))
                                goto error;
                        else
                                clear_send_address_event(hdl);

                        if (!send_subaddress(hdl, *fpos, hdl->config->sub_addr_mode))
                                goto error;
                }

                if (!start(hdl))
                        goto error;

                set_ACK_according_to_reception_size(hdl, count);
                if (!send_address(hdl,  false))
                        goto error;

                *rdcnt = receive(hdl, dst, count);
                status = ESUCC;

                error:
                _sys_mutex_unlock(I2C->periph[hdl->config->major].lock);
        }

        return status;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_IOCTL(I2C, void *device_handle, int request, void *arg)
{
        I2C_dev_t *hdl = device_handle;

        switch (request) {
        case IOCTL_I2C__SET_ADDRESS:
                hdl->address = reinterpret_cast(int, arg);
                return ESUCC;

        default:
                return EBADRQC;
        }
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
API_MOD_FLUSH(I2C, void *device_handle)
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
API_MOD_STAT(I2C, void *device_handle, struct vfs_dev_stat *device_stat)
{
        I2C_dev_t *hdl = device_handle;

        device_stat->st_size  = 0;
        device_stat->st_major = hdl->config->major;
        device_stat->st_minor = hdl->config->minor;

        return ESUCC;
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
                _sys_mutex_destroy(I2C->periph[major].lock);
                I2C->periph[major].lock = NULL;
        }

        if (I2C->periph[major].dev_cnt == 0 && I2C->periph[major].event) {
                _sys_semaphore_destroy(I2C->periph[major].event);
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
                _sys_free(static_cast(void**, &I2C));
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
 * @brief  Clear all DMA IRQ flags (of tx and rx)
 * @param  major        peripheral major number
 * @return None
 */
//==============================================================================
#if ((_I2C1_ENABLE > 0) && (_I2C1_USE_DMA > 0)) || ((_I2C2_ENABLE > 0) && (_I2C2_USE_DMA > 0))
static void clear_DMA_IRQ_flags(u8_t major)
{
        I2C_config_t  *cfg    = const_cast(I2C_config_t*, &I2C_cfg[major]);
        DMA_Channel_t *DMA_tx = const_cast(DMA_Channel_t*, cfg->DMA_tx);
        DMA_Channel_t *DMA_rx = const_cast(DMA_Channel_t*, cfg->DMA_rx);

        WRITE_REG(DMA1->IFCR, (DMA_IFCR_CGIF1  << (4 * (cfg->DMA_tx_number - 1)))
                            | (DMA_IFCR_CTCIF1 << (4 * (cfg->DMA_tx_number - 1)))
                            | (DMA_IFCR_CHTIF1 << (4 * (cfg->DMA_tx_number - 1)))
                            | (DMA_IFCR_CTEIF1 << (4 * (cfg->DMA_tx_number - 1)))
                            | (DMA_IFCR_CGIF1  << (4 * (cfg->DMA_rx_number - 1)))
                            | (DMA_IFCR_CTCIF1 << (4 * (cfg->DMA_rx_number - 1)))
                            | (DMA_IFCR_CHTIF1 << (4 * (cfg->DMA_rx_number - 1)))
                            | (DMA_IFCR_CTEIF1 << (4 * (cfg->DMA_rx_number - 1))) );

        CLEAR_BIT(DMA_tx->CCR, DMA_CCR1_EN);
        CLEAR_BIT(DMA_rx->CCR, DMA_CCR1_EN);
}
#endif

//==============================================================================
/**
 * @brief  Enables selected I2C peripheral according with configuration
 * @param  major        peripheral number
 * @return One of errno value.
 */
//==============================================================================
static int enable_I2C(u8_t major)
{
        const I2C_config_t *cfg = &I2C_cfg[major];
        I2C_t              *i2c = const_cast(I2C_t*, I2C_cfg[major].I2C);

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

        I2C->periph[major].use_DMA = false;

#if ((_I2C1_ENABLE > 0) && (_I2C1_USE_DMA > 0)) || ((_I2C2_ENABLE > 0) && (_I2C2_USE_DMA > 0))
        if (cfg->use_DMA) {
                if (!(cfg->DMA_rx->CCR & DMA_CCR1_EN) && !(cfg->DMA_tx->CCR & DMA_CCR1_EN)) {
                        SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA1EN);

                        clear_DMA_IRQ_flags(major);

                        NVIC_EnableIRQ(cfg->DMA_tx_IRQ_n);
                        NVIC_EnableIRQ(cfg->DMA_rx_IRQ_n);
                        NVIC_SetPriority(cfg->DMA_tx_IRQ_n, cfg->IRQ_prio);
                        NVIC_SetPriority(cfg->DMA_rx_IRQ_n, cfg->IRQ_prio);

                        I2C->periph[major].use_DMA = true;
                }
        }
#endif

        NVIC_EnableIRQ(cfg->IRQ_EV_n);
        NVIC_EnableIRQ(cfg->IRQ_ER_n);
        NVIC_SetPriority(cfg->IRQ_EV_n, cfg->IRQ_prio);
        NVIC_SetPriority(cfg->IRQ_ER_n, cfg->IRQ_prio);

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

        I2C->periph[major].initialized = true;

        return ESUCC;
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

#if ((_I2C1_ENABLE > 0) && (_I2C1_USE_DMA > 0)) || ((_I2C2_ENABLE > 0) && (_I2C2_USE_DMA > 0))
        if (I2C->periph[major].use_DMA) {
                NVIC_DisableIRQ(cfg->DMA_tx_IRQ_n);
                NVIC_DisableIRQ(cfg->DMA_rx_IRQ_n);

                DMA_Channel_t *DMA_rx = const_cast(DMA_Channel_t*, cfg->DMA_rx);
                DMA_Channel_t *DMA_tx = const_cast(DMA_Channel_t*, cfg->DMA_tx);

                DMA_rx->CCR = 0;
                DMA_tx->CCR = 0;
        }
#endif

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
 * @brief  Function handle error (try make the interface working)
 * @param  hdl          device handle
 * @return None
 */
//==============================================================================
static void error(I2C_dev_t *hdl)
{
        I2C_t *i2c = get_I2C(hdl);

//        errno = I2C->periph[hdl->config->major].error;

        CLEAR_BIT(i2c->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN);

        u8_t tmp = i2c->SR1;
             tmp = i2c->SR2;
             tmp = i2c->DR;
             tmp = i2c->DR;
        (void) tmp;

        i2c->SR1 = 0;

        stop(hdl);
        _sys_sleep_ms(1);

        if (I2C->periph[hdl->config->major].error == EIO) {
                enable_I2C(hdl->config->major);
        }
}

//==============================================================================
/**
 * @brief  Function wait for selected event (IRQ)
 * @param  hdl                  device handle
 * @param  SR1_event_mask       event mask (bits from SR1 register)
 * @return On success true is returned, otherwise false
 */
//==============================================================================
static bool wait_for_I2C_event(I2C_dev_t *hdl, u16_t SR1_event_mask)
{
        I2C->periph[hdl->config->major].SR1_mask = SR1_event_mask & 0xDF;
        I2C->periph[hdl->config->major].error    = 0;

        u16_t CR2 = I2C_CR2_ITEVTEN | I2C_CR2_ITERREN;
        if (SR1_event_mask & (I2C_SR1_RXNE | I2C_SR1_TXE)) {
                CR2 |= I2C_CR2_ITBUFEN;
        }
        SET_BIT(get_I2C(hdl)->CR2, CR2);

        if (_sys_semaphore_wait(I2C->periph[hdl->config->major].event, device_timeout) == ESUCC) {
                if (I2C->periph[hdl->config->major].error == 0) {
                        return true;
                }
        } else {
                I2C->periph[hdl->config->major].error = EIO;
        }

        error(hdl);
        return false;
}

//==============================================================================
/**
 * @brief  Function wait for DMA event (IRQ)
 * @param  hdl                  device handle
 * @param  DMA                  DMA channel
 * @return On success true is returned, otherwise false
 */
//==============================================================================
#if ((_I2C1_ENABLE > 0) && (_I2C1_USE_DMA > 0)) || ((_I2C2_ENABLE > 0) && (_I2C2_USE_DMA > 0))
static bool wait_for_DMA_event(I2C_dev_t *hdl, DMA_Channel_t *DMA)
{
        I2C->periph[hdl->config->major].error = 0;

        I2C_t *i2c = get_I2C(hdl);
        CLEAR_BIT(i2c->CR2, I2C_CR2_ITBUFEN | I2C_CR2_ITERREN | I2C_CR2_ITEVTEN);
        SET_BIT(i2c->CR2, I2C_CR2_DMAEN);
        SET_BIT(i2c->CR2, I2C_CR2_LAST);
        SET_BIT(DMA->CCR, DMA_CCR1_EN);

        if (_sys_semaphore_wait(I2C->periph[hdl->config->major].event, device_timeout) == ESUCC) {
                if (I2C->periph[hdl->config->major].error == 0) {
                        return true;
                }
        } else {
                I2C->periph[hdl->config->major].error = EIO;
        }

        error(hdl);
        return false;
}
#endif

//==============================================================================
/**
 * @brief  Function generate START sequence on I2C bus
 * @param  hdl                  device handle
 * @return On success true is returned, otherwise false
 */
//==============================================================================
static bool start(I2C_dev_t *hdl)
{
        I2C_t *i2c = get_I2C(hdl);

        CLEAR_BIT(i2c->CR1, I2C_CR1_STOP);
        SET_BIT(i2c->CR1, I2C_CR1_START);

        return wait_for_I2C_event(hdl, I2C_SR1_SB);
}

//==============================================================================
/**
 * @brief  Function generate STOP sequence on I2C bus
 * @param  hdl                  device handle
 * @return On success true is returned, otherwise false
 */
//==============================================================================
static void stop(I2C_dev_t *hdl)
{
        I2C_t *i2c = get_I2C(hdl);

        CLEAR_BIT(i2c->CR1, I2C_CR1_START);
        SET_BIT(i2c->CR1, I2C_CR1_STOP);
}

//==============================================================================
/**
 * @brief  Function send I2C address sequence
 * @param  hdl                  device handle
 * @param  write                true: compose write address
 * @return On success true is returned, otherwise false
 */
//==============================================================================
static bool send_address(I2C_dev_t *hdl, bool write)
{
        I2C_t *i2c = get_I2C(hdl);

        if (hdl->config->addr10bit) {
                u8_t hdr = header_addr_10bit | ((hdl->address >> 7) & 0x6);

                // send header + 2 most significant bits of 10-bit address
                i2c->DR = header_addr_10bit | ((hdl->address & 0xFE) >> 7);
                if (!wait_for_I2C_event(hdl, I2C_SR1_ADD10)) {
                        return false;
                }

                // send rest 8 bits of 10-bit address
                u8_t  addr = hdl->address & 0xFF;
                u16_t tmp  = i2c->SR1;
                (void)tmp;   i2c->DR = addr;
                if (!wait_for_I2C_event(hdl, I2C_SR1_ADDR)) {
                        return false;
                }

                clear_send_address_event(hdl);

                // send repeat start
                if (!start(hdl)) {
                        return false;
                }

                // send header
                i2c->DR = write ? hdr : hdr | 0x01;
                return wait_for_I2C_event(hdl, I2C_SR1_ADDR);

        } else {
                u16_t tmp = i2c->SR1;
                (void)tmp;  i2c->DR = write ? hdl->address & 0xFE : hdl->address | 0x01;
                return wait_for_I2C_event(hdl, I2C_SR1_ADDR);
        }
}

//==============================================================================
/**
 * @brief  Clear event of send address
 * @param  hdl                  device handle
 * @return None
 */
//==============================================================================
static void clear_send_address_event(I2C_dev_t *hdl)
{
        I2C_t *i2c = get_I2C(hdl);

        _sys_critical_section_begin();
        u16_t tmp;
        tmp = i2c->SR1;
        tmp = i2c->SR2;
        (void)tmp;
        _sys_critical_section_end();
}

//==============================================================================
/**
 * @brief  Function send subaddress to I2C device
 * @param  hdl                  device handle
 * @param  address              subaddress
 * @param  mode                 size of subaddress
 * @return On success true is returned, otherwise false
 */
//==============================================================================
static bool send_subaddress(I2C_dev_t *hdl, u32_t address, I2C_sub_addr_mode_t mode)
{
        I2C_t *i2c = get_I2C(hdl);

        switch (mode) {
        case I2C_SUB_ADDR_MODE__3_BYTES:
                i2c->DR = address >> 16;
                if (!wait_for_I2C_event(hdl, I2C_SR1_BTF))
                        break;
                // if there is no error then send next bytes

        case I2C_SUB_ADDR_MODE__2_BYTES:
                i2c->DR = address >> 8;
                if (!wait_for_I2C_event(hdl, I2C_SR1_BTF))
                        break;
                // if there is no error then send next bytes

        case I2C_SUB_ADDR_MODE__1_BYTE:
                i2c->DR = address & 0xFF;
                return wait_for_I2C_event(hdl, I2C_SR1_BTF);

        default:
                return false;
        }

        return true;
}

//==============================================================================
/**
 * @brief  Function set ACK/NACK status according to transfer size
 * @param  hdl                  device handle
 * @param  count                transfer size (bytes)
 * @return None
 */
//==============================================================================
static void set_ACK_according_to_reception_size(I2C_dev_t *hdl, size_t count)
{
        I2C_t *i2c = get_I2C(hdl);

        if (count == 2) {
                SET_BIT(i2c->CR1, I2C_CR1_POS | I2C_CR1_ACK);
        } else {
                SET_BIT(i2c->CR1, I2C_CR1_ACK);
        }
}

//==============================================================================
/**
 * @brief  Function receive bytes from I2C bus (master-receiver)
 * @param  hdl                  device handle
 * @param  dst                  destination buffer
 * @param  count                number of bytes to receive
 * @return Number of received bytes
 */
//==============================================================================
static ssize_t receive(I2C_dev_t *hdl, u8_t *dst, size_t count)
{
        ssize_t n = 0;

        I2C_t *i2c = get_I2C(hdl);

        if (count >= 3) {
                clear_send_address_event(hdl);

#if ((_I2C1_ENABLE > 0) && (_I2C1_USE_DMA > 0)) || ((_I2C2_ENABLE > 0) && (_I2C2_USE_DMA > 0))
                if (I2C->periph[hdl->config->major].use_DMA) {
                        DMA_Channel_t *DMA = const_cast(DMA_Channel_t*, I2C_cfg[hdl->config->major].DMA_rx);

                        DMA->CPAR  = reinterpret_cast(u32_t, &i2c->DR);
                        DMA->CMAR  = reinterpret_cast(u32_t, dst);
                        DMA->CNDTR = count;
                        DMA->CCR   = DMA_CCR1_MINC | DMA_CCR1_TCIE | DMA_CCR1_TEIE;

                        if (wait_for_DMA_event(hdl, DMA)) {
                                n = count;
                        }
                } else {
#else
                {
#endif
                        while (count) {
                                if (count == 3) {
                                        if (!wait_for_I2C_event(hdl, I2C_SR1_BTF))
                                                break;

                                        CLEAR_BIT(i2c->CR1, I2C_CR1_ACK);
                                        *dst++ = i2c->DR;
                                        n++;

                                        if (!wait_for_I2C_event(hdl, I2C_SR1_RXNE))
                                                break;

                                        stop(hdl);
                                        *dst++ = i2c->DR;
                                        n++;

                                        if (!wait_for_I2C_event(hdl, I2C_SR1_RXNE))
                                                break;

                                        *dst++ = i2c->DR;
                                        n++;

                                        count = 0;
                                } else {
                                        if (wait_for_I2C_event(hdl, I2C_SR1_RXNE)) {
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
                _sys_critical_section_begin();
                clear_send_address_event(hdl);
                CLEAR_BIT(i2c->CR1, I2C_CR1_ACK);
                _sys_critical_section_end();

                if (wait_for_I2C_event(hdl, I2C_SR1_BTF)) {
                        stop(hdl);

                        *dst++ = i2c->DR;
                        *dst++ = i2c->DR;
                        n     += 2;
                }

        } else if (count == 1) {
                CLEAR_BIT(i2c->CR1, I2C_CR1_ACK);
                clear_send_address_event(hdl);
                stop(hdl);

                if (wait_for_I2C_event(hdl, I2C_SR1_RXNE)) {
                        *dst++ = i2c->DR;
                        n     += 1;
                }
        }

        return n;
}

//==============================================================================
/**
 * @brief  Function transmit selected amount bytes to I2C bus
 * @param  hdl                  device handle
 * @param  src                  data source
 * @param  count                number of bytes to transfer
 * @return Number of written bytes
 */
//==============================================================================
static ssize_t transmit(I2C_dev_t *hdl, const u8_t *src, size_t count)
{
        ssize_t n    = 0;
        I2C_t  *i2c  = get_I2C(hdl);
        bool    succ = false;

        clear_send_address_event(hdl);

#if ((_I2C1_ENABLE > 0) && (_I2C1_USE_DMA > 0)) || ((_I2C2_ENABLE > 0) && (_I2C2_USE_DMA > 0))
        if (count >= 3 && I2C->periph[hdl->config->major].use_DMA) {
                DMA_Channel_t *DMA = const_cast(DMA_Channel_t*, I2C_cfg[hdl->config->major].DMA_tx);

                DMA->CPAR  = reinterpret_cast(u32_t, &i2c->DR);
                DMA->CMAR  = reinterpret_cast(u32_t, src);
                DMA->CNDTR = count;
                DMA->CCR   = DMA_CCR1_MINC | DMA_CCR1_TCIE | DMA_CCR1_TEIE | DMA_CCR1_DIR;

                if (wait_for_DMA_event(hdl, DMA)) {
                        n    = count;
                        succ = true;
                }
        } else {
#else
        {
#endif
                succ = true;
                while (count) {
                        if (wait_for_I2C_event(hdl, I2C_SR1_TXE)) {
                                i2c->DR = *src++;
                        } else {
                                succ = false;
                                break;
                        }

                        n++;
                        count--;
                }
        }

        if (n && succ) {
                if (!wait_for_I2C_event(hdl, I2C_SR1_BTF))
                        return n - 1;
        }

        stop(hdl);

        return n;
}

//==============================================================================
/**
 * @brief  Event IRQ handler (transaction state machine)
 * @param  major        number of peripheral
 * @return If IRQ was woken then true is returned, otherwise false
 */
//==============================================================================
static void IRQ_EV_handler(u8_t major)
{
        if (I2C1->SR1 & I2C->periph[major].SR1_mask) {
                _sys_semaphore_signal_from_ISR(I2C->periph[major].event, NULL);
                CLEAR_BIT(I2C1->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN);
                I2C->periph[major].unexp_event_cnt = 0;
        } else {
                /*
                 * This counter is used to check if there is no death loop of
                 * not handled IRQ. If counter reach specified value then
                 * the error flag is set.
                 */
                if (++I2C->periph[major].unexp_event_cnt >= 10) {
                        I2C->periph[major].error = EIO;
                        _sys_semaphore_signal_from_ISR(I2C->periph[major].event, NULL);
                        CLEAR_BIT(I2C1->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN);
                }
        }

        _sys_thread_yield_from_ISR();
}

//==============================================================================
/**
 * @brief  Error IRQ handler
 * @param  major        number of peripheral
 * @return If IRQ was woken then true is returned, otherwise false
 */
//==============================================================================
static void IRQ_ER_handler(u8_t major)
{
        I2C_t *i2c = const_cast(I2C_t*, I2C_cfg[major].I2C);

        if (i2c->SR1 & I2C_SR1_ARLO) {
                I2C->periph[major].error = EAGAIN;

        } else if (i2c->SR1 & I2C_SR1_AF) {
                if (I2C->periph[major].SR1_mask & (I2C_SR1_ADDR | I2C_SR1_ADD10))
                        I2C->periph[major].error = ENXIO;
                else
                        I2C->periph[major].error = EIO;
        } else {
                I2C->periph[major].error = EIO;
        }

        // clear error flags
        i2c->SR1 = 0;

        _sys_semaphore_signal_from_ISR(I2C->periph[major].event, NULL);
        CLEAR_BIT(I2C1->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN);
        _sys_thread_yield_from_ISR();
}

//==============================================================================
/**
 * @brief  DMA IRQ handler
 * @param  DMA_ch_no    DMA channel number
 * @param  major        number of peripheral
 * @return If IRQ was woken then true is returned, otherwise false
 */
//==============================================================================
#if ((_I2C1_ENABLE > 0) && (_I2C1_USE_DMA > 0)) || ((_I2C2_ENABLE > 0) && (_I2C2_USE_DMA > 0))
static void IRQ_DMA_handler(const int DMA_ch_no, u8_t major)
{
        if (DMA1->ISR & (DMA_ISR_TEIF1 << (4 * (DMA_ch_no - 1)))) {
                I2C->periph[major].error = EIO;
        }

        _sys_semaphore_signal_from_ISR(I2C->periph[major].event, NULL);
        clear_DMA_IRQ_flags(major);
        _sys_thread_yield_from_ISR();
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
        IRQ_EV_handler(_I2C1);
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
        IRQ_ER_handler(_I2C1);
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
        IRQ_EV_handler(_I2C2);
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
        IRQ_ER_handler(_I2C2);
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
        IRQ_DMA_handler(I2C_cfg[_I2C1].DMA_tx_number, _I2C1);
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
        IRQ_DMA_handler(I2C_cfg[_I2C1].DMA_rx_number, _I2C1);
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
        IRQ_DMA_handler(I2C_cfg[_I2C2].DMA_tx_number, _I2C2);
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
        IRQ_DMA_handler(I2C_cfg[_I2C2].DMA_rx_number, _I2C2);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
