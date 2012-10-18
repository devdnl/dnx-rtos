#ifndef GPIO_CFG_H_
#define GPIO_CFG_H_
/*=============================================================================================*//**
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


*//*==============================================================================================*/

#ifdef __cplusplus
   extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
/** enable (1) or disable (0) GPIOA port */
#define GPIOA_EN                                (1)

/** enable (1) or disable (0) GPIOB port */
#define GPIOB_EN                                (1)

/** enable (1) or disable (0) GPIOC port */
#define GPIOC_EN                                (1)

/** enable (1) or disable (0) GPIOD port */
#define GPIOD_EN                                (1)

/** enable (1) or disable (0) GPIOE port */
#define GPIOE_EN                                (0)

/** enable (1) or disable (0) GPIOF port */
#define GPIOF_EN                                (0)

/** enable (1) or disable (0) GPIOG port */
#define GPIOG_EN                                (0)

/** enable (1) or disable (0) AFIO */
#define AFIO_EN                                 (1)
#if (AFIO_EN > 0)

/** SPI3 remaped */
#define SPI3_REMAP                              (1)
#endif


/** define GPIO pin outputs types and speeds *//* ----------------------------------------------- */
#define GPIO_OUT_PUSH_PULL_10MHZ                0x01
#define GPIO_OUT_PUSH_PULL_2MHZ                 0x02
#define GPIO_OUT_PUSH_PULL_50MHZ                0x03
#define GPIO_OUT_OPEN_DRAIN_10MHZ               0x05
#define GPIO_OUT_OPEN_DRAIN_2MHZ                0x06
#define GPIO_OUT_OPEN_DRAIN_50MHZ               0x07
#define GPIO_ALT_OUT_PUSH_PULL_10MHZ            0x09
#define GPIO_ALT_OUT_PUSH_PULL_2MHZ             0x0A
#define GPIO_ALT_OUT_PUSH_PULL_50MHZ            0x0B
#define GPIO_ALT_OUT_OPEN_DRAIN_10MHZ           0x0D
#define GPIO_ALT_OUT_OPEN_DRAIN_2MHZ            0x0E
#define GPIO_ALT_OUT_OPEN_DRAIN_50MHZ           0x0F

/** define GPIO pin inputs */
#define GPIO_IN_ANALOG                          0x00
#define GPIO_IN_FLOAT                           0x04
#define GPIO_IN_PULLED                          0x08

/** define pin state */
#define FLOAT                                   0U
#define DOWN                                    0U
#define UP                                      1U
#define HIGH                                    1U
#define LOW                                     0U


/** define GPIOA pins names *//* ---------------------------------------------------------------- */
#if GPIOA_EN
#define ETH_MII_CRS_WKUP_BP                     0U                               /* pin name      */
#define ETH_MII_CRS_WKUP_BM                     (1 << 0)                         /* pin mask      */
#define ETH_MII_CRS_WKUP_PORT                   GPIOA                            /* port name     */
#define GPIOA_PIN_00_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_00_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define ETH_MII_RX_CLK_BP                       1U                               /* pin name      */
#define ETH_MII_RX_CLK_BM                       (1 << 1)                         /* pin mask      */
#define ETH_MII_RX_CLK_PORT                     GPIOA                            /* port name     */
#define GPIOA_PIN_01_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_01_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define ETH_MDIO_BP                             2U                               /* pin name      */
#define ETH_MDIO_BM                             (1 << 2)                         /* pin mask      */
#define ETH_MDIO_PORT                           GPIOA                            /* port name     */
#define GPIOA_PIN_02_MODE                       GPIO_ALT_OUT_PUSH_PULL_50MHZ     /* pin mode      */
#define GPIOA_PIN_02_DEFAULT_STATE              LOW                              /* def. pin state*/

#define ETH_MII_COL_BP                          3U                               /* pin name      */
#define ETH_MII_COL_BM                          (1 << 3)                         /* pin mask      */
#define ETH_MII_COL_PORT                        GPIOA                            /* port name     */
#define GPIOA_PIN_03_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_03_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define SPI_SS_BP                               4U                               /* pin name      */
#define SPI_SS_BM                               (1 << 4)                         /* pin mask      */
#define SPI_SS_PORT                             GPIOA                            /* port name     */
#define GPIOA_PIN_04_MODE                       GPIO_ALT_OUT_PUSH_PULL_2MHZ      /* pin mode      */
#define GPIOA_PIN_04_DEFAULT_STATE              HIGH                             /* def. pin state*/

