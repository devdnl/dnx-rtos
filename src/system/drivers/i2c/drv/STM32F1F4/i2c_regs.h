/*==============================================================================
File    i2c_regs.h

Author  Daniel Zorychta

Brief   STM32F1,F4 I2C driver registers definition.

        Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup _PI2C_REGS_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _PI2C_REGS_H_
#define _PI2C_REGS_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/*******************  Bit definition for PI2C_CR1 register  ********************/
#define PI2C_CR1_PE_Pos            (0U)
#define PI2C_CR1_PE_Msk            (0x1U << PI2C_CR1_PE_Pos)                     /*!< 0x00000001 */
#define PI2C_CR1_PE                PI2C_CR1_PE_Msk                               /*!<Peripheral Enable                             */
#define PI2C_CR1_SMBUS_Pos         (1U)
#define PI2C_CR1_SMBUS_Msk         (0x1U << PI2C_CR1_SMBUS_Pos)                  /*!< 0x00000002 */
#define PI2C_CR1_SMBUS             PI2C_CR1_SMBUS_Msk                            /*!<SMBus Mode                                    */
#define PI2C_CR1_SMBTYPE_Pos       (3U)
#define PI2C_CR1_SMBTYPE_Msk       (0x1U << PI2C_CR1_SMBTYPE_Pos)                /*!< 0x00000008 */
#define PI2C_CR1_SMBTYPE           PI2C_CR1_SMBTYPE_Msk                          /*!<SMBus Type                                    */
#define PI2C_CR1_ENARP_Pos         (4U)
#define PI2C_CR1_ENARP_Msk         (0x1U << PI2C_CR1_ENARP_Pos)                  /*!< 0x00000010 */
#define PI2C_CR1_ENARP             PI2C_CR1_ENARP_Msk                            /*!<ARP Enable                                    */
#define PI2C_CR1_ENPEC_Pos         (5U)
#define PI2C_CR1_ENPEC_Msk         (0x1U << PI2C_CR1_ENPEC_Pos)                  /*!< 0x00000020 */
#define PI2C_CR1_ENPEC             PI2C_CR1_ENPEC_Msk                            /*!<PEC Enable                                    */
#define PI2C_CR1_ENGC_Pos          (6U)
#define PI2C_CR1_ENGC_Msk          (0x1U << PI2C_CR1_ENGC_Pos)                   /*!< 0x00000040 */
#define PI2C_CR1_ENGC              PI2C_CR1_ENGC_Msk                             /*!<General Call Enable                           */
#define PI2C_CR1_NOSTRETCH_Pos     (7U)
#define PI2C_CR1_NOSTRETCH_Msk     (0x1U << PI2C_CR1_NOSTRETCH_Pos)              /*!< 0x00000080 */
#define PI2C_CR1_NOSTRETCH         PI2C_CR1_NOSTRETCH_Msk                        /*!<Clock Stretching Disable (Slave mode)         */
#define PI2C_CR1_START_Pos         (8U)
#define PI2C_CR1_START_Msk         (0x1U << PI2C_CR1_START_Pos)                  /*!< 0x00000100 */
#define PI2C_CR1_START             PI2C_CR1_START_Msk                            /*!<Start Generation                              */
#define PI2C_CR1_STOP_Pos          (9U)
#define PI2C_CR1_STOP_Msk          (0x1U << PI2C_CR1_STOP_Pos)                   /*!< 0x00000200 */
#define PI2C_CR1_STOP              PI2C_CR1_STOP_Msk                             /*!<Stop Generation                               */
#define PI2C_CR1_ACK_Pos           (10U)
#define PI2C_CR1_ACK_Msk           (0x1U << PI2C_CR1_ACK_Pos)                    /*!< 0x00000400 */
#define PI2C_CR1_ACK               PI2C_CR1_ACK_Msk                              /*!<Acknowledge Enable                            */
#define PI2C_CR1_POS_Pos           (11U)
#define PI2C_CR1_POS_Msk           (0x1U << PI2C_CR1_POS_Pos)                    /*!< 0x00000800 */
#define PI2C_CR1_POS               PI2C_CR1_POS_Msk                              /*!<Acknowledge/PEC Position (for data reception) */
#define PI2C_CR1_PEC_Pos           (12U)
#define PI2C_CR1_PEC_Msk           (0x1U << PI2C_CR1_PEC_Pos)                    /*!< 0x00001000 */
#define PI2C_CR1_PEC               PI2C_CR1_PEC_Msk                              /*!<Packet Error Checking                         */
#define PI2C_CR1_ALERT_Pos         (13U)
#define PI2C_CR1_ALERT_Msk         (0x1U << PI2C_CR1_ALERT_Pos)                  /*!< 0x00002000 */
#define PI2C_CR1_ALERT             PI2C_CR1_ALERT_Msk                            /*!<SMBus Alert                                   */
#define PI2C_CR1_SWRST_Pos         (15U)
#define PI2C_CR1_SWRST_Msk         (0x1U << PI2C_CR1_SWRST_Pos)                  /*!< 0x00008000 */
#define PI2C_CR1_SWRST             PI2C_CR1_SWRST_Msk                            /*!<Software Reset                                */

