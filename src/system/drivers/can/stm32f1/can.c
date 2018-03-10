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
        u32_t    send_timeout;
        u32_t    recv_timeout;
        u32_t    txpend_ctr;
        u32_t    rxpend_ctr;
        mutex_t *config_mtx;
        mutex_t *txmbox_mtx[TX_MAILBOXES];
        queue_t *txrdy_q[TX_MAILBOXES];
        queue_t *rxqueue_q;
} CANM_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void relese_resources(CANM_t *hdl);
static int configure(CANM_t *hdl, const CAN_config_t *cfg);
static int set_init_mode(CANM_t *hdl);
static int set_normal_mode(CANM_t *hdl);
static int set_sleep_mode(CANM_t *hdl);
static int set_filter(CANM_t *hdl, const CAN_filter_t *filter);
static int send_msg(CANM_t *hdl, const CAN_msg_t *msg);
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
        int err = EINVAL;

        if (major == 0 && minor == 0) {

                err = sys_zalloc(sizeof(CANM_t), device_handle);
                if (!err) {

                        CANM_t *hdl = *device_handle;

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

                                err = sys_queue_create(1, sizeof(int), &hdl->txrdy_q[i]);
                                if (err) {
                                        goto finish;
                                }
                        }

                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_CAN1EN);
                        SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_CAN1RST);
                        CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_CAN1RST);

                        SET_BIT(CAN1->MCR, CAN_MCR_RESET);
                        CLEAR_BIT(CAN1->MCR, CAN_MCR_RESET);

                        NVIC_ClearPendingIRQ(USB_HP_CAN1_TX_IRQn);
                        NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
                        NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                        NVIC_ClearPendingIRQ(USB_LP_CAN1_RX0_IRQn);
                        NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
                        NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                        NVIC_ClearPendingIRQ(CAN1_RX1_IRQn);
                        NVIC_EnableIRQ(CAN1_RX1_IRQn);
                        NVIC_SetPriority(CAN1_RX1_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                        NVIC_ClearPendingIRQ(CAN1_SCE_IRQn);
                        NVIC_EnableIRQ(CAN1_SCE_IRQn);
                        NVIC_SetPriority(CAN1_SCE_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                        err = set_init_mode(hdl);
                        if (err) {
                                goto finish;
                        }

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
        CANM_t *hdl = device_handle;

        // lock access to module
        CANM = NULL;
        sys_sleep_ms(100);

        // finish TX pending transfers
        int err = ECANCELED;

        for (u32_t n = 0; n < hdl->txpend_ctr; n++) {

                for (int i = 0; i < TX_MAILBOXES; i++) {
                        sys_queue_send(hdl->txrdy_q[i], &err, 0);
                }

                sys_sleep_ms(10);
        }

        // finish RX pending queue
        CAN_FIFOMailBox_t mbox;
        memset(&mbox, 0, sizeof(CAN_FIFOMailBox_t));

        for (u32_t n = 0; n < hdl->rxpend_ctr; n++) {
                sys_queue_send(hdl->rxqueue_q, &mbox, 0);
        }

        // wait for finish all transfers
        while ((hdl->txpend_ctr > 0) || (hdl->rxpend_ctr > 0)) {
                sys_sleep_ms(10);
        }

        // release all resources
        relese_resources(hdl);

        return ESUCC;
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
        UNUSED_ARG2(device_handle, flags);

        return CANM ? ESUCC : ENXIO;
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
        UNUSED_ARG2(device_handle, force);

        return ESUCC;
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
        UNUSED_ARG1(fpos);

        CANM_t *hdl = device_handle;

        if (CANM == NULL) {
                return ENXIO;
        }

        int err = EINVAL;

        if ((count % sizeof(CAN_msg_t)) == 0) {

                while (count >= sizeof(CAN_msg_t)) {

                        CAN_msg_t msg;
                        memcpy(&msg, src, sizeof(CAN_msg_t));

                        msg.timeout_ms = fattr.non_blocking_wr ? 0 : hdl->send_timeout;

                        err = send_msg(hdl, &msg);

                        if (!err) {
                                *wrcnt += sizeof(CAN_msg_t);
                                src    += sizeof(CAN_msg_t);
                                count  -= sizeof(CAN_msg_t);
                        } else {
                                if (err == ETIME) {
                                        err = ESUCC;
                                }

                                break;
                        }
                }
        }

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
        UNUSED_ARG1(fpos);

        CANM_t *hdl = device_handle;

        if (CANM == NULL) {
                return ENXIO;
        }

        int err = EINVAL;

        if ((count % sizeof(CAN_msg_t)) == 0) {

                while (count >= sizeof(CAN_msg_t)) {

                        CAN_msg_t msg;
                        msg.timeout_ms = fattr.non_blocking_rd ? 0 : hdl->recv_timeout;

                        err = recv_msg(hdl, &msg);

                        if (!err) {
                                memcpy(dst, &msg, sizeof(CAN_msg_t));

                                *rdcnt += sizeof(CAN_msg_t);
                                dst    += sizeof(CAN_msg_t);
                                count  -= sizeof(CAN_msg_t);
                        } else {
                                if (err == ETIME) {
                                        err = ESUCC;
                                }

                                break;
                        }
                }
        }

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

        if (CANM == NULL) {
                return ENXIO;
        }

        int err = EINVAL;

        switch (request) {
        case IOCTL_CAN__CONFIGURE:
                if (arg) {
                        err = configure(hdl, arg);
                }
                break;

        case IOCTL_CAN__SET_FILTER:
                if (arg) {
                        err = set_filter(hdl, arg);
                }
                break;

        case IOCTL_CAN__SET_MODE:
                if (arg) {
                        CAN_mode_t mode = *cast(CAN_mode_t*, arg);
                        switch (mode) {
                        case CAN_MODE__INIT  : err = set_init_mode(hdl); break;
                        case CAN_MODE__NORMAL: err = set_normal_mode(hdl); break;
                        case CAN_MODE__SLEEP : err = set_sleep_mode(hdl); break;
                        default              : err = EINVAL; break;
                        }
                }
                break;

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
                if (arg) {
                        err = send_msg(hdl, arg);
                }
                break;

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
        UNUSED_ARG1(device_handle);

        return ESUCC;
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
        CANM_t *hdl = device_handle;

        size_t items = 0;
        sys_queue_get_number_of_items(hdl->rxqueue_q, &items);

        device_stat->st_size = items * sizeof(CAN_msg_t);

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Release resources allocated by module.
 *
 * @param  hdl          module instance
 */
//==============================================================================
static void relese_resources(CANM_t *hdl)
{
        NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
        NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
        NVIC_DisableIRQ(CAN1_RX1_IRQn);
        NVIC_DisableIRQ(CAN1_SCE_IRQn);

        SET_BIT(CAN1->MCR, CAN_MCR_RESET);
        CLEAR_BIT(CAN1->MCR, CAN_MCR_RESET);

        SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_CAN1RST);
        CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_CAN1RST);

        CLEAR_BIT(RCC->APB1ENR, RCC_APB1ENR_CAN1EN);

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

                if (hdl->txrdy_q[i]) {
                        sys_queue_destroy(hdl->txrdy_q[i]);
                }
        }

        sys_free((void*)&hdl);
}