#define ADC12_IN5_BP                            5U                               /* pin name      */
#define ADC12_IN5_BM                            (1 << 5)                         /* pin mask      */
#define ADC12_IN5_PORT                          GPIOA                            /* port name     */
#define GPIOA_PIN_05_MODE                       GPIO_IN_ANALOG                   /* pin mode      */
#define GPIOA_PIN_05_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define ADC12_IN6_BP                            6U                               /* pin name      */
#define ADC12_IN6_BM                            (1 << 6)                         /* pin mask      */
#define ADC12_IN6_PORT                          GPIOA                            /* port name     */
#define GPIOA_PIN_06_MODE                       GPIO_IN_ANALOG                   /* pin mode      */
#define GPIOA_PIN_06_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define ETH_MII_RX_DV_BP                        7U                               /* pin name      */
#define ETH_MII_RX_DV_BM                        (1 << 7)                         /* pin mask      */
#define ETH_MII_RX_DV_PORT                      GPIOA                            /* port name     */
#define GPIOA_PIN_07_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_07_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define TP204_BP                                8U                               /* pin name      */
#define TP204_BM                                (1 << 8)                         /* pin mask      */
#define TP204_PORT                              GPIOA                            /* port name     */
#define GPIOA_PIN_08_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOA_PIN_08_DEFAULT_STATE              DOWN                             /* def. pin state*/

#define USART1_TX_BP                            9U                               /* pin name      */
#define USART1_TX_BM                            (1 << 9)                         /* pin mask      */
#define USART1_TX_PORT                          GPIOA                            /* port name     */
#define GPIOA_PIN_09_MODE                       GPIO_ALT_OUT_PUSH_PULL_2MHZ      /* pin mode      */
#define GPIOA_PIN_09_DEFAULT_STATE              HIGH                             /* def. pin state*/

#define USART1_RX_BP                            10U                              /* pin name      */
#define USART1_RX_BM                            (1 << 10)                        /* pin mask      */
#define USART1_RX_PORT                          GPIOA                            /* port name     */
#define GPIOA_PIN_10_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOA_PIN_10_DEFAULT_STATE              UP                               /* def. pin state*/

#define USART1_CTS_BP                           11U                              /* pin name      */
#define USART1_CTS_BM                           (1 << 11)                        /* pin mask      */
#define USART1_CTS_PORT                         GPIOA                            /* port name     */
#define GPIOA_PIN_11_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOA_PIN_11_DEFAULT_STATE              HIGH                             /* def. pin state*/

#define USART1_RTS_BP                           12U                              /* pin name      */
#define USART1_RTS_BM                           (1 << 12)                        /* pin mask      */
#define USART1_RTS_PORT                         GPIOA                            /* port name     */
#define GPIOA_PIN_12_MODE                       GPIO_ALT_OUT_PUSH_PULL_2MHZ      /* pin mode      */
#define GPIOA_PIN_12_DEFAULT_STATE              HIGH                             /* def. pin state*/

#define JTMS_BP                                 13U                              /* pin name      */
#define JTMS_BM                                 (1 << 13)                        /* pin mask      */
#define JTMS_PORT                               GPIOA                            /* port name     */
#define GPIOA_PIN_13_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_13_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define JTCK_BP                                 14U                              /* pin name      */
#define JTCK_BM                                 (1 << 14)                        /* pin mask      */
#define JTCK_PORT                               GPIOA                            /* port name     */
#define GPIOA_PIN_14_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_14_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define JTDI_BP                                 15U                              /* pin name      */
#define JTDI_BM                                 (1 << 15)                        /* pin mask      */
#define JTDI_PORT                               GPIOA                            /* port name     */
#define GPIOA_PIN_15_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_15_DEFAULT_STATE              FLOAT                            /* def. pin state*/
#endif /* GPIOA_EN */