/*******************  Bit definition for PI2C_CR2 register  ********************/
#define PI2C_CR2_FREQ_Pos          (0U)
#define PI2C_CR2_FREQ_Msk          (0x3FU << PI2C_CR2_FREQ_Pos)                  /*!< 0x0000003F */
#define PI2C_CR2_FREQ              PI2C_CR2_FREQ_Msk                             /*!<FREQ[5:0] bits (Peripheral Clock Frequency)   */
#define PI2C_CR2_FREQ_0            (0x01U << PI2C_CR2_FREQ_Pos)                  /*!< 0x00000001 */
#define PI2C_CR2_FREQ_1            (0x02U << PI2C_CR2_FREQ_Pos)                  /*!< 0x00000002 */
#define PI2C_CR2_FREQ_2            (0x04U << PI2C_CR2_FREQ_Pos)                  /*!< 0x00000004 */
#define PI2C_CR2_FREQ_3            (0x08U << PI2C_CR2_FREQ_Pos)                  /*!< 0x00000008 */
#define PI2C_CR2_FREQ_4            (0x10U << PI2C_CR2_FREQ_Pos)                  /*!< 0x00000010 */
#define PI2C_CR2_FREQ_5            (0x20U << PI2C_CR2_FREQ_Pos)                  /*!< 0x00000020 */

#define PI2C_CR2_ITERREN_Pos       (8U)
#define PI2C_CR2_ITERREN_Msk       (0x1U << PI2C_CR2_ITERREN_Pos)                /*!< 0x00000100 */
#define PI2C_CR2_ITERREN           PI2C_CR2_ITERREN_Msk                          /*!<Error Interrupt Enable  */
#define PI2C_CR2_ITEVTEN_Pos       (9U)
#define PI2C_CR2_ITEVTEN_Msk       (0x1U << PI2C_CR2_ITEVTEN_Pos)                /*!< 0x00000200 */
#define PI2C_CR2_ITEVTEN           PI2C_CR2_ITEVTEN_Msk                          /*!<Event Interrupt Enable  */
#define PI2C_CR2_ITBUFEN_Pos       (10U)
#define PI2C_CR2_ITBUFEN_Msk       (0x1U << PI2C_CR2_ITBUFEN_Pos)                /*!< 0x00000400 */
#define PI2C_CR2_ITBUFEN           PI2C_CR2_ITBUFEN_Msk                          /*!<Buffer Interrupt Enable */
#define PI2C_CR2_DMAEN_Pos         (11U)
#define PI2C_CR2_DMAEN_Msk         (0x1U << PI2C_CR2_DMAEN_Pos)                  /*!< 0x00000800 */
#define PI2C_CR2_DMAEN             PI2C_CR2_DMAEN_Msk                            /*!<DMA Requests Enable     */
#define PI2C_CR2_LAST_Pos          (12U)
#define PI2C_CR2_LAST_Msk          (0x1U << PI2C_CR2_LAST_Pos)                   /*!< 0x00001000 */
#define PI2C_CR2_LAST              PI2C_CR2_LAST_Msk                             /*!<DMA Last Transfer       */

