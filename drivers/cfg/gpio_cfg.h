#ifndef GPIO_CFG_H_
#define GPIO_CFG_H_
/*=============================================================================================*//**
@file    gpio.h

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
#define MY_GPIOA_PIN_00_BP                      0U                               /* pin name      */
#define MY_GPIOA_PIN_00_BM                      (1 << 0)                         /* pin mask      */
#define MY_GPIOA_PIN_00_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_00_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_00_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_01_BP                      1U                               /* pin name      */
#define MY_GPIOA_PIN_01_BM                      (1 << 1)                         /* pin mask      */
#define MY_GPIOA_PIN_01_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_01_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_01_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_02_BP                      2U                               /* pin name      */
#define MY_GPIOA_PIN_02_BM                      (1 << 2)                         /* pin mask      */
#define MY_GPIOA_PIN_02_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_02_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_02_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_03_BP                      3U                               /* pin name      */
#define MY_GPIOA_PIN_03_BM                      (1 << 3)                         /* pin mask      */
#define MY_GPIOA_PIN_03_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_03_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_03_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_04_BP                      4U                               /* pin name      */
#define MY_GPIOA_PIN_04_BM                      (1 << 4)                         /* pin mask      */
#define MY_GPIOA_PIN_04_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_04_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_04_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_05_BP                      5U                               /* pin name      */
#define MY_GPIOA_PIN_05_BM                      (1 << 5)                         /* pin mask      */
#define MY_GPIOA_PIN_05_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_05_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_05_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_06_BP                      6U                               /* pin name      */
#define MY_GPIOA_PIN_06_BM                      (1 << 6)                         /* pin mask      */
#define MY_GPIOA_PIN_06_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_06_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_06_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_07_BP                      7U                               /* pin name      */
#define MY_GPIOA_PIN_07_BM                      (1 << 7)                         /* pin mask      */
#define MY_GPIOA_PIN_07_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_07_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_07_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_08_BP                      8U                               /* pin name      */
#define MY_GPIOA_PIN_08_BM                      (1 << 8)                         /* pin mask      */
#define MY_GPIOA_PIN_08_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_08_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_08_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_09_BP                      9U                               /* pin name      */
#define MY_GPIOA_PIN_09_BM                      (1 << 9)                         /* pin mask      */
#define MY_GPIOA_PIN_09_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_09_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_09_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_10_BP                      10U                              /* pin name      */
#define MY_GPIOA_PIN_10_BM                      (1 << 10)                        /* pin mask      */
#define MY_GPIOA_PIN_10_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_10_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_10_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_11_BP                      11U                              /* pin name      */
#define MY_GPIOA_PIN_11_BM                      (1 << 11)                        /* pin mask      */
#define MY_GPIOA_PIN_11_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_11_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_11_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_12_BP                      12U                              /* pin name      */
#define MY_GPIOA_PIN_12_BM                      (1 << 12)                        /* pin mask      */
#define MY_GPIOA_PIN_12_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_12_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_12_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_13_BP                      13U                              /* pin name      */
#define MY_GPIOA_PIN_13_BM                      (1 << 13)                        /* pin mask      */
#define MY_GPIOA_PIN_13_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_13_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_13_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_14_BP                      14U                              /* pin name      */
#define MY_GPIOA_PIN_14_BM                      (1 << 14)                        /* pin mask      */
#define MY_GPIOA_PIN_14_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_14_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_14_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOA_PIN_15_BP                      15U                              /* pin name      */
#define MY_GPIOA_PIN_15_BM                      (1 << 15)                        /* pin mask      */
#define MY_GPIOA_PIN_15_PORT                    GPIOA                            /* port name     */
#define GPIOA_PIN_15_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOA_PIN_15_DEFAULT_STATE              FLOAT                            /* def. pin state*/
#endif /* GPIOA_EN */