/** define GPIOB pins names *//* ---------------------------------------------------------------- */
#if GPIOB_EN
#define ETH_MII_RXD2_BP                         0U                               /* pin name      */
#define ETH_MII_RXD2_BM                         (1 << 0)                         /* pin mask      */
#define ETH_MII_RXD2_PORT                       GPIOB                            /* port name     */
#define GPIOB_PIN_00_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_00_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define ETH_MII_RXD3_BP                         1U                               /* pin name      */
#define ETH_MII_RXD3_BM                         (1 << 1)                         /* pin mask      */
#define ETH_MII_RXD3_PORT                       GPIOB                            /* port name     */
#define GPIOB_PIN_01_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_01_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define BOOT1_BP                                2U                               /* pin name      */
#define BOOT1_BM                                (1 << 2)                         /* pin mask      */
#define BOOT1_PORT                              GPIOB                            /* port name     */
#define GPIOB_PIN_02_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_02_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define JTDO_BP                                 3U                               /* pin name      */
#define JTDO_BM                                 (1 << 3)                         /* pin mask      */
#define JTDO_PORT                               GPIOB                            /* port name     */
#define GPIOB_PIN_03_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_03_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define JTRST_BP                                4U                               /* pin name      */
#define JTRST_BM                                (1 << 4)                         /* pin mask      */
#define JTRST_PORT                              GPIOB                            /* port name     */
#define GPIOB_PIN_04_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_04_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define TP210_BP                                5U                               /* pin name      */
#define TP210_BM                                (1 << 5)                         /* pin mask      */
#define TP210_PORT                              GPIOB                            /* port name     */
#define GPIOB_PIN_05_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOB_PIN_05_DEFAULT_STATE              DOWN                             /* def. pin state*/

#define I2C1_SCL_BP                             6U                               /* pin name      */
#define I2C1_SCL_BM                             (1 << 6)                         /* pin mask      */
#define I2C1_SCL_PORT                           GPIOB                            /* port name     */
#define GPIOB_PIN_06_MODE                       GPIO_ALT_OUT_OPEN_DRAIN_2MHZ     /* pin mode      */
#define GPIOB_PIN_06_DEFAULT_STATE              HIGH                             /* def. pin state*/

#define I2C1_SDA_BP                             7U                               /* pin name      */
#define I2C1_SDA_BM                             (1 << 7)                         /* pin mask      */
#define I2C1_SDA_PORT                           GPIOB                            /* port name     */
#define GPIOB_PIN_07_MODE                       GPIO_ALT_OUT_OPEN_DRAIN_2MHZ     /* pin mode      */
#define GPIOB_PIN_07_DEFAULT_STATE              HIGH                             /* def. pin state*/

#define ETH_MII_TXD3_BP                         8U                               /* pin name      */
#define ETH_MII_TXD3_BM                         (1 << 8)                         /* pin mask      */
#define ETH_MII_TXD3_PORT                       GPIOB                            /* port name     */
#define GPIOB_PIN_08_MODE                       GPIO_ALT_OUT_PUSH_PULL_50MHZ     /* pin mode      */
#define GPIOB_PIN_08_DEFAULT_STATE              LOW                              /* def. pin state*/

#define TP211_BP                                9U                               /* pin name      */
#define TP211_BM                                (1 << 9)                         /* pin mask      */
#define TP211_PORT                              GPIOB                            /* port name     */
#define GPIOB_PIN_09_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOB_PIN_09_DEFAULT_STATE              DOWN                             /* def. pin state*/

#define ETH_MII_RX_ER_BP                        10U                              /* pin name      */
#define ETH_MII_RX_ER_BM                        (1 << 10)                        /* pin mask      */
#define ETH_MII_RX_ER_PORT                      GPIOB                            /* port name     */
#define GPIOB_PIN_10_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_10_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define ETH_MII_TX_EN_BP                        11U                              /* pin name      */
#define ETH_MII_TX_EN_BM                        (1 << 11)                        /* pin mask      */
#define ETH_MII_TX_EN_PORT                      GPIOB                            /* port name     */
#define GPIOB_PIN_11_MODE                       GPIO_ALT_OUT_PUSH_PULL_50MHZ     /* pin mode      */
#define GPIOB_PIN_11_DEFAULT_STATE              LOW                              /* def. pin state*/

#define ETH_MII_TXD0_BP                         12U                              /* pin name      */
#define ETH_MII_TXD0_BM                         (1 << 12)                        /* pin mask      */
#define ETH_MII_TXD0_PORT                       GPIOB                            /* port name     */
#define GPIOB_PIN_12_MODE                       GPIO_ALT_OUT_PUSH_PULL_50MHZ     /* pin mode      */
#define GPIOB_PIN_12_DEFAULT_STATE              LOW                              /* def. pin state*/

#define ETH_MII_TXD1_BP                         13U                              /* pin name      */
#define ETH_MII_TXD1_BM                         (1 << 13)                        /* pin mask      */
#define ETH_MII_TXD1_PORT                       GPIOB                            /* port name     */
#define GPIOB_PIN_13_MODE                       GPIO_ALT_OUT_PUSH_PULL_50MHZ     /* pin mode      */
#define GPIOB_PIN_13_DEFAULT_STATE              LOW                              /* def. pin state*/