/*******************  Bit definition for PI2C_OAR1 register  *******************/
#define PI2C_OAR1_ADD1_7           0x000000FEU                                  /*!<Interface Address */
#define PI2C_OAR1_ADD8_9           0x00000300U                                  /*!<Interface Address */

#define PI2C_OAR1_ADD0_Pos         (0U)
#define PI2C_OAR1_ADD0_Msk         (0x1U << PI2C_OAR1_ADD0_Pos)                  /*!< 0x00000001 */
#define PI2C_OAR1_ADD0             PI2C_OAR1_ADD0_Msk                            /*!<Bit 0 */
#define PI2C_OAR1_ADD1_Pos         (1U)
#define PI2C_OAR1_ADD1_Msk         (0x1U << PI2C_OAR1_ADD1_Pos)                  /*!< 0x00000002 */
#define PI2C_OAR1_ADD1             PI2C_OAR1_ADD1_Msk                            /*!<Bit 1 */
#define PI2C_OAR1_ADD2_Pos         (2U)
#define PI2C_OAR1_ADD2_Msk         (0x1U << PI2C_OAR1_ADD2_Pos)                  /*!< 0x00000004 */
#define PI2C_OAR1_ADD2             PI2C_OAR1_ADD2_Msk                            /*!<Bit 2 */
#define PI2C_OAR1_ADD3_Pos         (3U)
#define PI2C_OAR1_ADD3_Msk         (0x1U << PI2C_OAR1_ADD3_Pos)                  /*!< 0x00000008 */
#define PI2C_OAR1_ADD3             PI2C_OAR1_ADD3_Msk                            /*!<Bit 3 */
#define PI2C_OAR1_ADD4_Pos         (4U)
#define PI2C_OAR1_ADD4_Msk         (0x1U << PI2C_OAR1_ADD4_Pos)                  /*!< 0x00000010 */
#define PI2C_OAR1_ADD4             PI2C_OAR1_ADD4_Msk                            /*!<Bit 4 */
#define PI2C_OAR1_ADD5_Pos         (5U)
#define PI2C_OAR1_ADD5_Msk         (0x1U << PI2C_OAR1_ADD5_Pos)                  /*!< 0x00000020 */
#define PI2C_OAR1_ADD5             PI2C_OAR1_ADD5_Msk                            /*!<Bit 5 */
#define PI2C_OAR1_ADD6_Pos         (6U)
#define PI2C_OAR1_ADD6_Msk         (0x1U << PI2C_OAR1_ADD6_Pos)                  /*!< 0x00000040 */
#define PI2C_OAR1_ADD6             PI2C_OAR1_ADD6_Msk                            /*!<Bit 6 */
#define PI2C_OAR1_ADD7_Pos         (7U)
#define PI2C_OAR1_ADD7_Msk         (0x1U << PI2C_OAR1_ADD7_Pos)                  /*!< 0x00000080 */
#define PI2C_OAR1_ADD7             PI2C_OAR1_ADD7_Msk                            /*!<Bit 7 */
#define PI2C_OAR1_ADD8_Pos         (8U)
#define PI2C_OAR1_ADD8_Msk         (0x1U << PI2C_OAR1_ADD8_Pos)                  /*!< 0x00000100 */
#define PI2C_OAR1_ADD8             PI2C_OAR1_ADD8_Msk                            /*!<Bit 8 */
#define PI2C_OAR1_ADD9_Pos         (9U)
#define PI2C_OAR1_ADD9_Msk         (0x1U << PI2C_OAR1_ADD9_Pos)                  /*!< 0x00000200 */
#define PI2C_OAR1_ADD9             PI2C_OAR1_ADD9_Msk                            /*!<Bit 9 */

#define PI2C_OAR1_ADDMODE_Pos      (15U)
#define PI2C_OAR1_ADDMODE_Msk      (0x1U << PI2C_OAR1_ADDMODE_Pos)               /*!< 0x00008000 */
#define PI2C_OAR1_ADDMODE          PI2C_OAR1_ADDMODE_Msk                         /*!<Addressing Mode (Slave mode) */

