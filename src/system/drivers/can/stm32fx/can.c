/*==============================================================================
File    can.c

Author  Daniel Zorychta

Brief   CAN driver

        Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32fx/can_cfg.h"
#include "../can_ioctl.h"

#if defined(ARCH_stm32f1)
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"
#elif defined(ARCH_stm32f3)
#include "stm32f3/stm32f3xx.h"
#include "stm32f3/lib/stm32f3xx_ll_rcc.h"
#elif defined(ARCH_stm32f4)
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"
#elif defined(ARCH_stm32f7)
#include "stm32f7/stm32f7xx.h"
#include "stm32f7/lib/stm32f7xx_ll_rcc.h"
#endif

/*==============================================================================
  Local macros
==============================================================================*/
#define TX_MAILBOXES            3
#define RX_MAILBOXES            2
#define RX_FIFO_DEPTH           __CAN_CFG_RX_FIFO_SIZE__
#define INIT_TIMEOUT            2000
#define MTX_TIMEOUT             2000

#if defined(ARCH_stm32f1)
#if defined(STM32F10X_LD) || defined(STM32F10X_MD) || defined(STM32F10X_HD)\
 || defined(STM32F10X_XL)
#define CAN1_TX_IRQn            USB_HP_CAN1_TX_IRQn
#define CAN1_RX0_IRQn           USB_LP_CAN1_RX0_IRQn
#define CAN1_TX_IRQHANDLER      USB_HP_CAN1_TX_IRQHandler
#define CAN1_RX0_IRQHANDLER     USB_LP_CAN1_RX0_IRQHandler
#define CAN1_RX1_IRQHANDLER     CAN1_RX1_IRQHandler
#define CAN1_SCE_IRQHANDLER     CAN1_SCE_IRQHandler
#endif

#ifdef STM32F10X_CL
#define CAN1_TX_IRQHANDLER      CAN1_TX_IRQHandler
#define CAN1_RX0_IRQHANDLER     CAN1_RX0_IRQHandler
#define CAN1_RX1_IRQHANDLER     CAN1_RX1_IRQHandler
#define CAN1_SCE_IRQHANDLER     CAN1_SCE_IRQHandler
#define CAN2_TX_IRQHANDLER      CAN2_TX_IRQHandler
#define CAN2_RX0_IRQHANDLER     CAN2_RX0_IRQHandler
#define CAN2_RX1_IRQHANDLER     CAN2_RX1_IRQHandler
#define CAN2_SCE_IRQHANDLER     CAN2_SCE_IRQHandler
#endif
#endif

#if defined(ARCH_stm32f3)
#define CAN1_TX_IRQn            CAN_TX_IRQn
#define CAN1_RX0_IRQn           CAN_RX0_IRQn
#define CAN1_TX_IRQHANDLER      USB_HP_CAN_TX_IRQHandler
#define CAN1_RX0_IRQHANDLER     USB_LP_CAN_RX0_IRQHandler
#define CAN1_RX1_IRQHANDLER     CAN_RX1_IRQHandler
#define CAN1_SCE_IRQHandler     CAN_SCE_IRQHandler
#define RCC_APB1ENR_CAN1EN      RCC_APB1ENR_CANEN
#define RCC_APB1RSTR_CAN1RST    RCC_APB1RSTR_CANRST
#define RCC_APB1RSTR_CAN1RST    RCC_APB1RSTR_CANRST
#define CAN1_RX0_IRQn           CAN_RX0_IRQn
#define CAN1_RX1_IRQn           CAN_RX1_IRQn
#define CAN1_SCE_IRQn           CAN_SCE_IRQn
#endif

#if defined(ARCH_stm32f4)
#define CAN1_TX_IRQHANDLER      CAN1_TX_IRQHandler
#define CAN1_RX0_IRQHANDLER     CAN1_RX0_IRQHandler
#define CAN1_RX1_IRQHANDLER     CAN1_RX1_IRQHandler
#define CAN1_SCE_IRQHANDLER     CAN1_SCE_IRQHandler
#define CAN2_TX_IRQHANDLER      CAN2_TX_IRQHandler
#define CAN2_RX0_IRQHANDLER     CAN2_RX0_IRQHandler
#define CAN2_RX1_IRQHANDLER     CAN2_RX1_IRQHandler
#define CAN2_SCE_IRQHANDLER     CAN2_SCE_IRQHandler
#endif