#define TP212_BP                                14U                              /* pin name      */
#define TP212_BM                                (1 << 14)                        /* pin mask      */
#define TP212_PORT                              GPIOB                            /* port name     */
#define GPIOB_PIN_14_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOB_PIN_14_DEFAULT_STATE              DOWN                             /* def. pin state*/

#define TP213_BP                                15U                              /* pin name      */
#define TP213_BM                                (1 << 15)                        /* pin mask      */
#define TP213_PORT                              GPIOB                            /* port name     */
#define GPIOB_PIN_15_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOB_PIN_15_DEFAULT_STATE              DOWN                             /* def. pin state*/
#endif /* GPIOB_EN */


/** define GPIOC pins names *//* ---------------------------------------------------------------- */
#if GPIOC_EN
#define ETH_PWRON_IRQ_BP                        0U                               /* pin name      */
#define ETH_PWRON_IRQ_BM                        (1 << 0)                         /* pin mask      */
#define ETH_PWRON_IRQ_PORT                      GPIOC                            /* port name     */
#define GPIOC_PIN_00_MODE                       GPIO_OUT_OPEN_DRAIN_2MHZ         /* pin mode      */
#define GPIOC_PIN_00_DEFAULT_STATE              HIGH                             /* def. pin state*/

#define ETH_MDC_BP                              1U                               /* pin name      */
#define ETH_MDC_BM                              (1 << 1)                         /* pin mask      */
#define ETH_MDC_PORT                            GPIOC                            /* port name     */
#define GPIOC_PIN_01_MODE                       GPIO_ALT_OUT_PUSH_PULL_50MHZ     /* pin mode      */
#define GPIOC_PIN_01_DEFAULT_STATE              LOW                              /* def. pin state*/

#define ETH_MII_TXD2_BP                         2U                               /* pin name      */
#define ETH_MII_TXD2_BM                         (1 << 2)                         /* pin mask      */
#define ETH_MII_TXD2_PORT                       GPIOC                            /* port name     */
#define GPIOC_PIN_02_MODE                       GPIO_ALT_OUT_PUSH_PULL_50MHZ     /* pin mode      */
#define GPIOC_PIN_02_DEFAULT_STATE              LOW                              /* def. pin state*/

#define ETH_MII_TX_CLK_BP                       3U                               /* pin name      */
#define ETH_MII_TX_CLK_BM                       (1 << 3)                         /* pin mask      */
#define ETH_MII_TX_CLK_PORT                     GPIOC                            /* port name     */
#define GPIOC_PIN_03_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_03_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define ETH_MII_RXD0_BP                         4U                               /* pin name      */
#define ETH_MII_RXD0_BM                         (1 << 4)                         /* pin mask      */
#define ETH_MII_RXD0_PORT                       GPIOC                            /* port name     */
#define GPIOC_PIN_04_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_04_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define ETH_MII_RXD1_BP                         5U                               /* pin name      */
#define ETH_MII_RXD1_BM                         (1 << 5)                         /* pin mask      */
#define ETH_MII_RXD1_PORT                       GPIOC                            /* port name     */
#define GPIOC_PIN_05_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_05_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define TP218_BP                                6U                               /* pin name      */
#define TP218_BM                                (1 << 6)                         /* pin mask      */
#define TP218_PORT                              GPIOC                            /* port name     */
#define GPIOC_PIN_06_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOC_PIN_06_DEFAULT_STATE              DOWN                             /* def. pin state*/

#define TP219_BP                                7U                               /* pin name      */
#define TP219_BM                                (1 << 7)                         /* pin mask      */
#define TP219_PORT                              GPIOC                            /* port name     */
#define GPIOC_PIN_07_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOC_PIN_07_DEFAULT_STATE              DOWN                             /* def. pin state*/

#define TP220_BP                                8U                               /* pin name      */
#define TP220_BM                                (1 << 8)                         /* pin mask      */
#define TP220_PORT                              GPIOC                            /* port name     */
#define GPIOC_PIN_08_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOC_PIN_08_DEFAULT_STATE              DOWN                            /* def. pin state*/

#define TP221_BP                                9U                               /* pin name      */
#define TP221_BM                                (1 << 9)                         /* pin mask      */
#define TP221_PORT                              GPIOC                            /* port name     */
#define GPIOC_PIN_09_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOC_PIN_09_DEFAULT_STATE              DOWN                             /* def. pin state*/

