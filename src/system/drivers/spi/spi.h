/*==============================================================================
File    spi.h

Author  Daniel Zorychta

Brief   SPI driver.

        Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _SPI_H_
#define _SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "spi_ioctl.h"

#if defined(ARCH_stm32f1)
#include "stm32fx/spi_cfg.h"
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"
#elif defined(ARCH_stm32f4)
#include "stm32fx/spi_cfg.h"
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define _SPI_IRQ_WAIT_TIMEOUT           MAX_DELAY_MS

/*==============================================================================
  Exported object types
==============================================================================*/
#if defined(ARCH_stm32f1)
enum {
    #if defined(RCC_APB2ENR_SPI1EN)
        _SPI1,
    #endif
    #if defined(RCC_APB1ENR_SPI2EN)
        _SPI2,
    #endif
    #if defined(RCC_APB1ENR_SPI3EN)
        _SPI3,
    #endif
        _NUMBER_OF_SPI_PERIPHERALS
};
#elif defined(ARCH_stm32f4)
enum {
    #if defined(RCC_APB2ENR_SPI1EN)
        _SPI1,
    #endif
    #if defined(RCC_APB1ENR_SPI2EN)
        _SPI2,
    #endif
    #if defined(RCC_APB1ENR_SPI3EN)
        _SPI3,
    #endif
    #if defined(RCC_APB2ENR_SPI4EN)
        _SPI4,
    #endif
    #if defined(RCC_APB2ENR_SPI5EN)
        _SPI5,
    #endif
    #if defined(RCC_APB2ENR_SPI6EN)
        _SPI6,
    #endif
        _NUMBER_OF_SPI_PERIPHERALS
};
#endif

/* independent SPI instance */
struct SPI_slave {
        dev_lock_t               lock;                  //!< SPI slave lock
        u8_t                     major;                 //!< SPI major number
        SPI_config_t             config;                //!< SPI configuration
};

/* general module data */
struct SPI {
        sem_t                   *wait_irq_sem;          //!< IRQ detect semaphore
        mutex_t                 *periph_protect_mtx;    //!< SPI protection mutex
        struct SPI_slave        *slave;                 //!< current handled slave
        const u8_t              *tx_buffer;             //!< Tx buffer
        u8_t                    *rx_buffer;             //!< Rx buffer
        size_t                   count;                 //!< transaction length
        dev_lock_t               RAW_mode;              //!< RAW mode (locked for specified process)
        u8_t                     flush_byte;            //!< flush byte (read transaction)
        u8_t                     slave_count;           //!< number of slaves
};

/*==============================================================================
  Exported objects
==============================================================================*/
extern struct SPI *_SPI[];

/*==============================================================================
  Exported functions
==============================================================================*/
extern int  _SPI_LLD__turn_on(u8_t major);
extern void _SPI_LLD__turn_off(u8_t major);
extern int  _SPI_LLD__transceive(struct SPI_slave *hdl, const u8_t *txbuf, u8_t *rxbuf, size_t count);
extern void _SPI_LLD__apply_config(struct SPI_slave *hdl);
extern void _SPI_LLD__halt(u8_t major);

#ifdef __cplusplus
}
#endif

#endif /* _SPI_H_ */
/*==============================================================================
  End of file
==============================================================================*/
