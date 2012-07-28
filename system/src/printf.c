/*=============================================================================================*//**
@file    printf.c

@author  Daniel Zorychta

@brief   This file support message printing

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "printf.h"
#include "FreeRTOSConfig.h"
#include <string.h>
#include "uart.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
/** define function which provide terminal output */
#define SEND_BUFFER(buffer, size)         UART_Write(UART_DEV_1, buffer, size, 0)


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static void reverseBuffer(ch_t *begin, ch_t *end);
static u32_t vsnprint(ch_t *stream, u32_t size, const ch_t *format, va_list arg);
static u32_t svsnprint(stdio_t *stdio, const ch_t *format, va_list arg);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static bool_t kprintEnabled = FALSE;


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
void kprintEnable(void)
{
      kprintEnabled = TRUE;
}


//================================================================================================//
/**
 * @brief Function reverse buffer
 *
 * @param *begin     buffer begin
 * @param *end       buffer end
 */
//================================================================================================//
static void reverseBuffer(ch_t *begin, ch_t *end)
{
    ch_t temp;

    while (end > begin)
    {
        temp     = *end;
        *end--   = *begin;
        *begin++ = temp;
    }
}


//================================================================================================//
/**
 * @brief Function convert value to the character
 *
 * @param value         converted value
 * @param *buffer       result buffer
 * @param base          conversion base
 *
 * @return pointer in the buffer
 */
//================================================================================================//
ch_t *itoa(i32_t value, ch_t *buffer, u8_t base)
{
    static const ch_t digits[] = "0123456789abcdef";

    ch_t *bufferCopy = buffer;
    i32_t sign       = 0;
    i32_t quot;
    i32_t rem;

    if ((base >= 2) && (base <= 16))
    {
        if ((base == 10) && ((sign = value) < 0))
            value = -value;

        do
        {
            quot      = value / base;
            rem       = value % base;
            *buffer++ = digits[rem];
        }
        while ((value = quot));

        if (sign < 0)
            *buffer++ = '-';

        reverseBuffer(bufferCopy, (buffer - 1));
    }

    *buffer = '\0';

    return bufferCopy;
}


//================================================================================================//
/**
 * @brief Function send to buffer formated output string
 *
 * @param *format             formated text
 * @param ...                 format arguments
 *
 * @retval number of written characters
 */
//================================================================================================//
u32_t bprint(ch_t *stream, u32_t size, const ch_t *format, ...)
{
      va_list args;
      u32_t n;

      va_start(args, format);
      n = vsnprint(stream, size, format, args);
      va_end(args);

      return n;
}


//================================================================================================//
/**
 * @brief Function send on a standard output string
 *
 * @param *format             formated text
 * @param ...                 format arguments
 *
 * @retval number of written characters
 */
//================================================================================================//
u32_t sprint(stdio_t *stdio, const ch_t *format, ...)
{
      va_list args;
      u32_t n;

      va_start(args, format);
      n = svsnprint(stdio, format, args);
      va_end(args);

      return n;
}


//================================================================================================//
/**
 * @brief Function send kernel message on terminal
 *
 * @param *format             formated text
 * @param ...                 format arguments
 *
 * @retval number of written characters
 */
//================================================================================================//
u32_t kprint(const ch_t *format, ...)
{
      ch_t    buffer[constKPRINT_BUFFER_SIZE];
      va_list args;
      u32_t   n = 0;

      if (kprintEnabled)
      {
            memset(buffer, 0, constKPRINT_BUFFER_SIZE);

            va_start(args, format);
            n = vsnprint(buffer, constKPRINT_BUFFER_SIZE, format, args);
            va_end(args);

            SEND_BUFFER(buffer, strlen(buffer));
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function convert arguments to the stdio (vsnprintf)
 *
 * @param[out] *stream        buffer for stream
 * @param[in]  size           buffer size
 * @param[in]  *format        message format
 * @param[in]  arg            argument list
 *
 * @return number of printed characters
 */
//================================================================================================//
static u32_t svsnprint(stdio_t *stdio, const ch_t *format, va_list arg)
{
      ch_t  character;
      u32_t streamLen = 1;

      #define putStream()                                                     \
            TaskSuspendAll();                                                 \
            if (stdio->stdout.Level < configSTDIO_BUFFER_SIZE)                \
            {                                                                 \
                  stdio->stdout.Buffer[stdio->stdout.TxIdx++] = character;    \
                  if (stdio->stdout.TxIdx >= configSTDIO_BUFFER_SIZE)         \
                        stdio->stdout.TxIdx = 0;                              \
                                                                              \
                  stdio->stdout.Level++;                                      \
                  TaskResumeAll();                                            \
            }                                                                 \
            else                                                              \
            {                                                                 \
                  TaskResumeAll();                                            \
                  TaskDelay(10);                                              \
            }

      while ((character = *format++) != '\0')
      {
            if (character != '%')
            {
                  putStream();
            }
            else
            {
                  character = *format++;

                  if (character == '%' || character == 'c')
                  {
                        if (character == 'c')
                              character = va_arg(arg, i32_t);

                        putStream();

                        continue;
                  }

                  if (character == 's' || character == 'd' || character == 'x')
                  {
                        ch_t result[11];
                        ch_t *resultPtr;

                        if (character == 's')
                        {
                              resultPtr = va_arg(arg, ch_t*);
                        }
                        else
                        {
                              u8_t base = (character == 'd' ? 10 : 16);

                              resultPtr = itoa(va_arg(arg, i32_t), result, base);
                        }

                        while ((character = *resultPtr++))
                        {
                              putStream();
                        }

                        continue;
                  }
            }
      }

      return (streamLen - 1);

      #undef putStream
}



//================================================================================================//
/**
 * @brief Function convert arguments to the buffer (vsnprintf)
 *
 * @param[out] *stream        buffer for stream
 * @param[in]  size           buffer size
 * @param[in]  *format        message format
 * @param[in]  arg            argument list
 *
 * @return number of printed characters
 */
//================================================================================================//
static u32_t vsnprint(ch_t *stream, u32_t size, const ch_t *format, va_list arg)
{
      ch_t  character;
      u32_t streamLen = 1;

      #define putStream()                               \
            if (streamLen < size)                       \
            {                                           \
                  *stream++ = character; streamLen++;   \
            }                                           \
            else                                        \
            {                                           \
                  *stream = 0;                          \
                  return (streamLen - 1);               \
            }


      if (size == 0)
            return 0;

      while ((character = *format++) != '\0')
      {
            if (character != '%')
            {
                  putStream();
            }
            else
            {
                  character = *format++;

                  if (character == '%' || character == 'c')
                  {
                        if (character == 'c')
                              character = va_arg(arg, i32_t);

                        putStream();

                        continue;
                  }

                  if (character == 's' || character == 'd' || character == 'x')
                  {
                        ch_t result[11];
                        ch_t *resultPtr;

                        if (character == 's')
                        {
                              resultPtr = va_arg(arg, ch_t*);
                        }
                        else
                        {
                              u8_t base = (character == 'd' ? 10 : 16);

                              resultPtr = itoa(va_arg(arg, i32_t), result, base);
                        }

                        while ((character = *resultPtr++))
                        {
                              putStream();
                        }

                        continue;
                  }
            }
      }

      return (streamLen - 1);

      #undef putStream
}

/*==================================================================================================
                                            End of file
==================================================================================================*/
