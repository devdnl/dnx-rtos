/*==============================================================================
File    spi_ioctl.h

Author  Daniel Zorychta

Brief   SPI module ioctl request codes.

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

/**
@defgroup drv-spi SPI Driver

\section drv-spi-desc Description
Driver handles SPI peripheral.

\section drv-spi-sup-arch Supported architectures
\li stm32f1
\li stm32f4

\section drv-spi-ddesc Details
\subsection drv-spi-ddesc-num Meaning of major and minor numbers
Some manufactures enumerate devices starting from 1 instead of 0 (e.g. ST).
In this case major number starts from 0 and is connected to the first device
e.g. SPI1.
\arg major number selects SPI peripheral e.g.:@n
     major = 0 -> SPI0 (or SPI1 if numerated from 1)
\arg minor number selects particular device e.g.:@n
     major = 0 and minor = 0 -> device 0 (CS0) on SPI0,@n
     major = 0 and minor = 1 -> device 1 (CS1) on SPI0,@n
     major = 0 and minor = n -> device n (CSn) on SPI0,@n
     major = 1 and minor = 0 -> device 0 (CS0) on SPI1,@n
     major = 1 and minor = 1 -> device 1 (CS1) on SPI1,@n
     major = 1 and minor = n -> device n (CSn) on SPI1

\subsection drv-spi-ddesc-numres Numeration restrictions
Number of peripherals determines how big major number can be. If there is
only one SPI peripheral then the major number is always 0.
Number of devices (minor number) can be theoretically up to 256 or to limits
of memory capacity.

\subsection drv-spi-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("SPI", 0, 0, "/dev/SPI0-0");  // device path can be more descriptive
@endcode
@code
driver_init("SPI", 0, 1, "/dev/SPI0-1");  // next SPI device on SPI0 bus
@endcode

\subsection drv-spi-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("SPI", 0, 0);
@endcode
@code
driver_release("SPI", 0, 1);
@endcode

\subsection drv-spi-ddesc-cfg Driver configuration
Driver configuration should be done before usage and after initialization.
The best place to do this is user application. To configure particular SPI
device the ioctl() function shall be used:

@code
#include <stdio.h>
#include <stdbool.h>
#include <sys/ioctl.h>

static const char *dev_path = "/dev/SPI0-0";

FILE *dev = fopen(dev_path, "r+");
if (dev) {
      static const SPI_config_t cfg = {
           .flush_byte  = 0xFF,                             // flush byte
           .clk_divider = SPI_CLK_DIV__4,                   // Peripheral clock / 4
           .mode        = SPI_MODE__0,                      // SPI mode 0
           .msb_first   = true,                             // MSb first
           .CS_port_idx = IOCTL_GPIO_PORT_IDX__CS0,         // port index of CS0
           .CS_pin_idx  = IOCTL_GPIO_PIN_NO__CS0            // pin number of CS0
      };

      if (ioctl(fileno(dev), IOCTL_SPI__SET_CONFIGURATION, &cfg) != 0) {
            perror(dev_path);
      }

      fclose(dev);
} else {
      perror(dev_path);
}
@endcode

\subsubsection drv-spi-ddesc-cfgfb Meaning of Flush Byte
The Flush Byte is used by the SPI driver to flush frames from SPI device at
read procedure. The SPI interface is full duplex thus it is not possible to
write and read data by using fwrite() and fread() interfaces at one time.
Flush Byte have not any meaning if write procedure is used.

<tt>MOSI: [ 0xFF ][ 0xFF ][ 0xFF ][ 0xFF ][ 0xFF ]</tt> -> Flush bytes@n
<tt>MISO: [DATA_0][DATA_1][DATA_2][DATA_3][DATA_n]</tt> -> Data frames read by fread()

\subsection drv-spi-ddesc-write Data write
Data to a SPI device can be wrote as regular file, but there are some
restrictions because of SPI specification. Write operation by using fwrite()
function sends bytes to device but incoming data is not received. Continuous
write and read operation can be done by using ioctl() function. The fseek()
function has no impact for SPI communication.
Example assumptions:
- SPI device working on MODE 0
- SCLK frequency is 32 times slower than peripheral clock
- MSb is send first
- Flush Byte is 0xFF
- Device is visible in system as "/dev/SPI0-0"

@code
#include <stdio.h>
#include <stdbool.h>
#include <sys/ioctl.h>

static const char *dev_path = "/dev/SPI0-0";
static const char *blk      = "Data block sent by SPI bus";

static const SPI_config_t cfg = {
     .flush_byte  = 0xFF,                             // flush byte
     .clk_divider = SPI_CLK_DIV__32                   // Peripheral clock / 32
     .mode        = SPI_MODE__0,                      // SPI mode 0
     .msb_first   = true,                             // MSb first
     .CS_port_idx = IOCTL_GPIO_PORT_IDX__CS0,         // port index of CS0
     .CS_pin_idx  = IOCTL_GPIO_PIN_NO__CS0            // pin number of CS0
};

int_main(spi_ex, STACK_DEPTH_MEDIUM, int argc, char *argv[])
{
      FILE *dev = fopen(dev_path, "r+");

      if (f) {
            // set SPI device configuration
            ioctl(fileno(dev), IOCTL_SPI__SET_CONFIGURATION, &cfg);

            // write blk
            fwrite(blk, sizeof(char), strsize(blk), dev);

            // close SPI device
            fclose(dev);

      } else {
            perror(dev_path);
      }

      return 0;
}
@endcode

\subsection drv-spi-ddesc-read Data read
Data from a SPI device can be read as regular file, but there are some
restrictions because of SPI specification. Read operation by using fread()
function sends flush bytes to a device and incoming bytes are received.
Continuous write and read operation can be done by using ioctl() function.
The fseek() function has no impact for SPI communication.
Example assumptions:
- SPI device working on MODE 0
- SCLK frequency is 32 times slower than peripheral clock
- MSb is send first
- Flush Byte is 0xFF
- Device is visible in system as "/dev/SPI0-0"

@code
#include <stdio.h>
#include <stdbool.h>
#include <dnx/misc.h>
#include <sys/ioctl.h>

static const char *dev_path = "/dev/SPI0-0";

static const SPI_config_t cfg = {
     .flush_byte  = 0xFF,                             // flush byte
     .clk_divider = SPI_CLK_DIV__32                   // Peripheral clock / 32
     .mode        = SPI_MODE__0,                      // SPI mode 0
     .msb_first   = true,                             // MSb first
     .CS_port_idx = IOCTL_GPIO_PORT_IDX__CS0,         // port index of CS0
     .CS_pin_idx  = IOCTL_GPIO_PIN_NO__CS0            // pin number of CS0
};

GLOBAL_VARIABLES_SECTION {
      char blk[100];
};

int_main(spi_ex, STACK_DEPTH_MEDIUM, int argc, char *argv[])
{
      FILE *dev = fopen(dev_path, "r+");

      if (dev) {
            // set SPI device configuration
            ioctl(fileno(dev), IOCTL_SPI__SET_CONFIGURATION, &cfg);

            // write blk
            fread(global->blk, ARRAY_ITEM_SIZE(global->blk), ARRAY_SIZE(global->blk), dev);

            // close SPI device
            fclose(dev);

      } else {
            perror(dev_path);
      }

      return 0;
}
@endcode


\subsection drv-spi-ddesc-rdwr Data read and write at the same time
To read and write bytes from a SPI device can be done by using ioctl()
function. In this case special descriptor must be used: SPI_transceive_t.
By using this object the Tx and Rx buffers must have the same sizes.
Example assumptions:
- SPI device working on MODE 0
- SCLK frequency is 32 times slower than peripheral clock
- MSb is send first
- Device is visible in system as "/dev/SPI0-0"

@code
#include <stdio.h>
#include <stdbool.h>
#include <dnx/misc.h>
#include <sys/ioctl.h>

static const char *dev_path = "/dev/SPI0-0";

static const SPI_config_t cfg = {
     .flush_byte  = 0xFF,                             // not used in this example
     .clk_divider = SPI_CLK_DIV__32                   // Peripheral clock / 32
     .mode        = SPI_MODE__0,                      // SPI mode 0
     .msb_first   = true,                             // MSb first
     .CS_port_idx = IOCTL_GPIO_PORT_IDX__CS0,         // port index of CS0
     .CS_pin_idx  = IOCTL_GPIO_PIN_NO__CS0            // pin number of CS0
};

GLOBAL_VARIABLES_SECTION {
      char tx[5];
      char rx[5];
};

int_main(spi_ex, STACK_DEPTH_MEDIUM, int argc, char *argv[])
{
      FILE *dev = fopen(dev_path, "r+");

      if (dev) {
            // set SPI device configuration
            ioctl(fileno(dev), IOCTL_SPI__SET_CONFIGURATION, &cfg);

            // prepare data to send
            global->tx[0] = 0x10;
            global->tx[1] = 0x20;
            global->tx[2] = 0x30;
            global->tx[3] = 0x40;
            global->tx[4] = 0x50;

            // send and receive prepared block
            SPI_transceive_t t = {
                  .tx_buffer = global->tx,      // bytes to send
                  .rx_buffer = global->rx,      // buffer to received data
                  .count     = ARRAY_SIZE(tx),  // buffer size
                  .separated = false,
                  .next      = NULL
            };

            ioctl(fileno(dev), IOCTL_SPI__TRANSCEIVE, &t);

            ...

            if (global->rx[0] == ...) {
                    ...
            }

            ...

            // close SPI device
            fclose(dev);

      } else {
            perror(dev_path);
      }

      return 0;
}
@endcode

@{
 */

