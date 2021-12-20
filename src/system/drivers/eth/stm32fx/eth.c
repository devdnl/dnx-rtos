/*=========================================================================*//**
@file    eth.c

@author  Daniel Zorychta

@brief   This driver support Ethernet interface.

@note    Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "eth_ioctl.h"
#include "stm32f4xx_hal_eth.h"

#if defined(ARCH_stm32f1)
#include "stm32f10x.h"
#elif defined(ARCH_stm32f4)
#include "stm32f4xx.h"
#elif defined(ARCH_stm32f7)
#include "stm32f7xx.h"
#endif

/*==============================================================================
  Local macros
==============================================================================*/
#define DEBUG                   0

#define MUTEX_TIMEOUT           1000
#define PHY_BSR_LINK_STATUS     (1 << 2)

#if defined(ARCH_stm32f1)
#define AHBxENR                  AHBENR
#define RCC_AHBxENR_ETHMACRXEN   RCC_AHBENR_ETHMACRXEN
#define RCC_AHBxENR_ETHMACTXEN   RCC_AHBENR_ETHMACTXEN
#define RCC_AHBxENR_ETHMACEN     RCC_AHBENR_ETHMACEN
#define CPU_CACHE_ALIGN          1
#elif defined(ARCH_stm32f4)
#define AHBxENR                  AHB1ENR
#define RCC_AHBxENR_ETHMACRXEN   RCC_AHB1ENR_ETHMACRXEN
#define RCC_AHBxENR_ETHMACTXEN   RCC_AHB1ENR_ETHMACTXEN
#define RCC_AHBxENR_ETHMACEN     RCC_AHB1ENR_ETHMACEN
#define CPU_CACHE_ALIGN          1
#elif defined(ARCH_stm32f7)
#define AHBxENR                  AHB1ENR
#define RCC_AHBxENR_ETHMACRXEN   RCC_AHB1ENR_ETHMACRXEN
#define RCC_AHBxENR_ETHMACTXEN   RCC_AHB1ENR_ETHMACTXEN
#define RCC_AHBxENR_ETHMACEN     RCC_AHB1ENR_ETHMACEN
#define CPU_CACHE_ALIGN          32
#endif

#if CPU_CACHE_ALIGN != ETH_CPU_CACHE_ALIGN
#error "CPU cache alignments are different!"
#endif

#define DMA_BUFFER_MASK         (CPU_CACHE_ALIGN - 1)
#define DMA_ALIGN_UP(n)         (((n) + DMA_BUFFER_MASK) & ~DMA_BUFFER_MASK)
#define DMA_ALIGN_DOWN(n)       ((n) & ~DMA_BUFFER_MASK)

#define RX_DESC_ARRAY_SIZE      DMA_ALIGN_UP(sizeof(ETH_DMADescTypeDef) * __ETH_RXBUFNB__)
#define TX_DESC_ARRAY_SIZE      DMA_ALIGN_UP(sizeof(ETH_DMADescTypeDef) * __ETH_TXBUFNB__)
#define RX_BUF_ARRAY_SIZE       DMA_ALIGN_UP(ETH_RX_BUF_SIZE_ALIGN * __ETH_RXBUFNB__)
#define TX_BUF_ARRAY_SIZE       DMA_ALIGN_UP(ETH_TX_BUF_SIZE_ALIGN * __ETH_TXBUFNB__)

#define DESCS_BUFFS_SIZE        (RX_DESC_ARRAY_SIZE + TX_DESC_ARRAY_SIZE + RX_BUF_ARRAY_SIZE + TX_BUF_ARRAY_SIZE + CPU_CACHE_ALIGN)

