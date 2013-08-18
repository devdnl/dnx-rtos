#ifndef _GPIO_CFG_H_
#define _GPIO_CFG_H_
/*=========================================================================*//**
@file    gpio_cfg.h

@author  Daniel Zorychta

@brief   This driver support GPIO. Set here driver configuration.

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32f1/stm32f10x.h"
#include "stm32f1/gpio_macros.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** enable (1) or disable (0) GPIOA port */
#define _GPIOA_EN                               (1)

/** enable (1) or disable (0) GPIOB port */
#define _GPIOB_EN                               (1)

/** enable (1) or disable (0) GPIOC port */
#define _GPIOC_EN                               (1)

/** enable (1) or disable (0) GPIOD port */
#define _GPIOD_EN                               (1)

/** enable (1) or disable (0) GPIOE port */
#define _GPIOE_EN                               (0)

/** enable (1) or disable (0) GPIOF port */
#define _GPIOF_EN                               (0)

/** enable (1) or disable (0) GPIOG port */
#define _GPIOG_EN                               (0)

/** enable (1) or disable (0) AFIO */
#define _AFIO_EN                                (1)
#if (_AFIO_EN > 0)

/** SPI3 remaped */
#define SPI3_REMAP                              (1)
#endif

/** define GPIOA pins configuration */
#if _GPIOA_EN
_PIN_CONFIGURATION(GPIOA,  0, ETH_MII_CRS_WKUP, _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOA,  1, ETH_MII_RX_CLK  , _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOA,  2, ETH_MDIO        , _GPIO_ALT_OUT_PUSH_PULL_50MHZ, _LOW  );
_PIN_CONFIGURATION(GPIOA,  3, ETH_MII_COL     , _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOA,  4, SD_CS           , _GPIO_OUT_PUSH_PULL_50MHZ    , _HIGH );
_PIN_CONFIGURATION(GPIOA,  5, ADC12_IN5       , _GPIO_IN_ANALOG              , _FLOAT);
_PIN_CONFIGURATION(GPIOA,  6, ADC12_IN6       , _GPIO_IN_ANALOG              , _FLOAT);
_PIN_CONFIGURATION(GPIOA,  7, ETH_MII_RX_DV   , _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOA,  8, TP204           , _GPIO_IN_PULLED              , _DOWN );
_PIN_CONFIGURATION(GPIOA,  9, USART1_TX       , _GPIO_ALT_OUT_PUSH_PULL_2MHZ , _HIGH );
_PIN_CONFIGURATION(GPIOA, 10, USART1_RX       , _GPIO_IN_PULLED              , _UP   );
_PIN_CONFIGURATION(GPIOA, 11, USART1_CTS      , _GPIO_IN_PULLED              , _HIGH );
_PIN_CONFIGURATION(GPIOA, 12, USART1_RTS      , _GPIO_ALT_OUT_PUSH_PULL_2MHZ , _HIGH );
_PIN_CONFIGURATION(GPIOA, 13, JTMS            , _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOA, 14, JTCK            , _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOA, 15, JTDI            , _GPIO_IN_FLOAT               , _FLOAT);
#endif

/** define GPIOB pins configuration */
#if _GPIOB_EN
_PIN_CONFIGURATION(GPIOB,  0, ETH_MII_RXD2 , _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOB,  1, ETH_MII_RXD3 , _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOB,  2, BOOT1        , _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOB,  3, JTDO         , _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOB,  4, JTRST        , _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOB,  5, TP210        , _GPIO_IN_PULLED              , _DOWN );
_PIN_CONFIGURATION(GPIOB,  6, I2C1_SCL     , _GPIO_ALT_OUT_OPEN_DRAIN_2MHZ, _HIGH );
_PIN_CONFIGURATION(GPIOB,  7, I2C1_SDA     , _GPIO_ALT_OUT_OPEN_DRAIN_2MHZ, _HIGH );
_PIN_CONFIGURATION(GPIOB,  8, ETH_MII_TXD3 , _GPIO_ALT_OUT_PUSH_PULL_50MHZ, _LOW  );
_PIN_CONFIGURATION(GPIOB,  9, TP211        , _GPIO_IN_PULLED              , _DOWN );
_PIN_CONFIGURATION(GPIOB, 10, ETH_MII_RX_ER, _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOB, 11, ETH_MII_TX_EN, _GPIO_ALT_OUT_PUSH_PULL_50MHZ, _LOW  );
_PIN_CONFIGURATION(GPIOB, 12, ETH_MII_TXD0 , _GPIO_ALT_OUT_PUSH_PULL_50MHZ, _LOW  );
_PIN_CONFIGURATION(GPIOB, 13, ETH_MII_TXD1 , _GPIO_ALT_OUT_PUSH_PULL_50MHZ, _LOW  );
_PIN_CONFIGURATION(GPIOB, 14, TP212        , _GPIO_IN_PULLED              , _DOWN );
_PIN_CONFIGURATION(GPIOB, 15, TP213        , _GPIO_IN_PULLED              , _DOWN );
#endif