#ifndef _SPI_IOCTL_H_
#define _SPI_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 *  @brief  Set SPI configuration.
 *  @param  [WR] @ref SPI_config_t * SPI peripheral configuration
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_SPI__SET_CONFIGURATION    _IOW(SPI, 0x00, const SPI_config_t*)

/**
 *  @brief  Gets SPI configuration
 *  @param  [RD] @ref SPI_config_t * SPI peripheral configuration
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_SPI__GET_CONFIGURATION    _IOR(SPI, 0x01, SPI_config_t*)

/**
 *  @brief  Select specified slave (CS = 0) [RAW mode].
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_SPI__SELECT               _IO(SPI, 0x02)

/**
 *  @brief  Deselect specified slave (CS = 1) [RAW mode].
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_SPI__DESELECT             _IO(SPI, 0x03)

/**
 *  @brief  Transmit and receive specified buffer.
 *  @param  [WR] @ref SPI_transceive_t * transmit and receive frame
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_SPI__TRANSCEIVE           _IOWR(SPI, 0x04, SPI_transceive_t*)

/**
 *  @brief  Transmit without selection.
 *  @param  [WR] @ref u8_t*        byte to transfer
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_SPI__TRANSMIT_NO_SELECT   _IOW(SPI, 0x05, const u8_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent peripheral divider.
 */
