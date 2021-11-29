/**
  ******************************************************************************
  * @file    usbh_conf_template.h
  * @author  MCD Application Team
  * @brief   Header file for usbh_conf_template.c
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  /* replace 'stm32xxx' with your HAL driver header filename, ex: stm32f4xx.h */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "drivers/driver.h"

/** @addtogroup STM32_USB_HOST_LIBRARY
  * @{
  */

/** @defgroup USBH_CONF
  * @brief usb host low level driver configuration file
  * @{
  */

/** @defgroup USBH_CONF_Exported_Defines
  * @{
  */

#define USBH_MAX_NUM_ENDPOINTS                2U
#define USBH_MAX_NUM_INTERFACES               2U
#define USBH_MAX_NUM_CONFIGURATION            1U
#define USBH_KEEP_CFG_DESCRIPTOR              1U
#define USBH_MAX_NUM_SUPPORTED_CLASS          8U
#define USBH_MAX_SIZE_CONFIGURATION           0x200U
#define USBH_MAX_DATA_BUFFER                  0x200U
#define USBH_DEBUG_LEVEL                      3U
#define USBH_USE_OS                           1U
#define USE_USB_HS                            1U

/** @defgroup USBH_Exported_Macros
  * @{
  */

/* Memory management macros */
extern void *USBH_malloc(uint32_t size);
extern void USBH_free(void *mem);
#define USBH_memset               memset
#define USBH_memcpy               memcpy

extern bool usbh_irq;
#define __printk(...) if (!usbh_irq) printk(__VA_ARGS__)

/* DEBUG macros */
#if (USBH_DEBUG_LEVEL > 0U)
#define  USBH_UsrLog(...)   do { \
                            __printk("USBH: "__VA_ARGS__); \
} while (0)
#else
#define USBH_UsrLog(...) do {} while (0)
#endif

#if (USBH_DEBUG_LEVEL > 1U)

#define  USBH_ErrLog(...) do { \
                            __printk("USBH: error: "__VA_ARGS__); \
} while (0)
#else
#define USBH_ErrLog(...) do {} while (0)
#endif

#if (USBH_DEBUG_LEVEL > 2U)
#define  USBH_DbgLog(...)   do { \
                            __printk("USBH: debug: "__VA_ARGS__); \
} while (0)
#else
#define USBH_DbgLog(...) do {} while (0)
#endif

#define UNUSED(_u)              UNUSED_ARG1(_u)
#define osWaitForever           MAX_DELAY_MS


static inline void osMessagePutX(queue_t *queue, uint32_t *msg, u32_t timeout)
{
        extern bool usbh_irq;

        if (usbh_irq) {
                sys_queue_send_from_ISR(queue, msg, NULL);
        } else {
                sys_queue_send(queue, msg, timeout);
        }
}

#define osMessagePut(_event, _msg, _timeout) osMessagePutX(_event, &_msg, _timeout)

/**
  * @}
  */



/**
  * @}
  */


/** @defgroup USBH_CONF_Exported_Types
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_CONF_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_CONF_Exported_Variables
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_CONF_Exported_FunctionsPrototype
  * @{
  */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif


/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
