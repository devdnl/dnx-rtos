/*==============================================================================
File    can.c

Author  Daniel Zorychta

Brief   CAN driver

        Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "config.h"
#include "../can_ioctl.h"
#include "stm32h7xx_hal_fdcan.h"
#include "drivers/driver.h"
#include "stm32h7/stm32h7xx.h"
#include "stm32h7/lib/stm32h7xx_ll_rcc.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define TX_BUFFERS              8
#define RX_FIFO_DEPTH           __CAN_CFG_RX_FIFO_SIZE__
#define MTX_TIMEOUT             2000
#define MESSAGE_RAM_SIZE_WORDS  2560
#define SHARED_MESSAGE_RAM_SIZE (MESSAGE_RAM_SIZE_WORDS / _CAN_COUNT)
#define FILTERS_COUNT           32
#define FDCAN_TX_BUFFER_SIZE    18
#define FDCAN_STD_FILTER_SIZE   1
#define FDCAN_EXT_FILTER_SIZE   2
#define FDCAN_RX_FIFO_SIZE      18

/*==============================================================================
  Local object types
==============================================================================*/
enum CAN_COUNT {
        #if defined(FDCAN1)
        _CAN1,
        #endif
        #if defined(FDCAN2)
        _CAN2,
        #endif
        #if defined(FDCAN3)
        _CAN3,
        #endif
        _CAN_COUNT
};

typedef struct {
        u8_t                major;
        u8_t                minor;
        u32_t               send_timeout;
        u32_t               recv_timeout;
        u32_t               txpend_ctr;
        u32_t               rxpend_ctr;
        kmtx_t              *config_mtx;
        kmtx_t              *txbuf_mtx[TX_BUFFERS];
        kqueue_t            *txrdy_q[TX_BUFFERS];
        kqueue_t            *rxqueue_q;
        u64_t               tx_frames;
        u64_t               tx_bytes;
        u64_t               rx_frames;
        u64_t               rx_bytes;
        u32_t               rx_overrun;
        u32_t               nominal_baud;
        u32_t               data_baud;
        CAN_mode_t          mode;
        FDCAN_HandleTypeDef hfdcan;
        bool                irq_yield;
} CAN_t;