/*==============================================================================
  Local object types
==============================================================================*/
struct eth {
        ksem_t             *rx_semaphore;
        kmtx_t             *mutex;
        ETH_DMADescTypeDef *DMA_rx_desc;
        ETH_DMADescTypeDef *DMA_tx_desc;
        uint8_t            *rx_buff;
        uint8_t            *tx_buff;
        ETH_HandleTypeDef   eth;
        uint8_t             MAC_addr[6];
        bool                irq_yield;
        bool                configured;
        bool                run;
        u32_t               rx_timeout_ms;
        u32_t               tx_timeout_ms;
        u8_t                descs_and_buffs[DESCS_BUFFS_SIZE];
        struct {
                u32_t rx_packets;
                u32_t tx_packets;
                u64_t rx_bytes;
                u64_t tx_bytes;
                u32_t rx_dropped_frames;
        } stats;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int eth_configure(struct eth *hdl, const ETH_config_t *conf);
static int eth_start(struct eth *hdl);
static int eth_stop(struct eth *hdl);
static int eth_get_info(struct eth *hdl, ETH_status_t *status);
static int eth_flush(struct eth *hdl);
static int packet_receive(struct eth *hdl, ETH_packet_t *pkt);
static int packet_send(struct eth *hdl, const ETH_packet_t *pkt);
static void update_dropped_rx_frames(struct eth *hdl);

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(ETH);

static struct eth *eth;

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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(ETH, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        if (major != 0 || minor != 0) {
                return ENODEV;
        }

        int err = sys_zalloc(sizeof(struct eth), device_handle);
        if (!err) {
                struct eth *hdl = *device_handle;

                hdl->rx_timeout_ms = _MAX_DELAY_MS;
                hdl->tx_timeout_ms = _MAX_DELAY_MS;

                err = sys_semaphore_create(__ETH_RXBUFNB__, 0, &hdl->rx_semaphore);
                if (!err) {

                        err = sys_mutex_create(KMTX_TYPE_NORMAL, &hdl->mutex);
                        if (!err) {

                                /*
                                 * Each DMA description is aligned to DMA cache line.
                                 */
                                hdl->DMA_rx_desc = (void*)DMA_ALIGN_DOWN((u32_t)hdl->descs_and_buffs + CPU_CACHE_ALIGN - 1);
                                hdl->DMA_tx_desc = (void*)DMA_ALIGN_DOWN((u32_t)hdl->descs_and_buffs + RX_DESC_ARRAY_SIZE + CPU_CACHE_ALIGN - 1);

                                /*
                                 * Each RX/TX buffer (even for single packet) is aligned to
                                 * DMA cache line.
                                 */
                                hdl->rx_buff = (void*)DMA_ALIGN_DOWN((u32_t)hdl->DMA_tx_desc + TX_DESC_ARRAY_SIZE + CPU_CACHE_ALIGN - 1);
                                hdl->tx_buff = (void*)DMA_ALIGN_DOWN((u32_t)hdl->rx_buff + RX_BUF_ARRAY_SIZE + CPU_CACHE_ALIGN - 1);

                                #if DEBUG
                                printk("padding             : %u", (u32_t)hdl->DMA_rx_desc - (u32_t)hdl->descs_and_buffs);
                                printk("hdl->descs_and_buffs: %p %u", hdl->descs_and_buffs, sizeof(hdl->descs_and_buffs));
                                printk("hdl->DMA_rx_desc    : %p %u", hdl->DMA_rx_desc, RX_DESC_ARRAY_SIZE);
                                printk("hdl->DMA_tx_desc    : %p %u", hdl->DMA_tx_desc, TX_DESC_ARRAY_SIZE);
                                printk("hdl->rx_buff        : %p %u", hdl->rx_buff, RX_BUF_ARRAY_SIZE);
                                printk("hdl->tx_buff        : %p %u", hdl->tx_buff, TX_BUF_ARRAY_SIZE);
                                #endif

                                if (config) {
                                        err = eth_configure(hdl, config);
                                }
                        }
                }

                if (!err) {
                        eth = hdl;

                } else {
                        if (hdl->rx_semaphore) {
                                sys_semaphore_destroy(hdl->rx_semaphore);
                        }

                        if (hdl->mutex) {
                                sys_mutex_destroy(hdl->mutex);
                        }

                        sys_free(device_handle);
                }
        }

        return err;
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
API_MOD_RELEASE(ETH, void *device_handle)
{
        struct eth *hdl = device_handle;

        int err;

        if (not hdl->run) {
                kmtx_t *mtx = hdl->mutex;

                err = sys_mutex_lock(mtx, _MAX_DELAY_MS);
                if (!err) {

                        hdl->mutex = NULL;
                        sys_sleep_ms(MUTEX_TIMEOUT + MUTEX_TIMEOUT/2);

                        HAL_ETH_DeInit(&hdl->eth);
                        NVIC_DisableIRQ(ETH_IRQn);

                        CLEAR_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACRXEN
                                              | RCC_AHBxENR_ETHMACTXEN
                                              | RCC_AHBxENR_ETHMACEN);

                        sys_mutex_destroy(mtx);
                        sys_semaphore_destroy(hdl->rx_semaphore);
                        sys_free(&device_handle);
                        eth = NULL;
                }
        } else {
                err = EBUSY;
        }

        return err;
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
API_MOD_OPEN(ETH, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(ETH, void *device_handle, bool force)
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
 * @param[out]          *wrctr                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_WRITE(ETH,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrctr,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG2(fpos, fattr);

        struct eth *hdl = device_handle;

        int err  = EPERM;
        size_t n = 0;

        while (count > 0) {
                size_t len = min(count, ETH_MAX_PACKET_SIZE);

                ETH_packet_t pkt;
                pkt.payload = const_cast(src);
                pkt.length  = len;
                err = packet_send(hdl, &pkt);
                if (!err) {
                        src   += len;
                        count -= len;
                        n     += len;
                } else {
                        break;
                }
        }

        *wrctr = n;

        return err;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdctr                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_READ(ETH,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdctr,
             struct vfs_fattr fattr)
{
        UNUSED_ARG2(fpos, fattr);

        struct eth *hdl = device_handle;

        ETH_packet_t pkt;
        pkt.payload = dst;
        pkt.length  = min(count, ETH_MAX_PACKET_SIZE);
        int err = packet_receive(hdl, &pkt);
        if (!err) {
                *rdctr = pkt.length;
        }

        return err;
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
API_MOD_IOCTL(ETH, void *device_handle, int request, void *arg)
{
        struct eth *hdl = device_handle;

        int err = EINVAL;

        switch (request) {
        case IOCTL_ETH__CONFIGURE:
                if (arg) {
                        err = eth_configure(hdl, arg);
                }
                break;

        case IOCTL_ETH__SEND_PACKET:
                if (arg) {
                        err = packet_send(hdl, arg);
                }
                break;

        case IOCTL_ETH__RECEIVE_PACKET:
                if (arg) {
                        err = packet_receive(hdl, arg);
                }
                break;

        case IOCTL_ETH__SET_RX_TIMEOUT:
                if (arg) {
                        hdl->rx_timeout_ms = *cast(u32_t*, arg);
                }
                break;

        case IOCTL_ETH__SET_TX_TIMEOUT:
                if (arg) {
                        hdl->tx_timeout_ms = *cast(u32_t*, arg);
                }
                break;

        case IOCTL_ETH__START:
                err = eth_start(hdl);
                break;

        case IOCTL_ETH__STOP:
                err = eth_stop(hdl);
                break;

        case IOCTL_ETH__GET_STATUS:
                if (arg) {
                        err = eth_get_info(hdl, arg);
                }
                break;

        default:
                return EBADRQC;
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
API_MOD_FLUSH(ETH, void *device_handle)
{
        struct eth *hdl = device_handle;
        int err = eth_flush(hdl);
        return err;
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
API_MOD_STAT(ETH, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_size  = 0;
        device_stat->st_major = 0;
        device_stat->st_minor = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Configure Ethernet interface.
 *
 * @param  hdl          device handle
 * @param  conf         configuration
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int eth_configure(struct eth *hdl, const ETH_config_t *conf)
{
        int err = sys_mutex_lock(hdl->mutex, MUTEX_TIMEOUT);
        if (!err) {
                if (not hdl->run) {
                        if (hdl->configured) {
                                HAL_ETH_DeInit(&hdl->eth);
                        }

                        memcpy(hdl->MAC_addr, conf->MAC, sizeof(hdl->MAC_addr));

                        hdl->eth.Instance = ETH;
                        hdl->eth.Init.MACAddr = hdl->MAC_addr;
                        hdl->eth.Init.AutoNegotiation = __ETH_PHY_AUTONEGOTIATION__;
                        hdl->eth.Init.Speed = __ETH_SPEED__;
                        hdl->eth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
                        hdl->eth.Init.RxMode = ETH_RXINTERRUPT_MODE;
                        hdl->eth.Init.ChecksumMode = __ETH_CHECKSUM_BY_HARDWARE__;
                        hdl->eth.Init.PhyAddress = __ETH_PHY_ADDRESS__;

                        err = HAL_ETH_Init(&hdl->eth);
                        if (!err) {
                                /* Initialize Tx Descriptors list: Chain Mode */
                                HAL_ETH_DMATxDescListInit(&hdl->eth, hdl->DMA_tx_desc,
                                                          hdl->tx_buff, __ETH_TXBUFNB__);

                                /* Initialize Rx Descriptors list: Chain Mode  */
                                HAL_ETH_DMARxDescListInit(&hdl->eth, hdl->DMA_rx_desc,
                                                          hdl->rx_buff, __ETH_RXBUFNB__);
                                eth->configured = true;
                        }
                }

                sys_mutex_unlock(hdl->mutex);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Start Ethernet interface.
 *
 * @param  hdl          device handle
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int eth_start(struct eth *hdl)
{
        int err = sys_mutex_lock(hdl->mutex, MUTEX_TIMEOUT);
        if (!err) {
                if (not hdl->run and hdl->configured) {
                        err = HAL_ETH_Start(&hdl->eth);
                        if (!err) {
                                hdl->run = true;
                        }
                } else {
                        err = EPERM;
                }

                sys_mutex_unlock(hdl->mutex);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Stop Ethernet interface.
 *
 * @param  hdl          device handle
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int eth_stop(struct eth *hdl)
{
        int err = sys_mutex_lock(hdl->mutex, MUTEX_TIMEOUT);
        if (!err) {
                if (hdl->run) {
                        err = HAL_ETH_Stop(&hdl->eth);
                        if (!err) {
                                hdl->run = false;
                        }
                } else {
                        err = EPERM;
                }

                sys_mutex_unlock(hdl->mutex);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Get Ethernet interface info data.
 *
 * @param  hdl          device handle
 * @param  status       status data
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int eth_get_info(struct eth *hdl, ETH_status_t *status)
{
        int err = sys_mutex_lock(hdl->mutex, MUTEX_TIMEOUT);
        if (!err) {
                u32_t reg;
                if (HAL_ETH_ReadPHYRegister(&hdl->eth, PHY_BSR, &reg) == HAL_OK) {

                        if (reg & PHY_BSR_LINK_STATUS) {
                                status->link_status = ETH_LINK_STATUS__CONNECTED;
                        } else {
                                status->link_status = ETH_LINK_STATUS__DISCONNECTED;
                        }
                } else {
                        status->link_status = ETH_LINK_STATUS__PHY_ERROR;
                }

                if (HAL_ETH_ReadPHYRegister(&hdl->eth, __ETH_PHY_SR__, &reg) == HAL_OK) {

                        if (reg & __ETH_PHY_SPEED_STATUS_BM__) {
                                status->speed = ETH_SPEED__10Mbps;
                        } else {
                                status->speed = ETH_SPEED__100Mbps;
                        }

                        if (reg & __ETH_PHY_DUPLEX_STATUS_BM__) {
                                status->duplex = ETH_DUPLEX__FULL;
                        } else {
                                status->duplex = ETH_DUPLEX__HALF;
                        }
                } else {
                        status->link_status = ETH_LINK_STATUS__PHY_ERROR;
                }

                memcpy(status->MAC, hdl->MAC_addr, sizeof(status->MAC));

                status->configured = hdl->configured;
                status->rx_bytes   = hdl->stats.rx_bytes;
                status->tx_bytes   = hdl->stats.tx_bytes;
                status->rx_packets = hdl->stats.rx_packets;
                status->tx_packets = hdl->stats.tx_packets;

                switch (HAL_ETH_GetState(&hdl->eth)) {
                case HAL_ETH_STATE_RESET:
                        status->state = ETH_STATE__RESET;
                        break;
                case HAL_ETH_STATE_READY:
                case HAL_ETH_STATE_BUSY:
                case HAL_ETH_STATE_BUSY_TX:
                case HAL_ETH_STATE_BUSY_RX:
                case HAL_ETH_STATE_BUSY_TX_RX:
                case HAL_ETH_STATE_BUSY_WR:
                case HAL_ETH_STATE_BUSY_RD:
                        status->state = ETH_STATE__READY;
                        break;
                case HAL_ETH_STATE_TIMEOUT:
                case HAL_ETH_STATE_ERROR:
                        status->state = ETH_STATE__ERROR;
                        break;
                }

                update_dropped_rx_frames(hdl);
                status->rx_dropped_frames = hdl->stats.rx_dropped_frames;

                sys_mutex_unlock(hdl->mutex);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Flush Ethernet interface Tx buffers.
 *
 * @param  hdl          device handle
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int eth_flush(struct eth *hdl)
{
        int err = sys_mutex_lock(hdl->mutex, MUTEX_TIMEOUT);
        if (!err) {
                int empty_bufs;
                do {
                        empty_bufs = 0;

                        for (int i = 0; i < __ETH_TXBUFNB__; i++) {
                                if (not (hdl->DMA_tx_desc[i].Status & ETH_DMATXDESC_OWN)) {
                                        empty_bufs++;
                                }
                        }

                        sys_sleep_ms(10);

                } while (empty_bufs < __ETH_TXBUFNB__);

                sys_mutex_unlock(hdl->mutex);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Send packet.
 *
 * @param  hdl          device handle
 * @param  pkt          packet
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int packet_send(struct eth *hdl, const ETH_packet_t *pkt)
{
        int err = hdl->run ? ESUCC : EPERM;

        u64_t tref = sys_get_uptime_ms();

        while (true) {
                err = sys_mutex_lock(hdl->mutex, MUTEX_TIMEOUT);
                if (!err) {

                        __IO ETH_DMADescTypeDef *DmaTxDesc = hdl->eth.TxDesc;

                        _cpuctl_invalidate_dcache_by_addr((uint32_t*)DmaTxDesc, sizeof(ETH_DMADescTypeDef));

                        if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t) RESET) {
                                sys_mutex_unlock(hdl->mutex);

                                if (sys_get_uptime_ms() - tref >= hdl->tx_timeout_ms) {
                                        err = ETIME;
                                        break;
                                }

                                sys_sleep_ms(10);
                                continue;
                        }

                        /* Copy the remaining bytes */
                        uint8_t *buffer = (uint8_t*) (DmaTxDesc->Buffer1Addr);
                        memcpy(buffer, pkt->payload, pkt->length);

                        _cpuctl_clean_dcache_by_addr((uint32_t*)buffer, pkt->length);

                        /* Prepare transmit descriptors to give to DMA */
                        err = HAL_ETH_TransmitFrame(&hdl->eth, pkt->length);

                        /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
                        if ((hdl->eth.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t) RESET) {
                                /* Clear TUS ETHERNET DMA flag */
                                hdl->eth.Instance->DMASR = ETH_DMASR_TUS;

                                /* Resume DMA transmission*/
                                hdl->eth.Instance->DMATPDR = 0;
                        }

                        /* update statistics */
                        update_dropped_rx_frames(hdl);
                        hdl->stats.tx_packets++;
                        hdl->stats.tx_bytes += pkt->length;

                        sys_mutex_unlock(hdl->mutex);
                }

                break;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Receive packet.
 *
 * @param  hdl          device handle
 * @param  pkt          packet data
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int packet_receive(struct eth *hdl, ETH_packet_t *pkt)
{
        int err = hdl->run ? ESUCC : EPERM;

        u64_t tref = sys_get_uptime_ms();

        while (!err) {
                sys_thread_yield();

                err = sys_mutex_lock(hdl->mutex, MUTEX_TIMEOUT);
                if (!err) {
                        /* get received frame */
                        if (HAL_ETH_GetReceivedFrame_IT(&hdl->eth) != HAL_OK) {
                                sys_mutex_unlock(hdl->mutex);

                                if (sys_get_uptime_ms() - tref >= hdl->rx_timeout_ms) {
                                        err = ETIME;
                                        break;
                                }

                                sys_semaphore_wait(hdl->rx_semaphore, 100);
                                continue;
                        }

                        /* Obtain the size of the packet and put it into the "len" variable. */
                        uint16_t len = hdl->eth.RxFrameInfos.length;
                        uint8_t *buffer = (uint8_t*)hdl->eth.RxFrameInfos.buffer;

                        if (len > 0) {
                                pkt->length = len;
                                _cpuctl_invalidate_dcache_by_addr((uint32_t*)buffer, len);
                                memcpy(pkt->payload, buffer, min(pkt->length, len));
                        }

                        /* Release descriptors to DMA */
                        __IO ETH_DMADescTypeDef *dmarxdesc = hdl->eth.RxFrameInfos.FSRxDesc;

                        /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
                        for (u32_t i = 0; i < hdl->eth.RxFrameInfos.SegCount; i++) {
                                dmarxdesc->Status |= ETH_DMARXDESC_OWN;
                                _cpuctl_clean_dcache_by_addr((uint32_t*)dmarxdesc, sizeof(ETH_DMADescTypeDef));
                                dmarxdesc = (ETH_DMADescTypeDef*)(dmarxdesc->Buffer2NextDescAddr);
                        }

                        /* Clear Segment_Count */
                        hdl->eth.RxFrameInfos.SegCount = 0;

                        /* When Rx Buffer unavailable flag is set: clear it and resume reception */
                        if ((hdl->eth.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t) RESET) {
                                /* Clear RBUS ETHERNET DMA flag */
                                hdl->eth.Instance->DMASR = ETH_DMASR_RBUS;
                                /* Resume DMA reception */
                                hdl->eth.Instance->DMARPDR = 0;
                        }

                        /* update statistics */
                        update_dropped_rx_frames(hdl);
                        hdl->stats.rx_packets++;
                        hdl->stats.rx_bytes += len;

                        sys_mutex_unlock(hdl->mutex);
                }

                break;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Low level Ethernet initialization (library callback).
 *
 * @param  heth         hal ethernet handler
 */
//==============================================================================
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
        UNUSED_ARG1(heth);

        /* Enable the Ethernet global Interrupt */
        NVIC_SetPriority(ETH_IRQn, _CPU_IRQ_SAFE_PRIORITY_);
        NVIC_EnableIRQ(ETH_IRQn);

        SET_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACEN);
        volatile u32_t tmp = READ_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACEN);

        SET_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACTXEN);
        tmp = READ_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACTXEN);

        SET_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACRXEN);
        tmp = READ_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACRXEN);

        UNUSED_ARG1(tmp);
}

//==============================================================================
/**
 * @brief  Low level Ethernet deinitialization (library callback).
 *
 * @param  heth         hal ethernet handler
 */
//==============================================================================
void HAL_ETH_MspDeInit(ETH_HandleTypeDef *heth)
{
        UNUSED_ARG1(heth);

        NVIC_DisableIRQ(ETH_IRQn);

        CLEAR_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACEN);
        volatile u32_t tmp = READ_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACEN);

        CLEAR_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACTXEN);
        tmp = READ_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACTXEN);

        CLEAR_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACRXEN);
        tmp = READ_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACRXEN);

        UNUSED_ARG1(tmp);
}

//==============================================================================
/**
 * @brief  Packet receive complete callback (library callback).
 *
 * @param  heth         hal ethernet handler
 */
//==============================================================================
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
        UNUSED_ARG1(heth);
        sys_semaphore_signal_from_ISR(eth->rx_semaphore, &eth->irq_yield);
}

//==============================================================================
/**
 * @brief  Read number of dropped frames.
 *
 * @param  hdl          device handle
 */
//==============================================================================
static void update_dropped_rx_frames(struct eth *hdl)
{
        u32_t DMAMFBOCR = ETH->DMAMFBOCR;
        u32_t MFC = (DMAMFBOCR & ETH_DMAMFBOCR_MFC) >> ETH_DMAMFBOCR_MFC_Pos;
        hdl->stats.rx_dropped_frames += MFC;
}

//==============================================================================
/**
 * @brief  Ethernet interrupt
 * @param  None
 * @return None
 */
//==============================================================================
void ETH_IRQHandler(void)
{
        HAL_ETH_IRQHandler(&eth->eth);
        sys_thread_yield_from_ISR(eth->irq_yield);
}

/*==============================================================================
  End of file
==============================================================================*/