/** define GPIOC pins configuration */
#if _GPIOC_EN
_PIN_CONFIGURATION(GPIOC,  0, ETH_PWRON_IRQ , _GPIO_OUT_OPEN_DRAIN_2MHZ    , _HIGH );
_PIN_CONFIGURATION(GPIOC,  1, ETH_MDC       , _GPIO_ALT_OUT_PUSH_PULL_50MHZ, _LOW  );
_PIN_CONFIGURATION(GPIOC,  2, ETH_MII_TXD2  , _GPIO_ALT_OUT_PUSH_PULL_50MHZ, _LOW  );
_PIN_CONFIGURATION(GPIOC,  3, ETH_MII_TX_CLK, _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOC,  4, ETH_MII_RXD0  , _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOC,  5, ETH_MII_RXD1  , _GPIO_IN_FLOAT               , _FLOAT);
_PIN_CONFIGURATION(GPIOC,  6, TP218         , _GPIO_IN_PULLED              , _DOWN );
_PIN_CONFIGURATION(GPIOC,  7, TP219         , _GPIO_IN_PULLED              , _DOWN );
_PIN_CONFIGURATION(GPIOC,  8, TP220         , _GPIO_IN_PULLED              , _DOWN );
_PIN_CONFIGURATION(GPIOC,  9, TP221         , _GPIO_IN_PULLED              , _DOWN );
_PIN_CONFIGURATION(GPIOC, 10, SD_SCK        , _GPIO_ALT_OUT_PUSH_PULL_50MHZ, _LOW  );
_PIN_CONFIGURATION(GPIOC, 11, SD_MISO       , _GPIO_IN_PULLED              , _UP   );
_PIN_CONFIGURATION(GPIOC, 12, SD_MOSI       , _GPIO_ALT_OUT_PUSH_PULL_50MHZ, _LOW  );
_PIN_CONFIGURATION(GPIOC, 13, TP222         , _GPIO_IN_PULLED              , _DOWN );
_PIN_CONFIGURATION(GPIOC, 14, TP223         , _GPIO_IN_PULLED              , _DOWN );
_PIN_CONFIGURATION(GPIOC, 15, TP224         , _GPIO_IN_PULLED              , _DOWN );
#endif