/* CAN registers */
typedef struct {
        FDCAN_GlobalTypeDef *CAN;
        IRQn_Type           IT0_IRQn;
        IRQn_Type           IT1_IRQn;
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

static const CAN_CFG_t CAN_CFG[] = {
        #if defined(FDCAN1)
        {
                .CAN      = FDCAN1,
                .IT0_IRQn = FDCAN1_IT0_IRQn,
                .IT1_IRQn = FDCAN1_IT1_IRQn,
        },
        #endif
        #if defined(FDCAN2)
        {
                 .CAN      = FDCAN2,
                 .IT0_IRQn = FDCAN2_IT0_IRQn,
                 .IT1_IRQn = FDCAN2_IT1_IRQn,
        },
        #endif
        #if defined(FDCAN3)
        {
                 .CAN      = FDCAN3,
                 .IT0_IRQn = FDCAN3_IT0_IRQn,
                 .IT1_IRQn = FDCAN3_IT1_IRQn,
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
                        CAN_HDL[major] = hdl;

                        hdl->major        = major;
                        hdl->recv_timeout = _MAX_DELAY_MS;
                        hdl->send_timeout = _MAX_DELAY_MS;
                        hdl->mode         = CAN_MODE__INIT;

                        err = sys_mutex_create(KMTX_TYPE_RECURSIVE, &hdl->config_mtx);
                        if (err) {
                                goto finish;
                        }

                        err = sys_queue_create(RX_FIFO_DEPTH,
                                               sizeof(CAN_msg_t),
                                               &hdl->rxqueue_q);
                        if (err) {
                                goto finish;
                        }

                        for (int i = 0; i < TX_BUFFERS; i++) {
                                err = sys_mutex_create(KMTX_TYPE_NORMAL, &hdl->txbuf_mtx[i]);
                                if (err) {
                                        goto finish;
                                }

                                err = sys_queue_create(1, sizeof(int), &hdl->txrdy_q[i]);
                                if (err) {
                                        goto finish;
                                }
                        }

                        if (config) {
                                err = configure(hdl, config);
                        }

                        finish:
                        if (err) {
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

        if (CAN_HDL[hdl->major]) {
                hdl->hfdcan.Instance = CAN_CFG[hdl->major].CAN;
                hdl->hfdcan.UserContext = hdl;
                HAL_FDCAN_DeInit(&hdl->hfdcan);
        }

        sys_sleep_ms(100);

        /*
         * Terminate TX pending transfers.
         */
        int err = ECANCELED;

        for (u32_t n = 0; n < hdl->txpend_ctr; n++) {

                for (int i = 0; i < TX_BUFFERS; i++) {
                        sys_queue_send(hdl->txrdy_q[i], &err, 0);
                }

                sys_sleep_ms(10);
        }

        /*
         * Terminate RX pending queue.
         */
        CAN_msg_t msg;
        memset(&msg, 0, sizeof(msg));

        for (u32_t n = 0; n < hdl->rxpend_ctr; n++) {
                sys_queue_send(hdl->rxqueue_q, &msg, 0);
        }

        /*
         * Wait for all transfers to be finished.
         */
        while ((hdl->txpend_ctr > 0) || (hdl->rxpend_ctr > 0)) {
                sys_sleep_ms(10);
        }

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

        if (count == sizeof(CAN_msg_t)) {

                const CAN_msg_t *msg = cast(void*, src);

                u32_t timeout_ms = fattr.non_blocking_wr ? 0 : hdl->send_timeout;

                err = send_msg(hdl, msg, timeout_ms);

                if (!err) {
                        *wrcnt += sizeof(CAN_msg_t) + msg->data_length;
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

        if (count == sizeof(CAN_msg_t)) {

                u32_t timeout_ms = fattr.non_blocking_rd ? 0 : hdl->recv_timeout;

                err = recv_msg(hdl, cast(CAN_msg_t*, dst), timeout_ms);

                if (!err) {
                        *rdcnt += sizeof(CAN_msg_t);
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
                        *cast(u32_t*, arg) = FILTERS_COUNT;
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
                        FDCAN_ProtocolStatusTypeDef protocol_status;
                        HAL_FDCAN_GetProtocolStatus(&hdl->hfdcan, &protocol_status);

                        if (protocol_status.BusOff) {
                                status->can_bus_status = CAN_BUS_STATUS__OFF;

                        } else if (protocol_status.ErrorPassive) {
                                status->can_bus_status = CAN_BUS_STATUS__PASSIVE;

                        } else if (protocol_status.Warning) {
                                status->can_bus_status = CAN_BUS_STATUS__WARNING;

                        } else {
                                status->can_bus_status = CAN_BUS_STATUS__OK;
                        }

                        // CAN mode
                        status->mode = hdl->mode;
                        status->frame_mode = CAN_FRAME_MODE__FD_WITH_BIT_RATE_SWITCH;

                        // error countes
                        FDCAN_ErrorCountersTypeDef error_counters;
                        HAL_FDCAN_GetErrorCounters(&hdl->hfdcan, &error_counters);
                        status->rx_error_ctr   = error_counters.RxErrorCnt;
                        status->tx_error_ctr   = error_counters.TxErrorCnt;
                        status->rx_overrun_ctr = hdl->rx_overrun;

                        // transfer stats
                        status->tx_frames = hdl->tx_frames;
                        status->tx_bytes  = hdl->tx_bytes;
                        status->rx_frames = hdl->rx_frames;
                        status->rx_bytes  = hdl->rx_bytes;

                        // baud
                        status->nominal_baud_bps = hdl->nominal_baud;
                        status->data_baud_bps = hdl->data_baud;

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

        items += HAL_FDCAN_GetRxFifoFillLevel(&hdl->hfdcan, FDCAN_RX_FIFO0);
        items += HAL_FDCAN_GetRxFifoFillLevel(&hdl->hfdcan, FDCAN_RX_FIFO1);

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
        if (hdl->config_mtx) {
                sys_mutex_destroy(hdl->config_mtx);
        }

        if (hdl->rxqueue_q) {
                sys_queue_destroy(hdl->rxqueue_q);
        }

        for (int i = 0; i < TX_BUFFERS; i++) {
                if (hdl->txbuf_mtx[i]) {
                        sys_mutex_destroy(hdl->txbuf_mtx[i]);
                }

                if (hdl->txrdy_q[i]) {
                        sys_queue_destroy(hdl->txrdy_q[i]);
                }
        }

        sys_free((void*)&hdl);
        CAN_HDL[hdl->major] = NULL;
}

//==============================================================================
/**
 * @brief  Check and correct (if needed) the bit rate settings.
 *
 * @param  hdl          driver handle
 * @param  SJW          segment jump width
 * @param  TS1          time quanta segment 1
 * @param  TS2          time quanta segment 2
 * @param  BRP          baud rate prescealler
 *
 * @return Calculated baud rate.
 */
//==============================================================================
static u32_t check_bit_rate_settings(CAN_t *hdl, u32_t *SJW, u32_t *TS1, u32_t *TS2, u32_t *BRP)
{
        if ((*SJW < 1) || (*SJW > 16)) {
                *SJW = min(16, *SJW);
                *SJW = max(1, *SJW);
                dev_dbg(hdl, "SJW out of range! Applied %lu", *SJW);
        }

        if ((*TS1 < 1) || (*TS1 > 32)) {
                *TS1 = min(32, *TS1);
                *TS1 = max(1, *TS1);
                dev_dbg(hdl, "TS1 out of range! Applied %lu", *TS1);
        }

        if ((*TS2 < 1) || (*TS2 > 16)) {
                *TS2 = min(16, *TS2);
                *TS2 = max(1, *TS2);
                dev_dbg(hdl, "TS2 out of range! Applied %lu", *TS2);
        }

        if ((*BRP < 1) || (*BRP > 32)) {
                *BRP = min(32, *BRP);
                *BRP = max(1, *BRP);
                dev_dbg(hdl, "BRP out of range! Applied %lu", *BRP);
        }

        u32_t PCLK = LL_RCC_GetFDCANClockFreq(LL_RCC_FDCAN_CLKSOURCE);
        return PCLK / (*BRP * (1 + *TS1 + *TS2));
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

                hdl->hfdcan.Instance = CAN_CFG[hdl->major].CAN;
                hdl->hfdcan.UserContext = hdl;

                HAL_FDCAN_DeInit(&hdl->hfdcan);

                if (cfg->silent) {
                        hdl->hfdcan.Init.Mode = FDCAN_MODE_BUS_MONITORING;
                } else if (cfg->loopback) {
                        hdl->hfdcan.Init.Mode = FDCAN_MODE_INTERNAL_LOOPBACK;
                } else {
                        hdl->hfdcan.Init.Mode = FDCAN_MODE_NORMAL;
                }

                hdl->hfdcan.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
                hdl->hfdcan.Init.AutoRetransmission = cfg->auto_retransmission;
                hdl->hfdcan.Init.TransmitPause = DISABLE;
                hdl->hfdcan.Init.ProtocolException = ENABLE;

                u32_t SJW = cfg->nominal_bit_rate.SJW;
                u32_t TS1 = cfg->nominal_bit_rate.TS1;
                u32_t TS2 = cfg->nominal_bit_rate.TS2;
                u32_t BRP = cfg->nominal_bit_rate.prescaler;
                hdl->nominal_baud = check_bit_rate_settings(hdl, &SJW, &TS1, &TS2, &BRP);
                dev_dbg(hdl, "nominal baud rate: %lu bps", hdl->nominal_baud);

                hdl->hfdcan.Init.NominalPrescaler = cfg->nominal_bit_rate.prescaler;
                hdl->hfdcan.Init.NominalSyncJumpWidth = cfg->nominal_bit_rate.SJW;
                hdl->hfdcan.Init.NominalTimeSeg1 = cfg->nominal_bit_rate.TS1;
                hdl->hfdcan.Init.NominalTimeSeg2 = cfg->nominal_bit_rate.TS2;

                SJW = cfg->data_bit_rate.SJW;
                TS1 = cfg->data_bit_rate.TS1;
                TS2 = cfg->data_bit_rate.TS2;
                BRP = cfg->data_bit_rate.prescaler;
                hdl->data_baud = check_bit_rate_settings(hdl, &SJW, &TS1, &TS2, &BRP);
                dev_dbg(hdl, "data baud rate: %lu bps", hdl->data_baud);

                hdl->hfdcan.Init.DataPrescaler = cfg->data_bit_rate.prescaler;
                hdl->hfdcan.Init.DataSyncJumpWidth = cfg->data_bit_rate.SJW;
                hdl->hfdcan.Init.DataTimeSeg1 = cfg->data_bit_rate.TS1;
                hdl->hfdcan.Init.DataTimeSeg2 = cfg->data_bit_rate.TS2;

                /*
                 * Calculate FDCAN RAM objects sizes
                 */
                size_t RAM_filter_space = (FDCAN_STD_FILTER_SIZE * FILTERS_COUNT)
                                        + (FDCAN_EXT_FILTER_SIZE * FILTERS_COUNT);

                size_t RAM_tx_buffers_space = FDCAN_TX_BUFFER_SIZE * TX_BUFFERS;

                size_t RAM_rx_fifo_space = SHARED_MESSAGE_RAM_SIZE - RAM_filter_space
                                         - RAM_tx_buffers_space;

                hdl->hfdcan.Init.MessageRAMOffset = hdl->major * SHARED_MESSAGE_RAM_SIZE;
                hdl->hfdcan.Init.StdFiltersNbr = FILTERS_COUNT;
                hdl->hfdcan.Init.ExtFiltersNbr = FILTERS_COUNT;

                hdl->hfdcan.Init.RxFifo0ElmtsNbr = min(64, (RAM_rx_fifo_space / FDCAN_RX_FIFO_SIZE));
                hdl->hfdcan.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_64;
                hdl->hfdcan.Init.RxFifo1ElmtsNbr = 0;
                hdl->hfdcan.Init.RxBuffersNbr    = 0;

                hdl->hfdcan.Init.TxBuffersNbr = TX_BUFFERS;
                hdl->hfdcan.Init.TxEventsNbr  = 0;
                hdl->hfdcan.Init.TxElmtSize   = FDCAN_DATA_BYTES_64;
                hdl->hfdcan.Init.TxFifoQueueElmtsNbr = 0;
                hdl->hfdcan.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;

                err = HAL_FDCAN_Init(&hdl->hfdcan);
                if (!err) {
                        for (size_t i = 0; i < FILTERS_COUNT; i++) {
                                FDCAN_FilterTypeDef filter_config;
                                filter_config.IdType = FDCAN_STANDARD_ID;
                                filter_config.FilterIndex = i;
                                filter_config.FilterType = FDCAN_FILTER_MASK;
                                filter_config.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
                                filter_config.FilterID1 = 0;
                                filter_config.FilterID2 = 0x7FF;
                                filter_config.RxBufferIndex = FDCAN_RX_BUFFER0;
                                filter_config.IsCalibrationMsg = false;
                                HAL_FDCAN_ConfigFilter(&hdl->hfdcan, &filter_config);

                                filter_config.IdType = FDCAN_EXTENDED_ID;
                                filter_config.FilterID1 = 0;
                                filter_config.FilterID2 = 0x1FFFFFFF;
                                HAL_FDCAN_ConfigFilter(&hdl->hfdcan, &filter_config);

                                HAL_FDCAN_ConfigGlobalFilter(&hdl->hfdcan,
                                        FDCAN_REJECT, FDCAN_REJECT,
                                        FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
                        }
                }

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
                if (hdl->mode == CAN_MODE__SLEEP) {
                        HAL_FDCAN_ExitPowerDownMode(&hdl->hfdcan);
                        hdl->mode = CAN_MODE__NORMAL;
                }

                err = HAL_FDCAN_Stop(&hdl->hfdcan);

                if (!err) {
                        hdl->tx_bytes  = 0;
                        hdl->rx_bytes  = 0;
                        hdl->tx_frames = 0;
                        hdl->rx_frames = 0;

                        hdl->mode = CAN_MODE__INIT;
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
                if (hdl->mode == CAN_MODE__INIT) {
                        err = HAL_FDCAN_Start(&hdl->hfdcan);

                } else if (hdl->mode == CAN_MODE__SLEEP) {
                        err = HAL_FDCAN_ExitPowerDownMode(&hdl->hfdcan);

                } else {
                        err = ESUCC;
                }

                if (!err) {
                        hdl->mode = CAN_MODE__NORMAL;

                        HAL_FDCAN_ActivateNotification(&hdl->hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
                        HAL_FDCAN_ActivateNotification(&hdl->hfdcan, FDCAN_IT_RX_FIFO0_MESSAGE_LOST, 0);
                        HAL_FDCAN_ActivateNotification(&hdl->hfdcan, FDCAN_IT_RX_FIFO0_FULL, 0);

                        HAL_FDCAN_ActivateNotification(&hdl->hfdcan, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
                        HAL_FDCAN_ActivateNotification(&hdl->hfdcan, FDCAN_IT_RX_FIFO1_MESSAGE_LOST, 0);
                        HAL_FDCAN_ActivateNotification(&hdl->hfdcan, FDCAN_IT_RX_FIFO1_FULL, 0);

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
                err = HAL_FDCAN_EnterPowerDownMode(&hdl->hfdcan);
                if (!err) {
                        hdl->mode = CAN_MODE__SLEEP;
                }
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
        if (filter->number >= FILTERS_COUNT) {
                return EINVAL;
        }

        int err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
        if (!err) {

                FDCAN_FilterTypeDef filter_config;
                filter_config.IdType = filter->extended_ID ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
                filter_config.FilterIndex = filter->number;
                filter_config.FilterType = FDCAN_FILTER_MASK;
                filter_config.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
                filter_config.FilterID1 = filter->ID;
                filter_config.FilterID2 = filter->mask;
                filter_config.RxBufferIndex = FDCAN_RX_BUFFER0; // only valid if FilterConfig = FDCAN_FILTER_TO_RXBUFFER
                filter_config.IsCalibrationMsg = false;
                err = HAL_FDCAN_ConfigFilter(&hdl->hfdcan, &filter_config);

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
        if (filter_no >= FILTERS_COUNT) {
                return EINVAL;
        }

        int err = sys_mutex_lock(hdl->config_mtx, MTX_TIMEOUT);
        if (!err) {

                FDCAN_FilterTypeDef filter_config;
                filter_config.IdType = FDCAN_STANDARD_ID;
                filter_config.FilterIndex = filter_no;
                filter_config.FilterType = FDCAN_FILTER_MASK;
                filter_config.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
                filter_config.FilterID1 = 0;
                filter_config.FilterID2 = 0;
                filter_config.RxBufferIndex = FDCAN_RX_BUFFER0;
                filter_config.IsCalibrationMsg = false;

                err = HAL_FDCAN_ConfigFilter(&hdl->hfdcan, &filter_config);
                if (!err) {
                        filter_config.IdType = FDCAN_EXTENDED_ID;
                        err = HAL_FDCAN_ConfigFilter(&hdl->hfdcan, &filter_config);
                }

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
 * @param  timeout_ms   timeout in milliseconds
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_msg(CAN_t *hdl, const CAN_msg_t *msg, u32_t timeout_ms)
{
        u32_t DLC = 0;
        if      (msg->data_length == 0)  DLC = FDCAN_DLC_BYTES_0;
        else if (msg->data_length == 1)  DLC = FDCAN_DLC_BYTES_1;
        else if (msg->data_length == 2)  DLC = FDCAN_DLC_BYTES_2;
        else if (msg->data_length == 3)  DLC = FDCAN_DLC_BYTES_3;
        else if (msg->data_length == 4)  DLC = FDCAN_DLC_BYTES_4;
        else if (msg->data_length == 5)  DLC = FDCAN_DLC_BYTES_5;
        else if (msg->data_length == 6)  DLC = FDCAN_DLC_BYTES_6;
        else if (msg->data_length == 7)  DLC = FDCAN_DLC_BYTES_7;
        else if (msg->data_length == 8)  DLC = FDCAN_DLC_BYTES_8;
        else if (msg->data_length == 12) DLC = FDCAN_DLC_BYTES_12;
        else if (msg->data_length == 16) DLC = FDCAN_DLC_BYTES_16;
        else if (msg->data_length == 20) DLC = FDCAN_DLC_BYTES_20;
        else if (msg->data_length == 24) DLC = FDCAN_DLC_BYTES_24;
        else if (msg->data_length == 32) DLC = FDCAN_DLC_BYTES_32;
        else if (msg->data_length == 48) DLC = FDCAN_DLC_BYTES_48;
        else if (msg->data_length == 64) DLC = FDCAN_DLC_BYTES_64;
        else return EILSEQ;

        sys_context_switch_lock();
        hdl->txpend_ctr++;
        sys_context_switch_unlock();

        int   err     = ETIME;
        bool  loop    = true;
        u32_t timeout = max(1, timeout_ms);

        clock_t tref = sys_get_uptime_ms();

        while (loop && !sys_is_time_expired(tref, timeout)) {

                err = ETIME;

                for (int i = 0; loop && (i < TX_BUFFERS); i++) {

                        err = sys_mutex_trylock(hdl->txbuf_mtx[i]);
                        if (!err) {

                                FDCAN_TxHeaderTypeDef tx_header;
                                tx_header.Identifier = msg->ID;
                                tx_header.IdType = msg->extended_ID ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
                                tx_header.TxFrameType = msg->remote_transmission ? FDCAN_REMOTE_FRAME : FDCAN_DATA_FRAME;
                                tx_header.DataLength = DLC;
                                tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
                                tx_header.BitRateSwitch = msg->frame_mode == CAN_FRAME_MODE__FD_WITH_BIT_RATE_SWITCH ? FDCAN_BRS_ON : FDCAN_BRS_OFF;
                                tx_header.FDFormat = msg->frame_mode == CAN_FRAME_MODE__CLASSIC ? FDCAN_CLASSIC_CAN: FDCAN_FD_CAN;
                                tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
                                tx_header.MessageMarker = i;

                                u32_t buffer_index = FDCAN_TX_BUFFER0 << i;

                                err = HAL_FDCAN_AddMessageToTxBuffer(&hdl->hfdcan, &tx_header, msg->data, buffer_index);

                                if (not err) {
                                        sys_queue_reset(hdl->txrdy_q[i]);
                                        HAL_FDCAN_ActivateNotification(&hdl->hfdcan, FDCAN_IT_TX_ABORT_COMPLETE, buffer_index);
                                        HAL_FDCAN_ActivateNotification(&hdl->hfdcan, FDCAN_IT_TX_COMPLETE, buffer_index);
                                        HAL_FDCAN_EnableTxBufferRequest(&hdl->hfdcan, buffer_index);

                                        int erri = EIO;
                                        err = sys_queue_receive(hdl->txrdy_q[i], &erri, timeout);
                                        if (!err) {
                                                err = erri;

                                                if (!err) {
                                                        hdl->tx_frames++;
                                                        hdl->tx_bytes += msg->data_length;
                                                }

                                        } else {
                                                dev_dbg(hdl, "aborting message in buffer %lu", i);
                                                HAL_FDCAN_AbortTxRequest(&hdl->hfdcan, buffer_index);
                                                sys_queue_receive(hdl->txrdy_q[i], &erri, MAX_DELAY_MS);
                                        }

                                }

                                loop = false;

                                sys_mutex_unlock(hdl->txbuf_mtx[i]);
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

        int err = sys_queue_receive(hdl->rxqueue_q, msg, timeout_ms);

        sys_context_switch_lock();
        hdl->rxpend_ctr--;
        sys_context_switch_unlock();

        return err;
}

//==============================================================================
/**
 * @brief  Initializes the FDCAN MSP.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 */
//==============================================================================
void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *hfdcan)
{
        CAN_t *hdl = hfdcan->UserContext;

        __IO uint32_t tmpreg;
        SET_BIT(RCC->APB1HENR, RCC_APB1HENR_FDCANEN);
        tmpreg = READ_BIT(RCC->APB1HENR, RCC_APB1HENR_FDCANEN);
        UNUSED(tmpreg);

        NVIC_ClearPendingIRQ(CAN_CFG[hdl->major].IT0_IRQn);
        NVIC_SetPriority(CAN_CFG[hdl->major].IT0_IRQn, _CPU_IRQ_SAFE_PRIORITY_);
        NVIC_EnableIRQ(CAN_CFG[hdl->major].IT0_IRQn);

        NVIC_ClearPendingIRQ(CAN_CFG[hdl->major].IT1_IRQn);
        NVIC_SetPriority(CAN_CFG[hdl->major].IT1_IRQn, _CPU_IRQ_SAFE_PRIORITY_);
        NVIC_EnableIRQ(CAN_CFG[hdl->major].IT1_IRQn);

        if (hdl->major == _CAN1) {
                NVIC_ClearPendingIRQ(FDCAN_CAL_IRQn);
                NVIC_SetPriority(FDCAN_CAL_IRQn, _CPU_IRQ_SAFE_PRIORITY_);
                NVIC_EnableIRQ(FDCAN_CAL_IRQn);
        }
}

//==============================================================================
/**
 * @brief  DeInitializes the FDCAN MSP.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 */
//==============================================================================
void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef *hfdcan)
{
        CAN_t *hdl = hfdcan->UserContext;

        size_t active_peripherals = 0;

        for (size_t i = 0; i < ARRAY_SIZE(CAN_HDL); i++) {
                active_peripherals += CAN_HDL[i] ? 1 : 0;
        }

        NVIC_ClearPendingIRQ(CAN_CFG[hdl->major].IT0_IRQn);
        NVIC_DisableIRQ(CAN_CFG[hdl->major].IT0_IRQn);

        NVIC_ClearPendingIRQ(CAN_CFG[hdl->major].IT1_IRQn);
        NVIC_DisableIRQ(CAN_CFG[hdl->major].IT1_IRQn);

        if (hdl->major == _CAN1) {
                NVIC_ClearPendingIRQ(FDCAN_CAL_IRQn);
                NVIC_DisableIRQ(FDCAN_CAL_IRQn);
        }

        if (active_peripherals <= 1) {
                CLEAR_BIT(RCC->APB1HENR, RCC_APB1HENR_FDCANEN);

                SET_BIT(RCC->APB1HRSTR, RCC_APB1HRSTR_FDCANRST);
                CLEAR_BIT(RCC->APB1HRSTR, RCC_APB1HRSTR_FDCANRST);
        }
}

//==============================================================================
/**
 * @brief  Clock Calibration callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 * @param  ClkCalibrationITs indicates which Clock Calibration interrupts are signaled.
 *         This parameter can be any combination of @arg FDCAN_Clock_Calibration_Interrupts.
 */
//==============================================================================
void HAL_FDCAN_ClockCalibrationCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ClkCalibrationITs)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hfdcan);
        UNUSED(ClkCalibrationITs);

        /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_ClockCalibrationCallback could be implemented in the user file
        */
}

//==============================================================================
/**
 * @brief  Rx FIFO 0 callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 * @param  RxFifo0ITs indicates which Rx FIFO 0 interrupts are signaled.
 *         This parameter can be any combination of @arg FDCAN_Rx_Fifo0_Interrupts.
 */
//==============================================================================
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
        CAN_t *hdl = hfdcan->UserContext;

        if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) {

                HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

                CAN_msg_t msg;
                FDCAN_RxHeaderTypeDef rx_header;
                size_t items_in_queue = 0;

                while (    (sys_queue_get_number_of_items_from_ISR(hdl->rxqueue_q, &items_in_queue) == 0)
                       and (RX_FIFO_DEPTH - items_in_queue > 0) ) {

                        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, msg.data) == 0) {

                                if (rx_header.FDFormat == FDCAN_CLASSIC_CAN) {
                                        msg.frame_mode = CAN_FRAME_MODE__CLASSIC;
                                } else if (rx_header.BitRateSwitch == FDCAN_BRS_ON) {
                                        msg.frame_mode = CAN_FRAME_MODE__FD_WITH_BIT_RATE_SWITCH;
                                } else {
                                        msg.frame_mode = CAN_FRAME_MODE__FD_WITHOUT_BIT_RATE_SWITCH;
                                }

                                msg.ID = rx_header.Identifier;
                                msg.extended_ID = rx_header.IdType == FDCAN_EXTENDED_ID;
                                msg.remote_transmission = rx_header.RxFrameType == FDCAN_REMOTE_FRAME;

                                switch (rx_header.DataLength) {
                                default: // fall throught
                                case FDCAN_DLC_BYTES_0:  msg.data_length = 0;  break;
                                case FDCAN_DLC_BYTES_1:  msg.data_length = 1;  break;
                                case FDCAN_DLC_BYTES_2:  msg.data_length = 2;  break;
                                case FDCAN_DLC_BYTES_3:  msg.data_length = 3;  break;
                                case FDCAN_DLC_BYTES_4:  msg.data_length = 4;  break;
                                case FDCAN_DLC_BYTES_5:  msg.data_length = 5;  break;
                                case FDCAN_DLC_BYTES_6:  msg.data_length = 6;  break;
                                case FDCAN_DLC_BYTES_7:  msg.data_length = 7;  break;
                                case FDCAN_DLC_BYTES_8:  msg.data_length = 8;  break;
                                case FDCAN_DLC_BYTES_12: msg.data_length = 12; break;
                                case FDCAN_DLC_BYTES_16: msg.data_length = 16; break;
                                case FDCAN_DLC_BYTES_20: msg.data_length = 20; break;
                                case FDCAN_DLC_BYTES_24: msg.data_length = 24; break;
                                case FDCAN_DLC_BYTES_32: msg.data_length = 32; break;
                                case FDCAN_DLC_BYTES_48: msg.data_length = 48; break;
                                case FDCAN_DLC_BYTES_64: msg.data_length = 64; break;
                                }

                                bool irq_yield = false;
                                sys_queue_send_from_ISR(hdl->rxqueue_q, &msg, &irq_yield);
                                hdl->irq_yield |= irq_yield;

                                hdl->rx_frames++;
                                hdl->rx_bytes += msg.data_length;
                        } else {
                                // FDCAN fifo empty
                                break;
                        }
                }

        } else if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_MESSAGE_LOST) {

                hdl->rx_overrun++;
                HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_MESSAGE_LOST, 0);

        } else if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_FULL) {

                hdl->rx_overrun++;
                HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_FULL, 0);
        }
}

//==============================================================================
/**
 * @brief  Rx FIFO 1 callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 * @param  RxFifo1ITs indicates which Rx FIFO 1 interrupts are signaled.
 *         This parameter can be any combination of @arg FDCAN_Rx_Fifo1_Interrupts.
 */
//==============================================================================
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
        UNUSED_ARG2(hfdcan, RxFifo1ITs);

        // NOTE: RXFifo1 is not used
}

//==============================================================================
/**
 * @brief  Rx Buffer New Message callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 */
//==============================================================================
void HAL_FDCAN_RxBufferNewMessageCallback(FDCAN_HandleTypeDef *hfdcan)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hfdcan);

        // NOTE: Rx buffers are not used
}

//==============================================================================
/**
 * @brief  Tx Event callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 * @param  TxEventFifoITs indicates which Tx Event FIFO interrupts are signaled.
 *         This parameter can be any combination of @arg FDCAN_Tx_Event_Fifo_Interrupts.
 */
//==============================================================================
void HAL_FDCAN_TxEventFifoCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t TxEventFifoITs)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hfdcan);
        UNUSED(TxEventFifoITs);

        // NOTE: Tx evet fifo are not used
}

