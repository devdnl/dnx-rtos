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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "system/dnxmodule.h"
#include "stm32f1/sdspi_cfg.h"
#include "stm32f1/sdspi_def.h"
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define select_SD_card()                                SDSPI_SD_SELECT
#define deselect_SD_card()                              SDSPI_SD_DESELECT

#define MTX_BLOCK_TIME                                  0
#define MTX_BLOCK_TIME_LONG                             200
#define force_lock_recursive_mutex(mtx)                 while (lock_recursive_mutex(mtx, 10) != MUTEX_LOCKED)

/* SPI configuration macros */
#define enable_bidirectional_data_mode()                SDSPI_PORT->CR1 |=  SPI_CR1_BIDIMODE
#define enable_unidirectional_data_mode()               SDSPI_PORT->CR1 &= ~SPI_CR1_BIDIMODE
#define enable_output_in_bidirectional_data_mode()      SDSPI_PORT->CR1 |=  SPI_CR1_BIDIOE
#define disable_output_in_bidirectional_data_mode()     SDSPI_PORT->CR1 &= ~SPI_CR1_BIDIOE
#define enable_hardware_CRC_calculation()               SDSPI_PORT->CR1 |=  SPI_CR1_CRCEN
#define disable_hardware_CRC_calculation()              SDSPI_PORT->CR1 &= ~SPI_CR1_CRCEN
#define next_trasfer_is_CRC()                           SDSPI_PORT->CR1 |=  SPI_CR1_CRCNEXT
#define next_trasfer_is_data()                          SDSPI_PORT->CR1 &= ~SPI_CR1_CRCNEXT
#define enable_8_bit_data_frame()                       SDSPI_PORT->CR1 &= ~SPI_CR1_DFF
#define enable_16_bit_data_frame()                      SDSPI_PORT->CR1 |=  SPI_CR1_DFF
#define enable_full_duplex()                            SDSPI_PORT->CR1 &= ~SPI_CR1_RXONLY
#define enable_receive_only()                           SDSPI_PORT->CR1 |=  SPI_CR1_RXONLY
#define enable_software_slave_management()              SDSPI_PORT->CR1 |=  SPI_CR1_SSM
#define enable_hardware_slave_management()              SDSPI_PORT->CR1 &= ~SPI_CR1_SSM
#define select_slave_internal()                         SDSPI_PORT->CR1 &= ~SPI_CR1_SSI
#define deselect_slave_internal()                       SDSPI_PORT->CR1 |=  SPI_CR1_SSI
#define transmit_MSB_first()                            SDSPI_PORT->CR1 &= ~SPI_CR1_LSBFIRST
#define transmit_LSB_first()                            SDSPI_PORT->CR1 |=  SPI_CR1_LSBFIRST
#define enable_SPI_peripheral()                         SDSPI_PORT->CR1 |=  SPI_CR1_SPE
#define disable_SPI_peripheral()                        SDSPI_PORT->CR1 &= ~SPI_CR1_SPE
#define FPCLK_DIV_2                                     (0)
#define FPCLK_DIV_4                                     (SPI_CR1_BR_0)
#define FPCLK_DIV_8                                     (SPI_CR1_BR_1)
#define FPCLK_DIV_16                                    (SPI_CR1_BR_1 | SPI_CR1_BR_0)
#define FPCLK_DIV_32                                    (SPI_CR1_BR_2)
#define FPCLK_DIV_64                                    (SPI_CR1_BR_2 | SPI_CR1_BR_0)
#define FPCLK_DIV_128                                   (SPI_CR1_BR_2 | SPI_CR1_BR_1)
#define FPCLK_DIV_256                                   (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0)
#define set_baud_rate(baud)                             do{SDSPI_PORT->CR1 &= ~SPI_CR1_BR; SDSPI_PORT->CR1 |= baud;}while(0)
#define enable_master_mode()                            SDSPI_PORT->CR1 |=  SPI_CR1_MSTR
#define enable_slave_mode()                             SDSPI_PORT->CR1 &= ~SPI_CR1_MSTR
#define set_clock_polarity_to_0_when_idle()             SDSPI_PORT->CR1 &= ~SPI_CR1_CPOL
#define set_clock_polarity_to_1_when_idle()             SDSPI_PORT->CR1 |=  SPI_CR1_CPOL
#define capture_on_first_edge()                         SDSPI_PORT->CR1 &= ~SPI_CR1_CPHA
#define capture_on_second_edge()                        SDSPI_PORT->CR1 |=  SPI_CR1_CPHA

#define enable_ISR_when_Tx_buffer_empty()               SDSPI_PORT->CR2 |=  SPI_CR2_TXEIE
#define disbale_ISR_when_Tx_buffer_empty()              SDSPI_PORT->CR2 &= ~SPI_CR2_TXEIE
#define enable_ISR_when_Rx_buffer_not_empty()           SDSPI_PORT->CR2 |=  SPI_CR2_RXNEIE
#define disbale_ISR_when_Rx_buffer_not_empty()          SDSPI_PORT->CR2 &= ~SPI_CR2_RXNEIE
#define enable_ISR_when_error()                         SDSPI_PORT->CR2 |=  SPI_CR2_ERRIE
#define disbale_ISR_when_error()                        SDSPI_PORT->CR2 &= ~SPI_CR2_ERRIE
#define enable_SS_output_in_master_mode()               SDSPI_PORT->CR2 |=  SPI_CR2_SSOE
#define disable_SS_output_in_master_mode()              SDSPI_PORT->CR2 &= ~SPI_CR2_SSOE
#define enable_Tx_Rx_DMA()                              SDSPI_PORT->CR2 |=  SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN
#define disable_Tx_Rx_DMA()                             SDSPI_PORT->CR2 &= ~(SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN)