/** define GPIOB pins names *//* ---------------------------------------------------------------- */
#if GPIOB_EN
#define MY_GPIOB_PIN_00_BP                      0U                               /* pin name      */
#define MY_GPIOB_PIN_00_BM                      (1 << 0)                         /* pin mask      */
#define MY_GPIOB_PIN_00_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_00_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_00_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_01_BP                      1U                               /* pin name      */
#define MY_GPIOB_PIN_01_BM                      (1 << 1)                         /* pin mask      */
#define MY_GPIOB_PIN_01_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_01_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_01_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_02_BP                      2U                               /* pin name      */
#define MY_GPIOB_PIN_02_BM                      (1 << 2)                         /* pin mask      */
#define MY_GPIOB_PIN_02_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_02_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_02_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_03_BP                      3U                               /* pin name      */
#define MY_GPIOB_PIN_03_BM                      (1 << 3)                         /* pin mask      */
#define MY_GPIOB_PIN_03_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_03_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_03_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_04_BP                      4U                               /* pin name      */
#define MY_GPIOB_PIN_04_BM                      (1 << 4)                         /* pin mask      */
#define MY_GPIOB_PIN_04_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_04_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_04_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_05_BP                      5U                               /* pin name      */
#define MY_GPIOB_PIN_05_BM                      (1 << 5)                         /* pin mask      */
#define MY_GPIOB_PIN_05_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_05_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_05_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_06_BP                      6U                               /* pin name      */
#define MY_GPIOB_PIN_06_BM                      (1 << 6)                         /* pin mask      */
#define MY_GPIOB_PIN_06_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_06_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_06_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_07_BP                      7U                               /* pin name      */
#define MY_GPIOB_PIN_07_BM                      (1 << 7)                         /* pin mask      */
#define MY_GPIOB_PIN_07_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_07_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_07_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_08_BP                      8U                               /* pin name      */
#define MY_GPIOB_PIN_08_BM                      (1 << 8)                         /* pin mask      */
#define MY_GPIOB_PIN_08_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_08_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_08_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_09_BP                      9U                               /* pin name      */
#define MY_GPIOB_PIN_09_BM                      (1 << 9)                         /* pin mask      */
#define MY_GPIOB_PIN_09_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_09_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_09_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_10_BP                      10U                              /* pin name      */
#define MY_GPIOB_PIN_10_BM                      (1 << 10)                        /* pin mask      */
#define MY_GPIOB_PIN_10_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_10_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_10_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_11_BP                      11U                              /* pin name      */
#define MY_GPIOB_PIN_11_BM                      (1 << 11)                        /* pin mask      */
#define MY_GPIOB_PIN_11_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_11_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_11_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_12_BP                      12U                              /* pin name      */
#define MY_GPIOB_PIN_12_BM                      (1 << 12)                        /* pin mask      */
#define MY_GPIOB_PIN_12_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_12_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_12_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_13_BP                      13U                              /* pin name      */
#define MY_GPIOB_PIN_13_BM                      (1 << 13)                        /* pin mask      */
#define MY_GPIOB_PIN_13_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_13_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_13_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_14_BP                      14U                              /* pin name      */
#define MY_GPIOB_PIN_14_BM                      (1 << 14)                        /* pin mask      */
#define MY_GPIOB_PIN_14_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_14_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_14_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOB_PIN_15_BP                      15U                              /* pin name      */
#define MY_GPIOB_PIN_15_BM                      (1 << 15)                        /* pin mask      */
#define MY_GPIOB_PIN_15_PORT                    GPIOB                            /* port name     */
#define GPIOB_PIN_15_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOB_PIN_15_DEFAULT_STATE              FLOAT                            /* def. pin state*/
#endif /* GPIOB_EN */


/** define GPIOC pins names *//* ---------------------------------------------------------------- */
#if GPIOC_EN
#define MY_GPIOC_PIN_00_BP                      0U                               /* pin name      */
#define MY_GPIOC_PIN_00_BM                      (1 << 0)                         /* pin mask      */
#define MY_GPIOC_PIN_00_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_00_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_00_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_01_BP                      1U                               /* pin name      */
#define MY_GPIOC_PIN_01_BM                      (1 << 1)                         /* pin mask      */
#define MY_GPIOC_PIN_01_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_01_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_01_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_02_BP                      2U                               /* pin name      */
#define MY_GPIOC_PIN_02_BM                      (1 << 2)                         /* pin mask      */
#define MY_GPIOC_PIN_02_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_02_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_02_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_03_BP                      3U                               /* pin name      */
#define MY_GPIOC_PIN_03_BM                      (1 << 3)                         /* pin mask      */
#define MY_GPIOC_PIN_03_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_03_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_03_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_04_BP                      4U                               /* pin name      */
#define MY_GPIOC_PIN_04_BM                      (1 << 4)                         /* pin mask      */
#define MY_GPIOC_PIN_04_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_04_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_04_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_05_BP                      5U                               /* pin name      */
#define MY_GPIOC_PIN_05_BM                      (1 << 5)                         /* pin mask      */
#define MY_GPIOC_PIN_05_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_05_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_05_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_06_BP                      6U                               /* pin name      */
#define MY_GPIOC_PIN_06_BM                      (1 << 6)                         /* pin mask      */
#define MY_GPIOC_PIN_06_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_06_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_06_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_07_BP                      7U                               /* pin name      */
#define MY_GPIOC_PIN_07_BM                      (1 << 7)                         /* pin mask      */
#define MY_GPIOC_PIN_07_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_07_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_07_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_08_BP                      8U                               /* pin name      */
#define MY_GPIOC_PIN_08_BM                      (1 << 8)                         /* pin mask      */
#define MY_GPIOC_PIN_08_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_08_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_08_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_09_BP                      9U                               /* pin name      */
#define MY_GPIOC_PIN_09_BM                      (1 << 9)                         /* pin mask      */
#define MY_GPIOC_PIN_09_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_09_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_09_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_10_BP                      10U                              /* pin name      */
#define MY_GPIOC_PIN_10_BM                      (1 << 10)                        /* pin mask      */
#define MY_GPIOC_PIN_10_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_10_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_10_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_11_BP                      11U                              /* pin name      */
#define MY_GPIOC_PIN_11_BM                      (1 << 11)                        /* pin mask      */
#define MY_GPIOC_PIN_11_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_11_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_11_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_12_BP                      12U                              /* pin name      */
#define MY_GPIOC_PIN_12_BM                      (1 << 12)                        /* pin mask      */
#define MY_GPIOC_PIN_12_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_12_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_12_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_13_BP                      13U                              /* pin name      */
#define MY_GPIOC_PIN_13_BM                      (1 << 13)                        /* pin mask      */
#define MY_GPIOC_PIN_13_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_13_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_13_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_14_BP                      14U                              /* pin name      */
#define MY_GPIOC_PIN_14_BM                      (1 << 14)                        /* pin mask      */
#define MY_GPIOC_PIN_14_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_14_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_14_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOC_PIN_15_BP                      15U                              /* pin name      */
#define MY_GPIOC_PIN_15_BM                      (1 << 15)                        /* pin mask      */
#define MY_GPIOC_PIN_15_PORT                    GPIOC                            /* port name     */
#define GPIOC_PIN_15_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOC_PIN_15_DEFAULT_STATE              FLOAT                            /* def. pin state*/
#endif /* GPIOC_EN */