//==============================================================================
/**
 * @brief  Tx FIFO Empty callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 */
//==============================================================================
void HAL_FDCAN_TxFifoEmptyCallback(FDCAN_HandleTypeDef *hfdcan)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hfdcan);

        // NOTE: Tx fifo is not used
}

//==============================================================================
/**
 * @brief  Transmission Complete callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 * @param  BufferIndexes Indexes of the transmitted buffers.
 *         This parameter can be any combination of @arg FDCAN_Tx_location.
 */
//==============================================================================
void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
        CAN_t *hdl = hfdcan->UserContext;

        for (size_t i = 0; i < 32; i++) {

                if (BufferIndexes & (1 << i)) {
                        int err = ESUCC;
                        bool irq_yield = false;
                        sys_queue_send_from_ISR(hdl->txrdy_q[i], &err, &irq_yield);
                        hdl->irq_yield |= irq_yield;
                }
        }
}

//==============================================================================
/**
 * @brief  Transmission Cancellation callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 * @param  BufferIndexes Indexes of the aborted buffers.
 *         This parameter can be any combination of @arg FDCAN_Tx_location.
 * @retval None
 */
//==============================================================================
void HAL_FDCAN_TxBufferAbortCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
        CAN_t *hdl = hfdcan->UserContext;

        for (size_t i = 0; i < 32; i++) {

                if (BufferIndexes & (1 << i)) {
                        int err = ECANCELED;
                        bool irq_yield = false;
                        sys_queue_send_from_ISR(hdl->txrdy_q[i], &err, &irq_yield);
                        hdl->irq_yield |= irq_yield;
                }
        }
}