/*******************  Bit definition for PI2C_OAR2 register  *******************/
#define PI2C_OAR2_ENDUAL_Pos       (0U)
#define PI2C_OAR2_ENDUAL_Msk       (0x1U << PI2C_OAR2_ENDUAL_Pos)                /*!< 0x00000001 */
#define PI2C_OAR2_ENDUAL           PI2C_OAR2_ENDUAL_Msk                          /*!<Dual addressing mode enable */
#define PI2C_OAR2_ADD2_Pos         (1U)
#define PI2C_OAR2_ADD2_Msk         (0x7FU << PI2C_OAR2_ADD2_Pos)                 /*!< 0x000000FE */
#define PI2C_OAR2_ADD2             PI2C_OAR2_ADD2_Msk                            /*!<Interface address           */

/********************  Bit definition for PI2C_DR register  ********************/
#define PI2C_DR_DR_Pos             (0U)
#define PI2C_DR_DR_Msk             (0xFFU << PI2C_DR_DR_Pos)                     /*!< 0x000000FF */
#define PI2C_DR_DR                 PI2C_DR_DR_Msk                                /*!<8-bit Data Register         */

/*******************  Bit definition for PI2C_SR1 register  ********************/
#define PI2C_SR1_SB_Pos            (0U)
#define PI2C_SR1_SB_Msk            (0x1U << PI2C_SR1_SB_Pos)                     /*!< 0x00000001 */
#define PI2C_SR1_SB                PI2C_SR1_SB_Msk                               /*!<Start Bit (Master mode)                         */
#define PI2C_SR1_ADDR_Pos          (1U)
#define PI2C_SR1_ADDR_Msk          (0x1U << PI2C_SR1_ADDR_Pos)                   /*!< 0x00000002 */
#define PI2C_SR1_ADDR              PI2C_SR1_ADDR_Msk                             /*!<Address sent (master mode)/matched (slave mode) */
#define PI2C_SR1_BTF_Pos           (2U)
#define PI2C_SR1_BTF_Msk           (0x1U << PI2C_SR1_BTF_Pos)                    /*!< 0x00000004 */
#define PI2C_SR1_BTF               PI2C_SR1_BTF_Msk                              /*!<Byte Transfer Finished                          */
#define PI2C_SR1_ADD10_Pos         (3U)
#define PI2C_SR1_ADD10_Msk         (0x1U << PI2C_SR1_ADD10_Pos)                  /*!< 0x00000008 */
#define PI2C_SR1_ADD10             PI2C_SR1_ADD10_Msk                            /*!<10-bit header sent (Master mode)                */
#define PI2C_SR1_STOPF_Pos         (4U)
#define PI2C_SR1_STOPF_Msk         (0x1U << PI2C_SR1_STOPF_Pos)                  /*!< 0x00000010 */
#define PI2C_SR1_STOPF             PI2C_SR1_STOPF_Msk                            /*!<Stop detection (Slave mode)                     */
#define PI2C_SR1_RXNE_Pos          (6U)
#define PI2C_SR1_RXNE_Msk          (0x1U << PI2C_SR1_RXNE_Pos)                   /*!< 0x00000040 */
#define PI2C_SR1_RXNE              PI2C_SR1_RXNE_Msk                             /*!<Data Register not Empty (receivers)             */
#define PI2C_SR1_TXE_Pos           (7U)
#define PI2C_SR1_TXE_Msk           (0x1U << PI2C_SR1_TXE_Pos)                    /*!< 0x00000080 */
#define PI2C_SR1_TXE               PI2C_SR1_TXE_Msk                              /*!<Data Register Empty (transmitters)              */
#define PI2C_SR1_BERR_Pos          (8U)
#define PI2C_SR1_BERR_Msk          (0x1U << PI2C_SR1_BERR_Pos)                   /*!< 0x00000100 */
#define PI2C_SR1_BERR              PI2C_SR1_BERR_Msk                             /*!<Bus Error                                       */
#define PI2C_SR1_ARLO_Pos          (9U)
#define PI2C_SR1_ARLO_Msk          (0x1U << PI2C_SR1_ARLO_Pos)                   /*!< 0x00000200 */
#define PI2C_SR1_ARLO              PI2C_SR1_ARLO_Msk                             /*!<Arbitration Lost (master mode)                  */
#define PI2C_SR1_AF_Pos            (10U)
#define PI2C_SR1_AF_Msk            (0x1U << PI2C_SR1_AF_Pos)                     /*!< 0x00000400 */
#define PI2C_SR1_AF                PI2C_SR1_AF_Msk                               /*!<Acknowledge Failure                             */
#define PI2C_SR1_OVR_Pos           (11U)
#define PI2C_SR1_OVR_Msk           (0x1U << PI2C_SR1_OVR_Pos)                    /*!< 0x00000800 */
#define PI2C_SR1_OVR               PI2C_SR1_OVR_Msk                              /*!<Overrun/Underrun                                */
#define PI2C_SR1_PECERR_Pos        (12U)
#define PI2C_SR1_PECERR_Msk        (0x1U << PI2C_SR1_PECERR_Pos)                 /*!< 0x00001000 */
#define PI2C_SR1_PECERR            PI2C_SR1_PECERR_Msk                           /*!<PEC Error in reception                          */
#define PI2C_SR1_TIMEOUT_Pos       (14U)
#define PI2C_SR1_TIMEOUT_Msk       (0x1U << PI2C_SR1_TIMEOUT_Pos)                /*!< 0x00004000 */
#define PI2C_SR1_TIMEOUT           PI2C_SR1_TIMEOUT_Msk                          /*!<Timeout or Tlow Error                           */
#define PI2C_SR1_SMBALERT_Pos      (15U)
#define PI2C_SR1_SMBALERT_Msk      (0x1U << PI2C_SR1_SMBALERT_Pos)               /*!< 0x00008000 */
#define PI2C_SR1_SMBALERT          PI2C_SR1_SMBALERT_Msk                         /*!<SMBus Alert                                     */

