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
#define enable_bidirectional_data_mode()                SDSPI_PORT->CR1 |=  SPI_CR1_BIDIMODE
#define enable_unidirectional_data_mode()               SDSPI_PORT->CR1 &= ~SPI_CR1_BIDIMODE
#define enable_output_in_bidirectional_data_mode()      SDSPI_PORT->CR1 |=  SPI_CR1_BIDIOE
#define disable_output_in_bidirectional_data_mode()     SDSPI_PORT->CR1 &= ~SPI_CR1_BIDIOE
#define enable_hardware_CRC_calculation()               SDSPI_PORT->CR1 |=  SPI_CR1_CRCEN
#define disable_hardware_CRC_calculation()              SDSPI_PORT->CR1 &= ~SPI_CR1_CRCEN
#define next_trasfer_is_CRC()                           SDSPI_PORT->CR1 |=  SPI_CR1_CRCNEXT
#define next_trasfer_is_data()                          SDSPI_PORT->CR1 &= ~SPI_CR1_CRCNEXT
#define set_8_bit_data_frame()                          SDSPI_PORT->CR1 &= ~SPI_CR1_DFF
#define set_16_bit_data_frame()                         SDSPI_PORT->CR1 |=  SPI_CR1_DFF
#define enable_full_duplex()                            SDSPI_PORT->CR1 &= ~SPI_CR1_RXONLY
#define enable_receive_only()                           SDSPI_PORT->CR1 |=  SPI_CR1_RXONLY
#define enable_software_slave_management()              SDSPI_PORT->CR1 |=  SPI_CR1_SSM
#define enable_hardware_slave_management()              SDSPI_PORT->CR1 &= ~SPI_CR1_SSM
#define select_slave()                                  SDSPI_PORT->CR1 &= ~SPI_CR1_SSI
#define deselect_slave()                                SDSPI_PORT->CR1 |=  SPI_CR1_SSI
#define transmit_MSB_first()                            SDSPI_PORT->CR1 &= ~SPI_CR1_LSBFIRST
#define transmit_LSB_first()                            SDSPI_PORT->CR1 |=  SPI_CR1_LSBFIRST
#define enable_SPI_peripheral()                         SDSPI_PORT->CR1 |=  SPI_CR1_SPE
#define disable_SPI_peripheral()                        SDSPI_PORT->CR1 &= ~SPI_CR1_SPE
#define FPCLK_DIV_2                                     (0)
#define FPCLK_DIV_4                                     (SPI_CR1_BR_0)
#define FPCKL_DIV_8                                     (SPI_CR1_BR_1)
#define FPCKL_DIV_16                                    (SPI_CR1_BR_1 | SPI_CR1_BR_0)
#define FPCKL_DIV_32                                    (SPI_CR1_BR_2)
#define FPCKL_DIV_64                                    (SPI_CR1_BR_2 | SPI_CR1_BR_0)
#define FPCKL_DIV_128                                   (SPI_CR1_BR_2 | SPI_CR1_BR_1)
#define FPCKL_DIV_256                                   (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0)
#define set_baud_rate(baud)                             do{SDSPI_PORT->CR1 &= ~SPI_CR1_BR; SDSPI_PORT->CR1 |= baud}while(0)
#define enable_master_configuration()                   SDSPI_PORT->CR1 |=  SPI_CR1_MSTR
#define enable_slave_configuration()                    SDSPI_PORT->CR1 &= ~SPI_CR1_MSTR
#define set_clock_polarity_to_0_when_idle()             SDSPI_PORT->CR1 &= ~SPI_CR1_CPOL
#define set_clock_polarity_to_1_when_idle()             SDSPI_PORT->CR1 |=  SPI_CR1_CPOL
#define set_transition_on_first_edge()                  SDSPI_PORT->CR1 &= ~SPI_CR1_CPHA
#define set_transition_on_second_edge()                 SDSPI_PORT->CR1 |=  SPI_CR1_CPHA

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

#define transmit_data(data)                             SDSPI_PORT->DR = data
#define set_CRC_polynominal(poly)                       SDSPI_PORT->CRCPR = poly
#define get_Rx_CRC()                                    SDSPI_PORT->RXCRCR
#define get_Tx_CRC()                                    SDSPI_PORT->TXCRCR

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/** handling structure */
struct sdspi_data {
        bool     card_detected;
        mutex_t *port_lock_mtx;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t turn_on_SPI(void);
static stdret_t turn_off_SPI(void);

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
        if (dev != 0 || part != 0 || !drvhdl) {
                return STD_RET_ERROR;
        }

        if (!(*drvhdl = calloc(1, sizeof(struct sdspi_data)))) {
                return STD_RET_ERROR;
        }

        struct sdspi_data *sdspi = *drvhdl;

        if (!(sdspi->port_lock_mtx = new_recursive_mutex())) {
                goto error;
        }

        /* initialize SPI interface */
        if (turn_on_SPI() != STD_RET_OK) {
                goto error;
        }

        SDSPI_PORT->CR1 = 0;
        SPI3->CR1 |= SPI_CR1_BR;

error:
        if (sdspi) {
                if (sdspi->port_lock_mtx) {
                        delete_recursive_mutex(sdspi->port_lock_mtx);
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

}

//==============================================================================
/**
 * @brief Write data
 *
 * @param[in] *drvhdl           driver's memory handle
 * @param[in] *src              source
 * @param[in] size              size
 * @param[in] seek              seek
 *
 * @retval number of written nitems
 */
//==============================================================================
size_t SDSPI_write(void *drvhdl, const void *src, size_t size, size_t nitems, size_t seek)
{

}

//==============================================================================
/**
 * @brief Read data
 *
 * @param[in]  *drvhdl          driver's memory handle
 * @param[out] *dst             destination
 * @param[in]  size             size
 * @param[in]  seek             seek
 *
 * @retval number of read nitems
 */
//==============================================================================
size_t SDSPI_read(void *drvhdl, void *dst, size_t size, size_t nitems, size_t seek)
{

}

//==============================================================================
/**SDSPI_SPI
 * @brief Direct IO control
 *
 * @param[in]     *drvhdl       driver's memory handle
 * @param[in]     iorq          IO reqest
 * @param[in,out] args          additional arguments
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t SDSPI_ioctl(void *drvhdl, int iorq, va_list args)
{

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
 * @brief Function turn on SPI clock
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t turn_on_SPI(void)
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
static stdret_t turn_off_SPI(void)
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

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