//==============================================================================
/**
 * @brief  Timestamp Wraparound callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 */
//==============================================================================
void HAL_FDCAN_TimestampWraparoundCallback(FDCAN_HandleTypeDef *hfdcan)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hfdcan);
}

//==============================================================================
/**
 * @brief  Timeout Occurred callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 */
//==============================================================================
void HAL_FDCAN_TimeoutOccurredCallback(FDCAN_HandleTypeDef *hfdcan)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hfdcan);
}

//==============================================================================
/**
 * @brief  High Priority Message callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 */
//==============================================================================
void HAL_FDCAN_HighPriorityMessageCallback(FDCAN_HandleTypeDef *hfdcan)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hfdcan);
}

//==============================================================================
/**
 * @brief  Error callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 */
//==============================================================================
void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hfdcan)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hfdcan);
}

//==============================================================================
/**
 * @brief  Error status callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 * @param  ErrorStatusITs indicates which Error Status interrupts are signaled.
 *         This parameter can be any combination of @arg FDCAN_Error_Status_Interrupts.
 */
//==============================================================================
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hfdcan);
        UNUSED(ErrorStatusITs);
}

//==============================================================================
/**
 * @brief  TT Schedule Synchronization callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 * @param  TTSchedSyncITs indicates which TT Schedule Synchronization interrupts are signaled.
 *         This parameter can be any combination of @arg FDCAN_TTScheduleSynchronization_Interrupts.
 */