/*******************  Bit definition for PI2C_SR2 register  ********************/
#define PI2C_SR2_MSL_Pos           (0U)
#define PI2C_SR2_MSL_Msk           (0x1U << PI2C_SR2_MSL_Pos)                    /*!< 0x00000001 */
#define PI2C_SR2_MSL               PI2C_SR2_MSL_Msk                              /*!<Master/Slave                                    */
#define PI2C_SR2_BUSY_Pos          (1U)
#define PI2C_SR2_BUSY_Msk          (0x1U << PI2C_SR2_BUSY_Pos)                   /*!< 0x00000002 */
#define PI2C_SR2_BUSY              PI2C_SR2_BUSY_Msk                             /*!<Bus Busy                                        */
#define PI2C_SR2_TRA_Pos           (2U)
#define PI2C_SR2_TRA_Msk           (0x1U << PI2C_SR2_TRA_Pos)                    /*!< 0x00000004 */
#define PI2C_SR2_TRA               PI2C_SR2_TRA_Msk                              /*!<Transmitter/Receiver                            */
#define PI2C_SR2_GENCALL_Pos       (4U)
#define PI2C_SR2_GENCALL_Msk       (0x1U << PI2C_SR2_GENCALL_Pos)                /*!< 0x00000010 */
#define PI2C_SR2_GENCALL           PI2C_SR2_GENCALL_Msk                          /*!<General Call Address (Slave mode)               */
#define PI2C_SR2_SMBDEFAULT_Pos    (5U)
#define PI2C_SR2_SMBDEFAULT_Msk    (0x1U << PI2C_SR2_SMBDEFAULT_Pos)             /*!< 0x00000020 */
#define PI2C_SR2_SMBDEFAULT        PI2C_SR2_SMBDEFAULT_Msk                       /*!<SMBus Device Default Address (Slave mode)       */
#define PI2C_SR2_SMBHOST_Pos       (6U)
#define PI2C_SR2_SMBHOST_Msk       (0x1U << PI2C_SR2_SMBHOST_Pos)                /*!< 0x00000040 */
#define PI2C_SR2_SMBHOST           PI2C_SR2_SMBHOST_Msk                          /*!<SMBus Host Header (Slave mode)                  */
#define PI2C_SR2_DUALF_Pos         (7U)
#define PI2C_SR2_DUALF_Msk         (0x1U << PI2C_SR2_DUALF_Pos)                  /*!< 0x00000080 */
#define PI2C_SR2_DUALF             PI2C_SR2_DUALF_Msk                            /*!<Dual Flag (Slave mode)                          */
#define PI2C_SR2_PEC_Pos           (8U)
#define PI2C_SR2_PEC_Msk           (0xFFU << PI2C_SR2_PEC_Pos)                   /*!< 0x0000FF00 */
#define PI2C_SR2_PEC               PI2C_SR2_PEC_Msk                              /*!<Packet Error Checking Register                  */

