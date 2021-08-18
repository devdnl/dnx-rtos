/*==============================================================================
File    usbh.c

Author  Daniel Zorychta

Brief   USB Host driver

        Copyright (C) 2021  <daniel.zorychta@gmail.com>

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
#include <stdalign.h>
#include "drivers/driver.h"
#include "drivers/class/storage/mbr.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"
#include "stm32f4/dma_ddi.h"
#include "../usbh_ioctl.h"
#include "gpio/gpio_ddi.h"

#include "usbh_core.h"
#include "usbh_msc.h"
#include "stm32f4xx_hal_hcd.h"
#include "usbh_msc_scsi.h"

#include "usbh_conf.h"
#include "usbh_core.h"
#include "stm32f4xx.h"

/*==============================================================================
  Local macros
==============================================================================*/
#if __USBH_DEBUG_MSG__
#define DEBUG(...)      printk("USBH: "__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define RECOVERY_TIMEOUT        5000
#define SECTOR_SIZE             512

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        u8_t major;
        u8_t minor;
        mutex_t *mtx;
        USBH_HandleTypeDef hUSBHost;
        HCD_HandleTypeDef hhcd;
        bool class_active;
        u32_t irq_ctr;
        alignas(_HEAP_ALIGN_) u8_t buffer[8 * SECTOR_SIZE];
} USBH_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void free_resources(void);
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);
static int recovery_usb_connection(USBH_t *hdl);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(USBH);
static USBH_t *usbh;
bool usbh_irq;

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
API_MOD_INIT(USBH, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        int err = ENODEV;

        if ((major != 0) || (minor != 0)) {
                return err;
        }

        err = sys_zalloc(sizeof(*usbh), cast(void*, &usbh));
        if (!err) {
                *device_handle = usbh;
                USBH_t *hdl = usbh;

                SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_OTGHSEN | RCC_AHB1ENR_OTGHSULPIEN);

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->mtx);
                if (err) {
                        goto finish;
                }

                err = USBH_Init(&hdl->hUSBHost, USBH_UserProcess, 0);
                if (err) {
                        goto finish;
                }

                err = USBH_RegisterClass(&hdl->hUSBHost, USBH_MSC_CLASS);
                if (err) {
                        goto finish;
                }

                err = USBH_Start(&hdl->hUSBHost);
                if (err) {
                        goto finish;
                }

                finish:
                if (err) {
                        free_resources();
                        return err;
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
API_MOD_RELEASE(USBH, void *device_handle)
{
        free_resources();

        sys_free(&device_handle);

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
API_MOD_OPEN(USBH, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(USBH, void *device_handle, bool force)
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
API_MOD_WRITE(USBH,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        USBH_t *hdl = device_handle;

        if ((count & 0x1FF) || (*fpos & 0x1FF)) {
                return EINVAL;
        }

        int err = sys_mutex_lock(hdl->mtx, MAX_DELAY_MS);
        if (!err) {

                if (hdl->class_active) {

                        bool unaligned = hdl->hhcd.Init.dma_enable
                                         && (  ((uintptr_t)src % _HEAP_ALIGN_)
                                            || !sys_is_mem_dma_capable(src) );

                        if (unaligned) {
                                DEBUG("write from unaligned source pointer");
                        }

                        *wrcnt = 0;

                        while (!err && count) {

                                size_t len = unaligned ? min(count, sizeof(hdl->buffer)) : count;

                                if (unaligned) {
                                        if (_DMA_DDI_memcpy(hdl->buffer, src, len) != 0) {
                                                DEBUG("DMA M2M transfer fail!");
                                                memcpy(hdl->buffer, src, len);
                                        }
                                }

                                u8_t *buf = unaligned ? hdl->buffer : const_cast(src);
                                uint32_t sector_count = len / SECTOR_SIZE;
                                uint32_t sector_addr  = (*fpos / SECTOR_SIZE);

                                for (int try = 0; try < 3; try ++) {
                                        err = USBH_MSC_Write(&hdl->hUSBHost, 0, sector_addr, buf, sector_count);
                                        if (!err) {
                                                src    += len;
                                                *fpos  += len;
                                                *wrcnt += len;
                                                count  -= len;
                                                break;

                                        } else {
                                                MSC_LUNTypeDef info;
                                                USBH_MSC_GetLUNInfo(&hdl->hUSBHost, 0, &info);

                                                switch (info.sense.asc) {
                                                case SCSI_ASC_WRITE_PROTECTED:
                                                        printk("USBH: USB Disk is Write protected!");
                                                        err = EROFS;
                                                        try = UINT8_MAX;
                                                        break;

                                                case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
                                                case SCSI_ASC_MEDIUM_NOT_PRESENT:
                                                case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
                                                        printk("USBH: WR USB Disk is not ready! (%u)", info.sense.asc);
                                                        err = recovery_usb_connection(hdl);
                                                        if (err) {
                                                                try = UINT8_MAX;
                                                        }
                                                        break;

                                                default:
                                                        err = EIO;
                                                        try = UINT8_MAX;
                                                        break;
                                                }
                                        }
                                }
                        }
                } else {
                        err = ENOMEDIUM;
                }

                sys_mutex_unlock(hdl->mtx);
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
API_MOD_READ(USBH,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        USBH_t *hdl = device_handle;

        if ((count & 0x1FF) || (*fpos & 0x1FF)) {
                return EINVAL;
        }

        int err = sys_mutex_lock(hdl->mtx, MAX_DELAY_MS);
        if (!err) {

                if (hdl->class_active) {

                        bool unaligned = hdl->hhcd.Init.dma_enable
                                         && (  ((uintptr_t)dst % _HEAP_ALIGN_)
                                            || !sys_is_mem_dma_capable(dst) );

                        if (unaligned) {
                                DEBUG("read to unaligned destination pointer");
                        }

                        *rdcnt = 0;

                        while (!err && count) {

                                size_t len = unaligned ? min(count, sizeof(hdl->buffer)) : count;

                                u8_t *buf = unaligned ? hdl->buffer : dst;
                                uint32_t sector_count = len / SECTOR_SIZE;
                                uint32_t sector_addr  = (*fpos / SECTOR_SIZE);

                                for (int try = 0; try < 3; try ++) {
                                        err = USBH_MSC_Read(&hdl->hUSBHost, 0, sector_addr, buf, sector_count);
                                        if (!err) {
                                                if (unaligned) {
                                                        if (_DMA_DDI_memcpy(dst, hdl->buffer, len) != 0) {
                                                                DEBUG("DMA M2M transfer fail!");
                                                                memcpy(dst, hdl->buffer, len);
                                                        }
                                                }

                                                dst    += len;
                                                *fpos  += len;
                                                *rdcnt += len;
                                                count  -= len;
                                                break;

                                        } else {
                                                MSC_LUNTypeDef info;
                                                USBH_MSC_GetLUNInfo(&hdl->hUSBHost, 0, &info);

                                                switch (info.sense.asc) {
                                                case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
                                                case SCSI_ASC_MEDIUM_NOT_PRESENT:
                                                case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
                                                        printk("USBH: RD USB Disk is not ready!");
                                                        err = recovery_usb_connection(hdl);
                                                        if (err) {
                                                                try = UINT8_MAX;
                                                        }
                                                        break;

                                                default:
                                                        err = EIO;
                                                        try = UINT8_MAX;
                                                        break;
                                                }
                                        }
                                }
                        }
                } else {
                        err = ENOMEDIUM;
                }

                sys_mutex_unlock(hdl->mtx);
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
API_MOD_IOCTL(USBH, void *device_handle, int request, void *arg)
{
        USBH_t *hdl = device_handle;

        int err = EINVAL;

        switch (request) {
        case IOCTL_USBH__GET_MANUFACTURER_STRING:
                if (arg) {
                        usbh_buf_t *buf = arg;
                        if (buf->ptr && buf->len) {
                                strlcpy(buf->ptr, hdl->hUSBHost.manufacturer_name, buf->len);
                                err = 0;
                        }
                }
                break;

        case IOCTL_USBH__GET_PRODUCT_STRING:
                if (arg) {
                        usbh_buf_t *buf = arg;
                        if (buf->ptr && buf->len) {
                                strlcpy(buf->ptr, hdl->hUSBHost.product_name, buf->len);
                                err = 0;
                        }
                }
                break;

        case IOCTL_USBH__GET_SERIAL_STRING:
                if (arg) {
                        usbh_buf_t *buf = arg;
                        if (buf->ptr && buf->len) {
                                strlcpy(buf->ptr, hdl->hUSBHost.serial_number, buf->len);
                                err = 0;
                        }
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
API_MOD_FLUSH(USBH, void *device_handle)
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
API_MOD_STAT(USBH, void *device_handle, struct vfs_dev_stat *device_stat)
{
        USBH_t *hdl = device_handle;

        if (hdl->class_active) {
                MSC_LUNTypeDef info;
                USBH_MSC_GetLUNInfo(&hdl->hUSBHost, 0, &info);
                device_stat->st_size = cast(u64_t, info.capacity.block_nbr)
                                     * info.capacity.block_size;
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function free all driver resources.
 */
//==============================================================================
static void free_resources(void)
{
        if (usbh) {
                USBH_DeInit(&usbh->hUSBHost);

                if (usbh->mtx) {
                        sys_mutex_destroy(usbh->mtx);
                }

                sys_free(cast(void*, &usbh));
        }
}

//==============================================================================
/**
 * @brief  Recovery USB device connection (by reenumeration process).
 *
 * @param  hdl          device handle
 *
 * @return One of errno value.
 */
//==============================================================================
static int recovery_usb_connection(USBH_t *hdl)
{
        hdl->class_active = false;

        int err = USBH_ReEnumerate(&hdl->hUSBHost); // start re-enumeration
        if (!err) {
                u64_t tref = sys_get_uptime_ms();

                while (not hdl->class_active) {
                        sys_sleep_ms(100);

                        if (sys_time_is_expired(tref, RECOVERY_TIMEOUT)) {
                                err = EIO;
                                break;
                        }
                }

                if (!err) {
                        printk("USBH: connection recovered");
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  USB host library user process.
 *
 * @param  phost        USB host handle
 * @param  id           operation id
 */
//==============================================================================
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
        UNUSED_ARG1(phost);

        switch (id) {
        case HOST_USER_SELECT_CONFIGURATION:
                usbh->class_active = false;
                break;

        case HOST_USER_DISCONNECTION:
                usbh->class_active = false;
                memset(usbh->hUSBHost.manufacturer_name, 0, sizeof(usbh->hUSBHost.manufacturer_name));
                memset(usbh->hUSBHost.product_name, 0, sizeof(usbh->hUSBHost.product_name));
                memset(usbh->hUSBHost.serial_number, 0, sizeof(usbh->hUSBHost.serial_number));
                break;

        case HOST_USER_CONNECTION:
                usbh->class_active = false;
                break;

        case HOST_USER_CLASS_ACTIVE:
                usbh->class_active = true;
                break;

        default:
                break;
        }
}

/*******************************************************************************
                       HCD BSP Routines
*******************************************************************************/
//==============================================================================
/**
 * @brief  Initializes the HCD MSP.
 * @param  hhcd: HCD handle
 * @retval None
 */
//==============================================================================
void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd)
{
        if (hhcd->Instance == USB_OTG_FS) {
                /* Enable USB FS Clocks */
                SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_OTGFSEN);

                /* Set USBFS Interrupt to the lowest priority */
                NVIC_SetPriority(OTG_FS_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                /* Enable USBFS Interrupt */
                NVIC_EnableIRQ(OTG_FS_IRQn);

        } else if (hhcd->Instance == USB_OTG_HS) {
                /* Enable ULPI clock */
                SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_OTGHSULPIEN);

                /* Enable USB HS Clocks */
                SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_OTGHSEN);

                /* Set USBHS Interrupt to the lowest priority */
                NVIC_SetPriority(OTG_HS_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                /* Enable USBHS Interrupt */
                NVIC_EnableIRQ(OTG_HS_IRQn);
        }
}

//==============================================================================
/**
 * @brief  DeInitializes the HCD MSP.
 * @param  hhcd: HCD handle
 * @retval None
 */
//==============================================================================
void HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd)
{
        if (hhcd->Instance == USB_OTG_FS) {
                /* Disable USB FS Clocks */
                CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_OTGFSEN);

                /* Disable USBFS Interrupt */
                NVIC_DisableIRQ(OTG_FS_IRQn);

        } else if (hhcd->Instance == USB_OTG_HS) {
                /* Enable USB HS Clocks */
                CLEAR_BIT(RCC->AHB1ENR, RCC_AHB1ENR_OTGHSEN);

                /* Enable ULPI clock */
                CLEAR_BIT(RCC->AHB1ENR, RCC_AHB1ENR_OTGHSULPIEN);

                /* Disable USBHS Interrupt */
                NVIC_DisableIRQ(OTG_HS_IRQn);
        }
}

/*******************************************************************************
                       LL Driver Callbacks (HCD -> USB Host Library)
*******************************************************************************/
//==============================================================================
/**
 * @brief  SOF callback.
 * @param  hhcd: HCD handle
 * @retval None
 */
//==============================================================================
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd)
{
        USBH_LL_IncTimer(hhcd->pData);
}

//==============================================================================
/**
 * @brief  Connect callback.
 * @param  hhcd: HCD handle
 * @retval None
 */
//==============================================================================
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd)
{
        USBH_LL_Connect(hhcd->pData);
}

//==============================================================================
/**
 * @brief  Disconnect callback.
 * @param  hhcd: HCD handle
 * @retval None
 */
//==============================================================================
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd)
{
        USBH_LL_Disconnect(hhcd->pData);
}

//==============================================================================
/**
 * @brief  Port Port Enabled callback.
 * @param  hhcd: HCD handle
 * @retval None
 */
//==============================================================================
void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef *hhcd)
{
        USBH_LL_PortEnabled(hhcd->pData);
}

//==============================================================================
/**
 * @brief  Port Port Disabled callback.
 * @param  hhcd: HCD handle
 * @retval None
 */
//==============================================================================
void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef *hhcd)
{
        USBH_LL_PortDisabled(hhcd->pData);
}

//==============================================================================
/**
 * @brief  Notify URB state change callback.
 * @param  hhcd: HCD handle
 * @param  chnum: Channel number
 * @param  urb_state: URB State
 * @retval None
 */
//==============================================================================
void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{
        UNUSED_ARG2(chnum, urb_state);
#if (USBH_USE_OS == 1)
        USBH_LL_NotifyURBChange(hhcd->pData);
#endif
}

/*******************************************************************************
                       LL Driver Interface (USB Host Library --> HCD)
*******************************************************************************/
//==============================================================================
/**
 * @brief  USBH_LL_Init
 *         Initialize the Low Level portion of the Host driver.
 * @param  phost: Host handle
 * @retval USBH Status
 */
//==============================================================================
USBH_StatusTypeDef USBH_LL_Init(USBH_HandleTypeDef *phost)
{
        USBH_t *hdl = usbh;

#ifdef USE_USB_FS
        /* Set the LL Driver parameters */
        hdl->hhcd.Instance = USB_OTG_FS;
        hdl->hhcd.Init.Host_channels = 11;
        hdl->hhcd.Init.dma_enable = 0;
        hdl->hhcd.Init.low_power_enable = 0;
        hdl->hhcd.Init.phy_itface = HCD_PHY_EMBEDDED;
        hdl->hhcd.Init.Sof_enable = 0;
        hdl->hhcd.Init.speed = HCD_SPEED_FULL;
        hdl->hhcd.Init.vbus_sensing_enable = 0;
        hdl->hhcd.Init.lpm_enable = 0;

        /* Link the driver to the stack */
        hdl->hhcd.pData = phost;
        phost->pData = &hdl->hhcd;

        /* Initialize the LL Driver */
        HAL_HCD_Init(&hdl->hhcd);
#endif
#ifdef USE_USB_HS
        /* Set the LL driver parameters */
        hdl->hhcd.Instance = USB_OTG_HS;
        hdl->hhcd.Init.Host_channels = 11;
        hdl->hhcd.Init.dma_enable = 1;
        hdl->hhcd.Init.low_power_enable = 0;
        hdl->hhcd.Init.phy_itface = HCD_PHY_ULPI;
        hdl->hhcd.Init.Sof_enable = 0;
        hdl->hhcd.Init.speed = HCD_SPEED_HIGH;
        hdl->hhcd.Init.vbus_sensing_enable = 0;
        hdl->hhcd.Init.use_external_vbus = 1;
        hdl->hhcd.Init.lpm_enable = 0;

        /* Link the driver to the stack */
        hdl->hhcd.pData = phost;
        phost->pData = &hdl->hhcd;
        /* Initialize the LL driver */
        HAL_HCD_Init(&hdl->hhcd);
#endif /*USE_USB_HS*/

        USBH_LL_SetTimer(phost, HAL_HCD_GetCurrentFrame(&hdl->hhcd));

        return USBH_OK;
}

//==============================================================================
/**
 * @brief  USBH_LL_DeInit
 *         De-Initialize the Low Level portion of the Host driver.
 * @param  phost: Host handle
 * @retval USBH Status
 */
//==============================================================================
USBH_StatusTypeDef USBH_LL_DeInit(USBH_HandleTypeDef *phost)
{
        HAL_HCD_DeInit(phost->pData);
        return USBH_OK;
}

//==============================================================================
/**
 * @brief  USBH_LL_Start
 *         Start the Low Level portion of the Host driver.
 * @param  phost: Host handle
 * @retval USBH Status
 */
//==============================================================================
USBH_StatusTypeDef USBH_LL_Start(USBH_HandleTypeDef *phost)
{
        HAL_HCD_Start(phost->pData);
        return USBH_OK;
}

//==============================================================================
/**
 * @brief  USBH_LL_Stop
 *         Stop the Low Level portion of the Host driver.
 * @param  phost: Host handle
 * @retval USBH Status
 */
//==============================================================================
USBH_StatusTypeDef USBH_LL_Stop(USBH_HandleTypeDef *phost)
{
        HAL_HCD_Stop(phost->pData);
        return USBH_OK;
}

//==============================================================================
/**
 * @brief  USBH_LL_GetSpeed
 *         Return the USB Host Speed from the Low Level Driver.
 * @param  phost: Host handle
 * @retval USBH Speeds
 */
//==============================================================================
USBH_SpeedTypeDef USBH_LL_GetSpeed(USBH_HandleTypeDef *phost)
{
        USBH_SpeedTypeDef speed = USBH_SPEED_FULL;

        switch (HAL_HCD_GetCurrentSpeed(phost->pData)) {
        case 0:
                printk("USBH: connected high speed device");
                speed = USBH_SPEED_HIGH;
                break;

        case 1:
                printk("USBH: connected full speed device");
                speed = USBH_SPEED_FULL;
                break;

        case 2:
                printk("USBH: connected low speed device");
                speed = USBH_SPEED_LOW;
                break;

        default:
                printk("USBH: connected full speed device");
                speed = USBH_SPEED_FULL;
                break;
        }

        return speed;
}

//==============================================================================
/**
 * @brief  USBH_LL_ResetPort
 *         Reset the Host Port of the Low Level Driver.
 * @param  phost: Host handle
 * @retval USBH Status
 */
//==============================================================================
USBH_StatusTypeDef USBH_LL_ResetPort(USBH_HandleTypeDef *phost)
{
        _GPIO_DDI_set_pin(IOCTL_GPIO_PORT_IDX__UM_RESET, IOCTL_GPIO_PIN_IDX__UM_RESET);
        sys_sleep_ms(50);
        _GPIO_DDI_clear_pin(IOCTL_GPIO_PORT_IDX__UM_RESET, IOCTL_GPIO_PIN_IDX__UM_RESET);

        HAL_HCD_ResetPort(phost->pData);
        return USBH_OK;
}

//==============================================================================
/**
 * @brief  USBH_LL_GetLastXferSize
 *         Return the last transferred packet size.
 * @param  phost: Host handle
 * @param  pipe: Pipe index
 * @retval Packet Size
 */
//==============================================================================
uint32_t USBH_LL_GetLastXferSize(USBH_HandleTypeDef *phost, uint8_t pipe)
{
        return HAL_HCD_HC_GetXferCount(phost->pData, pipe);
}

//==============================================================================
/**
 * @brief  USBH_LL_OpenPipe
 *         Open a pipe of the Low Level Driver.
 * @param  phost: Host handle
 * @param  pipe_num: Pipe index
 * @param  epnum: Endpoint Number
 * @param  dev_address: Device USB address
 * @param  speed: Device Speed
 * @param  ep_type: Endpoint Type
 * @param  mps: Endpoint Max Packet Size
 * @retval USBH Status
 */
//==============================================================================
USBH_StatusTypeDef USBH_LL_OpenPipe(USBH_HandleTypeDef *phost,
                                    uint8_t pipe_num,
                                    uint8_t epnum,
                                    uint8_t dev_address,
                                    uint8_t speed,
                                    uint8_t ep_type,
                                    uint16_t mps)
{
        HAL_HCD_HC_Init(phost->pData,
                        pipe_num,
                        epnum,
                        dev_address,
                        speed,
                        ep_type,
                        mps);
        return USBH_OK;
}

//==============================================================================
/**
 * @brief  USBH_LL_ClosePipe
 *         Close a pipe of the Low Level Driver.
 * @param  phost: Host handle
 * @param  pipe_num: Pipe index
 * @retval USBH Status
 */
//==============================================================================
USBH_StatusTypeDef USBH_LL_ClosePipe(USBH_HandleTypeDef *phost, uint8_t pipe)
{
        HAL_HCD_HC_Halt(phost->pData, pipe);
        return USBH_OK;
}

//==============================================================================
/**
 * @brief  USBH_LL_SubmitURB
 *         Submit a new URB to the low level driver.
 * @param  phost: Host handle
 * @param  pipe: Pipe index
 *         This parameter can be a value from 1 to 15
 * @param  direction : Channel number
 *          This parameter can be one of the these values:
 *           0 : Output
 *           1 : Input
 * @param  ep_type : Endpoint Type
 *          This parameter can be one of the these values:
 *            @arg EP_TYPE_CTRL: Control type
 *            @arg EP_TYPE_ISOC: Isochronous type
 *            @arg EP_TYPE_BULK: Bulk type
 *            @arg EP_TYPE_INTR: Interrupt type
 * @param  token : Endpoint Type
 *          This parameter can be one of the these values:
 *            @arg 0: PID_SETUP
 *            @arg 1: PID_DATA
 * @param  pbuff : pointer to URB data
 * @param  length : Length of URB data
 * @param  do_ping : activate do ping protocol (for high speed only)
 *          This parameter can be one of the these values:
 *           0 : do ping inactive
 *           1 : do ping active
 * @retval Status
 */
//==============================================================================
USBH_StatusTypeDef USBH_LL_SubmitURB(USBH_HandleTypeDef *phost,
                                     uint8_t pipe,
                                     uint8_t direction,
                                     uint8_t ep_type,
                                     uint8_t token,
                                     uint8_t *pbuff,
                                     uint16_t length,
                                     uint8_t do_ping)
{
        HAL_HCD_HC_SubmitRequest(phost->pData,
                                 pipe,
                                 direction,
                                 ep_type,
                                 token,
                                 pbuff,
                                 length,
                                 do_ping);
        return USBH_OK;
}

//==============================================================================
/**
 * @brief  USBH_LL_GetURBState
 *         Get a URB state from the low level driver.
 * @param  phost: Host handle
 * @param  pipe: Pipe index
 *         This parameter can be a value from 1 to 15
 * @retval URB state
 *          This parameter can be one of the these values:
 *            @arg URB_IDLE
 *            @arg URB_DONE
 *            @arg URB_NOTREADY
 *            @arg URB_NYET
 *            @arg URB_ERROR
 *            @arg URB_STALL
 */
//==============================================================================
USBH_URBStateTypeDef USBH_LL_GetURBState(USBH_HandleTypeDef *phost, uint8_t pipe)
{
        return (USBH_URBStateTypeDef)HAL_HCD_HC_GetURBState (phost->pData, pipe);
}

//==============================================================================
/**
 * @brief  USBH_LL_DriverVBUS
 *         Drive VBUS.
 * @param  phost: Host handle
 * @param  state : VBUS state
 *          This parameter can be one of the these values:
 *           0 : VBUS Active
 *           1 : VBUS Inactive
 * @retval Status
 */
//==============================================================================
USBH_StatusTypeDef USBH_LL_DriverVBUS(USBH_HandleTypeDef *phost, uint8_t state)
{
        UNUSED_ARG2(phost, state);

#ifdef USE_USB_FS
        if(state == 0) {
                /* Configure Low Charge pump */
                BSP_IO_WritePin(OTG_FS1_POWER_SWITCH_PIN, BSP_IO_PIN_RESET);
        } else {
                /* Drive High Charge pump */
                BSP_IO_WritePin(OTG_FS1_POWER_SWITCH_PIN, BSP_IO_PIN_SET);
        }
#endif
        sys_sleep_ms(200);

        return USBH_OK;
}

//==============================================================================
/**
 * @brief  USBH_LL_SetToggle
 *         Set toggle for a pipe.
 * @param  phost: Host handle
 * @param  pipe: Pipe index
 * @param  pipe_num: Pipe index
 * @param  toggle: toggle (0/1)
 * @retval Status
 */
//==============================================================================
USBH_StatusTypeDef USBH_LL_SetToggle(USBH_HandleTypeDef *phost, uint8_t pipe, uint8_t toggle)
{
        UNUSED_ARG1(phost);

        USBH_t *hdl = usbh;

        if(hdl->hhcd.hc[pipe].ep_is_in) {
                hdl->hhcd.hc[pipe].toggle_in = toggle;
        } else {
                hdl->hhcd.hc[pipe].toggle_out = toggle;
        }

        return USBH_OK;
}

//==============================================================================
/**
 * @brief  USBH_LL_GetToggle
 *         Return the current toggle of a pipe.
 * @param  phost: Host handle
 * @param  pipe: Pipe index
 * @retval toggle (0/1)
 */
//==============================================================================
uint8_t USBH_LL_GetToggle(USBH_HandleTypeDef *phost, uint8_t pipe)
{
        UNUSED_ARG1(phost);

        USBH_t *hdl = usbh;

        uint8_t toggle = 0;

        if(hdl->hhcd.hc[pipe].ep_is_in) {
                toggle = hdl->hhcd.hc[pipe].toggle_in;
        } else {
                toggle = hdl->hhcd.hc[pipe].toggle_out;
        }

        return toggle;
}

//==============================================================================
/**
 * @brief  USBH_Delay
 *         Delay routine for the USB Host Library
 * @param  Delay: Delay in ms
 * @retval None
 */
//==============================================================================
void USBH_Delay(uint32_t Delay)
{
        sys_sleep_ms(Delay);
}

//==============================================================================
/**
 * @brief  Memory allocation function used in USB host library.
 *
 * @param  size         block size
 *
 * @return Block address.
 */
//==============================================================================
void *USBH_malloc(uint32_t size)
{
        void *mem = NULL;
        int err = sys_zalloc2(size, NULL, _MM_FLAG__DMA_CAPABLE, _MM_FLAG__DMA_CAPABLE, &mem);
        return !err ? mem : NULL;
}

//==============================================================================
/**
 * @brief  Memory free function used in USB host library.
 *
 * @param  mem          block to free
 */
//==============================================================================
void USBH_free(void *mem)
{
        sys_free(&mem);
}

//==============================================================================
/**
 * @brief  USB HS IRQ handler
 */
//==============================================================================
void CAN3_SCE_OTG_HS_IRQHandler(void)
{
        if (usbh) {
                usbh_irq = true;
                HAL_HCD_IRQHandler(&usbh->hhcd);
                usbh_irq = false;

                if (usbh->irq_ctr > 1000000) {
                        //HAL_HCD_MspDeInit(&usbh->hhcd);
                }
        }
}

/*==============================================================================
  End of file
==============================================================================*/