//==============================================================================
void HAL_FDCAN_TT_ScheduleSyncCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t TTSchedSyncITs)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hfdcan);
        UNUSED(TTSchedSyncITs);
}

//==============================================================================
/**
 * @brief  TT Time Mark callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 * @param  TTTimeMarkITs indicates which TT Schedule Synchronization interrupts are signaled.
 *         This parameter can be any combination of @arg FDCAN_TTTimeMark_Interrupts.
 */
//==============================================================================
void HAL_FDCAN_TT_TimeMarkCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t TTTimeMarkITs)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hfdcan);
        UNUSED(TTTimeMarkITs);
}

//==============================================================================
/**
 * @brief  TT Stop Watch callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 * @param  SWTime Time Value captured at the Stop Watch Trigger pin (fdcan1_swt) falling/rising
 *         edge (as configured via HAL_FDCAN_TTConfigStopWatch).
 *         This parameter is a number between 0 and 0xFFFF.
 * @param  SWCycleCount Cycle count value captured together with SWTime.
 *         This parameter is a number between 0 and 0x3F.
 */
//==============================================================================
void HAL_FDCAN_TT_StopWatchCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t SWTime, uint32_t SWCycleCount)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hfdcan);
        UNUSED(SWTime);
        UNUSED(SWCycleCount);
}