#define is_busy()                                       (SDSPI_PORT->SR & SPI_SR_BSY)
#define is_overrun()                                    (SDSPI_PORT->SR & SPI_SR_OVR)
#define is_mode_fault()                                 (SDSPI_PORT->SR & SPI_SR_MODF)
#define is_CRC_error()                                  (SDSPI_PORT->SR & SPI_SR_CRCERR)
#define is_Tx_buffer_empty()                            (SDSPI_PORT->SR & SPI_SR_TXE)
#define is_Rx_buffer_not_empty()                        (SDSPI_PORT->SR & SPI_SR_RXNE)

#define send_data(data)                                 SDSPI_PORT->DR = data
#define get_data()                                      SDSPI_PORT->DR
#define set_CRC_polynominal(poly)                       SDSPI_PORT->CRCPR = poly
#define get_Rx_CRC()                                    SDSPI_PORT->RXCRCR
#define get_Tx_CRC()                                    SDSPI_PORT->TXCRCR

/* card definitions */
#define CMD0                                            (0x40+0 )       /* GO_IDLE_STATE */
#define CMD1                                            (0x40+1 )       /* SEND_OP_COND (MMC) */
#define ACMD41                                          (0xC0+41)       /* SEND_OP_COND (SDC) */
#define CMD8                                            (0x40+8 )       /* SEND_IF_COND */
#define CMD9                                            (0x40+9 )       /* SEND_CSD */
#define CMD10                                           (0x40+10)       /* SEND_CID */
#define CMD12                                           (0x40+12)       /* STOP_TRANSMISSION */
#define ACMD13                                          (0xC0+13)       /* SD_STATUS (SDC) */
#define CMD16                                           (0x40+16)       /* SET_BLOCKLEN */
#define CMD17                                           (0x40+17)       /* READ_SINGLE_BLOCK */
#define CMD18                                           (0x40+18)       /* READ_MULTIPLE_BLOCK */
#define CMD23                                           (0x40+23)       /* SET_BLOCK_COUNT (MMC) */
#define ACMD23                                          (0xC0+23)       /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24                                           (0x40+24)       /* WRITE_BLOCK */
#define CMD25                                           (0x40+25)       /* WRITE_MULTIPLE_BLOCK */
#define CMD55                                           (0x40+55)       /* APP_CMD */
#define CMD58                                           (0x40+58)       /* READ_OCR */

#define CT_MMC                                          (1 << 0)
#define CT_SD1                                          (1 << 1)
#define CT_SD2                                          (1 << 2)
#define CT_SDC                                          (CT_SD1|CT_SD2)
#define CT_BLOCK                                        (1 << 3)

#define SECTOR_SIZE                                     512

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

#define LOAD_U32(buff, offset)                          (u32_t)(((u32_t)buff[offset + 0]) | ((u32_t)buff[offset + 1] << 8) | ((u32_t)buff[offset + 2] << 16) | ((u32_t)buff[offset + 3] << 24))
#define LOAD_U16(buff, offset)                          (u16_t)(((u16_t)buff[offset + 0]) | ((u16_t)buff[offset + 1] << 8))

#define MBR_GET_BOOT_SIGNATURE(sector)                  LOAD_U16(sector, MBR_BOOT_SIGNATURE_OFFSET)
#define MBR_GET_PARTITION_1_LBA_FIRST_SECTOR(sector)    LOAD_U32(sector, MBR_PARTITION_1_ENTRY_OFFSET + MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET)
#define MBR_GET_PARTITION_2_LBA_FIRST_SECTOR(sector)    LOAD_U32(sector, MBR_PARTITION_2_ENTRY_OFFSET + MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET)
#define MBR_GET_PARTITION_3_LBA_FIRST_SECTOR(sector)    LOAD_U32(sector, MBR_PARTITION_3_ENTRY_OFFSET + MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET)
#define MBR_GET_PARTITION_4_LBA_FIRST_SECTOR(sector)    LOAD_U32(sector, MBR_PARTITION_4_ENTRY_OFFSET + MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET)
#define MBR_GET_PARTITION_1_NUMBER_OF_SECTORS(sector)   LOAD_U32(sector, MBR_PARTITION_1_ENTRY_OFFSET + MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET)
#define MBR_GET_PARTITION_2_NUMBER_OF_SECTORS(sector)   LOAD_U32(sector, MBR_PARTITION_2_ENTRY_OFFSET + MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET)
#define MBR_GET_PARTITION_3_NUMBER_OF_SECTORS(sector)   LOAD_U32(sector, MBR_PARTITION_3_ENTRY_OFFSET + MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET)
#define MBR_GET_PARTITION_4_NUMBER_OF_SECTORS(sector)   LOAD_U32(sector, MBR_PARTITION_4_ENTRY_OFFSET + MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/* partition description */
struct partition {
        u32_t    first_sector;
        u32_t    size_in_sectors;
        bool     in_use;
};

/** card types */
typedef u8_t card_type;

