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
#include "drivers/sdspi.h"
#include "stm32f1/stm32f10x.h"

MODULE_NAME(SDSPI);

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define MTX_BLOCK_TIME                                  0
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
#define enable_Tx_DMA()                                 SDSPI_PORT->CR2 |=  SPI_CR2_TXDMAEN
#define disable_Tx_DMA()                                SDSPI_PORT->CR2 &= ~SPI_CR2_TXDMAEN
#define enable_Rx_DMA()                                 SDSPI_PORT->CR2 |=  SPI_CR2_RXDMAEN
#define disable_Rx_DMA()                                SDSPI_PORT->CR2 &= ~SPI_CR2_RXDMAEN

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

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/** card types */
typedef u8_t card_type;

/** handling structure */
struct sdspi_data {
        FILE      *gpio_file;
        card_type  card_type;
        bool       card_initialized;
        mutex_t   *card_protect_mtx;
        mutex_t   *port_lock_mtx;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static size_t   card_read(struct sdspi_data *hdl, void *dst, size_t size, size_t nitems, u64_t lseek);
static stdret_t turn_on_SPI_clock(void);
static stdret_t turn_off_SPI_clock(void);
static u8_t     send_cmd(struct sdspi_data *sdspi, u8_t cmd, u32_t arg);
static u8_t     wait_ready(void);
static u8_t     spi_rw(u8_t out);
static bool     receive_data_block(u8_t *buff, uint count);
static stdret_t initialize_card(struct sdspi_data *hdl);
static stdret_t detect_partitions(struct sdspi_data *hdl);

/*==============================================================================
  Local object definitions
==============================================================================*/


/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device
 *
 * @param[out] **drvhdl         driver's memory handler
 * @param[in]  dev              device number
 * @param[in]  part             device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t SDSPI_init(void **drvhdl, uint dev, uint part)
{
        struct sdspi_data *sdspi;

        if (dev != SDSPI_DEV_NO || part != SDSPI_DEV_PART || !drvhdl) {
                return STD_RET_ERROR;
        }

        if (!(sdspi = calloc(1, sizeof(struct sdspi_data)))) {
                return STD_RET_ERROR;
        }

        *drvhdl = sdspi;

        if (!(sdspi->port_lock_mtx = new_recursive_mutex())) {
                goto error;
        }

        if (!(sdspi->card_protect_mtx = new_mutex())) {
                goto error;
        }

        if (!(sdspi->gpio_file = fopen(SDSPI_GPIO_FILE, "r+"))) {
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
        set_baud_rate(FPCLK_DIV_2);
        enable_master_mode();
        set_clock_polarity_to_0_when_idle();
        capture_on_first_edge();
        enable_SPI_peripheral();

        return STD_RET_OK;

error:
        if (sdspi) {
                if (sdspi->port_lock_mtx) {
                        delete_recursive_mutex(sdspi->port_lock_mtx);
                }

                if (sdspi->card_protect_mtx) {
                        delete_mutex(sdspi->card_protect_mtx);
                }

                if (sdspi->gpio_file) {
                        fclose(sdspi->gpio_file);
                }

                free(sdspi);
        }
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t SDSPI_release(void *drvhdl)
{
        struct sdspi_data *hdl = drvhdl;

        if (!hdl) {
                return STD_RET_ERROR;
        }

        force_lock_recursive_mutex(hdl->port_lock_mtx);
        enter_critical();
        unlock_recursive_mutex(hdl->port_lock_mtx);
        delete_recursive_mutex(hdl->port_lock_mtx);
        delete_mutex(hdl->card_protect_mtx);
        fclose(hdl->gpio_file);
        turn_off_SPI_clock();
        free(hdl);
        exit_critical();

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Opens specified port
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t SDSPI_open(void *drvhdl)
{
        struct sdspi_data *hdl = drvhdl;

        if (!hdl) {
                return STD_RET_ERROR;
        }

        if (lock_recursive_mutex(hdl->port_lock_mtx, MTX_BLOCK_TIME) == MUTEX_LOCKED) {
                return STD_RET_OK;
        } else {
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Function close opened port
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t SDSPI_close(void *drvhdl)
{
        struct sdspi_data *hdl = drvhdl;

        if (!hdl) {
                return STD_RET_ERROR;
        }

        if (lock_recursive_mutex(hdl->port_lock_mtx, MTX_BLOCK_TIME) == MUTEX_LOCKED) {
                unlock_recursive_mutex(hdl->port_lock_mtx);     /* give this mutex */
                unlock_recursive_mutex(hdl->port_lock_mtx);     /* give mutex from open */
                return STD_RET_OK;
        } else {
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Write data
 *
 * @param[in] *drvhdl           driver's memory handle
 * @param[in] *src              source
 * @param[in]  size             item size
 * @param[in]  nitems           n-items to write
 * @param[in]  lseek            file index
 *
 * @retval number of written nitems
 */
//==============================================================================
size_t SDSPI_write(void *drvhdl, const void *src, size_t size, size_t nitems, u64_t lseek)
{
        (void) drvhdl;
        (void) src;
        (void) size;
        (void) nitems;
        (void) lseek;



        return 0;
}

//==============================================================================
/**
 * @brief Read data (read 512 byte sector)
 *
 * @param[in]  *drvhdl          driver's memory handle
 * @param[out] *dst             destination
 * @param[in]   size            item size
 * @param[in]   nitems          n-items to read
 * @param[in]   lseek           file index
 *
 * @retval number of read nitems
 */
//==============================================================================
size_t SDSPI_read(void *drvhdl, void *dst, size_t size, size_t nitems, u64_t lseek)
{
        struct sdspi_data *hdl = drvhdl;
        size_t n = 0;

        if (!hdl) {
                return 0;
        }

        if (lock_recursive_mutex(hdl->port_lock_mtx, MTX_BLOCK_TIME) == MUTEX_LOCKED) {
                n = card_read(hdl, dst, size, nitems, lseek);
                unlock_recursive_mutex(hdl->port_lock_mtx);
        }

        return n;
}

//==============================================================================
/**SDSPI_SPI
 * @brief Direct IO control
 *
 * @param[in]     *drvhdl       driver's memory handle
 * @param[in]     iorq          IO request
 * @param[in,out] args          additional arguments
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t SDSPI_ioctl(void *drvhdl, int iorq, va_list args)
{
        (void) args;

        struct sdspi_data *hdl = drvhdl;
        stdret_t status = STD_RET_OK;

        if (!hdl) {
                return STD_RET_ERROR;
        }

        if (lock_recursive_mutex(hdl->port_lock_mtx, MAX_DELAY) == MUTEX_LOCKED) {
                switch (iorq) {
                case SDSPI_IORQ_INITIALIZE_CARD:
                        if (lock_mutex(hdl->card_protect_mtx, MTX_BLOCK_TIME) == MUTEX_LOCKED) {
                                if (initialize_card(hdl) == STD_RET_OK) {
//                                     if (detect_partitions(hdl) == STD_RET_OK) {
                                               /* success */
//                                             break;
//                                     }
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

                unlock_recursive_mutex(hdl->port_lock_mtx);
        } else {
                return STD_RET_ERROR;
        }

        return status;
}

//==============================================================================
/**
 * @brief Function flush device
 *
 * @param[in] *drvhdl           driver's memory handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t SDSPI_flush(void *drvhdl)
{
        (void) drvhdl;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function open new partiotion file
 *
 * @param[in] *drvhdl   handler to partiotion description
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t partition_open(void *drvhdl)
{
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close partition file
 *
 * @param[in] *drvhdl   handler to partition description
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t partition_close(void *drvhdl)
{
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function write data to partition file
 *
 * @param[in] *drvhdl           handler to partition description
 * @param[in] *src              source
 * @param[in]  size             item size
 * @param[in]  nitems           n-items to write
 * @param[in]  lseek            file index
 *
 * @retval number of written nitems
 */
//==============================================================================
static size_t partition_write(void *drvhdl, const void *src, size_t size, size_t nitems, u64_t lseek)
{
        return 0;
}

//==============================================================================
/**
 * @brief Function read data from partition file
 *
 * @param[in]  *drvhdl          handler to partition description
 * @param[out] *dst             destination
 * @param[in]   size            item size
 * @param[in]   nitems          n-items to read
 * @param[in]   lseek           file index
 *
 * @retval number of written nitems
 */
//==============================================================================
static size_t partition_read(void *drvhdl, void *dst, size_t size, size_t nitems, u64_t lseek)
{
        return 0;
}

//==============================================================================
/**
 * @brief Function control partition
 *
 * @param[in]    *drvhdl        handler to partition description
 * @param[in]     iorq          IO request
 * @param[in,out] args          additional arguments
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t partition_ioctl(void *drvhdl, int iorq, va_list args)
{
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function flush partition
 *
 * @param[in] *drvhdl           handler to partition description
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t partition_flush(void *drvhdl)
{
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
        int  try = 10000;
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
        ioctl(sdspi->gpio_file, GPIO_IORQ_SD_DESELECT);
        ioctl(sdspi->gpio_file, GPIO_IORQ_SD_SELECT);

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
 * @param[out] *buff            data buffer
 * @param[in]   count           buffer size
 *
 * @retval true if success
 * @retval false if error
 */
//==============================================================================
static bool receive_data_block(u8_t *buff, uint count)
{
        u8_t token;
        uint try = 10000;

        while ((token = spi_rw(0xFF)) == 0xFF && --try);

        if (token != 0xFE)
                return false;

        /* memory alignment */
        do {
                *buff++ = spi_rw(0xFF);
                *buff++ = spi_rw(0xFF);
                *buff++ = spi_rw(0xFF);
                *buff++ = spi_rw(0xFF);
        } while (count -= 4);

        /* discard CRC */
        spi_rw(0xFF);
        spi_rw(0xFF);

        return true;
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
#include "core/io.h" /* FIXME -- niepotrzebne */
        ioctl(hdl->gpio_file, GPIO_IORQ_SD_DESELECT);
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
                        if (!timeout || send_cmd(hdl, CMD16, 512) != 0) {
                                hdl->card_type = 0;
                        }
                }

                if (timeout) {
                        hdl->card_initialized = true;
                }
        }

        ioctl(hdl->gpio_file, GPIO_IORQ_SD_DESELECT);
        spi_rw(0xFF);

        if (hdl->card_initialized == false) {
                printk(FONT_COLOR_RED"Card not initialized...\n"RESET_ATTRIBUTES);
                return STD_RET_ERROR;
        } else {
                printk("Card initialized. Card type: 0x%x\n", hdl->card_type);
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
static stdret_t detect_partitions(struct sdspi_data *hdl) /* TODO detect_partitions */
{
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Read data (read 512 byte sector)
 *
 * @param[in]  *hdl             driver's memory handle
 * @param[out] *dst             destination
 * @param[in]   size            item size
 * @param[in]   nitems          n-items to read
 * @param[in]   lseek           file index
 *
 * @retval number of read nitems
 */
//==============================================================================
static size_t card_read(struct sdspi_data *hdl, void *dst, size_t size, size_t nitems, u64_t lseek)
{
        size_t n = 0;

        if (lock_mutex(hdl->card_protect_mtx, MAX_DELAY) == MUTEX_LOCKED) {

                if (hdl->card_initialized == false) {
                        return 0;
                }

                /* whole sector(s) read */
                if (((size * nitems) % 512 == 0) && (lseek % 512 == 0)) {
                        if (hdl->card_type & CT_BLOCK) {
                                lseek >>= 9;    /* divide by 512 */
                        }

                        /* 1 sector to read */
                        if ((size * nitems) / 512 == 1) {
                                if (send_cmd(hdl, CMD17, (u32_t)lseek) == 0) {
                                        if (receive_data_block(dst, 512)) {
                                                n = nitems;
                                        }
                                }
                        } else {
                                if (send_cmd(hdl, CMD18, (u32_t)lseek) == 0) {
                                        do {
                                                if (!receive_data_block(dst, 512)) {
                                                        break;
                                                }

                                                dst += 512;
                                                n   += 512/size;
                                        } while (n < nitems);

                                        /* stop transmission */
                                        send_cmd(hdl, CMD12, 0);
                                }
                        }
                } else {
//                        u8_t *buff = malloc(512);
//                        if (buff) {
//                                u32_t sector = ;
//                        }
                }

                ioctl(hdl->gpio_file, GPIO_IORQ_SD_DESELECT);
                spi_rw(0xFF);

                unlock_mutex(hdl->card_protect_mtx);
        }

        return n;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