//==============================================================================
/**
 * @brief  Configure peripheral.
 *
 * @param  hdl          module instance
 * @param  cfg          configuration
 *
 * @return One of errno value.
 */
//==============================================================================
static int configure(CANM_t *hdl, const CAN_config_t *cfg)
{
        int err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
        if (!err) {
                SET_BIT(CAN1->MCR, (CAN_MCR_DBF * _CAN_CFG__DEBUG_FREEZE)
                                 | CAN_MCR_TXFP
                                 | CAN_MCR_RFLM);

                SET_BIT(CAN1->IER, CAN_IER_FMPIE1
                                 | CAN_IER_FMPIE0
                                 | CAN_IER_TMEIE);

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

                u32_t SJW = min(3, cfg->SJW - 1) << 24;
                u32_t TS2 = min(7, cfg->TS2 - 1) << 20;
                u32_t TS1 = min(15, cfg->TS1 - 1) << 16;
                u32_t BRP = min(1023, cfg->prescaler - 1);

                CAN1->BTR = (CAN_BTR_LBKM * cfg->loopback)
                          | (CAN_BTR_SILM * cfg->silent)
                          | SJW | TS2 | TS1 | BRP;

                sys_mutex_unlock(hdl->config_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Set CAN controller to initialization mode.
 *
 * @param  hdl          module instance
 *
 * @return One of errno value.
 */
//==============================================================================
static int set_init_mode(CANM_t *hdl)
{
        int err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
        if (!err) {
                err = EIO;

                u32_t tref = sys_get_uptime_ms();

                CLEAR_BIT(CAN1->MCR, CAN_MCR_SLEEP);
                SET_BIT(CAN1->MCR, CAN_MCR_INRQ);

                while (not sys_time_is_expired(tref, INIT_TIMEOUT)) {
                        if (CAN1->MSR & CAN_MSR_INAK) {
                                err = ESUCC;
                                break;
                        }
                }

                sys_mutex_unlock(hdl->config_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Set CAN controller to normal mode (active).
 *
 * @param  hdl          module instance
 *
 * @return One of errno value.
 */
//==============================================================================
static int set_normal_mode(CANM_t *hdl)
{
        int err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
        if (!err) {
                err = EIO;

                u32_t tref = sys_get_uptime_ms();

                CLEAR_BIT(CAN1->FMR, CAN_FMR_FINIT);
                CLEAR_BIT(CAN1->MCR, CAN_MCR_INRQ);

                while (not sys_time_is_expired(tref, INIT_TIMEOUT)) {
                        if (!(CAN1->MSR & CAN_MSR_INAK)) {
                                err = ESUCC;
                                break;
                        }
                }

                sys_mutex_unlock(hdl->config_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Set CAN controller to sleep mode.
 *
 * @param  hdl          module instance
 *
 * @return One of errno value.
 */
//==============================================================================
static int set_sleep_mode(CANM_t *hdl)
{
        int err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
        if (!err) {
                SET_BIT(CAN1->MCR, CAN_MCR_SLEEP);
                sys_mutex_unlock(hdl->config_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Set acceptance filter.
 *
 * @param  hdl          module instance
 * @param  filter       filter configuration
 *
 * @return One of errno value.
 */
//==============================================================================
static int set_filter(CANM_t *hdl, const CAN_filter_t *filter)
{
        if (filter->number >= 14) {
                return EINVAL;
        }

        int err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
        if (!err) {
                u32_t CAN2SB = 14;
                CAN1->FMR = (CAN2SB << 24) | CAN_FMR_FINIT;

                if (filter->mask) {
                        CLEAR_BIT(CAN1->FM1R, CAN_FM1R_FBM0 << filter->number);
                } else {
                        SET_BIT(CAN1->FM1R, CAN_FM1R_FBM0 << filter->number);
                }

                // 32b ID
                SET_BIT(CAN1->FS1R, CAN_FS1R_FSC0 << filter->number);

                // FIFO0: odd filters, FIFO1: even filters)
                WRITE_REG(CAN1->FFA1R, 0xAAAAAAAA);

                // set ID and mask
                CAN1->sFilterRegister[filter->number].FR1 = filter->ID1;
                CAN1->sFilterRegister[filter->number].FR2 = filter->ID2_mask;

                SET_BIT(CAN1->FA1R, CAN_FA1R_FACT0 << filter->number);

                sys_mutex_unlock(hdl->config_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function send CAN message.
 *
 * @param  hdl          module instance
 * @param  msg          message to send
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_msg(CANM_t *hdl, const CAN_msg_t *msg)
{
        sys_context_switch_lock();
        hdl->txpend_ctr++;
        sys_context_switch_unlock();

        int   err     = ETIME;
        bool  loop    = true;
        u32_t timeout = max(1, msg->timeout_ms);

        u32_t tref = sys_get_uptime_ms();

        while (CANM && loop && !sys_time_is_expired(tref, timeout)) {

                err = ETIME;

                for (int i = 0; CANM && loop && (i < TX_MAILBOXES); i++) {

                        err = sys_mutex_trylock(hdl->txmbox_mtx[i]);
                        if (!err) {
                                CAN_TxMailBox_t *mbox = &CAN1->sTxMailBox[i];

                                if (!(mbox->TIR & CAN_TI0R_TXRQ)) {
                                        mbox->TIR = CAN_TI0R_RTR * msg->remote_transmission;

                                        if (msg->extended_ID) {
                                                mbox->TIR |= (msg->ID << 3);
                                                mbox->TIR |= CAN_TI0R_IDE;
                                        } else {
                                                mbox->TIR |= (msg->ID << 21);
                                        }

                                        mbox->TDTR = min(8, msg->data_length) & CAN_TDT0R_DLC;

                                        mbox->TDLR = msg->data[0] << 0
                                                   | msg->data[1] << 8
                                                   | msg->data[2] << 16
                                                   | msg->data[3] << 24;

                                        mbox->TDHR = msg->data[4] << 0
                                                   | msg->data[5] << 8
                                                   | msg->data[6] << 16
                                                   | msg->data[7] << 24;

                                        sys_queue_reset(hdl->txrdy_q[i]);

                                        SET_BIT(mbox->TIR, CAN_TI0R_TXRQ);

                                        int erri = EIO;
                                        err = sys_queue_receive(hdl->txrdy_q[i], &erri, timeout);
                                        if (!err) {
                                                err = erri;

                                        } else {
                                                printk("CAN: message send abort");
                                                SET_BIT(CAN1->TSR, CAN_TSR_ABRQ0 << (i * 8));
                                                sys_sleep_ms(1);
                                        }

                                        loop = false;
                                } else {
                                        err = EBUSY;
                                }

                                sys_mutex_unlock(hdl->txmbox_mtx[i]);
                        }
                }

                if (loop) {
                        sys_sleep_ms(1);
                }
        }

        sys_context_switch_lock();
        hdl->txpend_ctr--;
        sys_context_switch_unlock();

        return err;
}

//==============================================================================
/**
 * @brief  Receive message.
 *
 * @param  hdl          module instance
 * @param  msg          message container to receive to
 *
 * @return One of errno value.
 */
//==============================================================================
static int recv_msg(CANM_t *hdl, CAN_msg_t *msg)
{
        sys_context_switch_lock();
        hdl->rxpend_ctr++;
        sys_context_switch_unlock();

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

                msg->remote_transmission = mbox.RIR & CAN_RI0R_RTR;
                msg->data_length         = mbox.RDTR & CAN_RDT0R_DLC;
                msg->data[0]             = mbox.RDLR >> 0;
                msg->data[1]             = mbox.RDLR >> 8;
                msg->data[2]             = mbox.RDLR >> 16;
                msg->data[3]             = mbox.RDLR >> 24;
                msg->data[4]             = mbox.RDHR >> 0;
                msg->data[5]             = mbox.RDHR >> 8;
                msg->data[6]             = mbox.RDHR >> 16;
                msg->data[7]             = mbox.RDHR >> 24;
        }

        sys_context_switch_lock();
        hdl->rxpend_ctr--;
        sys_context_switch_unlock();

        return err;
}

//==============================================================================
/**
 * @brief CAN TX ready IRQ.
 */
//==============================================================================
void USB_HP_CAN1_TX_IRQHandler(void)
{
        bool yield = false;

        static const u32_t CAN_TSR_RQCP[TX_MAILBOXES] = {CAN_TSR_RQCP0,
                                                         CAN_TSR_RQCP1,
                                                         CAN_TSR_RQCP2};

        static const u32_t CAN_TSR_TERR[TX_MAILBOXES] = {CAN_TSR_TERR0,
                                                         CAN_TSR_TERR1,
                                                         CAN_TSR_TERR2};

        static const u32_t CAN_TSR_ALST[TX_MAILBOXES] = {CAN_TSR_ALST0,
                                                         CAN_TSR_ALST1,
                                                         CAN_TSR_ALST2};

        static const u32_t CAN_TSR_TXOK[TX_MAILBOXES] = {CAN_TSR_TXOK0,
                                                         CAN_TSR_TXOK1,
                                                         CAN_TSR_TXOK2};

        for (int i = 0; i < TX_MAILBOXES; i++) {

                if (CAN1->TSR & CAN_TSR_RQCP[i]) {

                        int err = EIO;

                        if (CAN1->TSR & CAN_TSR_TERR[i]) {
                                err = EIO;
                        } else if (CAN1->TSR & CAN_TSR_ALST[i]) {
                                err = EAGAIN;
                        } else if (CAN1->TSR & CAN_TSR_TXOK[i]) {
                                err = ESUCC;
                        }

                        SET_BIT(CAN1->TSR, CAN_TSR_RQCP[i]);

                        bool woken = false;
                        sys_queue_send_from_ISR(CANM->txrdy_q[i], &err, &woken);

                        yield |= woken;
                }
        }

        sys_thread_yield_from_ISR(yield);
}

//==============================================================================
/**
 * @brief CAN FIFO0 Rx IRQ.
 */
//==============================================================================
void USB_LP_CAN1_RX0_IRQHandler(void)
{
        bool yield = false;

        sys_queue_send_from_ISR(CANM->rxqueue_q, &CAN1->sFIFOMailBox[0], &yield);

        SET_BIT(CAN1->RF0R, CAN_RF0R_RFOM0);

        sys_thread_yield_from_ISR(yield);
}

//==============================================================================
/**
 * @brief CAN FIFO1 Rx IRQ.
 */
//==============================================================================
void CAN1_RX1_IRQHandler(void)
{
        bool yield = false;

        sys_queue_send_from_ISR(CANM->rxqueue_q, &CAN1->sFIFOMailBox[1], &yield);

        SET_BIT(CAN1->RF1R, CAN_RF0R_RFOM0);

        sys_thread_yield_from_ISR(yield);
}

//==============================================================================
/**
 * @brief Bus change IRQ.
 */
//==============================================================================
void CAN1_SCE_IRQHandler(void)
{

}

/*==============================================================================
  End of file
==============================================================================*/
