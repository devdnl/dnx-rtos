/*=========================================================================*//**
@file    usbd.c

@author  Daniel Zorychta

@brief   USB-Device module.

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
#include "drivers/driver.h"
#include "stm32f1/stm32f10x.h"
#include "usbd_cfg.h"
#include "usbd_def.h"
#include "usbd_ioctl.h"
#include "stm32f1/gpio_cfg.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define BTABLE_ADDRESS          0
#define BTABLE_SIZE             32      /* number of uin16_t words */
#define PMA_SIZE                256     /* number of uin16_t words */
#define USB_PMA                 ((usb_pma_t*)((uint32_t)USB_PMA_BASE + BTABLE_ADDRESS))
#define NUMBER_OF_ENDPOINTS     _USBD_NUMBER_OF_ENDPOINTS

#define USB_EPR_EA              USB_EP0R_EA
#define USB_EPR_STAT_TX         USB_EP0R_STAT_TX
#define USB_EPR_STAT_TX_0       USB_EP0R_STAT_TX_0
#define USB_EPR_STAT_TX_1       USB_EP0R_STAT_TX_1
#define USB_EPR_DTOG_TX         USB_EP0R_DTOG_TX
#define USB_EPR_CTR_TX          USB_EP0R_CTR_TX
#define USB_EPR_EP_KIND         USB_EP0R_EP_KIND
#define USB_EPR_EP_TYPE         USB_EP0R_EP_TYPE
#define USB_EPR_EP_TYPE_0       USB_EP0R_EP_TYPE_0
#define USB_EPR_EP_TYPE_1       USB_EP0R_EP_TYPE_1
#define USB_EPR_SETUP           USB_EP0R_SETUP
#define USB_EPR_STAT_RX         USB_EP0R_STAT_RX
#define USB_EPR_STAT_RX_0       USB_EP0R_STAT_RX_0
#define USB_EPR_STAT_RX_1       USB_EP0R_STAT_RX_1
#define USB_EPR_DTOG_RX         USB_EP0R_DTOG_RX
#define USB_EPR_CTR_RX          USB_EP0R_CTR_RX

/*==============================================================================
  Local object types
==============================================================================*/
/**
 * Endpoint statuses
 */
typedef enum {
        USB_EP_STATUS__DISABLED,
        USB_EP_STATUS__STALL,
        USB_EP_STATUS__NAK,
        USB_EP_STATUS__VALID
} usb_ep_status_t;

/**
 * PMA buffer definition
 */
typedef struct {
        union {
                struct {
                        u32_t ADDR_TX;
                        u32_t COUNT_TX;
                        u32_t ADDR_RX;
                        u32_t COUNT_RX;
                } SBF;

                struct {
                        u32_t ADDR_0;
                        u32_t COUNT_0;
                        u32_t ADDR_1;
                        u32_t COUNT_1;
                } DBF_RX;

                struct {
                        u32_t ADDR_0;
                        u32_t COUNT_0;
                        u32_t ADDR_1;
                        u32_t COUNT_1;
                } DBF_TX;
        } EP[NUMBER_OF_ENDPOINTS];

        u32_t BUFFER[PMA_SIZE - BTABLE_SIZE];
} usb_pma_t;

/**
 * single endpoint object
 */
typedef struct {
        sem_t                  *tx;                     /* interrupt/setup transfer completed semaphore */
        sem_t                  *rx;                     /* interrupt/setup transfer completed semaphore */
        sem_t                  *setup;                  /* setup transfer completed semaphore */
        dev_lock_t              dev_lock;               /* device lock object */
        volatile bool           setup_in_progress :1;   /* flag that indicate setup in progress */
        volatile bool           read_in_progress  :1;   /* flag that indicate read in progress */
        volatile bool           write_in_progress :1;   /* flag that indicate write in progress */
        u8_t                    minor;                  /* endpoint number */
} USB_ep_t;

/**
 * General module variables
 */
typedef struct {
        USB_ep_t               *ep[NUMBER_OF_ENDPOINTS];
        const usbd_ep_config_t *ep_config;
        u8_t                    major;
        bool                    activated;
        bool                    reset;
        int                     error_cnt;
} USB_mem_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static inline void enable_usb_visible_pullup    ();
static inline void disable_usb_visible_pullup   ();
static void        set_ep_tx_status             (usb_ep_num_t ep, usb_ep_status_t status);
static void        set_ep_rx_status             (usb_ep_num_t ep, usb_ep_status_t status);
static void        disable_endpoint             (usb_ep_num_t ep);
static u16_t       endpoint_size_to_register    (u16_t size);
static void        configure_endpoint_0         ();
static bool        configure_endpoints_1_7      ();
static void        low_level_pma_write          (u32_t pma_offset, const u8_t *buffer, size_t count);
static void        low_level_pma_read           (u32_t pma_offset, u8_t *buffer, size_t count);
static bool        send_ZLP                     (usb_ep_num_t ep);
static size_t      get_ep_tx_buf_size           (usb_ep_num_t ep);
static size_t      get_ep_rx_buf_size           (usb_ep_num_t ep);
static size_t      get_ep_received_size         (usb_ep_num_t ep);
static void        set_setup_in_progress        (usb_ep_num_t ep, bool state);
static void        set_write_in_progress        (usb_ep_num_t ep, bool state);
static void        set_read_in_progress         (usb_ep_num_t ep, bool state);

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(USBD);

static USB_mem_t *usb_mem;