//==============================================================================
/**
 * @brief  TT Global Time callback.
 *
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 * @param  TTGlobTimeITs indicates which TT Global Time interrupts are signaled.
 *         This parameter can be any combination of @arg FDCAN_TTGlobalTime_Interrupts.
 */
//==============================================================================
void HAL_FDCAN_TT_GlobalTimeCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t TTGlobTimeITs)
{
        /* Prevent unused argument(s) compilation warning */
        UNUSED(hfdcan);
        UNUSED(TTGlobTimeITs);
}

//==============================================================================
/**
 * @brief  Execute CAN interrupt.
 *
 * @param  hdl          CAN handle
 *
 * @return Yield indicator.
 */
//==============================================================================
static void IRQ_handler(CAN_t *hdl)
{
        if (hdl) {
                hdl->irq_yield = false;
                HAL_FDCAN_IRQHandler(&hdl->hfdcan);
                sys_thread_yield_from_ISR(hdl->irq_yield);
        }
}

//==============================================================================
/**
 * @brief FDCAN1 Interrupt Line 0.
 */
//==============================================================================
#if defined(FDCAN1)
void FDCAN1_IT0_IRQHandler(void)
{
        IRQ_handler(CAN_HDL[_CAN1]);
}
#endif

//==============================================================================
/**
 * @brief FDCAN1 Interrupt Line 0.
 */