#define SPI_SCK_BP                              10U                              /* pin name      */
#define SPI_SCK_BM                              (1 << 10)                        /* pin mask      */
#define SPI_SCK_PORT                            GPIOC                            /* port name     */
#define GPIOC_PIN_10_MODE                       GPIO_ALT_OUT_PUSH_PULL_50MHZ     /* pin mode      */
#define GPIOC_PIN_10_DEFAULT_STATE              LOW                              /* def. pin state*/

#define SPI_MISO_BP                             11U                              /* pin name      */
#define SPI_MISO_BM                             (1 << 11)                        /* pin mask      */
#define SPI_MISO_PORT                           GPIOC                            /* port name     */
#define GPIOC_PIN_11_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOC_PIN_11_DEFAULT_STATE              DOWN                             /* def. pin state*/

#define SPI_MOSI_BP                             12U                              /* pin name      */
#define SPI_MOSI_BM                             (1 << 12)                        /* pin mask      */
#define SPI_MOSI_PORT                           GPIOC                            /* port name     */
#define GPIOC_PIN_12_MODE                       GPIO_ALT_OUT_PUSH_PULL_50MHZ     /* pin mode      */
#define GPIOC_PIN_12_DEFAULT_STATE              LOW                              /* def. pin state*/

#define TP222_BP                                13U                              /* pin name      */
#define TP222_BM                                (1 << 13)                        /* pin mask      */
#define TP222_PORT                              GPIOC                            /* port name     */
#define GPIOC_PIN_13_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOC_PIN_13_DEFAULT_STATE              DOWN                             /* def. pin state*/

#define TP223_BP                                14U                              /* pin name      */
#define TP223_BM                                (1 << 14)                        /* pin mask      */
#define TP223_PORT                              GPIOC                            /* port name     */
#define GPIOC_PIN_14_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOC_PIN_14_DEFAULT_STATE              DOWN                             /* def. pin state*/

#define TP224_BP                                15U                              /* pin name      */
#define TP224_BM                                (1 << 15)                        /* pin mask      */
#define TP224_PORT                              GPIOC                            /* port name     */
#define GPIOC_PIN_15_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOC_PIN_15_DEFAULT_STATE              DOWN                             /* def. pin state*/
#endif /* GPIOC_EN */


/** define GPIOD pins names *//* ---------------------------------------------------------------- */
#if GPIOD_EN
#define OSC_IN_BP                               0U                               /* pin name      */
#define OSC_IN_BM                               (1 << 0)                         /* pin mask      */
#define OSC_IN_PORT                             GPIOD                            /* port name     */
#define GPIOD_PIN_00_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_00_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define OSC_OUT_BP                              1U                               /* pin name      */
#define OSC_OUT_BM                              (1 << 1)                         /* pin mask      */
#define OSC_OUT_PORT                            GPIOD                            /* port name     */
#define GPIOD_PIN_01_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_01_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define TP216_BP                                2U                               /* pin name      */
#define TP216_BM                                (1 << 2)                         /* pin mask      */
#define TP216_PORT                              GPIOD                            /* port name     */
#define GPIOD_PIN_02_MODE                       GPIO_IN_PULLED                   /* pin mode      */
#define GPIOD_PIN_02_DEFAULT_STATE              DOWN                             /* def. pin state*/