#if defined(ARCH_stm32f7)
#define CAN1_TX_IRQHANDLER      CAN1_TX_IRQHandler
#define CAN1_RX0_IRQHANDLER     CAN1_RX0_IRQHandler
#define CAN1_RX1_IRQHANDLER     CAN1_RX1_IRQHandler
#define CAN1_SCE_IRQHANDLER     CAN1_SCE_IRQHandler
#define CAN2_TX_IRQHANDLER      CAN2_TX_IRQHandler
#define CAN2_RX0_IRQHANDLER     CAN2_RX0_IRQHandler
#define CAN2_RX1_IRQHANDLER     CAN2_RX1_IRQHandler
#define CAN2_SCE_IRQHANDLER     CAN2_SCE_IRQHandler
#define CAN3_TX_IRQHANDLER      CAN3_TX_IRQHandler
#define CAN3_RX0_IRQHANDLER     CAN3_RX0_IRQHandler
#define CAN3_RX1_IRQHANDLER     CAN3_RX1_IRQHandler
#define CAN3_SCE_IRQHANDLER     CAN3_SCE_IRQHandler
#endif

/*==============================================================================
  Local object types
==============================================================================*/
enum CAN_COUNT {
        #if defined(RCC_APB1ENR_CAN1EN)
        _CAN1,
        #endif
        #if defined(RCC_APB1ENR_CAN2EN)
        _CAN2,
        #endif
        #if defined(RCC_APB1ENR_CAN3EN)
        _CAN3,
        #endif
        _CAN_COUNT
};

typedef struct {
        u8_t     major;
        u32_t    send_timeout;
        u32_t    recv_timeout;
        u32_t    txpend_ctr;
        u32_t    rxpend_ctr;
        kmtx_t *config_mtx;
        kmtx_t *txmbox_mtx[TX_MAILBOXES];
        kqueue_t *txrdy_q[TX_MAILBOXES];
        kqueue_t *rxqueue_q;
        u64_t    tx_frames;
        u64_t    tx_bytes;
        u64_t    rx_frames;
        u64_t    rx_bytes;
        u32_t    rx_overrun;
        u32_t    baud;
        CAN_mode_t mode;
} CAN_t;

/* CAN registers */
typedef struct {
        CAN_TypeDef   *CAN;
        __IO uint32_t *APBENR;
        __IO uint32_t *APBRSTR;
        uint32_t       APBENR_CANEN;
        uint32_t       APBRSTR_CANRSTR;
        IRQn_Type      TX_IRQn;
        IRQn_Type      RX0_IRQn;
        IRQn_Type      RX1_IRQn;
        IRQn_Type      SCE_IRQn;
        u8_t           FILTERS_COUNT;
} CAN_CFG_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void relese_resources(CAN_t *hdl);
static int configure(CAN_t *hdl, const CAN_config_t *cfg);
static int set_init_mode(CAN_t *hdl);
static int set_normal_mode(CAN_t *hdl);
static int set_sleep_mode(CAN_t *hdl);
static int set_filter(CAN_t *hdl, const CAN_filter_t *filter);
static int disable_filter(CAN_t *hdl, u32_t filter_no);
static int send_msg(CAN_t *hdl, const CAN_msg_t *msg, u32_t timeout_ms);
static int recv_msg(CAN_t *hdl, CAN_msg_t *msg, u32_t timeout_ms);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(CAN);

static CAN_t *CAN_HDL[_CAN_COUNT];

