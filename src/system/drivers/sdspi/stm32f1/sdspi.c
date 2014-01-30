/*=========================================================================*//**
@file    sdspi.c

@author  Daniel Zorychta

@brief   This file support SD in SPI mode

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "core/module.h"
#include <dnx/thread.h>
#include <dnx/os.h>
#include <dnx/timer.h>
#include <dnx/misc.h>
#include <unistd.h>
#include "stm32f1/sdspi_cfg.h"
#include "stm32f1/sdspi_def.h"
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define MTX_BLOCK_TIME                                  0
#define MTX_BLOCK_TIME_LONG                             200
#define RELEASE_TIMEOUT_MS                              1000

/* MBR definitions */
#define MBR_BOOTSTRAP_CODE_OFFSET                       0x000
#define MBR_PARTITION_1_ENTRY_OFFSET                    0x1BE
#define MBR_PARTITION_2_ENTRY_OFFSET                    0x1CE
#define MBR_PARTITION_3_ENTRY_OFFSET                    0x1DE
#define MBR_PARTITION_4_ENTRY_OFFSET                    0x1EE
#define MBR_BOOT_SIGNATURE_OFFSET                       0x1FE

#define MBR_PARITION_ENTRY_STATUS_OFFSET                0x00
#define MBR_PARITION_ENTRY_CHS_FIRST_ADDR_OFFSET        0x01
#define MBR_PARITION_ENTRY_PARTITION_TYPE_OFFSET        0x04
#define MBR_PARITION_ENTRY_CHS_LAST_ADDR_OFFSET         0x05
#define MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET        0x08
#define MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET        0x0C

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/** card command definitions */
typedef enum {
        CMD0            = (0x40+0 ),    /* GO_IDLE_STATE */
        CMD1            = (0x40+1 ),    /* SEND_OP_COND (MMC) */
        ACMD41          = (0xC0+41),    /* SEND_OP_COND (SDC) */
        CMD8            = (0x40+8 ),    /* SEND_IF_COND */
        CMD9            = (0x40+9 ),    /* SEND_CSD */
        CMD10           = (0x40+10),    /* SEND_CID */
        CMD12           = (0x40+12),    /* STOP_TRANSMISSION */
        ACMD13          = (0xC0+13),    /* SD_STATUS (SDC) */
        CMD16           = (0x40+16),    /* SET_BLOCKLEN */
        CMD17           = (0x40+17),    /* READ_SINGLE_BLOCK */
        CMD18           = (0x40+18),    /* READ_MULTIPLE_BLOCK */
        CMD23           = (0x40+23),    /* SET_BLOCK_COUNT (MMC) */
        ACMD23          = (0xC0+23),    /* SET_WR_BLK_ERASE_COUNT (SDC) */
        CMD24           = (0x40+24),    /* WRITE_BLOCK */
        CMD25           = (0x40+25),    /* WRITE_MULTIPLE_BLOCK */
        CMD55           = (0x40+55),    /* APP_CMD */
        CMD58           = (0x40+58)     /* READ_OCR */
} card_cmd_t;

/** card types */
typedef struct {
        enum {
                        CT_UNKNOWN,
                        CT_MMC,
                        CT_SD1,
                        CT_SD2
        }               type  : 2;
        bool            block : 1;
} card_type;

/** device structure */
typedef struct {
        u32_t           first_sector;   /* sector number              */
        u32_t           size;           /* in sectors                 */
        bool            used;           /* true if part used          */
        u8_t            major;          /* device major number        */
        u8_t            minor;          /* device minor number        */
} sdpart_t;

/** main control structure */
typedef struct {
        mutex_t        *card_protect_mtx;
        sdpart_t       *part[5];
        card_type       card_type;
        bool            card_initialized;
#if (SDSPI_ENABLE_DMA != 0)
        volatile bool   DMA_tansaction_finished;
#endif
} sdctrl_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t         spi_turn_on_clock                       (void);
static stdret_t         spi_turn_off_clock                      (void);
static void             spi_configure                           (void);
static void             spi_select_card                         (void);
static void             spi_deselect_card                       (void);
static void             spi_configure_DMA                       (bool enable);
static void             spi_enable_Tx_Rx_DMA                    (void);
static void             spi_disable_Tx_Rx_DMA                   (void);
static bool             spi_is_rx_buffer_not_empty              (void);
static void             spi_send_data                           (u8_t data);
static u8_t             spi_get_data                            (void);
static u8_t             spi_transmit                            (u8_t out);
static u8_t             card_send_cmd                           (sdpart_t *hdl, card_cmd_t cmd, u32_t arg);
static u8_t             card_wait_ready                         (void);
static bool             card_receive_data_block                 (u8_t *buff);
static bool             card_transmit_data_block                (const u8_t *buff, u8_t token);
static ssize_t          card_read_whole_sectors                 (sdpart_t *hdl, void *dst, size_t nsectors, u64_t lseek);
static ssize_t          card_read_partial_sectors               (sdpart_t *hdl, void *dst, size_t size, u64_t lseek);
static ssize_t          card_write_whole_sectors                (sdpart_t *hdl, const void *src, size_t nsectors, u64_t lseek);
static ssize_t          card_write_partial_sectors              (sdpart_t *hdl, const void *src, size_t size, u64_t lseek);
static stdret_t         card_initialize                         (sdpart_t *hdl);
static ssize_t          card_read                               (sdpart_t *hdl, u8_t *dst, size_t count, u64_t lseek);
static ssize_t          card_write                              (sdpart_t *hdl, const u8_t *src, size_t count, u64_t lseek);
static u16_t            mbr_get_boot_signature                  (u8_t *sector);
static u32_t            mbr_get_partition_first_LBA_sector      (int partition, u8_t *sector);
static u32_t            mbr_get_partition_number_of_sectors     (int partition, u8_t *sector);
static stdret_t         mbr_detect_partitions                   (sdpart_t *hdl);