#define NA_GPIOD_PIN_03_BP                      3U                               /* pin name      */
#define NA_GPIOD_PIN_03_BM                      (1 << 3)                         /* pin mask      */
#define NA_GPIOD_PIN_03_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_03_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_03_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define NA_GPIOD_PIN_04_BP                      4U                               /* pin name      */
#define NA_GPIOD_PIN_04_BM                      (1 << 4)                         /* pin mask      */
#define NA_GPIOD_PIN_04_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_04_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_04_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define NA_GPIOD_PIN_05_BP                      5U                               /* pin name      */
#define NA_GPIOD_PIN_05_BM                      (1 << 5)                         /* pin mask      */
#define NA_GPIOD_PIN_05_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_05_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_05_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define NA_GPIOD_PIN_06_BP                      6U                               /* pin name      */
#define NA_GPIOD_PIN_06_BM                      (1 << 6)                         /* pin mask      */
#define NA_GPIOD_PIN_06_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_06_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_06_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define NA_GPIOD_PIN_07_BP                      7U                               /* pin name      */
#define NA_GPIOD_PIN_07_BM                      (1 << 7)                         /* pin mask      */
#define NA_GPIOD_PIN_07_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_07_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_07_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define NA_GPIOD_PIN_08_BP                      8U                               /* pin name      */
#define NA_GPIOD_PIN_08_BM                      (1 << 8)                         /* pin mask      */
#define NA_GPIOD_PIN_08_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_08_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_08_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define NA_GPIOD_PIN_09_BP                      9U                               /* pin name      */
#define NA_GPIOD_PIN_09_BM                      (1 << 9)                         /* pin mask      */
#define NA_GPIOD_PIN_09_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_09_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_09_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define NA_GPIOD_PIN_10_BP                      10U                              /* pin name      */
#define NA_GPIOD_PIN_10_BM                      (1 << 10)                        /* pin mask      */
#define NA_GPIOD_PIN_10_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_10_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_10_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define NA_GPIOD_PIN_11_BP                      11U                              /* pin name      */
#define NA_GPIOD_PIN_11_BM                      (1 << 11)                        /* pin mask      */
#define NA_GPIOD_PIN_11_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_11_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_11_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define NA_GPIOD_PIN_12_BP                      12U                              /* pin name      */
#define NA_GPIOD_PIN_12_BM                      (1 << 12)                        /* pin mask      */
#define NA_GPIOD_PIN_12_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_12_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_12_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define NA_GPIOD_PIN_13_BP                      13U                              /* pin name      */
#define NA_GPIOD_PIN_13_BM                      (1 << 13)                        /* pin mask      */
#define NA_GPIOD_PIN_13_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_13_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_13_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define NA_GPIOD_PIN_14_BP                      14U                              /* pin name      */
#define NA_GPIOD_PIN_14_BM                      (1 << 14)                        /* pin mask      */
#define NA_GPIOD_PIN_14_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_14_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_14_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define NA_GPIOD_PIN_15_BP                      15U                              /* pin name      */
#define NA_GPIOD_PIN_15_BM                      (1 << 15)                        /* pin mask      */
#define NA_GPIOD_PIN_15_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_15_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_15_DEFAULT_STATE              FLOAT                            /* def. pin state*/
#endif /* GPIOD_EN */


/** define GPIOE pins names *//* ---------------------------------------------------------------- */
#if GPIOE_EN
#define MY_GPIOE_PIN_00_BP                      0U                               /* pin name      */
#define MY_GPIOE_PIN_00_BM                      (1 << 0)                         /* pin mask      */
#define MY_GPIOE_PIN_00_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_00_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_00_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_01_BP                      1U                               /* pin name      */
#define MY_GPIOE_PIN_01_BM                      (1 << 1)                         /* pin mask      */
#define MY_GPIOE_PIN_01_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_01_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_01_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_02_BP                      2U                               /* pin name      */
#define MY_GPIOE_PIN_02_BM                      (1 << 2)                         /* pin mask      */
#define MY_GPIOE_PIN_02_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_02_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_02_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_03_BP                      3U                               /* pin name      */
#define MY_GPIOE_PIN_03_BM                      (1 << 3)                         /* pin mask      */
#define MY_GPIOE_PIN_03_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_03_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_03_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_04_BP                      4U                               /* pin name      */
#define MY_GPIOE_PIN_04_BM                      (1 << 4)                         /* pin mask      */
#define MY_GPIOE_PIN_04_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_04_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_04_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_05_BP                      5U                               /* pin name      */
#define MY_GPIOE_PIN_05_BM                      (1 << 5)                         /* pin mask      */
#define MY_GPIOE_PIN_05_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_05_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_05_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_06_BP                      6U                               /* pin name      */
#define MY_GPIOE_PIN_06_BM                      (1 << 6)                         /* pin mask      */
#define MY_GPIOE_PIN_06_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_06_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_06_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_07_BP                      7U                               /* pin name      */
#define MY_GPIOE_PIN_07_BM                      (1 << 7)                         /* pin mask      */
#define MY_GPIOE_PIN_07_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_07_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_07_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_08_BP                      8U                               /* pin name      */
#define MY_GPIOE_PIN_08_BM                      (1 << 8)                         /* pin mask      */
#define MY_GPIOE_PIN_08_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_08_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_08_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_09_BP                      9U                               /* pin name      */
#define MY_GPIOE_PIN_09_BM                      (1 << 9)                         /* pin mask      */
#define MY_GPIOE_PIN_09_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_09_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_09_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_10_BP                      10U                              /* pin name      */
#define MY_GPIOE_PIN_10_BM                      (1 << 10)                        /* pin mask      */
#define MY_GPIOE_PIN_10_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_10_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_10_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_11_BP                      11U                              /* pin name      */
#define MY_GPIOE_PIN_11_BM                      (1 << 11)                        /* pin mask      */
#define MY_GPIOE_PIN_11_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_11_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_11_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_12_BP                      12U                              /* pin name      */
#define MY_GPIOE_PIN_12_BM                      (1 << 12)                        /* pin mask      */
#define MY_GPIOE_PIN_12_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_12_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_12_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_13_BP                      13U                              /* pin name      */
#define MY_GPIOE_PIN_13_BM                      (1 << 13)                        /* pin mask      */
#define MY_GPIOE_PIN_13_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_13_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_13_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_14_BP                      14U                              /* pin name      */
#define MY_GPIOE_PIN_14_BM                      (1 << 14)                        /* pin mask      */
#define MY_GPIOE_PIN_14_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_14_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_14_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOE_PIN_15_BP                      15U                              /* pin name      */
#define MY_GPIOE_PIN_15_BM                      (1 << 15)                        /* pin mask      */
#define MY_GPIOE_PIN_15_PORT                    GPIOE                            /* port name     */
#define GPIOE_PIN_15_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOE_PIN_15_DEFAULT_STATE              FLOAT                            /* def. pin state*/
#endif /* GPIOE_EN */