static const CAN_CFG_t CANX[] = {
        #if defined(RCC_APB1ENR_CAN1EN)
        {
                .CAN             = CAN1,
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_CANEN    = RCC_APB1ENR_CAN1EN,
                .APBRSTR_CANRSTR = RCC_APB1RSTR_CAN1RST,
                .TX_IRQn         = CAN1_TX_IRQn,
                .RX0_IRQn        = CAN1_RX0_IRQn,
                .RX1_IRQn        = CAN1_RX1_IRQn,
                .SCE_IRQn        = CAN1_SCE_IRQn,
                .FILTERS_COUNT   = 14,
        },
        #endif
        #if defined(RCC_APB1ENR_CAN2EN)
        {
                .CAN             = CAN2,
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_CANEN    = RCC_APB1ENR_CAN2EN,
                .APBRSTR_CANRSTR = RCC_APB1RSTR_CAN2RST,
                .TX_IRQn         = CAN2_TX_IRQn,
                .RX0_IRQn        = CAN2_RX0_IRQn,
                .RX1_IRQn        = CAN2_RX1_IRQn,
                .SCE_IRQn        = CAN2_SCE_IRQn,
                .FILTERS_COUNT   = 14,
        },
        #endif
        #if defined(RCC_APB1ENR_CAN3EN)
        {
                .CAN             = CAN3,
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_CANEN    = RCC_APB1ENR_CAN3EN,
                .APBRSTR_CANRSTR = RCC_APB1RSTR_CAN3RST,
                .TX_IRQn         = CAN3_TX_IRQn,
                .RX0_IRQn        = CAN3_RX0_IRQn,
                .RX1_IRQn        = CAN3_RX1_IRQn,
                .SCE_IRQn        = CAN3_SCE_IRQn,
                .FILTERS_COUNT   = 14,
        },
        #endif
};

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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_INIT(CAN, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        int err = EINVAL;

        if (major < _CAN_COUNT && minor == 0) {

                err = sys_zalloc(sizeof(CAN_t), device_handle);
                if (!err) {

                        CAN_t *hdl = *device_handle;

                        hdl->major        = major;
                        hdl->recv_timeout = _MAX_DELAY_MS;
                        hdl->send_timeout = _MAX_DELAY_MS;
                        hdl->mode         = CAN_MODE__INIT;

                        err = sys_mutex_create(KMTX_TYPE_RECURSIVE, &hdl->config_mtx);
                        if (err) {
                                goto finish;
                        }

                        err = sys_queue_create(RX_MAILBOXES * RX_FIFO_DEPTH,
                                               sizeof(CAN_FIFOMailBox_TypeDef),
                                               &hdl->rxqueue_q);
                        if (err) {
                                goto finish;
                        }

                        for (int i = 0; i < TX_MAILBOXES; i++) {
                                err = sys_mutex_create(KMTX_TYPE_NORMAL, &hdl->txmbox_mtx[i]);
                                if (err) {
                                        goto finish;
                                }

                                err = sys_queue_create(1, sizeof(int), &hdl->txrdy_q[i]);
                                if (err) {
                                        goto finish;
                                }
                        }

                        #if defined(RCC_APB1ENR_CAN2EN)
                        if (major == _CAN2) {
                                // CAN2 is slave of CAN1, this means that CAN1
                                // should be enabled to get filter access
                                SET_BIT(*CANX[_CAN1].APBENR, CANX[_CAN1].APBENR_CANEN);
                        }
                        #endif

                        SET_BIT(*CANX[hdl->major].APBENR, CANX[hdl->major].APBENR_CANEN);

                        #if defined(RCC_APB1ENR_CAN2EN)
                        if ((major == _CAN1) && (*CANX[_CAN2].APBENR & CANX[_CAN2].APBENR_CANEN)) {
                                // when CAN2 is already used then CAN1 cannot be reset

                        } else
                        #endif
                        {
                                SET_BIT(*CANX[hdl->major].APBRSTR, CANX[hdl->major].APBRSTR_CANRSTR);
                                CLEAR_BIT(*CANX[hdl->major].APBRSTR, CANX[hdl->major].APBRSTR_CANRSTR);
                        }

                        SET_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_RESET);
                        CLEAR_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_RESET);

                        //  Priority driven TX FIFO by the request order (chronologically)
                        SET_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_TXFP);

                        NVIC_ClearPendingIRQ(CANX[hdl->major].TX_IRQn);
                        NVIC_EnableIRQ(CANX[hdl->major].TX_IRQn);
                        NVIC_SetPriority(CANX[hdl->major].TX_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                        NVIC_ClearPendingIRQ(CANX[hdl->major].RX0_IRQn);
                        NVIC_EnableIRQ(CANX[hdl->major].RX0_IRQn);
                        NVIC_SetPriority(CANX[hdl->major].RX0_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                        NVIC_ClearPendingIRQ(CANX[hdl->major].RX1_IRQn);
                        NVIC_EnableIRQ(CANX[hdl->major].RX1_IRQn);
                        NVIC_SetPriority(CANX[hdl->major].RX1_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                        NVIC_ClearPendingIRQ(CANX[hdl->major].SCE_IRQn);
                        NVIC_EnableIRQ(CANX[hdl->major].SCE_IRQn);
                        NVIC_SetPriority(CANX[hdl->major].SCE_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                        err = set_init_mode(hdl);
                        if (err) {
                                goto finish;
                        }

                        if (config) {
                                err = configure(hdl, config);
                        }

                        finish:
                        if (!err) {
                                CAN_HDL[major] = hdl;

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
        CAN_t *hdl = device_handle;

        // lock access to module
        NVIC_ClearPendingIRQ(CANX[hdl->major].TX_IRQn);
        NVIC_DisableIRQ(CANX[hdl->major].TX_IRQn);

        NVIC_ClearPendingIRQ(CANX[hdl->major].RX0_IRQn);
        NVIC_DisableIRQ(CANX[hdl->major].RX0_IRQn);

        NVIC_ClearPendingIRQ(CANX[hdl->major].RX1_IRQn);
        NVIC_DisableIRQ(CANX[hdl->major].RX1_IRQn);

        NVIC_ClearPendingIRQ(CANX[hdl->major].SCE_IRQn);
        NVIC_DisableIRQ(CANX[hdl->major].SCE_IRQn);

        sys_sleep_ms(100);

        CAN_HDL[hdl->major] = NULL;

        // finish TX pending transfers
        int err = ECANCELED;

        for (u32_t n = 0; n < hdl->txpend_ctr; n++) {

                for (int i = 0; i < TX_MAILBOXES; i++) {
                        sys_queue_send(hdl->txrdy_q[i], &err, 0);
                }

                sys_sleep_ms(10);
        }

        // finish RX pending queue
        CAN_FIFOMailBox_TypeDef mbox;
        memset(&mbox, 0, sizeof(CAN_FIFOMailBox_TypeDef));

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

        return ESUCC;
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

        CAN_t *hdl = device_handle;

        int err = EINVAL;

        if ((count % sizeof(CAN_msg_t)) == 0) {

                while (count >= sizeof(CAN_msg_t)) {

                        const CAN_msg_t *msg = (void*)src;

                        u32_t timeout_ms = fattr.non_blocking_wr ? 0 : hdl->send_timeout;

                        err = send_msg(hdl, msg, timeout_ms);

                        if (!err) {
                                *wrcnt += sizeof(CAN_msg_t);
                                src    += sizeof(CAN_msg_t);
                                count  -= sizeof(CAN_msg_t);
                        } else {
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

        CAN_t *hdl = device_handle;

        int err = EINVAL;

        if ((count % sizeof(CAN_msg_t)) == 0) {

                while (count >= sizeof(CAN_msg_t)) {

                        CAN_msg_t msg;
                        u32_t timeout_ms = fattr.non_blocking_rd ? 0 : hdl->recv_timeout;

                        err = recv_msg(hdl, &msg, timeout_ms);

                        if (!err) {
                                memcpy(dst, &msg, sizeof(CAN_msg_t));

                                *rdcnt += sizeof(CAN_msg_t);
                                dst    += sizeof(CAN_msg_t);
                                count  -= sizeof(CAN_msg_t);
                        } else {
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
        CAN_t *hdl = device_handle;

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

        case IOCTL_CAN__DISABLE_FILTER:
                if (arg) {
                        err = disable_filter(hdl, *cast(u32_t*, arg));
                }
                break;

        case IOCTL_CAN__GET_NUMBER_OF_FILTERS:
                if (arg) {
                        *cast(u32_t*, arg) = CANX[hdl->major].FILTERS_COUNT;
                        err = ESUCC;
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

                        if (!err) {
                                hdl->mode = mode;
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
                        err = send_msg(hdl, arg, hdl->send_timeout);
                }
                break;

        case IOCTL_CAN__RECV_MSG:
                if (arg) {
                        err = recv_msg(hdl, arg, hdl->recv_timeout);
                }
                break;

        case IOCTL_CAN__GET_STATUS:
                if (arg) {
                        CAN_status_t *status = arg;

                        // CAN bus status
                        if (CANX[hdl->major].CAN->ESR & CAN_ESR_BOFF) {
                                status->can_bus_status = CAN_BUS_STATUS__OFF;

                        } else if (CANX[hdl->major].CAN->ESR & CAN_ESR_EPVF) {
                                status->can_bus_status = CAN_BUS_STATUS__PASSIVE;

                        } else if (CANX[hdl->major].CAN->ESR & CAN_ESR_EWGF) {
                                status->can_bus_status = CAN_BUS_STATUS__WARNING;

                        } else {
                                status->can_bus_status = CAN_BUS_STATUS__OK;
                        }

                        // CAN mode
                        status->mode = hdl->mode;

                        // error countes
                        status->rx_error_ctr   = ((CANX[hdl->major].CAN->ESR & CAN_ESR_REC) >> 24);
                        status->tx_error_ctr   = ((CANX[hdl->major].CAN->ESR & CAN_ESR_TEC) >> 16);
                        status->rx_overrun_ctr = hdl->rx_overrun;

                        // transfer stats
                        status->tx_frames = hdl->tx_frames;
                        status->tx_bytes  = hdl->tx_bytes;
                        status->rx_frames = hdl->rx_frames;
                        status->rx_bytes  = hdl->rx_bytes;

                        // baud
                        status->baud_bps = hdl->baud;

                        err = ESUCC;
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
        CAN_t *hdl = device_handle;

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
static void relese_resources(CAN_t *hdl)
{
        SET_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_RESET);
        CLEAR_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_RESET);

        #if defined(RCC_APB1ENR_CAN2EN)
        if ((hdl->major == _CAN1) && (*CANX[_CAN2].APBENR & CANX[_CAN2].APBENR_CANEN)) {
                // when CAN2 is already used then CAN1 cannot be reset

        } else
        #endif
        {
                SET_BIT(*CANX[hdl->major].APBRSTR, CANX[hdl->major].APBRSTR_CANRSTR);
                CLEAR_BIT(*CANX[hdl->major].APBRSTR, CANX[hdl->major].APBRSTR_CANRSTR);
                CLEAR_BIT(*CANX[hdl->major].APBENR, CANX[hdl->major].APBENR_CANEN);

                #if defined(RCC_APB1ENR_CAN2EN)
                if ((hdl->major == _CAN2) && (CAN_HDL[_CAN1] == NULL)) {
                        CLEAR_BIT(*CANX[_CAN1].APBENR, CANX[_CAN1].APBENR_CANEN);
                }
                #endif
        }

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
static int configure(CAN_t *hdl, const CAN_config_t *cfg)
{
        int err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
        if (!err) {
                SET_BIT(CANX[hdl->major].CAN->MCR, (CAN_MCR_DBF * _CAN_CFG__DEBUG_FREEZE)
                                                 | CAN_MCR_TXFP
                                                 | CAN_MCR_RFLM);

                SET_BIT(CANX[hdl->major].CAN->IER, CAN_IER_FMPIE1
                                                 | CAN_IER_FMPIE0
                                                 | CAN_IER_TMEIE);

                if (cfg->auto_bus_off_management) {
                        SET_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_ABOM);
                } else {
                        CLEAR_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_ABOM);
                }

                if (cfg->auto_retransmission) {
                        CLEAR_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_NART);
                } else {
                        SET_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_NART);
                }

                if (cfg->auto_wake_up) {
                        SET_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_AWUM);
                } else {
                        CLEAR_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_AWUM);
                }

                if (cfg->time_triggered_comm) {
                        SET_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_TTCM);
                } else {
                        CLEAR_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_TTCM);
                }

                if (cfg->loopback) {
                        SET_BIT(CANX[hdl->major].CAN->BTR, CAN_BTR_LBKM);
                } else {
                        CLEAR_BIT(CANX[hdl->major].CAN->BTR, CAN_BTR_LBKM);
                }

                if (cfg->silent) {
                        SET_BIT(CANX[hdl->major].CAN->BTR, CAN_BTR_SILM);
                } else {
                        CLEAR_BIT(CANX[hdl->major].CAN->BTR, CAN_BTR_SILM);
                }

                u32_t SJW = cfg->SJW;
                u32_t TS1 = cfg->TS1;
                u32_t TS2 = cfg->TS2;
                u32_t BRP = cfg->prescaler;

                if ((SJW < 1) || (SJW > 4)) {
                        SJW = min(4, SJW);
                        SJW = max(1, SJW);
                        printk("%s%d-%d: SJW out of range! Applied %d", GET_MODULE_NAME(), hdl->major, 0, SJW);
                }

                if ((TS1 < 1) || (TS1 > 15)) {
                        TS1 = min(15, TS1);
                        TS1 = max(1, TS1);
                        printk("%s%d-%d: TS1 out of range! Applied %d", GET_MODULE_NAME(), hdl->major, 0,  TS1);
                }

                if ((TS2 < 1) || (TS2 > 7)) {
                        TS2 = min(7, TS2);
                        TS2 = max(1, TS2);
                        printk("%s%d-%d: TS2 out of range! Applied %d", GET_MODULE_NAME(), hdl->major, 0,  TS2);
                }

                if ((BRP < 1) || (BRP > 1024)) {
                        BRP = min(1024, BRP);
                        BRP = max(1, BRP);
                        printk("%s%d-%d: BRP out of range! Applied %d", GET_MODULE_NAME(), hdl->major, 0,  BRP);
                }

                CANX[hdl->major].CAN->BTR = (CAN_BTR_LBKM * cfg->loopback)
                                          | (CAN_BTR_SILM * cfg->silent)
                                          | ((SJW - 1) << 24)
                                          | ((TS2 - 1) << 20)
                                          | ((TS1 - 1) << 16)
                                          | ((BRP - 1) <<  0);

                LL_RCC_ClocksTypeDef freq;
                LL_RCC_GetSystemClocksFreq(&freq);
                u32_t PCLK = freq.PCLK1_Frequency;

                hdl->baud = PCLK / (cfg->prescaler * (1 + TS1 + TS2));
                printk("%s%d-%d: baud rate: %u bps", GET_MODULE_NAME(), hdl->major, 0,  hdl->baud);

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
static int set_init_mode(CAN_t *hdl)
{
        int err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
        if (!err) {
                err = EIO;

                clock_t tref = sys_get_uptime_ms();

                CLEAR_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_SLEEP);
                SET_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_INRQ);

                while (not sys_is_time_expired(tref, INIT_TIMEOUT)) {
                        if (CANX[hdl->major].CAN->MSR & CAN_MSR_INAK) {
                                hdl->tx_bytes  = 0;
                                hdl->rx_bytes  = 0;
                                hdl->tx_frames = 0;
                                hdl->rx_frames = 0;
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
static int set_normal_mode(CAN_t *hdl)
{
        int err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
        if (!err) {
                err = EIO;

                clock_t tref = sys_get_uptime_ms();

                CLEAR_BIT(CANX[hdl->major].CAN->FMR, CAN_FMR_FINIT);
                CLEAR_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_INRQ | CAN_MCR_SLEEP);

                while (not sys_is_time_expired(tref, INIT_TIMEOUT)) {
                        if (!(CANX[hdl->major].CAN->MSR & CAN_MSR_INAK)) {
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
static int set_sleep_mode(CAN_t *hdl)
{
        int err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
        if (!err) {
                SET_BIT(CANX[hdl->major].CAN->MCR, CAN_MCR_SLEEP);
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
static int set_filter(CAN_t *hdl, const CAN_filter_t *filter)
{
        if (filter->number >= CANX[hdl->major].FILTERS_COUNT) {
                return EINVAL;
        }

        int err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
        if (!err) {
                u8_t major = hdl->major;
                u8_t filter_no = filter->number;

                #if defined(RCC_APB1ENR_CAN2EN)
                // CAN2 is slave of CAN1
                // CAN1 and CAN2 have common registers
                if (hdl->major == _CAN2) {
                        filter_no += CANX[_CAN1].FILTERS_COUNT;
                        major = _CAN1;
                }
                #endif

                // CAN2 filter offset + filter initialization bit
                CANX[major].CAN->FMR = (CANX[_CAN1].FILTERS_COUNT << 8) | CAN_FMR_FINIT;

                // mask mode
                CLEAR_BIT(CANX[major].CAN->FM1R, CAN_FM1R_FBM0 << filter_no);

                // 32b ID
                SET_BIT(CANX[major].CAN->FS1R, CAN_FS1R_FSC0 << filter_no);

                // FIFO0: odd filters, FIFO1: even filters)
                WRITE_REG(CANX[major].CAN->FFA1R, 0xAAAAAAAA);

                // set ID and mask
                if (filter->extended_ID) {
                        CANX[major].CAN->sFilterRegister[filter_no].FR1 = (filter->ID << 3) | CAN_TI0R_IDE;
                        CANX[major].CAN->sFilterRegister[filter_no].FR2 = (filter->mask << 3) | CAN_TI0R_IDE;

                } else {
                        CANX[major].CAN->sFilterRegister[filter_no].FR1 = (filter->ID << 21);
                        CANX[major].CAN->sFilterRegister[filter_no].FR2 = (filter->mask << 21);
                }

                SET_BIT(CANX[major].CAN->FA1R, CAN_FA1R_FACT0 << filter_no);

                sys_mutex_unlock(hdl->config_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function disable selected filter number. To disable filter INIT mode
 *         must be activated.
 *
 * @param  hdl                  module instance
 * @param  filter_no            filter number
 *
 * @return One of errno value.
 */
//==============================================================================
static int disable_filter(CAN_t *hdl, u32_t filter_no)
{
        int err = EINVAL;

        if (filter_no < CANX[hdl->major].FILTERS_COUNT) {

                err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
                if (!err) {
                        u8_t major = hdl->major;

                        #if defined(RCC_APB1ENR_CAN2EN)
                        // CAN2 is slave of CAN1
                        // CAN1 and CAN2 have common registers
                        if (hdl->major == _CAN2) {
                                filter_no += CANX[_CAN1].FILTERS_COUNT;
                                major = _CAN1;
                        }
                        #endif

                        // CAN2 filter offset + filter initialization bit
                        CANX[major].CAN->FMR = (CANX[_CAN1].FILTERS_COUNT << 8) | CAN_FMR_FINIT;

                        CLEAR_BIT(CANX[major].CAN->FA1R, CAN_FA1R_FACT0 << filter_no);

                        sys_mutex_unlock(hdl->config_mtx);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function send CAN message.
 *
 * @param  hdl          module instance
 * @param  msg          message to send
 * @param  timeout_ms   timeout in milliseconds
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_msg(CAN_t *hdl, const CAN_msg_t *msg, u32_t timeout_ms)
{
        sys_context_switch_lock();
        hdl->txpend_ctr++;
        sys_context_switch_unlock();

        int   err     = ETIME;
        bool  loop    = true;
        u32_t timeout = max(1, timeout_ms);

        clock_t tref = sys_get_uptime_ms();

        while (loop && !sys_is_time_expired(tref, timeout)) {

                err = ETIME;

                for (int i = 0; loop && (i < TX_MAILBOXES); i++) {

                        err = sys_mutex_trylock(hdl->txmbox_mtx[i]);
                        if (!err) {
                                CAN_TxMailBox_TypeDef *mbox = &CANX[hdl->major].CAN->sTxMailBox[i];

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

                                                if (!err) {
                                                        hdl->tx_frames++;
                                                        hdl->tx_bytes += msg->data_length;
                                                }

                                        } else {
                                                printk("%s%d-%d: message send abort", GET_MODULE_NAME(), hdl->major, 0);
                                                SET_BIT(CANX[hdl->major].CAN->TSR, CAN_TSR_ABRQ0 << (i * 8));
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
static int recv_msg(CAN_t *hdl, CAN_msg_t *msg, u32_t timeout_ms)
{
        sys_context_switch_lock();
        hdl->rxpend_ctr++;
        sys_context_switch_unlock();

        CAN_FIFOMailBox_TypeDef mbox;

        int err = sys_queue_receive(hdl->rxqueue_q, &mbox, timeout_ms);
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
 * @brief  CAN Tx handle function.
 *
 * @param  hdl          driver handle
 *
 * @return True if yield needed.
 */
//==============================================================================
static bool CAN_TX_IRQ(CAN_t *hdl)
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

                if (CANX[hdl->major].CAN->TSR & CAN_TSR_RQCP[i]) {

                        int err = EIO;

                        if (CANX[hdl->major].CAN->TSR & CAN_TSR_TERR[i]) {

                                switch ((CANX[hdl->major].CAN->ESR & CAN_ESR_LEC) >> 4) {
                                case 3:  err = EFAULT; break;
                                case 6:  err = EILSEQ; break;
                                default: err = EIO; break;
                                }

                        } else if (CANX[hdl->major].CAN->TSR & CAN_TSR_ALST[i]) {
                                err = EAGAIN;

                        } else if (CANX[hdl->major].CAN->TSR & CAN_TSR_TXOK[i]) {
                                err = ESUCC;
                        }

                        SET_BIT(CANX[hdl->major].CAN->TSR, CAN_TSR_RQCP[i]);

                        bool woken = false;
                        sys_queue_send_from_ISR(hdl->txrdy_q[i], &err, &woken);

                        yield |= woken;
                }
        }

        return yield;
}

//==============================================================================
/**
 * @brief  Function receive FIFO message.
 *
 * @param  hdl          driver handle
 * @param  fifo         fifo number
 *
 * @return True if yield needed.
 */
//==============================================================================
static bool CAN_RX_IRQ(CAN_t *hdl, u8_t fifo)
{
        bool yield = false;

        CAN_FIFOMailBox_TypeDef *mbox = &CANX[hdl->major].CAN->sFIFOMailBox[fifo];

        if (sys_queue_send_from_ISR(hdl->rxqueue_q, mbox, &yield) != 0) {
                hdl->rx_overrun++;

        } else {
                hdl->rx_frames++;
                hdl->rx_bytes += mbox->RDTR & CAN_RDT0R_DLC;
        }

        if (fifo == 0) {
                SET_BIT(CANX[hdl->major].CAN->RF0R, CAN_RF0R_RFOM0);
        } else {
                SET_BIT(CANX[hdl->major].CAN->RF1R, CAN_RF1R_RFOM1);
        }

        return yield;
}

//==============================================================================
/**
 * @brief CAN1 TX ready IRQ.
 */
//==============================================================================
#if defined(RCC_APB1ENR_CAN1EN)
void CAN1_TX_IRQHANDLER(void)
{
        CAN_t *hdl = CAN_HDL[_CAN1];
        if (hdl) {
                bool yield = CAN_TX_IRQ(hdl);
                sys_thread_yield_from_ISR(yield);
        }
}
#endif

//==============================================================================
/**
 * @brief CAN1 FIFO0 Rx IRQ.
 */
//==============================================================================
#if defined(RCC_APB1ENR_CAN1EN)
void CAN1_RX0_IRQHANDLER(void)
{
        CAN_t *hdl = CAN_HDL[_CAN1];
        if (hdl) {
                bool yield = CAN_RX_IRQ(hdl, 0);
                sys_thread_yield_from_ISR(yield);
        }
}
#endif

//==============================================================================
/**
 * @brief CAN1 FIFO1 Rx IRQ.
 */
//==============================================================================
#if defined(RCC_APB1ENR_CAN1EN)
void CAN1_RX1_IRQHANDLER(void)
{
        CAN_t *hdl = CAN_HDL[_CAN1];
        if (hdl) {
                bool yield = CAN_RX_IRQ(hdl, 1);
                sys_thread_yield_from_ISR(yield);
        }
}
#endif

//==============================================================================
/**
 * @brief CAN1 status change IRQ.
 */
//==============================================================================
#if defined(RCC_APB1ENR_CAN1EN)
void CAN1_SCE_IRQHANDLER(void)
{

}
#endif

//==============================================================================
/**
 * @brief CAN2 TX ready IRQ.
 */
//==============================================================================
#if defined(RCC_APB1ENR_CAN2EN)
void CAN2_TX_IRQHANDLER(void)
{
        CAN_t *hdl = CAN_HDL[_CAN2];
        if (hdl) {
                bool yield = CAN_TX_IRQ(hdl);
                sys_thread_yield_from_ISR(yield);
        }
}
#endif

//==============================================================================
/**
 * @brief CAN2 FIFO0 Rx IRQ.
 */
//==============================================================================
#if defined(RCC_APB1ENR_CAN2EN)
void CAN2_RX0_IRQHANDLER(void)
{
        CAN_t *hdl = CAN_HDL[_CAN2];
        if (hdl) {
                bool yield = CAN_RX_IRQ(hdl, 0);
                sys_thread_yield_from_ISR(yield);
        }
}
#endif

//==============================================================================
/**
 * @brief CAN2 FIFO1 Rx IRQ.
 */
//==============================================================================
#if defined(RCC_APB1ENR_CAN2EN)
void CAN2_RX1_IRQHANDLER(void)
{
        CAN_t *hdl = CAN_HDL[_CAN2];
        if (hdl) {
                bool yield = CAN_RX_IRQ(hdl, 1);
                sys_thread_yield_from_ISR(yield);
        }
}
#endif

//==============================================================================
/**
 * @brief CAN2 status change IRQ.
 */
//==============================================================================
#if defined(RCC_APB1ENR_CAN2EN)
void CAN2_SCE_IRQHANDLER(void)
{

}
#endif

//==============================================================================
/**
 * @brief CAN3 FIFO0 Rx IRQ.
 */
//==============================================================================
#if defined(RCC_APB1ENR_CAN3EN)
void CAN3_RX0_IRQHANDLER(void)
{
        CAN_t *hdl = CAN_HDL[_CAN3];
        if (hdl) {
                bool yield = CAN_RX_IRQ(hdl, 0);
                sys_thread_yield_from_ISR(yield);
        }
}
#endif

//==============================================================================
/**
 * @brief CAN3 TX ready IRQ.
 */
//==============================================================================
#if defined(RCC_APB1ENR_CAN3EN)
void CAN3_TX_IRQHANDLER(void)
{
        CAN_t *hdl = CAN_HDL[_CAN3];
        if (hdl) {
                bool yield = CAN_TX_IRQ(hdl);
                sys_thread_yield_from_ISR(yield);
        }
}
#endif

//==============================================================================
/**
 * @brief CAN3 FIFO1 Rx IRQ.
 */
//==============================================================================
#if defined(RCC_APB1ENR_CAN3EN)
void CAN3_RX1_IRQHANDLER(void)
{
        CAN_t *hdl = CAN_HDL[_CAN3];
        if (hdl) {
                bool yield = CAN_RX_IRQ(hdl, 1);
                sys_thread_yield_from_ISR(yield);
        }
}
#endif

//==============================================================================
/**
 * @brief CAN3 status change IRQ.
 */
//==============================================================================
#if defined(RCC_APB1ENR_CAN3EN)
void CAN3_SCE_IRQHANDLER(void)
{

}
#endif

/*==============================================================================
  End of file
==============================================================================*/