static const uint16_t ep_tx_status_reg[] = {
        [USB_EP_STATUS__DISABLED] = 0,
        [USB_EP_STATUS__STALL   ] = (USB_EP0R_STAT_TX_0),
        [USB_EP_STATUS__NAK     ] = (USB_EP0R_STAT_TX_1),
        [USB_EP_STATUS__VALID   ] = (USB_EP0R_STAT_TX_1 | USB_EP0R_STAT_TX_0)
};

static const uint16_t ep_rx_status_reg[] = {
        [USB_EP_STATUS__DISABLED] = 0,
        [USB_EP_STATUS__STALL   ] = (USB_EP0R_STAT_RX_0),
        [USB_EP_STATUS__NAK     ] = (USB_EP0R_STAT_RX_1),
        [USB_EP_STATUS__VALID   ] = (USB_EP0R_STAT_RX_1 | USB_EP0R_STAT_RX_0)
};

static const uint16_t transfer_type_reg[] = {
        [USB_TRANSFER__BULK       ] = 0,
        [USB_TRANSFER__CONTROL    ] = (USB_EP0R_EP_TYPE_0),
        [USB_TRANSFER__ISOCHRONOUS] = (USB_EP0R_EP_TYPE_1),
        [USB_TRANSFER__INTERRUPT  ] = (USB_EP0R_EP_TYPE_1 | USB_EP0R_EP_TYPE_0)
};

static const uint ep0_status_stage_timeout_ms          = 5;
static const uint ep0_data_stage_transmit_timeout_ms   = 2000;
static const uint ep0_data_stage_receive_timeout_ms    = MAX_DELAY_MS;
static const uint ep1_7_data_stage_transmit_timeout_ms = 4500;