/** define GPIOF pins names *//* ---------------------------------------------------------------- */
#if GPIOF_EN
#define MY_GPIOF_PIN_00_BP                      0U                               /* pin name      */
#define MY_GPIOF_PIN_00_BM                      (1 << 0)                         /* pin mask      */
#define MY_GPIOF_PIN_00_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_00_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_00_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_01_BP                      1U                               /* pin name      */
#define MY_GPIOF_PIN_01_BM                      (1 << 1)                         /* pin mask      */
#define MY_GPIOF_PIN_01_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_01_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_01_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_02_BP                      2U                               /* pin name      */
#define MY_GPIOF_PIN_02_BM                      (1 << 2)                         /* pin mask      */
#define MY_GPIOF_PIN_02_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_02_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_02_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_03_BP                      3U                               /* pin name      */
#define MY_GPIOF_PIN_03_BM                      (1 << 3)                         /* pin mask      */
#define MY_GPIOF_PIN_03_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_03_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_03_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_04_BP                      4U                               /* pin name      */
#define MY_GPIOF_PIN_04_BM                      (1 << 4)                         /* pin mask      */
#define MY_GPIOF_PIN_04_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_04_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_04_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_05_BP                      5U                               /* pin name      */
#define MY_GPIOF_PIN_05_BM                      (1 << 5)                         /* pin mask      */
#define MY_GPIOF_PIN_05_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_05_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_05_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_06_BP                      6U                               /* pin name      */
#define MY_GPIOF_PIN_06_BM                      (1 << 6)                         /* pin mask      */
#define MY_GPIOF_PIN_06_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_06_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_06_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_07_BP                      7U                               /* pin name      */
#define MY_GPIOF_PIN_07_BM                      (1 << 7)                         /* pin mask      */
#define MY_GPIOF_PIN_07_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_07_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_07_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_08_BP                      8U                               /* pin name      */
#define MY_GPIOF_PIN_08_BM                      (1 << 8)                         /* pin mask      */
#define MY_GPIOF_PIN_08_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_08_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_08_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_09_BP                      9U                               /* pin name      */
#define MY_GPIOF_PIN_09_BM                      (1 << 9)                         /* pin mask      */
#define MY_GPIOF_PIN_09_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_09_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_09_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_10_BP                      10U                              /* pin name      */
#define MY_GPIOF_PIN_10_BM                      (1 << 10)                        /* pin mask      */
#define MY_GPIOF_PIN_10_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_10_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_10_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_11_BP                      11U                              /* pin name      */
#define MY_GPIOF_PIN_11_BM                      (1 << 11)                        /* pin mask      */
#define MY_GPIOF_PIN_11_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_11_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_11_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_12_BP                      12U                              /* pin name      */
#define MY_GPIOF_PIN_12_BM                      (1 << 12)                        /* pin mask      */
#define MY_GPIOF_PIN_12_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_12_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_12_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_13_BP                      13U                              /* pin name      */
#define MY_GPIOF_PIN_13_BM                      (1 << 13)                        /* pin mask      */
#define MY_GPIOF_PIN_13_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_13_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_13_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_14_BP                      14U                              /* pin name      */
#define MY_GPIOF_PIN_14_BM                      (1 << 14)                        /* pin mask      */
#define MY_GPIOF_PIN_14_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_14_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_14_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOF_PIN_15_BP                      15U                              /* pin name      */
#define MY_GPIOF_PIN_15_BM                      (1 << 15)                        /* pin mask      */
#define MY_GPIOF_PIN_15_PORT                    GPIOF                            /* port name     */
#define GPIOF_PIN_15_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOF_PIN_15_DEFAULT_STATE              FLOAT                            /* def. pin state*/
#endif /* GPIOF_EN */


