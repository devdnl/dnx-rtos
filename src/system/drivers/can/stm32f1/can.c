/*==============================================================================
File    can.c

Author  Daniel Zorychta

Brief   CAN driver

        Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "stm32f1/can_cfg.h"
#include "stm32f1/stm32f10x.h"
#include "../can_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define TX_MAILBOXES    3
#define RX_MAILBOXES    2
#define RX_FIFO_DEPTH   3
#define INIT_TIMEOUT    1000
#define MTX_TIMEOUT     1000

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        int   major;
        u32_t send_timeout;
        u32_t recv_timeout;

        mutex_t *config_mtx;
        mutex_t *txmbox_mtx[TX_MAILBOXES];
        queue_t *rxqueue_q;

} CANM_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void relese_resources(CANM_t *hdl);
static int recv_msg(CANM_t *hdl, CAN_msg_t *msg);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(CAN);

static CANM_t *CANM;

/*==============================================================================
  Exported object
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device.
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_INIT(CAN, void **device_handle, u8_t major, u8_t minor)
{
        int err = EFAULT;

        if (major == 0 && minor == 0) {
                err = sys_zalloc(sizeof(CANM_t), device_handle);
                if (!err) {

                        CANM_t *hdl = *device_handle;

                        hdl->major        = major;
                        hdl->recv_timeout = MAX_DELAY_MS;
                        hdl->send_timeout = 0;

                        err = sys_mutex_create(MUTEX_TYPE_RECURSIVE, &hdl->config_mtx);
                        if (err) {
                                goto finish;
                        }

                        err = sys_queue_create(RX_MAILBOXES * RX_FIFO_DEPTH,
                                               sizeof(CAN_FIFOMailBox_t),
                                               &hdl->rxqueue_q);
                        if (err) {
                                goto finish;
                        }

                        for (int i = 0; i < TX_MAILBOXES; i++) {
                                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->txmbox_mtx[i]);
                                if (err) {
                                        goto finish;
                                }
                        }

                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_CAN1EN);
                        SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_CAN1RST);
                        CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_CAN1RST);

                        SET_BIT(CAN1->MCR, CAN_MCR_RESET);
                        CLEAR_BIT(CAN1->MCR, CAN_MCR_RESET);

                        NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
                        NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                        NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
                        NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                        NVIC_EnableIRQ(CAN1_RX1_IRQn);
                        NVIC_SetPriority(CAN1_RX1_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                        SET_BIT(CAN1->IER, CAN_IER_FMPIE1 | CAN_IER_FMPIE0);

                        finish:
                        if (!err) {
                                CANM = hdl;

                        } else {
                                relese_resources(hdl);
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Release device.
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_RELEASE(CAN, void *device_handle)
{
        int err = sys_free(&device_handle);
        return err;
}

//==============================================================================
/**
 * @brief Open device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_OPEN(CAN, void *device_handle, u32_t flags)
{
        CANM_t *hdl = device_handle;

        int err = ESUCC;

        // ...

        return err;
}

//==============================================================================
/**
 * @brief Close device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_CLOSE(CAN, void *device_handle, bool force)
{
        CANM_t *hdl = device_handle;

        int err = ESUCC;

        // ...

        return err;
}

//==============================================================================
/**
 * @brief Write data to device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_WRITE(CAN,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        CANM_t *hdl = device_handle;

        int err = ESUCC;

        // ...

        return err;
}

//==============================================================================
/**
 * @brief Read data from device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_READ(CAN,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        CANM_t *hdl = device_handle;

        int err = ESUCC;

        // ...

        return err;
}

//==============================================================================
/**
 * @brief IO control.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_IOCTL(CAN, void *device_handle, int request, void *arg)
{
        CANM_t *hdl = device_handle;

        int err = EINVAL;

        switch (request) {
        case IOCTL_CAN__CONFIGURE:
                if (arg) {

                }
                break;

        case IOCTL_CAN__SET_FILTER:
        case IOCTL_CAN__SET_MODE:
        case IOCTL_CAN__SET_SEND_TIMEOUT:
                if (arg) {
                        hdl->send_timeout = *cast(u32_t*, arg);
                }
                break;

        case IOCTL_CAN__SET_RECV_TIMEOUT:
                if (arg) {
                        hdl->recv_timeout = *cast(u32_t*, arg);
                }
                break;

        case IOCTL_CAN__SEND_MSG:
        case IOCTL_CAN__RECV_MSG:
                if (arg) {
                        err = recv_msg(hdl, arg);
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
 * @brief Flush device.
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_FLUSH(CAN, void *device_handle)
{
        CANM_t *hdl = device_handle;

        int err = ESUCC;

        // ...

        return err;
}

//==============================================================================
/**
 * @brief Device information.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_STAT(CAN, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_size = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
static void relese_resources(CANM_t *hdl)
{
        if (hdl->config_mtx) {
                sys_mutex_destroy(hdl->config_mtx);
        }

        if (hdl->rxqueue_q) {
                sys_queue_destroy(hdl->rxqueue_q);
        }

        for (int i = 0; i < TX_MAILBOXES; i++) {
                if (hdl->txmbox_mtx[i]) {
                        sys_mutex_destroy(hdl->txmbox_mtx[i]);
                }
        }

        sys_free((void*)&hdl);
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
static int configure(CANM_t *hdl, const CAN_config_t *cfg)
{
        int err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
        if (!err) {
                SET_BIT(CAN1);

                if (cfg->auto_bus_off) {
                        SET_BIT(CAN1->MCR, CAN_MCR_ABOM);
                } else {
                        CLEAR_BIT(CAN1->MCR, CAN_MCR_ABOM);
                }

                if (cfg->auto_retransmission) {
                        CLEAR_BIT(CAN1->MCR, CAN_MCR_NART);
                } else {
                        SET_BIT(CAN1->MCR, CAN_MCR_NART);
                }

                if (cfg->auto_wake_up) {
                        SET_BIT(CAN1->MCR, CAN_MCR_AWUM);
                } else {
                        CLEAR_BIT(CAN1->MCR, CAN_MCR_AWUM);
                }

                if (cfg->time_triggered_communication) {
                        SET_BIT(CAN1->MCR, CAN_MCR_TTCM);
                } else {
                        CLEAR_BIT(CAN1->MCR, CAN_MCR_TTCM);
                }

                if (cfg->loopback) {
                        SET_BIT(CAN1->BTR, CAN_BTR_LBKM);
                } else {
                        CLEAR_BIT(CAN1->BTR, CAN_BTR_LBKM);
                }

                if (cfg->silent) {
                        SET_BIT(CAN1->BTR, CAN_BTR_SILM);
                } else {
                        CLEAR_BIT(CAN1->BTR, CAN_BTR_SILM);
                }


                u32_t cfg->SJW
        }

        return err;
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
static int send_msg(CANM_t *hdl, const CAN_msg_t *msg)
{
        int err = sys_mutex_lock();

        return err;
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
static int recv_msg(CANM_t *hdl, CAN_msg_t *msg)
{
        CAN_FIFOMailBox_t mbox;

        int err = sys_queue_receive(hdl->rxqueue_q, &mbox, msg->timeout_ms);
        if (!err) {
                if (mbox.RIR & CAN_RI0R_IDE) {
                        msg->ID = mbox.RIR >> 3;
                        msg->extended_ID = true;
                } else {
                        msg->ID = mbox.RIR >> 21;
                        msg->extended_ID = false;
                }

                msg->remote_tranmission = mbox.RIR & CAN_RI0R_RTR;
                msg->data_length        = mbox.RDTR & CAN_RDT0R_DLC;
                msg->data[0]            = mbox.RDLR >> 0;
                msg->data[1]            = mbox.RDLR >> 8;
                msg->data[2]            = mbox.RDLR >> 16;
                msg->data[3]            = mbox.RDLR >> 24;
                msg->data[4]            = mbox.RDHR >> 0;
                msg->data[5]            = mbox.RDHR >> 8;
                msg->data[6]            = mbox.RDHR >> 16;
                msg->data[7]            = mbox.RDHR >> 24;
        }

        return err;
}

void USB_HP_CAN1_TX_IRQHandler(void)
{

}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
        bool yield = false;

        sys_queue_send_from_ISR(CANM->rxqueue_q, &CAN1->sFIFOMailBox[0], &yield);

        SET_BIT(CAN1->RF0R, CAN_RF0R_RFOM0);

        sys_thread_yield_from_ISR(yield);
}

void CAN1_RX1_IRQHandler(void)
{
        bool yield = false;

        sys_queue_send_from_ISR(CANM->rxqueue_q, &CAN1->sFIFOMailBox[1], &yield);

        SET_BIT(CAN1->RF1R, CAN_RF0R_RFOM0);

        sys_thread_yield_from_ISR(yield);
}

void CAN1_SCE_IRQHandler(void)
{

}

/*==============================================================================
  End of file
==============================================================================*/