/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
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
API_MOD_INIT(USBD, void **device_handle, u8_t major, u8_t minor)
{
        if (major == _USBD_MAJOR_NUMBER && minor <= _USBD_MINOR_NUMBER_EP_7) {

                /* allocate main USB memory */
                if (usb_mem == NULL) {
                        if (RCC->APB1ENR & RCC_APB1ENR_CAN1EN) {
                                return EADDRINUSE;
                        }

                        USB_mem_t *hdl = calloc(1, sizeof(USB_mem_t));
                        if (hdl) {
                                /* enable USB clock */
                                SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USBRST);
                                CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USBRST);
                                SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USBEN);

                                /* enable USB interrupts in NVIC */
                                NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, _USBD_IRQ_PRIORITY);
                                NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

                                /* set allocated memory */
                                *device_handle = hdl;
                                usb_mem        = hdl;
                                hdl->activated = false;
                                hdl->major     = major;
                        } else {
                                return ENOMEM;
                        }
                }

                /* allocate Endpoint memory */
                if (usb_mem != NULL) {
                        sem_t    *setup_sem = (minor == USB_EP_NUM__ENDP0 ? _sys_semaphore_new(1, 0) : NULL);
                        sem_t    *rx_sem    = _sys_semaphore_new(1, 0);
                        sem_t    *tx_sem    = _sys_semaphore_new(1, 0);
                        USB_ep_t *ep_hdl    = calloc(1, sizeof(USB_ep_t));

                        if (ep_hdl && tx_sem && rx_sem && (setup_sem || minor != USB_EP_NUM__ENDP0)) {

                                if (minor == USB_EP_NUM__ENDP0) {
                                        _sys_semaphore_wait(setup_sem, 0);
                                }

                                ep_hdl->minor        = minor;
                                ep_hdl->rx           = rx_sem;
                                ep_hdl->tx           = tx_sem;
                                ep_hdl->setup        = setup_sem;
                                usb_mem->ep[minor]   = ep_hdl;
                                *device_handle       = ep_hdl;

                                return ESUCC;
                        } else {
                                if (rx_sem) {
                                        _sys_semaphore_delete(rx_sem);
                                }

                                if (tx_sem) {
                                        _sys_semaphore_delete(tx_sem);
                                }

                                if (setup_sem) {
                                        _sys_semaphore_delete(setup_sem);
                                }

                                if (ep_hdl) {
                                        free(ep_hdl);
                                }

                                return ENOMEM;
                        }
                }
        }

        return ENODEV;
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
API_MOD_RELEASE(USBD, void *device_handle)
{
        if (usb_mem->activated == false) {
                USB_ep_t *ep_hdl = device_handle;

                disable_endpoint(ep_hdl->minor);

                _sys_semaphore_delete(ep_hdl->rx);
                ep_hdl->rx = NULL;

                _sys_semaphore_delete(ep_hdl->tx);
                ep_hdl->tx = NULL;

                if (ep_hdl->setup) {
                        _sys_semaphore_delete(ep_hdl->setup);
                        ep_hdl->setup = NULL;
                }

                usb_mem->ep[ep_hdl->minor] = NULL;
                free(ep_hdl);

                /* find if all endpoints are disabled */
                for (int i = _USBD_MINOR_NUMBER_EP_0; i <= _USBD_MINOR_NUMBER_EP_7; i++) {
                        if (usb_mem->ep[i] != NULL) {
                                return ESUCC;
                        }
                }

                /* deinitializes enitre module because all endpoints are disabled */
                /* disable USB IRQs */
                NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);

                /* disable USB clock */
                SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USBRST);
                CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USBRST);
                CLEAR_BIT(RCC->APB1ENR, RCC_APB1ENR_USBEN);

                /* free USB memory */
                free(usb_mem);
                usb_mem = NULL;

                return ESUCC;
        } else {
                return EBUSY;
        }
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
API_MOD_OPEN(USBD, void *device_handle, u32_t flags)
{
        UNUSED_ARG(flags);

        USB_ep_t *hdl = device_handle;

        return _sys_device_lock(&hdl->dev_lock) ? ESUCC : EBUSY;
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
API_MOD_CLOSE(USBD, void *device_handle, bool force)
{
        USB_ep_t *hdl = device_handle;

        if (_sys_device_is_access_granted(&hdl->dev_lock) || force) {
                _sys_device_unlock(&hdl->dev_lock, force);
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
API_MOD_WRITE(USBD,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        USB_ep_t *hdl = device_handle;
        int       status;

        if (_sys_device_is_access_granted(&hdl->dev_lock) && usb_mem->activated) {
                bool   IN_enabled = false;
                size_t ep_size    = 0;

                if (hdl->minor == USB_EP_NUM__ENDP0) {
                        IN_enabled = true;
                        ep_size    = _USBD_ENDPOINT0_SIZE;
                } else if (usb_mem->ep_config) {
                        IN_enabled = usb_mem->ep_config->ep[hdl->minor].IN_enabled;
                        ep_size    = get_ep_tx_buf_size(hdl->minor);
                }

                if (IN_enabled && ep_size) {

                        set_write_in_progress(hdl->minor, true);

                        *wrcnt = 0;

                        if (hdl->minor == USB_EP_NUM__ENDP0) {
                                size_t len = 0;

                                while (count) {
                                        /* adjust endpoint size */
                                        len = (count > ep_size) ? ep_size : count;

                                        /* check if send or status stage are skipped by SETUP packet*/
                                        if (_sys_semaphore_wait(hdl->setup, 0) == true) {
                                                _sys_semaphore_signal(hdl->setup);
                                                goto write_end;
                                        }

                                        /* send data */
                                        low_level_pma_write(USB_PMA->EP[hdl->minor].SBF.ADDR_TX, src, len);
                                        USB_PMA->EP[hdl->minor].SBF.COUNT_TX = len;
                                        set_ep_tx_status(hdl->minor, USB_EP_STATUS__VALID);
                                        set_ep_rx_status(hdl->minor, USB_EP_STATUS__VALID);

                                        /* wait for data sent */
                                        if (_sys_semaphore_wait(hdl->tx, ep0_data_stage_transmit_timeout_ms) != true) {
                                                set_ep_tx_status(hdl->minor, USB_EP_STATUS__STALL);
                                                set_ep_rx_status(hdl->minor, USB_EP_STATUS__VALID);
                                                goto write_end;
                                        } else {
                                                if (hdl->write_in_progress == false) {
                                                        goto write_end;
                                                }
                                        }

                                        *wrcnt += len;
                                        src    += len;
                                        count  -= len;

                                        /* check Status Stage */
                                        if (_sys_semaphore_wait(hdl->rx, ep0_status_stage_timeout_ms) == true) {
                                                goto write_end;
                                        } else {
                                                if (_sys_semaphore_wait(hdl->setup, 0) == true) {
                                                        _sys_semaphore_signal(hdl->setup);
                                                        goto write_end;
                                                }
                                        }
                                }

                                /* send Zero-Length Packet */
                                if (len == ep_size) {
                                        send_ZLP(hdl->minor);
                                }
                        } else {
                                while (count) {
                                        size_t len = (count > ep_size) ? ep_size : count;

                                        low_level_pma_write(USB_PMA->EP[hdl->minor].SBF.ADDR_TX, src, len);
                                        USB_PMA->EP[hdl->minor].SBF.COUNT_TX = len;
                                        set_ep_tx_status(hdl->minor, USB_EP_STATUS__VALID);

                                        if (_sys_semaphore_wait(hdl->tx, MAX_DELAY_MS) != true) {
                                                set_ep_tx_status(hdl->minor, USB_EP_STATUS__NAK);
                                                break;
                                        } else {
                                                if (hdl->write_in_progress == false) {
                                                        break;
                                                }
                                        }

                                        *wrcnt += len;
                                        src    += len;
                                        count  -= len;
                                }
                        }

                        write_end:
                        set_write_in_progress(hdl->minor, false);
                        status = ESUCC;

                } else {
                        status = EPERM;
                }
        } else {
                status = EACCES;
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
API_MOD_READ(USBD,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        USB_ep_t *hdl = device_handle;
        int       status;

        if (_sys_device_is_access_granted(&hdl->dev_lock) && usb_mem->activated) {
                bool   OUT_enabled = false;
                size_t ep_size     = 0;

                if (hdl->minor == USB_EP_NUM__ENDP0) {
                        OUT_enabled = true;
                        ep_size     = _USBD_ENDPOINT0_SIZE;
                } else if (usb_mem->ep_config) {
                        OUT_enabled = usb_mem->ep_config->ep[hdl->minor].OUT_enabled;
                        ep_size     = get_ep_rx_buf_size(hdl->minor);
                }

                if (OUT_enabled && ep_size) {

                        set_read_in_progress(hdl->minor, true);

                        *rdcnt = 0;

                        if (hdl->minor == USB_EP_NUM__ENDP0) {
                                while (count) {
                                        /* wait for OUT token and set ZLP in case of status stage */
                                        USB_PMA->EP[hdl->minor].SBF.COUNT_TX = 0;
                                        set_ep_tx_status(hdl->minor, USB_EP_STATUS__VALID);
                                        set_ep_rx_status(hdl->minor, USB_EP_STATUS__VALID);

                                        if (_sys_semaphore_wait(hdl->rx, ep0_data_stage_receive_timeout_ms) != true) {
                                                break;
                                        } else {
                                                if (hdl->read_in_progress == false) {
                                                        break;
                                                }
                                        }

                                        /* copy data to user's buffer */
                                        size_t len = (count > ep_size) ? ep_size : count;

                                        if (len > get_ep_received_size(hdl->minor)) {
                                                len = get_ep_received_size(hdl->minor);
                                        }

                                        low_level_pma_read(USB_PMA->EP[hdl->minor].SBF.ADDR_RX, dst, len);

                                        dst    += len;
                                        count  -= len;
                                        *rdcnt += len;

                                        /* check status stage */
                                        if (_sys_semaphore_wait(hdl->tx, ep0_status_stage_timeout_ms) == true) {
                                                break;
                                        }
                                }
                        } else {
                                while (count) {
                                        /* wait for OUT token */
                                        set_ep_rx_status(hdl->minor, USB_EP_STATUS__VALID);

                                        if (_sys_semaphore_wait(hdl->rx, MAX_DELAY_MS) != true) {
                                                break;
                                        } else {
                                                if (hdl->read_in_progress == false) {
                                                        break;
                                                }
                                        }

                                        /* copy data to user's buffer */
                                        size_t len = (count > ep_size) ? ep_size : count;

                                        if (len > get_ep_received_size(hdl->minor)) {
                                                len = get_ep_received_size(hdl->minor);
                                        }

                                        low_level_pma_read(USB_PMA->EP[hdl->minor].SBF.ADDR_RX, dst, len);

                                        dst    += len;
                                        count  -= len;
                                        *rdcnt += len;

                                        if (len < ep_size) {
                                                break;
                                        }
                                }
                        }

                        set_read_in_progress(hdl->minor, false);

                        status = ESUCC;

                } else {
                        status = EPERM;
                }
        } else {
                status = EACCES;
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
API_MOD_IOCTL(USBD, void *device_handle, int request, void *arg)
{
        USB_ep_t *hdl    = device_handle;
        int       status = EBADRQC;

        switch (request) {
        case IOCTL_USBD__START:
                if (usb_mem->activated == false) {
                        /* reset USB peripherial */
                        SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USBRST);
                        CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USBRST);

                        /* enable USB transceiver */
                        CLEAR_BIT(USB->CNTR, USB_CNTR_PDWN | USB_CNTR_FRES);
                        _sys_sleep_ms(1);

                        /* clear USB IRQ flags */
                        USB->ISTR = 0;

                        /* enable specified USB interrupts */
                        SET_BIT(USB->CNTR, USB_CNTR_CTRM | USB_CNTR_RESETM | USB_CNTR_ERRM /*| USB_CNTR_SOFM*/);

                        usb_mem->activated = true;

                        enable_usb_visible_pullup();

                        while (usb_mem->reset != true) {
                                _sys_sleep_ms(5);
                        }

                        if (_sys_semaphore_wait(usb_mem->ep[USB_EP_NUM__ENDP0]->setup, 1000) == true) {
                                _sys_semaphore_signal(usb_mem->ep[USB_EP_NUM__ENDP0]->setup);
                        }

                        usb_mem->reset = false;

                        status = ESUCC;
                } else {
                        status = ECANCELED;
                }
                break;

        case IOCTL_USBD__STOP:
                if (usb_mem->activated) {
                        usb_mem->activated = false;

                        disable_usb_visible_pullup();

                        /* reset USB peripherial */
                        SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USBRST);
                        CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USBRST);

                        /* clear semaphores */
                        for (usb_ep_num_t ep = USB_EP_NUM__ENDP0; ep < NUMBER_OF_ENDPOINTS; ep++) {
                                if (usb_mem->ep[ep]) {
                                        if (usb_mem->ep[ep]->rx) {
                                                _sys_semaphore_wait(usb_mem->ep[ep]->rx, 0);
                                        }

                                        if (usb_mem->ep[ep]->tx) {
                                                _sys_semaphore_wait(usb_mem->ep[ep]->tx, 0);
                                        }

                                        if (usb_mem->ep[ep]->setup) {
                                                _sys_semaphore_wait(usb_mem->ep[ep]->setup, 0);
                                        }

                                        usb_mem->ep[ep]->read_in_progress  = false;
                                        usb_mem->ep[ep]->setup_in_progress = false;
                                        usb_mem->ep[ep]->write_in_progress = false;
                                }
                        }

                        usb_mem->ep_config = NULL;

                        status = ESUCC;
                } else {
                        status = ECANCELED;
                }
                break;

        case IOCTL_USBD__CONFIGURE_EP_1_7:
                if (arg) {
                        if (usb_mem->activated) {
                                usb_mem->ep_config = arg;
                                status = configure_endpoints_1_7() ? ESUCC : EIO;
                        } else {
                                status = EBUSY;
                        }
                } else {
                        status = EINVAL;
                }
                break;

        case IOCTL_USBD__SET_ADDRESS:
                if (usb_mem->activated) {
                        USB->DADDR = USB_DADDR_EF | ((int)arg & USB_DADDR_ADD);
                        status = ESUCC;
                } else {
                        status = ECANCELED;
                }
                break;

        case IOCTL_USBD__SEND_ZLP:
                if (usb_mem->activated) {
                        status = send_ZLP(hdl->minor) ? ESUCC : EIO;
                } else {
                        status = ECANCELED;
                }
                break;

        case IOCTL_USBD__SET_EP_STALL:
                if (usb_mem->activated) {
                        int ep = (int)arg;

                        if ((ep & 0x7) < NUMBER_OF_ENDPOINTS) {
                                if (ep & USB_ENDP_IN) {
                                        set_ep_tx_status(ep & 0x7, USB_EP_STATUS__STALL);
                                } else {
                                        set_ep_rx_status(ep & 0x7, USB_EP_STATUS__STALL);
                                }

                                status = ESUCC;
                        } else {
                                status = EINVAL;
                        }
                } else {
                        status = ECANCELED;
                }
                break;

        case IOCTL_USBD__SET_EP_VALID:
                if (usb_mem->activated) {
                        int ep = (int)arg;

                        if ((ep & 0x7) < NUMBER_OF_ENDPOINTS) {
                                if (ep & USB_ENDP_IN) {
                                        set_ep_tx_status(ep & 0x7, USB_EP_STATUS__VALID);
                                } else {
                                        set_ep_rx_status(ep & 0x7, USB_EP_STATUS__VALID);
                                }

                                status = ESUCC;
                        } else {
                                status = EINVAL;
                        }
                } else {
                        status = ECANCELED;
                }
                break;

        case IOCTL_USBD__WAS_RESET:
                if (arg) {
                        if (usb_mem->activated) {
                                _sys_critical_section_begin();
                                bool reset     = usb_mem->reset;
                                usb_mem->reset = false;
                                _sys_critical_section_end();
                                *reinterpret_cast(bool*, arg) = reset;
                                status = ESUCC;
                        } else {
                                status = ECANCELED;
                        }
                } else {
                        status = EINVAL;
                }
                break;

        case IOCTL_USBD__SET_ERROR_STATUS:
                if (usb_mem->activated) {
                        set_ep_rx_status(hdl->minor, USB_EP_STATUS__STALL);
                        set_ep_tx_status(hdl->minor, USB_EP_STATUS__STALL);
                        status = ESUCC;
                } else {
                        status = ECANCELED;
                }
                break;

        case IOCTL_USBD__GET_SETUP_PACKET:
                if (usb_mem->activated && hdl->minor == USB_EP_NUM__ENDP0) {
                        if (arg) {
                                set_setup_in_progress(hdl->minor, true);

                                usbd_setup_container_t *setup = arg;
                                if (_sys_semaphore_wait(hdl->setup, setup->timeout) == true) {

                                        if (hdl->setup_in_progress == true) {
                                                set_setup_in_progress(hdl->minor, false);

                                                low_level_pma_read(USB_PMA->EP[hdl->minor].SBF.ADDR_RX,
                                                                   (u8_t*)&setup->packet,
                                                                   sizeof(usb_setup_packet_t));

                                                status = ESUCC;
                                        } else {
                                                status = ECANCELED;
                                        }
                                } else {
                                        set_setup_in_progress(hdl->minor, false);
                                        status = ETIME;
                                }
                        } else {
                                status = EINVAL;
                        }
                } else {
                        status = ECANCELED;
                }
                break;

        case IOCTL_USBD__GET_ERROR_COUNTER:
                if (arg) {
                        _sys_critical_section_begin();
                        *reinterpret_cast(int*, arg) = usb_mem->error_cnt;
                        usb_mem->error_cnt = 0;
                        _sys_critical_section_end();
                        status = ESUCC;
                } else {
                        status = EINVAL;
                }
                break;

        default:
                status = EBADRQC;
                break;
        }

        return status;
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
API_MOD_FLUSH(USBD, void *device_handle)
{
        UNUSED_ARG(device_handle);

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
API_MOD_STAT(USBD, void *device_handle, struct vfs_dev_stat *device_stat)
{
        USB_ep_t *hdl = device_handle;

        if (usb_mem->ep_config) {
                size_t size = 0;
                if (usb_mem->ep_config->ep[hdl->minor].OUT_enabled)
                        size = get_ep_received_size(hdl->minor);

                if (usb_mem->ep_config->ep[hdl->minor].IN_enabled)
                        size = usb_mem->ep_config->ep[hdl->minor].IN_buffer_size;

                device_stat->st_size  = size;
                device_stat->st_major = _USBD_MAJOR_NUMBER;
                device_stat->st_minor = hdl->minor;

                return ESUCC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function enables the USB D+ pullup to make device visible by the host
 */
//==============================================================================
static inline void enable_usb_visible_pullup()
{
        GPIO_SET_PIN(_USBD_CONFIG_PIN_PULLUP);
}

//==============================================================================
/**
 * @brief Function disables the USB D+ pullup to make device not visible by the host
 */
//==============================================================================
static inline void disable_usb_visible_pullup()
{
        GPIO_CLEAR_PIN(_USBD_CONFIG_PIN_PULLUP);
}

//==============================================================================
/**
 * @brief Function sets a Tx (IN) status of selected endpoint
 *
 * @param ep            endpoint number
 * @param status        status to set
 */
//==============================================================================
static void set_ep_tx_status(usb_ep_num_t ep, usb_ep_status_t status)
{
        _sys_critical_section_begin();
        USB->EPxR[ep] = ((USB->EPxR[ep] & (USB_EPR_EP_TYPE | USB_EPR_EP_KIND | USB_EPR_EA | USB_EPR_STAT_TX))
                         | USB_EPR_CTR_RX | USB_EPR_CTR_TX) ^ ep_tx_status_reg[status];
        _sys_critical_section_end();
}

//==============================================================================
/**
 * @brief Function sets a Rx (OUT) status of selected endpoint
 *
 * @param ep            endpoint number
 * @param status        status to set
 */
//==============================================================================
static void set_ep_rx_status(usb_ep_num_t ep, usb_ep_status_t status)
{
        _sys_critical_section_begin();
        USB->EPxR[ep] = ((USB->EPxR[ep] & (USB_EPR_EP_TYPE | USB_EPR_EP_KIND | USB_EPR_EA | USB_EPR_STAT_RX))
                         | USB_EPR_CTR_RX | USB_EPR_CTR_TX) ^ ep_rx_status_reg[status];
        _sys_critical_section_end();
}

//==============================================================================
/**
 * @brief Disables selected endpoint
 *
 * @param ep            endpoint number
 */
//==============================================================================
static void disable_endpoint(usb_ep_num_t ep)
{
        USB_PMA->EP[ep].SBF.ADDR_TX  = 0;
        USB_PMA->EP[ep].SBF.ADDR_RX  = 0;
        USB_PMA->EP[ep].SBF.COUNT_TX = 0;
        USB_PMA->EP[ep].SBF.COUNT_RX = 0;
        USB->EPxR[ep] = USB->EPxR[ep] | USB_EP0R_CTR_RX | USB_EP0R_CTR_TX;
        USB->EPxR[ep] = 0;
}

//==============================================================================
/**
 * @brief Check and convert endpoint size to BTABLE register value
 *
 * @param size          size to convert
 *
 * @return On success, returns a value converted to BTABLE format. On error,
 *         returns the UINT16_MAX value.
 */
//==============================================================================
static u16_t endpoint_size_to_register(u16_t size)
{
        if (size <= 62 && size % 2 == 0)
                return (size / 2) << 10;
        else if (size >= 32 && size <= 256 && size % 32 == 0) {
                return USB_COUNT0_RX_BLSIZE | (((size / 32) - 1) << 10);
        } else {
                return UINT16_MAX;
        }
}

//==============================================================================
/**
 * @brief This function enables endpoint 0 for control purposes and disables all
 *        other endpoints.
 */
//==============================================================================
static void configure_endpoint_0()
{
        /* Endpoint 0 configuration - exist only 1 configuration */
        if (  _USBD_ENDPOINT0_SIZE == 8  || _USBD_ENDPOINT0_SIZE == 16
           || _USBD_ENDPOINT0_SIZE == 32 || _USBD_ENDPOINT0_SIZE == 64) {

                USB_PMA->EP[USB_EP_NUM__ENDP0].SBF.ADDR_TX  = (BTABLE_SIZE) << 1;
                USB_PMA->EP[USB_EP_NUM__ENDP0].SBF.ADDR_RX  = (BTABLE_SIZE + _USBD_ENDPOINT0_SIZE) << 1;
                USB_PMA->EP[USB_EP_NUM__ENDP0].SBF.COUNT_TX = 0;
                USB_PMA->EP[USB_EP_NUM__ENDP0].SBF.COUNT_RX = endpoint_size_to_register(_USBD_ENDPOINT0_SIZE);
                USB->EPxR[USB_EP_NUM__ENDP0] = USB->EPxR[USB_EP_NUM__ENDP0] | USB_EP0R_CTR_RX | USB_EP0R_CTR_TX;
                USB->EPxR[USB_EP_NUM__ENDP0] = USB_EP0R_CTR_RX | USB_EP0R_CTR_TX
                                             | ep_rx_status_reg[USB_EP_STATUS__STALL]
                                             | ep_tx_status_reg[USB_EP_STATUS__STALL]
                                             | transfer_type_reg[USB_TRANSFER__CONTROL];
        } else {
                disable_endpoint(USB_EP_NUM__ENDP0);
        }

        /* disable all other endpoints */
        for (usb_ep_num_t ep = USB_EP_NUM__ENDP1; ep < NUMBER_OF_ENDPOINTS; ep++) {
                disable_endpoint(ep);
        }
}

//==============================================================================
/**
 * @brief Configure endpoints 1 to 7 to configured values. The Endpoint 0 is not
 *        reconfigured.
 *
 * @return Returns true on success, otherwise false.
 */
//==============================================================================
static bool configure_endpoints_1_7()
{
        bool status          = true;
        u8_t buf_start_index = BTABLE_SIZE + (2 * _USBD_ENDPOINT0_SIZE);

        _sys_critical_section_begin();

        for (usb_ep_num_t ep = USB_EP_NUM__ENDP1; ep < NUMBER_OF_ENDPOINTS; ep++) {
                if (usb_mem->ep[ep]) {
                        const struct usbd_ep_config *epcfg = &usb_mem->ep_config->ep[ep];

                        /* convert buffer size to register value */
                        u16_t tx_buf_size = endpoint_size_to_register(epcfg->IN_buffer_size);
                        u16_t rx_buf_size = endpoint_size_to_register(epcfg->OUT_buffer_size);

                        if (tx_buf_size == UINT16_MAX || rx_buf_size == UINT16_MAX) {
                                status = false;
                                break;
                        }

                        /* configure BTABLE */
                        USB_PMA->EP[ep].SBF.ADDR_TX  = buf_start_index << 1;
                        USB_PMA->EP[ep].SBF.COUNT_TX = 0;
                        USB_PMA->EP[ep].SBF.ADDR_RX  = ((buf_start_index + epcfg->IN_buffer_size) << 1);
                        USB_PMA->EP[ep].SBF.COUNT_RX = tx_buf_size;

                        buf_start_index += (epcfg->IN_buffer_size + epcfg->OUT_buffer_size);

                        /* set Endpoint OUT status according to configuration */
                        u16_t rx_status;
                        if (epcfg->OUT_buffer_size > 0 && epcfg->OUT_enabled == true) {
                                rx_status = ep_rx_status_reg[USB_EP_STATUS__VALID];
                        } else {
                                rx_status = ep_rx_status_reg[USB_EP_STATUS__DISABLED];
                        }

                        /* set Endpoint IN status according to configuration */
                        u16_t tx_status;
                        if (epcfg->IN_buffer_size > 0 && epcfg->IN_enabled == true) {
                                if (epcfg->transfer_type == USB_TRANSFER__ISOCHRONOUS) {
                                        tx_status = ep_tx_status_reg[USB_EP_STATUS__VALID];
                                } else {
                                        tx_status = ep_tx_status_reg[USB_EP_STATUS__NAK];
                                }
                        } else {
                                tx_status = ep_tx_status_reg[USB_EP_STATUS__DISABLED];
                        }

                        /* configure Endpoint */
                        USB->EPxR[ep] = USB->EPxR[ep] | USB_EP0R_CTR_RX | USB_EP0R_CTR_TX;
                        USB->EPxR[ep] = USB_EP0R_CTR_RX
                                      | USB_EP0R_CTR_TX
                                      | transfer_type_reg[epcfg->transfer_type]
                                      | rx_status
                                      | tx_status
                                      | ep;
                } else {
                        disable_endpoint(ep);
                }
        }

        _sys_critical_section_end();

        return status;
}

//==============================================================================
/**
 * @brief Clears the CTR RX flag in the selected endpoint
 *
 * @param ep            selected endpoint
 */
//==============================================================================
static void clear_EPR_CTR_RX(usb_ep_num_t ep)
{
        USB->EPxR[ep] = (USB->EPxR[ep] & (USB_EPR_EP_TYPE | USB_EPR_EP_KIND | USB_EPR_EA)) | USB_EPR_CTR_TX;
}

//==============================================================================
/**
 * @brief Clears the CTR TX flag in the selected endpoint
 *
 * @param ep            selected endpoint
 */
//==============================================================================
static void clear_EPR_CTR_TX(usb_ep_num_t ep)
{
        USB->EPxR[ep] = (USB->EPxR[ep] & (USB_EPR_EP_TYPE | USB_EPR_EP_KIND | USB_EPR_EA)) | USB_EPR_CTR_RX;
}

//==============================================================================
/**
 * @brief Function perform a low level write operation to the PMA
 *
 * @param pma_offset    PMA register offset
 * @param buffer        data source
 * @param count         number of bytes to write
 */
//==============================================================================
static void low_level_pma_write(u32_t pma_offset, const u8_t *buffer, size_t count)
{
        u32_t *pma = &USB_PMA->BUFFER[(pma_offset / 2) - BTABLE_SIZE];

        while (count >= 2) {
                u8_t b0  = *(buffer++);
                u8_t b1  = *(buffer++);
                *(pma++) = (b1 << 8) | b0;
                count   -= 2;
        }

        if (count == 1) {
                *(pma++) = *(buffer++);
        }
}

//==============================================================================
/**
 * @brief Function perform a low level read operation from the PMA
 *
 * @param pma_offset    PMA register offset
 * @param buffer        data destination
 * @param count         number of bytes to read
 */
//==============================================================================
static void low_level_pma_read(u32_t pma_offset, u8_t *buffer, size_t count)
{
        u32_t  *pma = &USB_PMA->BUFFER[(pma_offset / 2) - BTABLE_SIZE];

        while (count >= 2) {
                u16_t data  = *(pma++);
                *(buffer++) = data & 0xFF;
                *(buffer++) = data >> 8;
                count      -= 2;
        }

        if (count == 1) {
                *(buffer++) = *(pma++) & 0xFF;
        }
}

//==============================================================================
/**
 * @brief Function send Zero Length Packet from selected endpoint
 *
 * @param ep            endpoint number
 *
 * @return On success true is returned, otherwise false.
 */
//==============================================================================
static bool send_ZLP(usb_ep_num_t ep)
{
        bool result = false;

        if (ep == USB_EP_NUM__ENDP0) {
                if (_sys_semaphore_wait(usb_mem->ep[ep]->setup, 0) == true) {
                        _sys_semaphore_signal(usb_mem->ep[ep]->setup);

                } else {
                        USB_PMA->EP[ep].SBF.COUNT_TX = 0;
                        set_ep_rx_status(ep, USB_EP_STATUS__VALID);
                        set_ep_tx_status(ep, USB_EP_STATUS__VALID);

                        result = _sys_semaphore_wait(usb_mem->ep[ep]->tx, ep0_data_stage_transmit_timeout_ms);
                        if (result == true) {

                                /* check Status Stage (if exist) */
                                if (_sys_semaphore_wait(usb_mem->ep[ep]->rx, ep0_status_stage_timeout_ms) == false) {
                                        if (_sys_semaphore_wait(usb_mem->ep[ep]->setup, ep0_status_stage_timeout_ms) == true) {
                                                _sys_semaphore_signal(usb_mem->ep[ep]->setup);
                                        }
                                }
                        } else {
                                set_ep_tx_status(ep, USB_EP_STATUS__STALL);
                        }
                }

        } else {
                USB_PMA->EP[ep].SBF.COUNT_TX = 0;
                set_ep_tx_status(ep, USB_EP_STATUS__VALID);
                result = _sys_semaphore_wait(usb_mem->ep[ep]->tx, ep1_7_data_stage_transmit_timeout_ms);
        }

        return result;
}

//==============================================================================
/**
 * @brief Function returns a size of Tx (IN) buffer of selected endpoint
 *
 * @param ep            endpoint number
 *
 * @return A size of Tx (IN) endpoint.
 */
//==============================================================================
static size_t get_ep_tx_buf_size(usb_ep_num_t ep)
{
        return usb_mem->ep_config->ep[ep].IN_buffer_size;
}

//==============================================================================
/**
 * @brief Function returns a size of Rx (OUT) buffer of selected endpoint
 *
 * @param ep            endpoint number
 *
 * @return A size of Rx (OUT) endpoint.
 */
//==============================================================================
static size_t get_ep_rx_buf_size(usb_ep_num_t ep)
{
        return usb_mem->ep_config->ep[ep].OUT_buffer_size;
}

//==============================================================================
/**
 * @brief Function returns a number of received bytes by selected endpoint
 *
 * @param ep            endpoint number
 *
 * @return A number of received bytes.
 */
//==============================================================================
static size_t get_ep_received_size(usb_ep_num_t ep)
{
        return USB_PMA->EP[ep].SBF.COUNT_RX & USB_COUNT0_RX_0_COUNT0_RX_0;
}

//==============================================================================
/**
 * @brief Function set "setup in progress" flag in the critical section
 */
//==============================================================================
static void set_setup_in_progress(usb_ep_num_t ep, bool state)
{
        _sys_critical_section_begin();
        usb_mem->ep[ep]->setup_in_progress = state;
        _sys_critical_section_end();
}

//==============================================================================
/**
 * @brief Function set "write in progress" flag in the critical section
 */
//==============================================================================
static void set_write_in_progress(usb_ep_num_t ep, bool state)
{
        _sys_critical_section_begin();
        usb_mem->ep[ep]->write_in_progress = state;
        _sys_critical_section_end();
}

//==============================================================================
/**
 * @brief Function set "read in progress" flag in the critical section
 */
//==============================================================================
static void set_read_in_progress(usb_ep_num_t ep, bool state)
{
        _sys_critical_section_begin();
        usb_mem->ep[ep]->read_in_progress = state;
        _sys_critical_section_end();
}

//==============================================================================
/**
 * @brief High priority USB IRQ (isochronous transfers)
 */
//==============================================================================
void USB_HP_CAN1_TX_IRQHandler(void)
{
        /* Interrupt not used (can be (or must?) be used in the Isochronous transfer) */
}

//==============================================================================
/**
 * @brief Low priority USB IRQ
 */
//==============================================================================
void USB_LP_CAN1_RX0_IRQHandler(void)
{
        bool  rx_woken    = false;
        bool  tx_woken    = false;
        bool  setup_woken = false;
        u16_t flags       = USB->CNTR & USB->ISTR;

        if (flags & USB_ISTR_CTR) {
                u8_t  ep_id    = USB->ISTR & USB_ISTR_EP_ID;
                u32_t ep_flags = USB->EPxR[ep_id];

                if (ep_flags & USB_EPR_SETUP) {

                        clear_EPR_CTR_RX(ep_id);
                        if (ep_id == USB_EP_NUM__ENDP0) {
                                _sys_semaphore_signal_from_ISR(usb_mem->ep[ep_id]->setup, &setup_woken);
                        }

                } else if (ep_flags & USB_EPR_CTR_RX) {

                        clear_EPR_CTR_RX(ep_id);
                        _sys_semaphore_signal_from_ISR(usb_mem->ep[ep_id]->rx, &rx_woken);
                        rx_woken = true;

                } else if (ep_flags & USB_EPR_CTR_TX) {

                        clear_EPR_CTR_TX(ep_id);
                        _sys_semaphore_signal_from_ISR(usb_mem->ep[ep_id]->tx, &tx_woken);
                        tx_woken = true;
                }
        }

        if (flags & USB_ISTR_PMAOVR) {
                USB->ISTR = ~USB_ISTR_PMAOVR;
        }

        if (flags & USB_ISTR_ERR) {
                USB->ISTR = ~USB_ISTR_ERR;
                usb_mem->error_cnt++;
        }

        if (flags & USB_ISTR_WKUP) {
                USB->ISTR = ~USB_ISTR_WKUP;
        }

        if (flags & USB_ISTR_SUSP) {
                USB->ISTR = ~USB_ISTR_SUSP;
        }

        if (flags & USB_ISTR_RESET) {
                USB->ISTR   = ~USB_ISTR_RESET;
                USB->BTABLE = BTABLE_ADDRESS;
                USB->DADDR  = USB_DADDR_EF | 0;
                configure_endpoint_0();

                usb_mem->reset = true;

                for (int i = 0; i < NUMBER_OF_ENDPOINTS; i++) {
                        if (usb_mem->ep[i]) {
                                if (usb_mem->ep[i]->write_in_progress) {
                                        usb_mem->ep[i]->write_in_progress = false;
                                        _sys_semaphore_signal_from_ISR(usb_mem->ep[i]->tx, &tx_woken);
                                } else {
                                        _sys_semaphore_wait_from_ISR(usb_mem->ep[i]->tx, &tx_woken);
                                }

                                if (usb_mem->ep[i]->read_in_progress) {
                                        usb_mem->ep[i]->read_in_progress = false;
                                        _sys_semaphore_signal_from_ISR(usb_mem->ep[i]->rx, &rx_woken);
                                } else {
                                        _sys_semaphore_wait_from_ISR(usb_mem->ep[i]->rx, &rx_woken);
                                }

                                if (usb_mem->ep[i]->setup) {
                                        if (usb_mem->ep[i]->setup_in_progress) {
                                                usb_mem->ep[i]->setup_in_progress = false;
                                                _sys_semaphore_signal_from_ISR(usb_mem->ep[i]->setup, &setup_woken);
                                        } else {
                                                _sys_semaphore_wait_from_ISR(usb_mem->ep[i]->setup, &setup_woken);
                                        }
                                }
                        }
                }
        }

        if (flags & USB_ISTR_SOF) {
                USB->ISTR = ~USB_ISTR_SOF;
        }

        if (flags & USB_ISTR_ESOF) {
                USB->ISTR = ~USB_ISTR_ESOF;
        }

        if (rx_woken || tx_woken || setup_woken) {
                _sys_task_yield_from_ISR();
        }
}

/*==============================================================================
  End of file
==============================================================================*/