/** define GPIOG pins names *//* ---------------------------------------------------------------- */
#if GPIOG_EN
#define MY_GPIOG_PIN_00_BP                      0U                               /* pin name      */
#define MY_GPIOG_PIN_00_BM                      (1 << 0)                         /* pin mask      */
#define MY_GPIOG_PIN_00_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_00_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_00_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_01_BP                      1U                               /* pin name      */
#define MY_GPIOG_PIN_01_BM                      (1 << 1)                         /* pin mask      */
#define MY_GPIOG_PIN_01_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_01_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_01_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_02_BP                      2U                               /* pin name      */
#define MY_GPIOG_PIN_02_BM                      (1 << 2)                         /* pin mask      */
#define MY_GPIOG_PIN_02_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_02_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_02_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_03_BP                      3U                               /* pin name      */
#define MY_GPIOG_PIN_03_BM                      (1 << 3)                         /* pin mask      */
#define MY_GPIOG_PIN_03_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_03_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_03_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_04_BP                      4U                               /* pin name      */
#define MY_GPIOG_PIN_04_BM                      (1 << 4)                         /* pin mask      */
#define MY_GPIOG_PIN_04_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_04_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_04_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_05_BP                      5U                               /* pin name      */
#define MY_GPIOG_PIN_05_BM                      (1 << 5)                         /* pin mask      */
#define MY_GPIOG_PIN_05_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_05_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_05_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_06_BP                      6U                               /* pin name      */
#define MY_GPIOG_PIN_06_BM                      (1 << 6)                         /* pin mask      */
#define MY_GPIOG_PIN_06_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_06_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_06_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_07_BP                      7U                               /* pin name      */
#define MY_GPIOG_PIN_07_BM                      (1 << 7)                         /* pin mask      */
#define MY_GPIOG_PIN_07_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_07_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_07_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_08_BP                      8U                               /* pin name      */
#define MY_GPIOG_PIN_08_BM                      (1 << 8)                         /* pin mask      */
#define MY_GPIOG_PIN_08_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_08_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_08_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_09_BP                      9U                               /* pin name      */
#define MY_GPIOG_PIN_09_BM                      (1 << 9)                         /* pin mask      */
#define MY_GPIOG_PIN_09_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_09_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_09_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_10_BP                      10U                              /* pin name      */
#define MY_GPIOG_PIN_10_BM                      (1 << 10)                        /* pin mask      */
#define MY_GPIOG_PIN_10_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_10_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_10_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_11_BP                      11U                              /* pin name      */
#define MY_GPIOG_PIN_11_BM                      (1 << 11)                        /* pin mask      */
#define MY_GPIOG_PIN_11_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_11_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_11_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_12_BP                      12U                              /* pin name      */
#define MY_GPIOG_PIN_12_BM                      (1 << 12)                        /* pin mask      */
#define MY_GPIOG_PIN_12_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_12_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_12_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_13_BP                      13U                              /* pin name      */
#define MY_GPIOG_PIN_13_BM                      (1 << 13)                        /* pin mask      */
#define MY_GPIOG_PIN_13_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_13_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_13_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_14_BP                      14U                              /* pin name      */
#define MY_GPIOG_PIN_14_BM                      (1 << 14)                        /* pin mask      */
#define MY_GPIOG_PIN_14_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_14_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_14_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOG_PIN_15_BP                      15U                              /* pin name      */
#define MY_GPIOG_PIN_15_BM                      (1 << 15)                        /* pin mask      */
#define MY_GPIOG_PIN_15_PORT                    GPIOG                            /* port name     */
#define GPIOG_PIN_15_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOG_PIN_15_DEFAULT_STATE              FLOAT                            /* def. pin state*/
#endif /* GPIOG_EN */


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/


#ifdef __cplusplus
   }
#endif

#endif /* GPIO_CFG_H_ */
/*==================================================================================================
                                             End of file
==================================================================================================*/