enum SPI_clk_div {
        SPI_CLK_DIV__2,         /*!< SPI peripheral clock divided by 2.*/
        SPI_CLK_DIV__4,         /*!< SPI peripheral clock divided by 4.*/
        SPI_CLK_DIV__8,         /*!< SPI peripheral clock divided by 8.*/
        SPI_CLK_DIV__16,        /*!< SPI peripheral clock divided by 16.*/
        SPI_CLK_DIV__32,        /*!< SPI peripheral clock divided by 32.*/
        SPI_CLK_DIV__64,        /*!< SPI peripheral clock divided by 64.*/
        SPI_CLK_DIV__128,       /*!< SPI peripheral clock divided by 128.*/
        SPI_CLK_DIV__256        /*!< SPI peripheral clock divided by 256.*/
};

/**
 * SPI peripheral modes.
 */
enum SPI_mode {
        SPI_MODE__0,    /*!< CPOL = 0; CPHA = 0 (SCK 0 at idle, capture on rising edge).*/
        SPI_MODE__1,    /*!< CPOL = 0; CPHA = 1 (SCK 0 at idle, capture on falling edge).*/
        SPI_MODE__2,    /*!< CPOL = 1; CPHA = 0 (SCK 1 at idle, capture on falling edge).*/
        SPI_MODE__3     /*!< CPOL = 1; CPHA = 1 (SCK 1 at idle, capture on rising edge).*/
};

/**
 *  SPI configuration type.
 */
typedef struct {
        u8_t             flush_byte;            /*!< Flush byte in read transmission.*/
        enum SPI_clk_div clk_divider;           /*!< Peripheral clock divider.*/
        enum SPI_mode    mode;                  /*!< SPI mode.*/
        bool             msb_first;             /*!< MSb first (@b true).*/
        u8_t             CS_port_idx;           /*!< Chip Select port index.*/
        u8_t             CS_pin_idx;            /*!< Chip Select pin index.*/
        bool             CS_reverse;
} SPI_config_t;

/**
 * SPI transmit and receive type.
 */
typedef struct SPI_transceive {
        const u8_t            *tx_buffer;       /*!< TX buffer pointer.*/
        u8_t                  *rx_buffer;       /*!< RX buffer pointer.*/
        size_t                 count;           /*!< RX and TX buffer size.*/
        bool                   separated;       /*!< Each chain in separated transfer. */
        struct SPI_transceive *next;            /*!< Next transceive buffer.*/
} SPI_transceive_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _SPI_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