/*******************  Bit definition for PI2C_CCR register  ********************/
#define PI2C_CCR_CCR_Pos           (0U)
#define PI2C_CCR_CCR_Msk           (0xFFFU << PI2C_CCR_CCR_Pos)                  /*!< 0x00000FFF */
#define PI2C_CCR_CCR               PI2C_CCR_CCR_Msk                              /*!<Clock Control Register in Fast/Standard mode (Master mode) */
#define PI2C_CCR_DUTY_Pos          (14U)
#define PI2C_CCR_DUTY_Msk          (0x1U << PI2C_CCR_DUTY_Pos)                   /*!< 0x00004000 */
#define PI2C_CCR_DUTY              PI2C_CCR_DUTY_Msk                             /*!<Fast Mode Duty Cycle                                       */
#define PI2C_CCR_FS_Pos            (15U)
#define PI2C_CCR_FS_Msk            (0x1U << PI2C_CCR_FS_Pos)                     /*!< 0x00008000 */
#define PI2C_CCR_FS                PI2C_CCR_FS_Msk                               /*!<I2C Master Mode Selection                                  */

/******************  Bit definition for PI2C_TRISE register  *******************/
#define PI2C_TRISE_TRISE_Pos       (0U)
#define PI2C_TRISE_TRISE_Msk       (0x3FU << PI2C_TRISE_TRISE_Pos)               /*!< 0x0000003F */
#define PI2C_TRISE_TRISE           PI2C_TRISE_TRISE_Msk                          /*!<Maximum Rise Time in Fast/Standard mode (Master mode) */

/******************  Bit definition for PI2C_FLTR register  *******************/
#define PI2C_FLTR_DNF_Pos          (0U)
#define PI2C_FLTR_DNF_Msk          (0xFU << PI2C_FLTR_DNF_Pos)                   /*!< 0x0000000F */
#define PI2C_FLTR_DNF              PI2C_FLTR_DNF_Msk                             /*!<Digital Noise Filter */
#define PI2C_FLTR_ANOFF_Pos        (4U)
#define PI2C_FLTR_ANOFF_Msk        (0x1U << PI2C_FLTR_ANOFF_Pos)                 /*!< 0x00000010 */
#define PI2C_FLTR_ANOFF            PI2C_FLTR_ANOFF_Msk                           /*!<Analog Noise Filter OFF */

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        volatile uint32_t CR1;          /*!< I2C Control register 1,     Address offset: 0x00 */
        volatile uint32_t CR2;          /*!< I2C Control register 2,     Address offset: 0x04 */
        volatile uint32_t OAR1;         /*!< I2C Own address register 1, Address offset: 0x08 */
        volatile uint32_t OAR2;         /*!< I2C Own address register 2, Address offset: 0x0C */
        volatile uint32_t DR;           /*!< I2C Data register,          Address offset: 0x10 */
        volatile uint32_t SR1;          /*!< I2C Status register 1,      Address offset: 0x14 */
        volatile uint32_t SR2;          /*!< I2C Status register 2,      Address offset: 0x18 */
        volatile uint32_t CCR;          /*!< I2C Clock control register, Address offset: 0x1C */
        volatile uint32_t TRISE;        /*!< I2C TRISE register,         Address offset: 0x20 */
        volatile uint32_t FLTR;         /*!< I2C FLTR register,          Address offset: 0x24 */
} I2C_periph_t;

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

#endif /* _F147_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