/** define GPIOD pins names *//* ---------------------------------------------------------------- */
#if GPIOD_EN
#define MY_GPIOD_PIN_00_BP                      0U                               /* pin name      */
#define MY_GPIOD_PIN_00_BM                      (1 << 0)                         /* pin mask      */
#define MY_GPIOD_PIN_00_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_00_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_00_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_01_BP                      1U                               /* pin name      */
#define MY_GPIOD_PIN_01_BM                      (1 << 1)                         /* pin mask      */
#define MY_GPIOD_PIN_01_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_01_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_01_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_02_BP                      2U                               /* pin name      */
#define MY_GPIOD_PIN_02_BM                      (1 << 2)                         /* pin mask      */
#define MY_GPIOD_PIN_02_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_02_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_02_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_03_BP                      3U                               /* pin name      */
#define MY_GPIOD_PIN_03_BM                      (1 << 3)                         /* pin mask      */
#define MY_GPIOD_PIN_03_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_03_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_03_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_04_BP                      4U                               /* pin name      */
#define MY_GPIOD_PIN_04_BM                      (1 << 4)                         /* pin mask      */
#define MY_GPIOD_PIN_04_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_04_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_04_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_05_BP                      5U                               /* pin name      */
#define MY_GPIOD_PIN_05_BM                      (1 << 5)                         /* pin mask      */
#define MY_GPIOD_PIN_05_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_05_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_05_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_06_BP                      6U                               /* pin name      */
#define MY_GPIOD_PIN_06_BM                      (1 << 6)                         /* pin mask      */
#define MY_GPIOD_PIN_06_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_06_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_06_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_07_BP                      7U                               /* pin name      */
#define MY_GPIOD_PIN_07_BM                      (1 << 7)                         /* pin mask      */
#define MY_GPIOD_PIN_07_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_07_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_07_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_08_BP                      8U                               /* pin name      */
#define MY_GPIOD_PIN_08_BM                      (1 << 8)                         /* pin mask      */
#define MY_GPIOD_PIN_08_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_08_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_08_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_09_BP                      9U                               /* pin name      */
#define MY_GPIOD_PIN_09_BM                      (1 << 9)                         /* pin mask      */
#define MY_GPIOD_PIN_09_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_09_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_09_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_10_BP                      10U                              /* pin name      */
#define MY_GPIOD_PIN_10_BM                      (1 << 10)                        /* pin mask      */
#define MY_GPIOD_PIN_10_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_10_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_10_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_11_BP                      11U                              /* pin name      */
#define MY_GPIOD_PIN_11_BM                      (1 << 11)                        /* pin mask      */
#define MY_GPIOD_PIN_11_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_11_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_11_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_12_BP                      12U                              /* pin name      */
#define MY_GPIOD_PIN_12_BM                      (1 << 12)                        /* pin mask      */
#define MY_GPIOD_PIN_12_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_12_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_12_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_13_BP                      13U                              /* pin name      */
#define MY_GPIOD_PIN_13_BM                      (1 << 13)                        /* pin mask      */
#define MY_GPIOD_PIN_13_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_13_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_13_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_14_BP                      14U                              /* pin name      */
#define MY_GPIOD_PIN_14_BM                      (1 << 14)                        /* pin mask      */
#define MY_GPIOD_PIN_14_PORT                    GPIOD                            /* port name     */
#define GPIOD_PIN_14_MODE                       GPIO_IN_FLOAT                    /* pin mode      */
#define GPIOD_PIN_14_DEFAULT_STATE              FLOAT                            /* def. pin state*/

#define MY_GPIOD_PIN_15_BP                      15U                              /* pin name      */
#define MY_GPIOD_PIN_15_BM                      (1 << 15)                        /* pin mask      */
#define MY_GPIOD_PIN_15_PORT                    GPIOD                            /* port name     */
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
