/*=========================================================================*//**
@file    gpio.c

@author  Daniel Zorychta

@brief   This driver support GPIO. GPIO driver not provide any interface
         functions. All operations on ports should be made direct via definitions
         (much faster). When operation on a ports are needed please write own
         driver which controls pins directly and register it in the VFS if
         needed.

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "efr32/gpio_cfg.h"
#include "efr32/gpio_macros.h"
#include "efr32/efr32xx.h"
#include "efr32/lib/em_cmu.h"
#include "../gpio_ioctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/** define number of pins per port */
#define PINS_PER_PORT                   16

/** define 4 bit field pin shift (0-7) */
#define GPIO_SET_F4L(CFG, PIN)          ( (CFG) << (4 * (PIN)) )

/** define 4 bit field pin shift (8-15) */
#define GPIO_SET_F4H(CFG, PIN)          ( (CFG) << (4 * ((PIN) - 8)) )

/** CTRL register configuration */
#define GPIO_Px_CTRL(port) (((_##port##_DRIVE_STRENGTH     & 1) << _GPIO_P_CTRL_DRIVESTRENGTH_SHIFT   )\
                           |((_##port##_DRIVE_STRENGTH_ALT & 1) << _GPIO_P_CTRL_DRIVESTRENGTHALT_SHIFT)\
                           |((_##port##_SLEW_RATE          & 7) << _GPIO_P_CTRL_SLEWRATE_SHIFT        )\
                           |((_##port##_SLEW_RATE_ALT      & 7) << _GPIO_P_CTRL_SLEWRATEALT_SHIFT     )\
                           |((_##port##_INPUT_DISABLE      & 1) << _GPIO_P_CTRL_DINDIS_SHIFT          )\
                           |((_##port##_INPUT_DISABLE_ALT  & 1) << _GPIO_P_CTRL_DINDISALT_SHIFT       ))

/** MODEL register configuration */
#define GPIOx_MODEL(port) ( GPIO_SET_F4L(_##port##_PIN_0_MODE, 0 ) | GPIO_SET_F4L(_##port##_PIN_1_MODE, 1 ) \
                          | GPIO_SET_F4L(_##port##_PIN_2_MODE, 2 ) | GPIO_SET_F4L(_##port##_PIN_3_MODE, 3 ) \
                          | GPIO_SET_F4L(_##port##_PIN_4_MODE, 4 ) | GPIO_SET_F4L(_##port##_PIN_5_MODE, 5 ) \
                          | GPIO_SET_F4L(_##port##_PIN_6_MODE, 6 ) | GPIO_SET_F4L(_##port##_PIN_7_MODE, 7 ) )

/** MODEH register configuration */
#define GPIOx_MODEH(port) ( GPIO_SET_F4H(_##port##_PIN_8_MODE ,  8) | GPIO_SET_F4H(_##port##_PIN_9_MODE ,  9) \
                          | GPIO_SET_F4H(_##port##_PIN_10_MODE, 10) | GPIO_SET_F4H(_##port##_PIN_11_MODE, 11) \
                          | GPIO_SET_F4H(_##port##_PIN_12_MODE, 12) | GPIO_SET_F4H(_##port##_PIN_13_MODE, 13) \
                          | GPIO_SET_F4H(_##port##_PIN_14_MODE, 14) | GPIO_SET_F4H(_##port##_PIN_15_MODE, 15) )

/** ODR register value for GPIO */
#define GPIOx_DOUT(port) ( (_##port##_PIN_0_STATE  <<  0) | (_##port##_PIN_1_STATE  <<  1) \
                         | (_##port##_PIN_2_STATE  <<  2) | (_##port##_PIN_3_STATE  <<  3) \
                         | (_##port##_PIN_4_STATE  <<  4) | (_##port##_PIN_5_STATE  <<  5) \
                         | (_##port##_PIN_6_STATE  <<  6) | (_##port##_PIN_7_STATE  <<  7) \
                         | (_##port##_PIN_8_STATE  <<  8) | (_##port##_PIN_9_STATE  <<  9) \
                         | (_##port##_PIN_10_STATE << 10) | (_##port##_PIN_11_STATE << 11) \
                         | (_##port##_PIN_12_STATE << 12) | (_##port##_PIN_13_STATE << 13) \
                         | (_##port##_PIN_14_STATE << 14) | (_##port##_PIN_15_STATE << 15) )

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct GPIO_reg {
        uint32_t CTRL;
        uint32_t MODEL;
        uint32_t MODEH;
        uint32_t DOUT;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
static const struct GPIO_reg GPIOx[] = {
        {.CTRL = GPIO_Px_CTRL(GPIOA), .MODEL = GPIOx_MODEL(GPIOA), .MODEH = GPIOx_MODEH(GPIOA), .DOUT = GPIOx_DOUT(GPIOA)},
        {.CTRL = GPIO_Px_CTRL(GPIOB), .MODEL = GPIOx_MODEL(GPIOB), .MODEH = GPIOx_MODEH(GPIOB), .DOUT = GPIOx_DOUT(GPIOB)},
        {.CTRL = GPIO_Px_CTRL(GPIOC), .MODEL = GPIOx_MODEL(GPIOC), .MODEH = GPIOx_MODEH(GPIOC), .DOUT = GPIOx_DOUT(GPIOC)},
        {.CTRL = GPIO_Px_CTRL(GPIOD), .MODEL = GPIOx_MODEL(GPIOD), .MODEH = GPIOx_MODEH(GPIOD), .DOUT = GPIOx_DOUT(GPIOD)},
        {.CTRL = GPIO_Px_CTRL(GPIOE), .MODEL = GPIOx_MODEL(GPIOE), .MODEH = GPIOx_MODEH(GPIOE), .DOUT = GPIOx_DOUT(GPIOE)},
        {.CTRL = GPIO_Px_CTRL(GPIOF), .MODEL = GPIOx_MODEL(GPIOF), .MODEH = GPIOx_MODEH(GPIOF), .DOUT = GPIOx_DOUT(GPIOF)},
        {.CTRL = GPIO_Px_CTRL(GPIOG), .MODEL = GPIOx_MODEL(GPIOG), .MODEH = GPIOx_MODEH(GPIOG), .DOUT = GPIOx_DOUT(GPIOG)},
        {.CTRL = GPIO_Px_CTRL(GPIOH), .MODEL = GPIOx_MODEL(GPIOH), .MODEH = GPIOx_MODEH(GPIOH), .DOUT = GPIOx_DOUT(GPIOH)},
        {.CTRL = GPIO_Px_CTRL(GPIOI), .MODEL = GPIOx_MODEL(GPIOI), .MODEH = GPIOx_MODEH(GPIOI), .DOUT = GPIOx_DOUT(GPIOI)},
        {.CTRL = GPIO_Px_CTRL(GPIOJ), .MODEL = GPIOx_MODEL(GPIOJ), .MODEH = GPIOx_MODEH(GPIOJ), .DOUT = GPIOx_DOUT(GPIOJ)},
        {.CTRL = GPIO_Px_CTRL(GPIOK), .MODEL = GPIOx_MODEL(GPIOK), .MODEH = GPIOx_MODEH(GPIOK), .DOUT = GPIOx_DOUT(GPIOK)},
};

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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(GPIO, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        if (major < ARRAY_SIZE(GPIOx) && minor == 0) {

                CMU_ClockEnable(cmuClock_GPIO, true);

                GPIO->P[major].CTRL  = GPIOx[major].CTRL;
                GPIO->P[major].DOUT  = GPIOx[major].DOUT;
                GPIO->P[major].MODEL = GPIOx[major].MODEL;
                GPIO->P[major].MODEH = GPIOx[major].MODEH;

                *device_handle = const_cast(void*, &GPIO->P[major]);

                return ESUCC;
        } else {
                return ENODEV;
        }
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
API_MOD_RELEASE(GPIO, void *device_handle)
{
        GPIO_P_TypeDef *hdl = device_handle;

        hdl->MODEL = 0;
        hdl->MODEH = 0;
        hdl->DOUT  = 0;
        hdl->CTRL  = 0;

        return ESUCC;
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
API_MOD_OPEN(GPIO, void *device_handle, u32_t flags)
{
        UNUSED_ARG2(device_handle, flags);

        return ESUCC;
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
API_MOD_CLOSE(GPIO, void *device_handle, bool force)
{
        UNUSED_ARG2(device_handle, force);

        return ESUCC;
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
API_MOD_WRITE(GPIO,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG2(fpos, fattr);

        GPIO_P_TypeDef *hdl = device_handle;

        if (count >= 2) {
                for (size_t i = 0; i < count / 2; i++) {
                        hdl->DOUT = cast(u16_t*, src)[i];
                }

                *wrcnt = count & ~(1);
        }

        return ESUCC;
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
API_MOD_READ(GPIO,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG2(fpos, fattr);

        GPIO_P_TypeDef *hdl = device_handle;

        if (count >= 2) {
                for (size_t i = 0; i < count / 2; i++) {
                        cast(u16_t*, dst)[i] = hdl->DIN;
                }

                *rdcnt = count & ~(1);
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return On success return 0. On error, -1 is returned.
 */
//==============================================================================
API_MOD_IOCTL(GPIO, void *device_handle, int request, void *arg)
{
        int err = EINVAL;

        GPIO_P_TypeDef *hdl = device_handle;

        if (arg) {
                switch (request) {
                case IOCTL_GPIO__SET_PIN: {
                        u8_t pin = *cast(u8_t*, arg);
                        if (pin < PINS_PER_PORT) {
                                hdl->DOUT |= (1 << pin);
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__CLEAR_PIN: {
                        u8_t pin = *cast(u8_t*, arg);
                        if (pin < PINS_PER_PORT) {
                                hdl->DOUT &= ~(1 << pin);
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__TOGGLE_PIN: {
                        u8_t pin = *cast(u8_t*, arg);
                        if (pin < PINS_PER_PORT) {
                                hdl->DOUTTGL = (1 << pin);
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__SET_PIN_STATE: {
                        GPIO_pin_state_t *pinstate = arg;
                        if (pinstate->pin_idx < PINS_PER_PORT) {
                                if (pinstate->state) {
                                        hdl->DOUT |=  (1 << pinstate->pin_idx);
                                } else {
                                        hdl->DOUT &= ~(1 << pinstate->pin_idx);
                                }
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__GET_PIN_STATE: {
                        GPIO_pin_state_t *pinstate = arg;
                        if (pinstate->pin_idx < PINS_PER_PORT) {
                                pinstate->state = (hdl->DIN & (1 << pinstate->pin_idx)) ? 1 : 0;
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__SET_PIN_IN_PORT: {
                        GPIO_pin_in_port_t *pin = arg;
                        if ((pin->port_idx < ARRAY_SIZE(GPIOx)) && (pin->pin_idx < 16)) {
                                GPIO->P[pin->port_idx].DOUT |= (1 << pin->pin_idx);
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__CLEAR_PIN_IN_PORT: {
                        GPIO_pin_in_port_t *pin = arg;
                        if ((pin->port_idx < ARRAY_SIZE(GPIOx)) && (pin->pin_idx < 16)) {
                                GPIO->P[pin->port_idx].DOUT &= ~(1 << pin->pin_idx);
                        }
                        break;
                }

                case IOCTL_GPIO__TOGGLE_PIN_IN_PORT: {
                        GPIO_pin_in_port_t *pin = arg;
                        if ((pin->port_idx < ARRAY_SIZE(GPIOx)) && (pin->pin_idx < 16)) {
                                GPIO->P[pin->port_idx].DOUTTGL = (1 << pin->pin_idx);
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__SET_PIN_STATE_IN_PORT: {
                        GPIO_pin_in_port_state_t *pin = arg;
                        if ((pin->port_idx < ARRAY_SIZE(GPIOx)) && (pin->pin_idx < 16)) {
                                if (pin->state) {
                                        GPIO->P[pin->port_idx].DOUT |= (1 << pin->pin_idx);
                                } else {
                                        GPIO->P[pin->port_idx].DOUT &= ~(1 << pin->pin_idx);
                                }
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__GET_PIN_STATE_IN_PORT: {
                        GPIO_pin_in_port_state_t *pin = arg;
                        if ((pin->port_idx < ARRAY_SIZE(GPIOx)) && (pin->pin_idx < 16)) {
                                pin->state = (hdl->DIN & (1 << pin->pin_idx)) ? 1 : 0;
                                err = ESUCC;
                        }
                        break;
                }

                default:
                        err = EBADRQC;
                        break;
                }
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
API_MOD_FLUSH(GPIO, void *device_handle)
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
API_MOD_STAT(GPIO, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_size = 2;

        return ESUCC;
}

//==============================================================================
/**
 * @brief Function set pin to 1.
 * @param port_idx      Port index
 * @param pin_idx       Pin index
 */
//==============================================================================
void _GPIO_DDI_set_pin(u8_t port_idx, u8_t pin_idx)
{
        if ((port_idx < ARRAY_SIZE(GPIOx)) && (pin_idx < 16)) {
                GPIO->P[port_idx].DOUT |= (1 << pin_idx);
        }
}

//==============================================================================
/**
 * @brief Function clear pin to 0.
 * @param port_idx      Port index.
 * @param pin_idx       Pin index.
 */
//==============================================================================
void _GPIO_DDI_clear_pin(u8_t port_idx, u8_t pin_idx)
{
        if ((port_idx < ARRAY_SIZE(GPIOx)) && (pin_idx < 16)) {
                GPIO->P[port_idx].DOUT &= ~(1 << pin_idx);
        }
}

//==============================================================================
/**
 * @brief  Function get pin state.
 * @param  port_idx      Port index.
 * @param  pin_idx       Pin index.
 * @return Pin value. On error -1.
 */
//==============================================================================
i8_t _GPIO_DDI_get_pin(u8_t port_idx, u8_t pin_idx)
{
        if ((port_idx < ARRAY_SIZE(GPIOx)) && (pin_idx < 16)) {
                return GPIO->P[port_idx].DIN & (1 << pin_idx) ? 1 : 0;
        } else {
                return -1;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