/** handling structure */
struct sdspi_data {
        mutex_t         *card_protect_mtx;
        card_type        card_type;
        bool             card_initialized;
        struct partition partition[4];
#if (SDSPI_ENABLE_DMA != 0)
        volatile bool    DMA_tansaction_finished;
#endif
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t         turn_on_SPI_clock       (void);
static stdret_t         turn_off_SPI_clock      (void);
static u8_t             send_cmd                (struct sdspi_data *sdspi, u8_t cmd, u32_t arg);
static u8_t             wait_ready              (void);
static u8_t             spi_rw                  (u8_t out);
static bool             receive_data_block      (u8_t *buff);
static bool             transmit_data_block     (const u8_t *buff, u8_t token);
static size_t           read_whole_sectors      (struct sdspi_data *hdl, void *dst, size_t nsectors, u64_t lseek);
static size_t           read_partial_sectors    (struct sdspi_data *hdl, void *dst, size_t size, u64_t lseek);
static size_t           write_whole_sectors     (struct sdspi_data *hdl, const void *src, size_t nsectors, u64_t lseek);
static size_t           write_partial_sectors   (struct sdspi_data *hdl, const void *src, size_t size, u64_t lseek);
static stdret_t         initialize_card         (struct sdspi_data *hdl);
static stdret_t         detect_partitions       (struct sdspi_data *hdl);
static size_t           card_read               (struct sdspi_data *hdl, u8_t *dst, size_t count, u64_t lseek);
static size_t           card_write              (struct sdspi_data *hdl, const u8_t *src, size_t count, u64_t lseek);

/*==============================================================================
  Local object definitions
==============================================================================*/
static struct sdspi_data *sdspi_data;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device
 */
//==============================================================================
API_MOD_INIT(SDSPI, void **device_handle, u8_t major, u8_t minor)
{
        STOP_IF(device_handle == NULL);

        if (major != SDSPI_MAJOR_NUMBER || minor != SDSPI_MINOR_NUMBER) {
                return STD_RET_ERROR;
        }

        struct sdspi_data *sdspi;
        if (!(sdspi = calloc(1, sizeof(struct sdspi_data)))) {
                return STD_RET_ERROR;
        }

        *device_handle = sdspi;
        sdspi_data     = sdspi;

#if (SDSPI_ENABLE_DMA != 0)
        if ((u32_t)SDSPI_DMA == DMA1_BASE) {
                RCC->AHBENR |= RCC_AHBENR_DMA1EN;
        } else if ((u32_t)SDSPI_DMA == DMA2_BASE) {
                RCC->AHBENR |= RCC_AHBENR_DMA2EN;
        }

        NVIC_SetPriority(SDSPI_DMA_IRQ_NUMBER, SDSPI_DMA_IRQ_PRIORITY);
#endif

        if (!(sdspi->card_protect_mtx = new_mutex())) {
                goto error;
        }

        /* initialize SPI interface */
        if (turn_on_SPI_clock() != STD_RET_OK) {
                goto error;
        }

        enable_SS_output_in_master_mode();
        enable_unidirectional_data_mode();
        disable_hardware_CRC_calculation();
        enable_8_bit_data_frame();
        enable_software_slave_management();
        transmit_MSB_first();

#if   (SDSPI_PERIPHERAL_DIVIDER <=   2)
        set_baud_rate(FPCLK_DIV_2);
#elif (SDSPI_PERIPHERAL_DIVIDER <=   4)
        set_baud_rate(FPCLK_DIV_4);
#elif (SDSPI_PERIPHERAL_DIVIDER <=   8)
        set_baud_rate(FPCLK_DIV_8);
#elif (SDSPI_PERIPHERAL_DIVIDER <=  16)
        set_baud_rate(FPCLK_DIV_16);
#elif (SDSPI_PERIPHERAL_DIVIDER <=  32)
        set_baud_rate(FPCLK_DIV_32);
#elif (SDSPI_PERIPHERAL_DIVIDER <=  64)
        set_baud_rate(FPCLK_DIV_64);
#elif (SDSPI_PERIPHERAL_DIVIDER <= 128)
        set_baud_rate(FPCLK_DIV_128);
#else
        set_baud_rate(FPCLK_DIV_256);
#endif

        enable_master_mode();
        set_clock_polarity_to_0_when_idle();
        capture_on_first_edge();
        enable_SPI_peripheral();

        return STD_RET_OK;

error:
        if (sdspi) {
                if (sdspi->card_protect_mtx) {
                        delete_mutex(sdspi->card_protect_mtx);
                }

                free(sdspi);
        }
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release device
 */
//==============================================================================
API_MOD_RELEASE(SDSPI, void *device_handle)
{
        STOP_IF(device_handle == NULL);

        struct sdspi_data *hdl = device_handle;

        /* wait for all partition are released */
        int timeout = 50;
        while (  hdl->partition[0].in_use == true
              || hdl->partition[1].in_use == true
              || hdl->partition[2].in_use == true
              || hdl->partition[3].in_use == true) {

                if (--timeout == 0)
                        return STD_RET_ERROR;

                sleep_ms(100);
        }

        lock_mutex(hdl->card_protect_mtx, MAX_DELAY);
        enter_critical_section();
        unlock_mutex(hdl->card_protect_mtx);
        delete_mutex(hdl->card_protect_mtx);
        turn_off_SPI_clock();
        free(hdl);
        exit_critical_section();

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Open device
 */
//==============================================================================
API_MOD_OPEN(SDSPI, void *device_handle, int flags)
{
        UNUSED_ARG(flags);
        STOP_IF(device_handle == NULL);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Close device
 */
//==============================================================================
API_MOD_CLOSE(SDSPI, void *device_handle, bool force, task_t *opened_by_task)
{
        UNUSED_ARG(force);
        UNUSED_ARG(opened_by_task);

        STOP_IF(device_handle == NULL);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Write data to device
 */
//==============================================================================
API_MOD_WRITE(SDSPI, void *device_handle, const u8_t *src, size_t count, u64_t *fpos)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(src == NULL);
        STOP_IF(count == 0);
        STOP_IF(fpos == NULL);

        struct sdspi_data *hdl = device_handle;

        size_t n = 0;
        if (lock_mutex(hdl->card_protect_mtx, MAX_DELAY) == MUTEX_LOCKED) {
                n = card_write(hdl, src, count, *fpos);
                unlock_mutex(hdl->card_protect_mtx);
        }

        return n;
}

//==============================================================================
/**
 * @brief Read data from device
 */
//==============================================================================
API_MOD_READ(SDSPI, void *device_handle, u8_t *dst, size_t count, u64_t *fpos)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(dst == NULL);
        STOP_IF(count == 0);
        STOP_IF(fpos == NULL);

        struct sdspi_data *hdl = device_handle;

        size_t n = 0;
        if (lock_mutex(hdl->card_protect_mtx, MAX_DELAY) == MUTEX_LOCKED) {
                n = card_read(hdl, dst, count, *fpos);
                unlock_mutex(hdl->card_protect_mtx);
        }

        return n;
}

//==============================================================================
/**
 * @brief Direct IO control
 */
//==============================================================================
API_MOD_IOCTL(SDSPI, void *device_handle, int iorq, void *arg)
{
        STOP_IF(device_handle == NULL);

        struct sdspi_data *hdl = device_handle;

        stdret_t status = STD_RET_OK;

        switch (iorq) {
        case SDSPI_IORQ_INITIALIZE_CARD:
                if (lock_mutex(hdl->card_protect_mtx, MTX_BLOCK_TIME) == MUTEX_LOCKED) {
                        bool *result = arg;
                        *result      = false;

                        vfs_remove(SDSPI_PARTITION_1_PATH);
                        vfs_remove(SDSPI_PARTITION_2_PATH);
                        vfs_remove(SDSPI_PARTITION_3_PATH);
                        vfs_remove(SDSPI_PARTITION_4_PATH);

                        if (initialize_card(hdl) == STD_RET_OK) {
                                if (detect_partitions(hdl) == STD_RET_OK) {
                                        *result = true;
                                }
                        }

                        unlock_mutex(hdl->card_protect_mtx);
                } else {
                        status = STD_RET_ERROR;
                }
                break;

        default:
                status = STD_RET_ERROR;
                break;
        }

        return status;
}

//==============================================================================
/**
 * @brief Flush device
 */
//==============================================================================
API_MOD_FLUSH(SDSPI, void *device_handle)
{
        STOP_IF(device_handle == NULL);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Interface returns device information
 */
//==============================================================================
API_MOD_STAT(SDSPI, void *device_handle, struct vfs_dev_stat *device_stat)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(device_stat == NULL);

        struct sdspi_data *hdl = device_handle;

        if (lock_mutex(hdl->card_protect_mtx, MTX_BLOCK_TIME_LONG) == MUTEX_LOCKED) {
                /* size info */
                if (send_cmd(hdl, CMD9, 0) == 0) {
                        u8_t  csd[16];
                        u8_t  token;
                        uint  try = 1000000;

                        while ((token = spi_rw(0xFF)) == 0xFF && --try);
                        if (token != 0xFE)
                                return STD_RET_ERROR;

                        u8_t *ptr = &csd[0];
                        for (int i = 0; i < 4; i++) {
                                *ptr++ = spi_rw(0xFF);
                                *ptr++ = spi_rw(0xFF);
                                *ptr++ = spi_rw(0xFF);
                                *ptr++ = spi_rw(0xFF);
                        }
                        spi_rw(0xFF);
                        spi_rw(0xFF);

                        /* SDC version 2.00 */
                        device_stat->st_size = 0;
                        if ((csd[0] >> 6) == 1) {
                                int csize     = csd[9] + ((u16_t)csd[8] << 8) + 1;
                                device_stat->st_size = (u64_t)csize << 10;
                        } else { /* SDC version 1.XX or MMC*/
                                int n     = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                                int csize = (csd[8] >> 6) + ((u16_t)csd[7] << 2) + ((u16_t)(csd[6] & 3) << 10) + 1;
                                device_stat->st_size = (u64_t)csize << (n - 9);
                        }
                        device_stat->st_size *= SECTOR_SIZE;
                        device_stat->st_major = 0;
                        device_stat->st_minor = 0;
                }

                unlock_mutex(hdl->card_protect_mtx);
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function open new partition file
 *
 * @param[in] *device_handle    handle to partition description
 * @param[in]  flags            file open flags
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t partition_open(void *device_handle, int flags)
{
        UNUSED_ARG(flags);
        STOP_IF(device_handle == NULL);

        struct partition *hdl = device_handle;

        if (hdl->in_use == true)
                return STD_RET_ERROR;
        else
                return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function close partition file
 *
 * @param[in] *device_handle    handle to partition description
 * @param[in]  forced           force close
 * @param[in] *task             task which open file, valid if forced = true
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t partition_close(void *device_handle, bool forced, task_t *task)
{
        UNUSED_ARG(forced);
        UNUSED_ARG(task);
        STOP_IF(device_handle == NULL);

        struct partition *hdl = device_handle;

        hdl->in_use = false;
        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function write data to partition file
 *
 * @param[in] *device_handle    handle to partition description
 * @param[in] *src              source
 * @param[in]  size             item size
 * @param[in]  count            bytes to write
 * @param[in] *fpos             file index
 *
 * @retval number of written bytes
 */
//==============================================================================
static size_t partition_write(void *device_handle, const u8_t *src, size_t count, u64_t *fpos)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(src == NULL);
        STOP_IF(count == 0);
        STOP_IF(fpos == NULL);

        struct partition *hdl = device_handle;

        size_t n = 0;
        if (lock_mutex(sdspi_data->card_protect_mtx, MAX_DELAY) == MUTEX_LOCKED) {
                n = card_write(sdspi_data, src, count, *fpos + ((u64_t)hdl->first_sector * SECTOR_SIZE));
                unlock_mutex(sdspi_data->card_protect_mtx);
        }

        return n;
}

//==============================================================================
/**
 * @brief Function read data from partition file
 *
 * @param[in]  *device_handle   handle to partition description
 * @param[out] *dst             destination
 * @param[in]   count           bytes to read
 * @param[in]  *fpos            file index
 *
 * @retval number of read bytes
 */
//==============================================================================
static size_t partition_read(void *device_handle, u8_t *dst, size_t count, u64_t *fpos)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(dst == NULL);
        STOP_IF(count == 0);
        STOP_IF(fpos == NULL);

        struct partition *hdl = device_handle;

        size_t n = 0;
        if (lock_mutex(sdspi_data->card_protect_mtx, MAX_DELAY) == MUTEX_LOCKED) {
                n = card_read(sdspi_data, dst, count, *fpos + ((u64_t)hdl->first_sector * SECTOR_SIZE));
                unlock_mutex(sdspi_data->card_protect_mtx);
        }

        return n;
}

//==============================================================================
/**
 * @brief Function control partition
 *
 * @param[in]    *device_handle         handle to partition description
 * @param[in]     iorq                  IO request
 * @param[in,out]*arg                   request's argument
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t partition_ioctl(void *device_handle, int iorq, void *arg)
{
        STOP_IF(device_handle == NULL);
        UNUSED_ARG(iorq);
        UNUSED_ARG(arg);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function flush partition
 *
 * @param[in] *device_handle           handle to partition description
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t partition_flush(void *device_handle)
{
        STOP_IF(device_handle == NULL);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function returns device informations
 *
 * @param[in]  *device_handle   driver's memory handle
 * @param[out] *info            device/file info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t partition_stat(void *device_handle, struct vfs_dev_stat *stat)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(stat == NULL);

        struct partition *hdl = device_handle;
        stat->st_size  = (u64_t)hdl->size_in_sectors * SECTOR_SIZE;
        stat->st_major = 0;
        stat->st_minor = 0;
        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function turn on SPI clock
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t turn_on_SPI_clock(void)
{
        switch ((u32_t)SDSPI_PORT) {
        #if defined(RCC_APB2ENR_SPI1EN)
        case SPI1_BASE:
                RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
                return STD_RET_OK;
        #endif
        #if defined(RCC_APB1ENR_SPI2EN)
        case SPI2_BASE:
                RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
                return STD_RET_OK;
        #endif
        #if defined(RCC_APB1ENR_SPI3EN)
        case SPI3_BASE:
                RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
                return STD_RET_OK;
        #endif
        default:
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
static stdret_t turn_off_SPI_clock(void)
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
                return STD_RET_ERROR;
        }
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
static u8_t spi_rw(u8_t out)
{
        send_data(out);
        while (!is_Rx_buffer_not_empty());
        return get_data();
}

//==============================================================================
/**
 * @brief Function wait for card ready
 *
 * @return card response
 */
//==============================================================================
static u8_t wait_ready(void)
{
        int  try = 1000000;
        u8_t response;

        spi_rw(0xFF);

        while ((response = spi_rw(0xFF)) != 0xFF && --try);

        return response;
}

//==============================================================================
/**
 * @brief Function transmit command to card
 *
 * @param[in] *sdspi    SD SPI interface data
 * @param[in] cmd       card command
 * @param[in] arg       command's argument
 */
//==============================================================================
static u8_t send_cmd(struct sdspi_data *sdspi, u8_t cmd, u32_t arg)
{
        u8_t response;

        /* ACMD<n> is the command sequence of CMD55-CMD<n> */
        if (cmd & 0x80) {
                cmd &= 0x7F;
                response = send_cmd(sdspi, CMD55, 0);
                if (response > 1)
                        return response;
        }

        /* select the card and wait for ready */
        deselect_SD_card();
        select_SD_card();

        if (wait_ready() != 0xFF) {
                return 0xFF;
        }

        /* send command packet */
        spi_rw(cmd);                    /* Start + Command index */
        spi_rw(arg >> 24);              /* Argument[31..24]      */
        spi_rw(arg >> 16);              /* Argument[23..16]      */
        spi_rw(arg >> 8);               /* Argument[15..8]       */
        spi_rw(arg);

        if (cmd == CMD0)
                spi_rw(0x95);           /* Valid CRC for CMD0(0) */
        else if (cmd == CMD8)
                spi_rw(0x87);           /* Valid CRC for CMD8(0x1AA) */
        else
                spi_rw(0x01);           /* Dummy CRC + Stop */

        /* receive command response */
        if (cmd == CMD12)
                spi_rw(0xFF);           /* Skip a stuff byte when stop reading */

        /* wait for a valid response in timeout of 10 attempts */
        int n = 10;
        do {
                response = spi_rw(0xFF);
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
static bool receive_data_block(u8_t *buff)
{
        u8_t token;
        uint try = 1000000;

        while ((token = spi_rw(0xFF)) == 0xFF && --try);

        if (token != 0xFE)
                return false;

#if (SDSPI_ENABLE_DMA != 0)
        SDSPI_DMA_RX_CHANNEL->CPAR  = (u32_t)&SDSPI_PORT->DR;
        SDSPI_DMA_RX_CHANNEL->CMAR  = (u32_t)buff;
        SDSPI_DMA_RX_CHANNEL->CNDTR = SECTOR_SIZE;
        SDSPI_DMA_RX_CHANNEL->CCR   = DMA_CCR1_MINC | DMA_CCR1_TCIE | DMA_CCR1_EN;
        NVIC_EnableIRQ(SDSPI_DMA_IRQ_NUMBER);

        u16_t dummy = 0xFFFF;
        SDSPI_DMA_TX_CHANNEL->CPAR  = (u32_t)&SDSPI_PORT->DR;
        SDSPI_DMA_TX_CHANNEL->CMAR  = (u32_t)&dummy;
        SDSPI_DMA_TX_CHANNEL->CNDTR = SECTOR_SIZE;
        SDSPI_DMA_TX_CHANNEL->CCR   = DMA_CCR1_DIR | DMA_CCR1_EN;

        sdspi_data->DMA_tansaction_finished = false;

        enable_Tx_Rx_DMA();

        while (sdspi_data->DMA_tansaction_finished == false);

#else
        /* memory alignment */
        int size = SECTOR_SIZE;
        do {
                *buff++ = spi_rw(0xFF);
                *buff++ = spi_rw(0xFF);
                *buff++ = spi_rw(0xFF);
                *buff++ = spi_rw(0xFF);
        } while (size -= 4);
#endif

        /* discard CRC */
        spi_rw(0xFF);
        spi_rw(0xFF);

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
static bool transmit_data_block(const u8_t *buff, u8_t token)
{
        if (wait_ready() != 0xFF)
                return false;

        spi_rw(token);

        if (token != 0xFD) {
#if (SDSPI_ENABLE_DMA != 0)
                u16_t dummy;
                SDSPI_DMA_RX_CHANNEL->CMAR  = (u32_t)&dummy;
                SDSPI_DMA_RX_CHANNEL->CPAR  = (u32_t)&SDSPI_PORT->DR;
                SDSPI_DMA_RX_CHANNEL->CNDTR = SECTOR_SIZE;
                SDSPI_DMA_RX_CHANNEL->CCR   = DMA_CCR1_TCIE | DMA_CCR1_EN;
                NVIC_EnableIRQ(SDSPI_DMA_IRQ_NUMBER);

                SDSPI_DMA_TX_CHANNEL->CMAR  = (u32_t)buff;
                SDSPI_DMA_TX_CHANNEL->CPAR  = (u32_t)&SDSPI_PORT->DR;
                SDSPI_DMA_TX_CHANNEL->CNDTR = SECTOR_SIZE;
                SDSPI_DMA_TX_CHANNEL->CCR   = DMA_CCR1_MINC | DMA_CCR1_DIR | DMA_CCR1_EN;

                sdspi_data->DMA_tansaction_finished = false;

                enable_Tx_Rx_DMA();

                while (sdspi_data->DMA_tansaction_finished == false);

#else
                int size = SECTOR_SIZE;
                do {
                        spi_rw(*buff++);
                        spi_rw(*buff++);
                        spi_rw(*buff++);
                        spi_rw(*buff++);
                } while (size -= 4);
#endif

                /* CRC dummy */
                spi_rw(0xFF);
                spi_rw(0xFF);

                /* receive data response */
                if ((spi_rw(0xFF) & 0x1F) != 0x05)
                        return false;
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
static size_t read_whole_sectors(struct sdspi_data *hdl, void *dst, size_t nsectors, u64_t lseek)
{
        size_t n = 0;

        if (hdl->card_type & CT_BLOCK) {
                lseek >>= 9;    /* divide by 512 */
        }

        /* 1 sector to read */
        if (nsectors == 1) {
                if (send_cmd(hdl, CMD17, (u32_t)lseek) == 0) {
                        if (receive_data_block(dst)) {
                                n = 1;
                        }
                }
        } else {
                if (send_cmd(hdl, CMD18, (u32_t)lseek) == 0) {
                        do {
                                if (!receive_data_block(dst)) {
                                        break;
                                }

                                dst += SECTOR_SIZE;
                        } while (++n < nsectors);

                        /* stop transmission */
                        send_cmd(hdl, CMD12, 0);
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
static size_t read_partial_sectors(struct sdspi_data *hdl, void *dst, size_t size, u64_t lseek)
{
        u8_t *buffer = malloc(SECTOR_SIZE);
        if (!buffer)
                return 0;

        u32_t recv_data  = 0;
        while (recv_data < size) {
                if (lseek % SECTOR_SIZE == 0 && (size - recv_data) / SECTOR_SIZE > 0) {
                        size_t n = read_whole_sectors(hdl, dst, size / SECTOR_SIZE, lseek);
                        if (n != size / SECTOR_SIZE)
                                break;

                        dst       += n * SECTOR_SIZE;
                        lseek     += n * SECTOR_SIZE;
                        recv_data += n * SECTOR_SIZE;
                } else {
                        if (read_whole_sectors(hdl, buffer, 1, lseek & ~(0x1FF)) != 1)
                                break;

                        u32_t rest;
                        if ((SECTOR_SIZE - (lseek % SECTOR_SIZE)) > (size - recv_data))
                                rest = size - recv_data;
                        else
                                rest = SECTOR_SIZE - (lseek % SECTOR_SIZE);

                        memcpy(dst, buffer + (lseek % SECTOR_SIZE), rest);
                        dst       += rest;
                        recv_data += rest;
                        lseek     += rest;
                }
        }

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
static size_t write_whole_sectors(struct sdspi_data *hdl, const void *src, size_t nsectors, u64_t lseek)
{
        size_t n = 0;

        if (hdl->card_type & CT_BLOCK) {
                lseek >>= 9;    /* divide by 512 */
        }

        /* 1 sector to read */
        if (nsectors == 1) {
                if (send_cmd(hdl, CMD24, (u32_t)lseek) == 0) {
                        if (transmit_data_block(src, 0xFE)) {
                                n = 1;
                        }
                }
        } else {
                if (hdl->card_type & CT_SDC) {
                        send_cmd(hdl, ACMD23, nsectors);
                }

                if (send_cmd(hdl, CMD25, (u32_t)lseek) == 0) {
                        do {
                                if (!transmit_data_block(src, 0xFC)) {
                                        break;
                                }

                                src += SECTOR_SIZE;
                        } while (++n < nsectors);

                        /* stop transmission */
                        if (!transmit_data_block(NULL, 0xFD)) {
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
static size_t write_partial_sectors(struct sdspi_data *hdl, const void *src, size_t size, u64_t lseek)
{
        u8_t *buffer = malloc(SECTOR_SIZE);
        if (!buffer)
                return 0;

        u32_t transmit_data = 0;
        while (transmit_data < size) {
                if (lseek % SECTOR_SIZE == 0 && (size - transmit_data) / SECTOR_SIZE > 0) {
                        size_t n = write_whole_sectors(hdl, src, size / SECTOR_SIZE, lseek);
                        if (n != size / SECTOR_SIZE)
                                break;

                        src           += n * SECTOR_SIZE;
                        lseek         += n * SECTOR_SIZE;
                        transmit_data += n * SECTOR_SIZE;
                } else {
                        if (read_whole_sectors(hdl, buffer, 1, lseek & ~(0x1FF)) != 1)
                                break;

                        u32_t rest;
                        if ((SECTOR_SIZE - (lseek % SECTOR_SIZE)) > (size - transmit_data))
                                rest = size - transmit_data;
                        else
                                rest = SECTOR_SIZE - (lseek % SECTOR_SIZE);

                        memcpy(buffer + (lseek % SECTOR_SIZE), src, rest);

                        if (write_whole_sectors(hdl, buffer, 1, lseek & ~(0x1FF)) != 1)
                                break;

                        src           += rest;
                        transmit_data += rest;
                        lseek         += rest;
                }
        }

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
static stdret_t initialize_card(struct sdspi_data *hdl)
{
        deselect_SD_card();
        for (int n = 0; n < 10; n++) {
                spi_rw(0xFF);
        }

        int  timeout = 2 * SDSPI_WAIT_TIMEOUT;
        u8_t cmd;
        u8_t OCR[4];

        hdl->card_type = 0;
        hdl->card_initialized = false;

        if (send_cmd(hdl, CMD0, 0) == 0x01) {
                if (send_cmd(hdl, CMD8, 0x1AA) == 0x01) { /* check SDHC card */
                        for (int n = 0; n < 4; n++) {
                                OCR[n] = spi_rw(0xFF);
                        }

                        if (OCR[2] == 0x01 && OCR[3] == 0xAA) {
                                while (--timeout && send_cmd(hdl, ACMD41, 1UL << 30)) {
                                        sleep_ms(1);
                                }

                                if (timeout && send_cmd(hdl, CMD58, 0) == 0) {
                                        for (int n = 0; n < 4; n++) {
                                                OCR[n] = spi_rw(0xFF);
                                        }

                                        hdl->card_type = (OCR[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
                                }
                        }
                } else { /* SDSC or MMC */
                        if (send_cmd(hdl, ACMD41, 0) <= 0x01)   {
                                hdl->card_type = CT_SD1;
                                cmd = ACMD41;   /* SDSC */
                        } else {
                                hdl->card_type = CT_MMC;
                                cmd = CMD1;     /* MMC */
                        }

                        /* Wait for leaving idle state */
                        while (timeout && send_cmd(hdl, cmd, 0)) {
                                timeout--;
                                sleep_ms(1);
                        }

                        /* set R/W block length to 512 */
                        if (!timeout || send_cmd(hdl, CMD16, SECTOR_SIZE) != 0) {
                                hdl->card_type = 0;
                        }
                }

                if (timeout) {
                        hdl->card_initialized = true;
                }
        }

        deselect_SD_card();
        spi_rw(0xFF);

        if (hdl->card_initialized == false) {
                return STD_RET_ERROR;
        } else {
                return STD_RET_OK;
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
static stdret_t detect_partitions(struct sdspi_data *hdl)
{
        stdret_t status = STD_RET_ERROR;

        u8_t *MBR = malloc(SECTOR_SIZE);

        if (MBR) {
                if (card_read(hdl, MBR, SECTOR_SIZE, 0) != SECTOR_SIZE) {
                        goto error;
                }

                if (MBR_GET_BOOT_SIGNATURE(MBR) != 0xAA55) {
                        goto error;
                }

                struct vfs_drv_interface drvif;
                drvif.drv_open  = partition_open;
                drvif.drv_close = partition_close;
                drvif.drv_write = partition_write;
                drvif.drv_read  = partition_read;
                drvif.drv_ioctl = partition_ioctl;
                drvif.drv_flush = partition_flush;
                drvif.drv_stat  = partition_stat;

                if (MBR_GET_PARTITION_1_NUMBER_OF_SECTORS(MBR) > 0) {
                        hdl->partition[0].first_sector    = MBR_GET_PARTITION_1_LBA_FIRST_SECTOR(MBR);
                        hdl->partition[0].size_in_sectors = MBR_GET_PARTITION_1_NUMBER_OF_SECTORS(MBR);
                        drvif.handle                      = &hdl->partition[0];
                        vfs_mknod(SDSPI_PARTITION_1_PATH, &drvif);
                }

                if (MBR_GET_PARTITION_2_NUMBER_OF_SECTORS(MBR) > 0) {
                        hdl->partition[1].first_sector    = MBR_GET_PARTITION_2_LBA_FIRST_SECTOR(MBR);
                        hdl->partition[1].size_in_sectors = MBR_GET_PARTITION_2_NUMBER_OF_SECTORS(MBR);
                        drvif.handle                      = &hdl->partition[1];
                        vfs_mknod(SDSPI_PARTITION_2_PATH, &drvif);
                }

                if (MBR_GET_PARTITION_3_NUMBER_OF_SECTORS(MBR) > 0) {
                        hdl->partition[2].first_sector    = MBR_GET_PARTITION_3_LBA_FIRST_SECTOR(MBR);
                        hdl->partition[2].size_in_sectors = MBR_GET_PARTITION_3_NUMBER_OF_SECTORS(MBR);
                        drvif.handle                      = &hdl->partition[2];
                        vfs_mknod(SDSPI_PARTITION_3_PATH, &drvif);
                }

                if (MBR_GET_PARTITION_4_NUMBER_OF_SECTORS(MBR) > 0) {
                        hdl->partition[3].first_sector    = MBR_GET_PARTITION_4_LBA_FIRST_SECTOR(MBR);
                        hdl->partition[3].size_in_sectors = MBR_GET_PARTITION_4_NUMBER_OF_SECTORS(MBR);
                        drvif.handle                      = &hdl->partition[3];
                        vfs_mknod(SDSPI_PARTITION_4_PATH, &drvif);
                }

                status = STD_RET_OK;
error:
                free(MBR);
        }

        return status;
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
static size_t card_read(struct sdspi_data *hdl, u8_t *dst, size_t count, u64_t lseek)
{
        size_t n = 0;

        if (hdl->card_initialized == false) {
                return 0;
        }

        /* whole sector(s) read */
        if ((count % SECTOR_SIZE == 0) && (lseek % SECTOR_SIZE == 0)) {

                n  = read_whole_sectors(hdl, dst, count / SECTOR_SIZE, lseek);
                n *= SECTOR_SIZE;

        } else {
                n  = read_partial_sectors(hdl, dst, count, lseek);
        }

        deselect_SD_card();
        spi_rw(0xFF);
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
static size_t card_write(struct sdspi_data *hdl, const u8_t *src, size_t count, u64_t lseek)
{
        size_t n = 0;

        if (hdl->card_initialized == false) {
                return 0;
        }

        /* whole sector(s) read */
        if ((count % SECTOR_SIZE == 0) && (lseek % SECTOR_SIZE == 0)) {

                n  = write_whole_sectors(hdl, src, count / SECTOR_SIZE, lseek);
                n *= SECTOR_SIZE;

        } else {
                n  = write_partial_sectors(hdl, src, count, lseek);
        }

        deselect_SD_card();
        spi_rw(0xFF);
        return n;
}

//==============================================================================
/**
 * @brief DMA operation finish interrupt
 */
//==============================================================================
#if (SDSPI_ENABLE_DMA != 0)
void SDSPI_DMA_IRQ_ROUTINE(void)
{
        disable_Tx_Rx_DMA();
        SDSPI_DMA_RX_CHANNEL->CCR = 0x00;
        SDSPI_DMA_TX_CHANNEL->CCR = 0x00;

        NVIC_DisableIRQ(SDSPI_DMA_IRQ_NUMBER);

        SDSPI_DMA->IFCR = DMA_IFCR_CTCIF1 << (4 * (SDSPI_DMA_RX_CHANNEL_NO - 1));

        sdspi_data->DMA_tansaction_finished = true;
}
#endif

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