/*==============================================================================
  Local object definitions
==============================================================================*/
static sdctrl_t   *sdspi_ctrl;
static const u16_t sector_size = 512;

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
API_MOD_INIT(SDSPI, void **device_handle, u8_t major, u8_t minor)
{
        STOP_IF(device_handle == NULL);

        if (major != _SDSPI_CARD_0 || minor > _SDSPI_PARTITION_4) {
                errno = EINVAL;
                return STD_RET_ERROR;
        }

        if (sdspi_ctrl == NULL) {
                sdctrl_t *sdspi = calloc(1, sizeof(sdctrl_t));
                mutex_t  *mtx   = mutex_new(MUTEX_NORMAL);

                if (sdspi && mtx) {
                        spi_turn_on_clock();
                        spi_configure();
                        spi_configure_DMA(true);

                        sdspi->card_protect_mtx = mtx;
                        sdspi_ctrl              = sdspi;

                } else {
                        if (sdspi)
                                free(sdspi);

                        if (mtx)
                                mutex_delete(mtx);

                        return STD_RET_ERROR;
                }
        }

        if (sdspi_ctrl) {
                sdpart_t *part = calloc(1, sizeof(sdpart_t));
                if (part) {
                        part->first_sector      = 0;
                        part->size              = 0;
                        part->major             = major;
                        part->minor             = minor;

                        sdspi_ctrl->part[minor] = part;

                        *device_handle          = part;

                        return STD_RET_OK;
                }
        }

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
API_MOD_RELEASE(SDSPI, void *device_handle)
{
        STOP_IF(device_handle == NULL);

        sdpart_t *part = device_handle;

        timer_t timer = timer_reset();
        while (part->used) {
                if (timer_is_expired(timer, RELEASE_TIMEOUT_MS)) {
                        errno = EBUSY;
                        return STD_RET_ERROR;
                }

                sleep_ms(10);
        }

        critical_section_begin();
        sdspi_ctrl->part[part->minor] = NULL;

        part->first_sector = 0;
        part->size         = 0;
        part->used         = false;
        part->minor        = 0;

        free(part);

        if (  sdspi_ctrl->part[_SDSPI_FULL_VOLUME] == NULL
           && sdspi_ctrl->part[_SDSPI_PARTITION_1] == NULL
           && sdspi_ctrl->part[_SDSPI_PARTITION_2] == NULL
           && sdspi_ctrl->part[_SDSPI_PARTITION_3] == NULL
           && sdspi_ctrl->part[_SDSPI_PARTITION_4] == NULL  ) {

                mutex_delete(sdspi_ctrl->card_protect_mtx);
                sdspi_ctrl->card_initialized = false;
                sdspi_ctrl->card_protect_mtx = NULL;
                free(sdspi_ctrl);

                spi_turn_off_clock();
                spi_configure_DMA(false);
        }

        critical_section_end();

        return STD_RET_OK;
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
API_MOD_OPEN(SDSPI, void *device_handle, int flags)
{
        UNUSED_ARG(flags);
        STOP_IF(device_handle == NULL);

        sdpart_t *part = device_handle;

        if (part->used == true) {
                errno = EBUSY;
                return STD_RET_ERROR;
        } else {
                return STD_RET_OK;
        }
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
API_MOD_CLOSE(SDSPI, void *device_handle, bool force)
{
        UNUSED_ARG(force);
        STOP_IF(device_handle == NULL);

        sdpart_t *part = device_handle;

        part->used = false;

        return STD_RET_OK;
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
API_MOD_WRITE(SDSPI, void *device_handle, const u8_t *src, size_t count, u64_t *fpos, struct vfs_fattr fattr)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(src == NULL);
        STOP_IF(count == 0);
        STOP_IF(fpos == NULL);
        UNUSED_ARG(fattr);

        sdpart_t *part = device_handle;

        ssize_t n = -1;
        if (part->size > 0) {
                if (mutex_lock(sdspi_ctrl->card_protect_mtx, MAX_DELAY_MS)) {
                        n = card_write(part, src, count, *fpos + ((u64_t)part->first_sector * sector_size));
                        mutex_unlock(sdspi_ctrl->card_protect_mtx);
                } else {
                        errno = EBUSY;
                }
        } else {
                errno = ENOMEDIUM;
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
API_MOD_READ(SDSPI, void *device_handle, u8_t *dst, size_t count, u64_t *fpos, struct vfs_fattr fattr)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(dst == NULL);
        STOP_IF(count == 0);
        STOP_IF(fpos == NULL);
        UNUSED_ARG(fattr);

        sdpart_t *part = device_handle;

        ssize_t n = -1;
        if (part->size > 0) {
                if (mutex_lock(sdspi_ctrl->card_protect_mtx, MAX_DELAY_MS)) {
                        n = card_read(part, dst, count, *fpos + ((u64_t)part->first_sector * sector_size));
                        mutex_unlock(sdspi_ctrl->card_protect_mtx);
                } else {
                        errno = EBUSY;
                }
        } else {
                errno = ENOMEDIUM;
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_IOCTL(SDSPI, void *device_handle, int request, void *arg)
{
        STOP_IF(device_handle == NULL);

        sdpart_t *part = device_handle;

        stdret_t status = STD_RET_OK;

        switch (request) {
        case SDSPI_IORQ_INITIALIZE_CARD:
                if (mutex_lock(sdspi_ctrl->card_protect_mtx, MTX_BLOCK_TIME)) {
                        bool *result = arg;
                        *result      = false;

                        if (card_initialize(part) == STD_RET_OK) {
                                if (mbr_detect_partitions(part) == STD_RET_OK) {
                                        *result = true;
                                }
                        }

                        mutex_unlock(sdspi_ctrl->card_protect_mtx);
                } else {
                        errno  = EBUSY;
                        status = STD_RET_ERROR;
                }
                break;

        default:
                errno = EBADRQC;
                status = STD_RET_ERROR;
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_FLUSH(SDSPI, void *device_handle)
{
        STOP_IF(device_handle == NULL);

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
API_MOD_STAT(SDSPI, void *device_handle, struct vfs_dev_stat *device_stat)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(device_stat == NULL);

        sdpart_t *part = device_handle;

        if (sdspi_ctrl->card_initialized) {
                device_stat->st_major = part->major;
                device_stat->st_minor = part->minor;
                device_stat->st_size  = (u64_t)part->size * sector_size;

                return STD_RET_OK;
        } else {
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Function turn on SPI clock
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t spi_turn_on_clock(void)
{
        switch ((u32_t)SDSPI_PORT) {
        #if defined(RCC_APB2ENR_SPI1EN)
        case SPI1_BASE:
                RCC->APB2RSTR |=  RCC_APB2RSTR_SPI1RST;
                RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
                RCC->APB2ENR  |=  RCC_APB2ENR_SPI1EN;
                return STD_RET_OK;
        #endif
        #if defined(RCC_APB1ENR_SPI2EN)
        case SPI2_BASE:
                RCC->APB1RSTR |=  RCC_APB1RSTR_SPI2RST;
                RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;
                RCC->APB1ENR  |=  RCC_APB1ENR_SPI2EN;
                return STD_RET_OK;
        #endif
        #if defined(RCC_APB1ENR_SPI3EN)
        case SPI3_BASE:
                RCC->APB1RSTR |=  RCC_APB1RSTR_SPI3RST;
                RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI3RST;
                RCC->APB1ENR  |=  RCC_APB1ENR_SPI3EN;
                return STD_RET_OK;
        #endif
        default:
                errno = EIO;
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Function turn off SPI clock
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t spi_turn_off_clock(void)
{
        switch ((u32_t)SDSPI_PORT) {
        #if defined(RCC_APB2ENR_SPI1EN)
        case SPI1_BASE:
                RCC->APB2RSTR |=  RCC_APB2RSTR_SPI1RST;
                RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
                RCC->APB2ENR  &= ~RCC_APB2ENR_SPI1EN;
                return STD_RET_OK;
        #endif
        #if defined(RCC_APB1ENR_SPI2EN)
        case SPI2_BASE:
                RCC->APB1RSTR |=  RCC_APB1RSTR_SPI2RST;
                RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;
                RCC->APB1ENR  &= ~RCC_APB1ENR_SPI2EN;
                return STD_RET_OK;
        #endif
        #if defined(RCC_APB1ENR_SPI3EN)
        case SPI3_BASE:
                RCC->APB1RSTR |=  RCC_APB1RSTR_SPI3RST;
                RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI3RST;
                RCC->APB1ENR  &= ~RCC_APB1ENR_SPI3EN;
                return STD_RET_OK;
        #endif
        default:
                errno = EIO;
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Function configure SPI interface
 *
 * SPI configuration (Mode 0):
 * - MSB First
 * - sample on rising edge
 * - SCLK L on idle state
 * - 8-bit data
 */
//==============================================================================
static void spi_configure(void)
{
        /* enable_SS_output_in_master_mode */
        SET_BIT(SDSPI_PORT->CR2, SPI_CR2_SSOE);

        /* enable_software_slave_management */
        /* enable_master_mode */
        SET_BIT(SDSPI_PORT->CR1, SPI_CR1_SSM | SPI_CR1_MSTR);

        /* set clock divider */
        CLEAR_BIT(SDSPI_PORT->CR1, SPI_CR1_BR);
        if (SDSPI_SPI_CLOCK_DIVIDER <= 2)
                SET_BIT(SDSPI_PORT->CR1, 0);
        else if (SDSPI_SPI_CLOCK_DIVIDER <= 4)
                SET_BIT(SDSPI_PORT->CR1, SPI_CR1_BR_0);
        else if (SDSPI_SPI_CLOCK_DIVIDER <= 8)
                SET_BIT(SDSPI_PORT->CR1, SPI_CR1_BR_1);
        else if (SDSPI_SPI_CLOCK_DIVIDER <= 16)
                SET_BIT(SDSPI_PORT->CR1, SPI_CR1_BR_1 | SPI_CR1_BR_0);
        else if (SDSPI_SPI_CLOCK_DIVIDER <= 32)
                SET_BIT(SDSPI_PORT->CR1, SPI_CR1_BR_2);
        else if (SDSPI_SPI_CLOCK_DIVIDER <= 64)
                SET_BIT(SDSPI_PORT->CR1, SPI_CR1_BR_2 | SPI_CR1_BR_0);
        else if (SDSPI_SPI_CLOCK_DIVIDER <= 128)
                SET_BIT(SDSPI_PORT->CR1, SPI_CR1_BR_2 | SPI_CR1_BR_1);
        else
                SET_BIT(SDSPI_PORT->CR1, SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0);

        SET_BIT(SDSPI_PORT->CR1, SPI_CR1_SPE);
}

//==============================================================================
/**
 * @brief Function select card
 */
//==============================================================================
static void spi_select_card(void)
{
        SDSPI_SD_SELECT;
}

//==============================================================================
/**
 * @brief Function deselect card
 */
//==============================================================================
static void spi_deselect_card(void)
{
        SDSPI_SD_DESELECT;
}

//==============================================================================
/**
 * @brief Configure DMA
 */
//==============================================================================
static void spi_configure_DMA(bool enable)
{
#if (SDSPI_ENABLE_DMA != 0)
        if (enable) {
                if ((u32_t)SDSPI_DMA == DMA1_BASE) {
                        SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA1EN);
                } else if ((u32_t)SDSPI_DMA == DMA2_BASE) {
                        SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA2EN);
                } else {
                        return;
                }

                NVIC_SetPriority(SDSPI_DMA_IRQ_NUMBER, SDSPI_DMA_IRQ_PRIORITY);

        } else {
                if ((u32_t)SDSPI_DMA == DMA1_BASE) {
                        CLEAR_BIT(RCC->AHBENR, RCC_AHBENR_DMA1EN);
                } else if ((u32_t)SDSPI_DMA == DMA2_BASE) {
                        CLEAR_BIT(RCC->AHBENR, RCC_AHBENR_DMA2EN);
                } else {
                        return;
                }
        }
#endif
}

//==============================================================================
/**
 * @brief Function enable Tx and Rx DMA transaction
 */
//==============================================================================
static void spi_enable_Tx_Rx_DMA(void)
{
        SET_BIT(SDSPI_PORT->CR2,SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);
}

//==============================================================================
/**
 * @brief Function disable Tx and Rx DMA transaction
 */
//==============================================================================
static void spi_disable_Tx_Rx_DMA(void)
{
        CLEAR_BIT(SDSPI_PORT->CR2,SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);
}

//==============================================================================
/**
 * @brief Function check if RX buffer is empty
 */
//==============================================================================
static bool spi_is_rx_buffer_not_empty(void)
{
        return (SDSPI_PORT->SR & SPI_SR_RXNE);
}

//==============================================================================
/**
 * @brief Function send data using SPI register
 *
 * @param data
 */
//==============================================================================
static void spi_send_data(u8_t data)
{
        SDSPI_PORT->DR = data;
}

//==============================================================================
/**
 * @brief Function gets data from SPI register
 *
 * @return received byte
 */
//==============================================================================
static u8_t spi_get_data(void)
{
        return SDSPI_PORT->DR & 0xFF;
}

//==============================================================================
/**
 * @brief Function send byte by SPI peripheral
 *
 * @param[in] out       data to send
 *
 * @return received byte
 */
//==============================================================================
static u8_t spi_transmit(u8_t out)
{
        spi_send_data(out);
        while (!spi_is_rx_buffer_not_empty());
        return spi_get_data();
}

//==============================================================================
/**
 * @brief Function wait for card ready
 *
 * @return card response
 */
//==============================================================================
static u8_t card_wait_ready(void)
{
        spi_transmit(0xFF);

        u8_t    response;
        timer_t timer = timer_reset();
        while ((response = spi_transmit(0xFF)) != 0xFF && timer_is_not_expired(timer, SDSPI_TIMEOUT));

        return response;
}

//==============================================================================
/**
 * @brief Function transmit command to card
 *
 * @param[in] hdl       partition handler
 * @param[in] cmd       card command
 * @param[in] arg       command's argument
 */
//==============================================================================
static u8_t card_send_cmd(sdpart_t *hdl, card_cmd_t cmd, u32_t arg)
{
        u8_t response;

        /* ACMD<n> is the command sequence of CMD55-CMD<n> */
        if (cmd & 0x80) {
                cmd &= 0x7F;
                response = card_send_cmd(hdl, CMD55, 0);
                if (response > 1)
                        return response;
        }

        /* select the card and wait for ready */
        spi_deselect_card();
        spi_select_card();

        if (card_wait_ready() != 0xFF) {
                errno = EIO;
                return 0xFF;
        }

        /* send command packet */
        spi_transmit(cmd);                    /* Start + Command index */
        spi_transmit(arg >> 24);              /* Argument[31..24]      */
        spi_transmit(arg >> 16);              /* Argument[23..16]      */
        spi_transmit(arg >> 8);               /* Argument[15..8]       */
        spi_transmit(arg);

        if (cmd == CMD0)
                spi_transmit(0x95);           /* Valid CRC for CMD0(0) */
        else if (cmd == CMD8)
                spi_transmit(0x87);           /* Valid CRC for CMD8(0x1AA) */
        else
                spi_transmit(0x01);           /* Dummy CRC + Stop */

        /* receive command response */
        if (cmd == CMD12)
                spi_transmit(0xFF);           /* Skip a stuff byte when stop reading */

        /* wait for a valid response in timeout of 10 attempts */
        int n = 10;
        do {
                response = spi_transmit(0xFF);
        } while ((response & 0x80) && --n);

        return response;
}

//==============================================================================
/**
 * @brief Function receive data block
 *
 * @param[out] *buff            data buffer (sector size)
 *
 * @retval true if success
 * @retval false if error
 */
//==============================================================================
static bool card_receive_data_block(u8_t *buff)
{
        u8_t    token;
        timer_t timer = timer_reset();
        while ((token = spi_transmit(0xFF)) == 0xFF && timer_is_not_expired(timer, SDSPI_TIMEOUT));

        if (token != 0xFE) {
                errno = EIO;
                return false;
        }

#if (SDSPI_ENABLE_DMA != 0)
        SDSPI_DMA_RX_CHANNEL->CPAR  = (u32_t)&SDSPI_PORT->DR;
        SDSPI_DMA_RX_CHANNEL->CMAR  = (u32_t)buff;
        SDSPI_DMA_RX_CHANNEL->CNDTR = sector_size;
        SDSPI_DMA_RX_CHANNEL->CCR   = DMA_CCR1_MINC | DMA_CCR1_TCIE | DMA_CCR1_EN;
        NVIC_EnableIRQ(SDSPI_DMA_IRQ_NUMBER);

        u16_t dummy = 0xFFFF;
        SDSPI_DMA_TX_CHANNEL->CPAR  = (u32_t)&SDSPI_PORT->DR;
        SDSPI_DMA_TX_CHANNEL->CMAR  = (u32_t)&dummy;
        SDSPI_DMA_TX_CHANNEL->CNDTR = sector_size;
        SDSPI_DMA_TX_CHANNEL->CCR   = DMA_CCR1_DIR | DMA_CCR1_EN;

        sdspi_ctrl->DMA_tansaction_finished = false;

        spi_enable_Tx_Rx_DMA();

        while (sdspi_ctrl->DMA_tansaction_finished == false);

#else
        /* memory alignment */
        int size = sector_size;
        do {
                *buff++ = spi_transmit(0xFF);
                *buff++ = spi_transmit(0xFF);
                *buff++ = spi_transmit(0xFF);
                *buff++ = spi_transmit(0xFF);
        } while (size -= 4);
#endif

        /* discard CRC */
        spi_transmit(0xFF);
        spi_transmit(0xFF);

        return true;
}

//==============================================================================
/**
 * @brief Function transmit data block
 *
 * @param[in] *buff             source buffer (sector size)
 * @param[in]  toke             token to send
 *
 * @retval true if success
 * @retval false if error
 */
//==============================================================================
static bool card_transmit_data_block(const u8_t *buff, u8_t token)
{
        if (card_wait_ready() != 0xFF) {
                errno = EIO;
                return false;
        }

        spi_transmit(token);

        if (token != 0xFD) {
#if (SDSPI_ENABLE_DMA != 0)
                u16_t dummy;
                SDSPI_DMA_RX_CHANNEL->CMAR  = (u32_t)&dummy;
                SDSPI_DMA_RX_CHANNEL->CPAR  = (u32_t)&SDSPI_PORT->DR;
                SDSPI_DMA_RX_CHANNEL->CNDTR = sector_size;
                SDSPI_DMA_RX_CHANNEL->CCR   = DMA_CCR1_TCIE | DMA_CCR1_EN;
                NVIC_EnableIRQ(SDSPI_DMA_IRQ_NUMBER);

                SDSPI_DMA_TX_CHANNEL->CMAR  = (u32_t)buff;
                SDSPI_DMA_TX_CHANNEL->CPAR  = (u32_t)&SDSPI_PORT->DR;
                SDSPI_DMA_TX_CHANNEL->CNDTR = sector_size;
                SDSPI_DMA_TX_CHANNEL->CCR   = DMA_CCR1_MINC | DMA_CCR1_DIR | DMA_CCR1_EN;

                sdspi_ctrl->DMA_tansaction_finished = false;

                spi_enable_Tx_Rx_DMA();

                while (sdspi_ctrl->DMA_tansaction_finished == false);

#else
                int size = sector_size;
                do {
                        spi_transmit(*buff++);
                        spi_transmit(*buff++);
                        spi_transmit(*buff++);
                        spi_transmit(*buff++);
                } while (size -= 4);
#endif

                /* CRC dummy */
                spi_transmit(0xFF);
                spi_transmit(0xFF);

                /* receive data response */
                if ((spi_transmit(0xFF) & 0x1F) != 0x05) {
                        errno = EIO;
                        return false;
                }
        }

        return true;
}

//==============================================================================
/**
 * @brief Function read whole sectors
 *
 * @param[in]  *hdl             driver's memory handle
 * @param[out] *dst             destination
 * @param[in]   nsectors        sectors to read
 * @param[in]   lseek           file index
 *
 * @retval number of read sectors
 */
//==============================================================================
static ssize_t card_read_whole_sectors(sdpart_t *hdl, void *dst, size_t nsectors, u64_t lseek)
{
        if (sdspi_ctrl->card_type.block) {
                lseek >>= 9;    /* divide by 512 */
        }

        /* 1 sector to read */
        ssize_t n = -1;
        if (nsectors == 1) {
                if (card_send_cmd(hdl, CMD17, (u32_t)lseek) == 0) {
                        if (card_receive_data_block(dst)) {
                                n = 1;
                        }
                }
        } else {
                if (card_send_cmd(hdl, CMD18, (u32_t)lseek) == 0) {
                        n = 0;
                        do {
                                if (!card_receive_data_block(dst)) {
                                        break;
                                }

                                dst += sector_size;
                        } while (++n < (ssize_t)nsectors);

                        /* stop transmission */
                        card_send_cmd(hdl, CMD12, 0);
                }
        }

        return n;
}

//==============================================================================
/**
 * @brief Function read only selected data from sectors
 *
 * @param[in]  *hdl             driver's memory handle
 * @param[out] *dst             destination
 * @param[in]   size            number of bytes to read
 * @param[in]   lseek           file index
 *
 * @retval number of read bytes
 */
//==============================================================================
static ssize_t card_read_partial_sectors(sdpart_t *hdl, void *dst, size_t size, u64_t lseek)
{
        u8_t *buffer = malloc(sector_size);
        if (!buffer)
                return -1;

        u32_t recv_data = 0;
        while (recv_data < size) {
                if (lseek % sector_size == 0 && (size - recv_data) / sector_size > 0) {
                        ssize_t n = card_read_whole_sectors(hdl, dst, size / sector_size, lseek);
                        if (n == -1) {
                                recv_data = -1;
                                goto exit;
                        } else if (n != (ssize_t)size / sector_size) {
                                break;
                        }

                        dst       += n * sector_size;
                        lseek     += n * sector_size;
                        recv_data += n * sector_size;
                } else {
                        ssize_t n = card_read_whole_sectors(hdl, buffer, 1, lseek & ~(0x1FF));
                        if (n == -1) {
                                recv_data = -1;
                                goto exit;
                        } else if (n != 1) {
                                break;
                        }

                        u32_t rest;
                        if ((sector_size - (lseek % sector_size)) > (size - recv_data))
                                rest = size - recv_data;
                        else
                                rest = sector_size - (lseek % sector_size);

                        memcpy(dst, buffer + (lseek % sector_size), rest);
                        dst       += rest;
                        recv_data += rest;
                        lseek     += rest;
                }
        }

exit:
        free(buffer);

        return recv_data;
}

//==============================================================================
/**
 * @brief Function write whole sectors
 *
 * @param[in]  *hdl             driver's memory handle
 * @param[out] *src             source
 * @param[in]   nsectors        sectors to read
 * @param[in]   lseek           file index
 *
 * @retval number of written sectors
 */
//==============================================================================
static ssize_t card_write_whole_sectors(sdpart_t *hdl, const void *src, size_t nsectors, u64_t lseek)
{
        if (sdspi_ctrl->card_type.block) {
                lseek >>= 9;    /* divide by 512 */
        }

        /* 1 sector to read */
        ssize_t n = -1;
        if (nsectors == 1) {
                if (card_send_cmd(hdl, CMD24, (u32_t)lseek) == 0) {
                        if (card_transmit_data_block(src, 0xFE)) {
                                n = 1;
                        }
                }
        } else {
                if (sdspi_ctrl->card_type.type == CT_SD1 || sdspi_ctrl->card_type.type == CT_SD2) {
                        card_send_cmd(hdl, ACMD23, nsectors);
                }

                if (card_send_cmd(hdl, CMD25, (u32_t)lseek) == 0) {
                        n = 0;
                        do {
                                if (!card_transmit_data_block(src, 0xFC)) {
                                        break;
                                }

                                src += sector_size;
                        } while (++n < (ssize_t)nsectors);

                        /* stop transmission */
                        if (!card_transmit_data_block(NULL, 0xFD)) {
                                n = 0;
                        }
                }
        }

        return n;
}

//==============================================================================
/**
 * @brief Function write only selected data to sectors
 *
 * @param[in]  *hdl             driver's memory handle
 * @param[out] *src             source
 * @param[in]   size            number of bytes to read
 * @param[in]   lseek           file index
 *
 * @retval number of written bytes
 */
//==============================================================================
static ssize_t card_write_partial_sectors(sdpart_t *hdl, const void *src, size_t size, u64_t lseek)
{
        u8_t *buffer = malloc(sector_size);
        if (!buffer)
                return -1;

        u32_t transmit_data = 0;
        while (transmit_data < size) {
                if (lseek % sector_size == 0 && (size - transmit_data) / sector_size > 0) {
                        ssize_t n = card_write_whole_sectors(hdl, src, size / sector_size, lseek);
                        if (n == -1) {
                                transmit_data = -1;
                                goto exit;
                        } else if (n != (ssize_t)size / sector_size) {
                                break;
                        }

                        src           += n * sector_size;
                        lseek         += n * sector_size;
                        transmit_data += n * sector_size;
                } else {
                        ssize_t n = card_read_whole_sectors(hdl, buffer, 1, lseek & ~(0x1FF));
                        if (n == -1) {
                                transmit_data = -1;
                                goto exit;
                        } else if (n != 1) {
                                break;
                        }

                        u32_t rest;
                        if ((sector_size - (lseek % sector_size)) > (size - transmit_data))
                                rest = size - transmit_data;
                        else
                                rest = sector_size - (lseek % sector_size);

                        memcpy(buffer + (lseek % sector_size), src, rest);

                        n = card_write_whole_sectors(hdl, buffer, 1, lseek & ~(0x1FF));
                        if (n == -1) {
                                transmit_data = -1;
                                goto exit;
                        } else if (n != 1) {
                                break;
                        }

                        src           += rest;
                        transmit_data += rest;
                        lseek         += rest;
                }
        }

exit:
        free(buffer);

        return transmit_data;
}

//==============================================================================
/**
 * @brief Function initialize card
 *
 * @param[in] *hdl      SD module data
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t card_initialize(sdpart_t *hdl)
{
        spi_deselect_card();
        for (int n = 0; n < 50; n++) {
                spi_transmit(0xFF);
        }

        sdspi_ctrl->card_type.type   = CT_UNKNOWN;
        sdspi_ctrl->card_type.block  = false;
        sdspi_ctrl->card_initialized = false;

        timer_t timer = timer_reset();

        if (card_send_cmd(hdl, CMD0, 0) == 0x01) {
                if (card_send_cmd(hdl, CMD8, 0x1AA) == 0x01) { /* check SDHC card */
                        u8_t OCR[4];

                        for (int n = 0; n < 4; n++) {
                                OCR[n] = spi_transmit(0xFF);
                        }

                        if (OCR[2] == 0x01 && OCR[3] == 0xAA) {
                                while ( timer_is_not_expired(timer, SDSPI_TIMEOUT)
                                      && card_send_cmd(hdl, ACMD41, 1UL << 30) ) {

                                        sleep_ms(1);
                                }

                                if ( !timer_is_expired(timer, SDSPI_TIMEOUT)
                                   && card_send_cmd(hdl, CMD58, 0) == 0 ) {

                                        for (int n = 0; n < 4; n++) {
                                                OCR[n] = spi_transmit(0xFF);
                                        }

                                        sdspi_ctrl->card_type.type   = CT_SD2;
                                        sdspi_ctrl->card_type.block  = (OCR[0] & 0x40) ? true : false;

                                        sdspi_ctrl->card_initialized = true;
                                }
                        }
                } else { /* SDSC or MMC */
                        u8_t cmd;
                        if (card_send_cmd(hdl, ACMD41, 0) <= 0x01)   {
                                sdspi_ctrl->card_type.type = CT_SD1;
                                cmd = ACMD41;   /* SDSC */
                        } else {
                                sdspi_ctrl->card_type.type = CT_MMC;
                                cmd = CMD1;     /* MMC */
                        }

                        /* Wait for leaving idle state */
                        while (!timer_is_expired(timer, SDSPI_TIMEOUT) && card_send_cmd(hdl, cmd, 0)) {
                                sleep_ms(1);
                        }

                        /* set R/W block length to 512 */
                        if ( !timer_is_expired(timer, SDSPI_TIMEOUT)
                           || card_send_cmd(hdl, CMD16, sector_size) != 0) {

                                sdspi_ctrl->card_type.type   = CT_UNKNOWN;
                                sdspi_ctrl->card_type.block  = false;
                        } else {
                                sdspi_ctrl->card_initialized = true;
                        }
                }
        }

        /* size info */
        for (int i = _SDSPI_FULL_VOLUME; i <= _SDSPI_PARTITION_4; i++) {
                if (sdspi_ctrl->part[i]) {
                        sdspi_ctrl->part[i]->size = 0;
                }
        }

        if (card_send_cmd(hdl, CMD9, 0) == 0) {
                u8_t csd[16];
                u8_t token;

                timer_t timer = timer_reset();
                while ((token = spi_transmit(0xFF)) == 0xFF && timer_is_not_expired(timer, SDSPI_TIMEOUT));

                if (token == 0xFE) {
                        u8_t *ptr = &csd[0];
                        for (int i = 0; i < 4; i++) {
                                *ptr++ = spi_transmit(0xFF);
                                *ptr++ = spi_transmit(0xFF);
                                *ptr++ = spi_transmit(0xFF);
                                *ptr++ = spi_transmit(0xFF);
                        }
                        spi_transmit(0xFF);
                        spi_transmit(0xFF);

                        /* SDC version 2.00 */
                        u32_t size;
                        if ((csd[0] >> 6) == 1) {
                                u32_t csize = csd[9] + ((u16_t)csd[8] << 8) + 1;
                                size        = csize << 10;
                        } else { /* SDC version 1.XX or MMC*/
                                u32_t n     = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                                u32_t csize = (csd[8] >> 6) + ((u16_t)csd[7] << 2) + ((u16_t)(csd[6] & 3) << 10) + 1;
                                size        = csize << (n - 9);
                        }

                        if (sdspi_ctrl->part[_SDSPI_FULL_VOLUME]) {
                                sdspi_ctrl->part[_SDSPI_FULL_VOLUME]->size = size;
                        }
                }
        }

        spi_deselect_card();
        spi_transmit(0xFF);

        if (sdspi_ctrl->card_initialized == false) {
                return STD_RET_ERROR;
        } else {
                return STD_RET_OK;
        }
}

//==============================================================================
/**
 * @brief Read data from card
 *
 * @param[in]  *hdl             driver's memory handle
 * @param[out] *dst             destination
 * @param[in]   count           bytes to read
 * @param[in]   lseek           file index
 *
 * @retval number of read bytes
 */
//==============================================================================
static ssize_t card_read(sdpart_t *hdl, u8_t *dst, size_t count, u64_t lseek)
{
        if (sdspi_ctrl->card_initialized == false) {
                errno = EIO;
                return -1;
        }

        /* whole sector(s) read */
        ssize_t n = 0;
        if ((count % sector_size == 0) && (lseek % sector_size == 0)) {

                n  = card_read_whole_sectors(hdl, dst, count / sector_size, lseek);
                n *= sector_size;

        } else {
                n  = card_read_partial_sectors(hdl, dst, count, lseek);
        }

        spi_deselect_card();
        spi_transmit(0xFF);

        return n;
}

//==============================================================================
/**
 * @brief Write data to card
 *
 * @param[in] *hdl              driver's memory handle
 * @param[in] *src              source
 * @param[in]  count            bytes to write
 * @param[in]  lseek            file index
 *
 * @retval number of written bytes
 */
//==============================================================================
static ssize_t card_write(sdpart_t *hdl, const u8_t *src, size_t count, u64_t lseek)
{
        if (sdspi_ctrl->card_initialized == false) {
                errno = EIO;
                return -1;
        }

        /* whole sector(s) read */
        ssize_t n = 0;
        if ((count % sector_size == 0) && (lseek % sector_size == 0)) {

                n  = card_write_whole_sectors(hdl, src, count / sector_size, lseek);
                n *= sector_size;

        } else {
                n  = card_write_partial_sectors(hdl, src, count, lseek);
        }

        spi_deselect_card();
        spi_transmit(0xFF);

        return n;
}

//==============================================================================
/**
 * @brief Function load 32b data from selected buffer and offset
 *
 * @param buff                  buffer
 * @param offset                variable offset
 *
 * @return 32-bit value
 */
//==============================================================================
static u32_t load_u32(u8_t *buff, u16_t offset)
{
        return (u32_t)( ((u32_t)buff[offset + 0] <<  0)
                      | ((u32_t)buff[offset + 1] <<  8)
                      | ((u32_t)buff[offset + 2] << 16)
                      | ((u32_t)buff[offset + 3] << 24) );
}

//==============================================================================
/**
 * @brief Function load 16b data from selected buffer and offset
 *
 * @param buff                  buffer
 * @param offset                variable offset
 *
 * @return 16-bit value
 */
//==============================================================================
static u16_t load_u16(u8_t *buff, u16_t offset)
{
        return (u16_t)(((u16_t)buff[offset + 0]) | ((u16_t)buff[offset + 1] << 8));
}

//==============================================================================
/**
 * @brief Function gets boot signature
 *
 * @param sector                buffer with sector
 *
 * @return boot signature
 */
//==============================================================================
static u16_t mbr_get_boot_signature(u8_t *sector)
{
     return load_u16(sector, MBR_BOOT_SIGNATURE_OFFSET);
}

//==============================================================================
/**
 * @brief Function return partition first LBA sector number
 *
 * @param partition             partition number (1 - 4)
 * @param sector                buffer with sector
 *
 * @return first LBA sector number
 */
//==============================================================================
static u32_t mbr_get_partition_first_LBA_sector(int partition, u8_t *sector)
{
        switch (partition) {
        case 1 : return load_u32(sector, MBR_PARTITION_1_ENTRY_OFFSET + MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET);
        case 2 : return load_u32(sector, MBR_PARTITION_2_ENTRY_OFFSET + MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET);
        case 3 : return load_u32(sector, MBR_PARTITION_3_ENTRY_OFFSET + MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET);
        case 4 : return load_u32(sector, MBR_PARTITION_4_ENTRY_OFFSET + MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET);
        default: return 0;
        }
}

//==============================================================================
/**
 * @brief Function return partition number of sectors
 *
 * @param partition             partition number (1 - 4)
 * @param sector                buffer with sector
 *
 * @return number of partition sectors
 */
//==============================================================================
static u32_t mbr_get_partition_number_of_sectors(int partition, u8_t *sector)
{
        switch (partition) {
        case 1 : return load_u32(sector, MBR_PARTITION_1_ENTRY_OFFSET + MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET);
        case 2 : return load_u32(sector, MBR_PARTITION_2_ENTRY_OFFSET + MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET);
        case 3 : return load_u32(sector, MBR_PARTITION_3_ENTRY_OFFSET + MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET);
        case 4 : return load_u32(sector, MBR_PARTITION_4_ENTRY_OFFSET + MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET);
        default: return 0;
        }


}

//==============================================================================
/**
 * @brief Function detect partitions
 *
 * @param[in] *hdl      SD module data
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t mbr_detect_partitions(sdpart_t *hdl)
{
        stdret_t status = STD_RET_ERROR;

        u8_t *MBR = malloc(sector_size);
        if (MBR) {
                if (card_read(hdl, MBR, sector_size, 0) != sector_size) {
                        goto error;
                }

                if (mbr_get_boot_signature(MBR) != 0xAA55) {
                        errno = EMEDIUMTYPE;
                        goto error;
                }

                for (int i = _SDSPI_PARTITION_1; i <= _SDSPI_PARTITION_4; i++) {
                        u32_t size = mbr_get_partition_number_of_sectors(i, MBR);
                        if (size > 0 && sdspi_ctrl->part[i]) {
                                sdspi_ctrl->part[i]->size         = size;
                                sdspi_ctrl->part[i]->first_sector = mbr_get_partition_first_LBA_sector(i, MBR);
                        }
                }

                status = STD_RET_OK;
error:
                free(MBR);
        }

        return status;
}

//==============================================================================
/**
 * @brief DMA operation finish interrupt
 */
//==============================================================================
#if (SDSPI_ENABLE_DMA != 0)
void SDSPI_DMA_IRQ_ROUTINE(void)
{
        spi_disable_Tx_Rx_DMA();
        SDSPI_DMA_RX_CHANNEL->CCR = 0x00;
        SDSPI_DMA_TX_CHANNEL->CCR = 0x00;

        NVIC_DisableIRQ(SDSPI_DMA_IRQ_NUMBER);

        SDSPI_DMA->IFCR = DMA_IFCR_CTCIF1 << (4 * (SDSPI_DMA_RX_CHANNEL_NO - 1));

        sdspi_ctrl->DMA_tansaction_finished = true;
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