//==============================================================================
#if defined(FDCAN1)
void FDCAN1_IT1_IRQHandler(void)
{
        IRQ_handler(CAN_HDL[_CAN1]);
}
#endif

//==============================================================================
/**
 * @brief FDCAN2 Interrupt Line 0.
 */
//==============================================================================
#if defined(FDCAN2)
void FDCAN2_IT0_IRQHandler(void)
{
        IRQ_handler(CAN_HDL[_CAN2]);
}
#endif

//==============================================================================
/**
 * @brief FDCAN2 Interrupt Line 0.
 */
//==============================================================================
#if defined(FDCAN2)
void FDCAN2_IT1_IRQHandler(void)
{
        IRQ_handler(CAN_HDL[_CAN2]);
}
#endif

//==============================================================================
/**
 * @brief FDCAN3 Interrupt Line 0.
 */
//==============================================================================
#if defined(FDCAN3)
void FDCAN3_IT0_IRQHandler(void)
{
        IRQ_handler(CAN_HDL[_CAN3]);
}
#endif

//==============================================================================
/**
 * @brief FDCAN3 Interrupt Line 0.
 */
//==============================================================================
#if defined(FDCAN3)
void FDCAN3_IT1_IRQHandler(void)
{
        IRQ_handler(CAN_HDL[_CAN3]);
}
#endif

//==============================================================================
/**
 * @brief  FDCAN1 Clock calibration interrupt.
 */
//==============================================================================
#if defined(FDCAN1)
void FDCAN_CAL_IRQHandler(void)
{
        IRQ_handler(CAN_HDL[_CAN1]);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