/** define GPIOD pins configuration */
#if _GPIOD_EN
_PIN_CONFIGURATION(GPIOD,  0, OSC_IN     , _GPIO_IN_FLOAT , _FLOAT);
_PIN_CONFIGURATION(GPIOD,  1, OSC_OUT    , _GPIO_IN_FLOAT , _FLOAT);
_PIN_CONFIGURATION(GPIOD,  2, TP216      , _GPIO_IN_PULLED, _DOWN );
_PIN_CONFIGURATION(GPIOD,  3, NC_GPIOD_3 , _GPIO_IN_PULLED, _DOWN );
_PIN_CONFIGURATION(GPIOD,  4, NC_GPIOD_4 , _GPIO_IN_PULLED, _DOWN );
_PIN_CONFIGURATION(GPIOD,  5, NC_GPIOD_5 , _GPIO_IN_PULLED, _DOWN );
_PIN_CONFIGURATION(GPIOD,  6, NC_GPIOD_6 , _GPIO_IN_PULLED, _DOWN );
_PIN_CONFIGURATION(GPIOD,  7, NC_GPIOD_7 , _GPIO_IN_PULLED, _DOWN );
_PIN_CONFIGURATION(GPIOD,  8, NC_GPIOD_8 , _GPIO_IN_PULLED, _DOWN );
_PIN_CONFIGURATION(GPIOD,  9, NC_GPIOD_9 , _GPIO_IN_PULLED, _DOWN );
_PIN_CONFIGURATION(GPIOD, 10, NC_GPIOD_10, _GPIO_IN_PULLED, _DOWN );
_PIN_CONFIGURATION(GPIOD, 11, NC_GPIOD_11, _GPIO_IN_PULLED, _DOWN );
_PIN_CONFIGURATION(GPIOD, 12, NC_GPIOD_12, _GPIO_IN_PULLED, _DOWN );
_PIN_CONFIGURATION(GPIOD, 13, NC_GPIOD_13, _GPIO_IN_PULLED, _DOWN );
_PIN_CONFIGURATION(GPIOD, 14, NC_GPIOD_14, _GPIO_IN_PULLED, _DOWN );
_PIN_CONFIGURATION(GPIOD, 15, NC_GPIOD_15, _GPIO_IN_PULLED, _DOWN );
#endif

/** define GPIOE pins configuration */
#if _GPIOE_EN
_PIN_CONFIGURATION(GPIOE,  0, NC_GPIOE_0 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE,  1, NC_GPIOE_1 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE,  2, NC_GPIOE_2 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE,  3, NC_GPIOE_3 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE,  4, NC_GPIOE_4 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE,  5, NC_GPIOE_5 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE,  6, NC_GPIOE_6 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE,  7, NC_GPIOE_7 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE,  8, NC_GPIOE_8 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE,  9, NC_GPIOE_9 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE, 10, NC_GPIOE_10, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE, 11, NC_GPIOE_11, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE, 12, NC_GPIOE_12, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE, 13, NC_GPIOE_13, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE, 14, NC_GPIOE_14, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOE, 15, NC_GPIOE_15, _GPIO_IN_PULLED, _DOWN);
#endif

/** define GPIOF pins configuration */
#if _GPIOF_EN
_PIN_CONFIGURATION(GPIOF,  0, NC_GPIOF_0 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF,  1, NC_GPIOF_1 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF,  2, NC_GPIOF_2 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF,  3, NC_GPIOF_3 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF,  4, NC_GPIOF_4 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF,  5, NC_GPIOF_5 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF,  6, NC_GPIOF_6 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF,  7, NC_GPIOF_7 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF,  8, NC_GPIOF_8 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF,  9, NC_GPIOF_9 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF, 10, NC_GPIOF_10, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF, 11, NC_GPIOF_11, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF, 12, NC_GPIOF_12, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF, 13, NC_GPIOF_13, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF, 14, NC_GPIOF_14, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOF, 15, NC_GPIOF_15, _GPIO_IN_PULLED, _DOWN);
#endif

/** define GPIOG pins configuration */
#if _GPIOG_EN
_PIN_CONFIGURATION(GPIOG,  0, NC_GPIOG_0 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG,  1, NC_GPIOG_1 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG,  2, NC_GPIOG_2 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG,  3, NC_GPIOG_3 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG,  4, NC_GPIOG_4 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG,  5, NC_GPIOG_5 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG,  6, NC_GPIOG_6 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG,  7, NC_GPIOG_7 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG,  8, NC_GPIOG_8 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG,  9, NC_GPIOG_9 , _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG, 10, NC_GPIOG_10, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG, 11, NC_GPIOG_11, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG, 12, NC_GPIOG_12, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG, 13, NC_GPIOG_13, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG, 14, NC_GPIOG_14, _GPIO_IN_PULLED, _DOWN);
_PIN_CONFIGURATION(GPIOG, 15, NC_GPIOG_15, _GPIO_IN_PULLED, _DOWN);
#endif

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _GPIO_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
