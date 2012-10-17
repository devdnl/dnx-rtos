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
#include "print.h"
#include "basic_types.h"
#include "systypes.h"
#include <string.h>
#include "memman.h"
#include "tty.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static void reverseBuffer(ch_t *begin, ch_t *end);
static u32_t vsnprint(bool_t stdio, void *streamStdout, u32_t size, const ch_t *format, va_list arg);


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
ch_t *itoa(i32_t value, ch_t *buffer, u8_t base, bool_t unsignedValue, u8_t zerosRequired)
{
      static const ch_t digits[] = "0123456789ABCDEF";

      ch_t *bufferCopy = buffer;
      i32_t sign    = 0;
      u8_t  zeroCnt = 0;
      i32_t quot;
      i32_t rem;

      if ((base >= 2) && (base <= 16))
      {
            if (unsignedValue)
            {
                  do
                  {
                        quot = (u32_t)((u32_t)value / (u32_t)base);
                        rem  = (u32_t)((u32_t)value % (u32_t)base);
                        *buffer++   = digits[rem];
                        zeroCnt++;
                  }
                  while ((value = quot));
            }
            else
            {
                  if ((base == 10) && ((sign = value) < 0))
                        value = -value;

                  do
                  {
                        quot = value / base;
                        rem  = value % base;
                        *buffer++ = digits[rem];
                        zeroCnt++;
                  }
                  while ((value = quot));
            }

            while (zerosRequired > zeroCnt)
            {
                  *buffer++ = '0';
                  zeroCnt++;
            }

            if (sign < 0)
                  *buffer++ = '-';

            reverseBuffer(bufferCopy, (buffer - 1));
      }

      *buffer = '\0';

      return bufferCopy;
}


//================================================================================================//
/**
 * @brief Function convert ASCII to the number
 * When function find any other character than number (depended of actual base) immediately finished
 * operation and return pointer when bad character was found
 *
 * @param[in]  *string       string to decode
 * @param[in]  base          decode base
 * @param[out] *value        pointer to result
 *
 * @return pointer in string when operation was finished
 */
//================================================================================================//
ch_t *atoi(ch_t *string, u8_t base, i32_t *value)
{
      ch_t   character;
      i32_t  sign      = 1;
      bool_t charFound = FALSE;

      *value = 0;

      if (base < 2 && base > 16)
            goto atoi_end;

      while ((character = *string) != ASCII_NULL)
      {
            /* if space exist, atoi continue finding correct character */
            if ((character == ' ') && (charFound == FALSE))
            {
                  string++;
                  continue;
            }
            else
            {
                  charFound = TRUE;
            }

            /* check signum */
            if (character == '-')
            {
                  if (base == 10)
                  {
                        if (sign == 1)
                              sign = -1;

                        string++;
                        continue;
                  }
                  else
                  {
                        goto atoi_sign;
                  }
            }

            /* check character range */
            if (character >= 'a')
                  character -= 'a' - 10;
            else if (character >= 'A')
                  character -= 'A' - 10;
            else if (character >= '0')
                  character -= '0';
            else
                  goto atoi_sign;

            /* check character range according to actual base */
            if (character >= base)
                  break;

            /* compute value */
            *value = *value * base;
            *value = *value + character;

            string++;
      }

      atoi_sign:
      *value *= sign;

      atoi_end:
      return string;
}


//================================================================================================//
/**
 * @brief Function send to buffer formated output string
 *
 * @param *stream             output buffer
 * @param size                buffer size
 * @param *format             formated text
 * @param ...                 format arguments
 *
 * @retval number of written characters
 */
//================================================================================================//
u32_t snprint(ch_t *stream, u32_t size, const ch_t *format, ...)
{
      va_list args;
      u32_t n = 0;

      if (stream)
      {
            va_start(args, format);
            n = vsnprint(FALSE, stream, size, format, args);
            va_end(args);
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function send on a standard output string
 *
 * @param *stdout             stdout
 * @param *format             formated text
 * @param ...                 format arguments
 *
 * @retval number of written characters
 */
//================================================================================================//
u32_t fprint(stdioFIFO_t *stdout, const ch_t *format, ...)
{
      va_list args;
      u32_t n = 0;

      if (stdout)
      {
            va_start(args, format);
            n = vsnprint(TRUE, stdout, 0, format, args);
            va_end(args);
      }

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
      va_list args;
      u32_t   n = 0;

      if (kprintEnabled)
      {
            ch_t *buffer = (ch_t*)Calloc(constKPRINT_BUFFER_SIZE, sizeof(ch_t));

            if (buffer)
            {
                  va_start(args, format);
                  n = vsnprint(FALSE, buffer, constKPRINT_BUFFER_SIZE, format, args);
                  va_end(args);

                  ch_t *msg = (ch_t*)Calloc((n + 1), sizeof(ch_t));

                  if (msg)
                  {
                        memcpy(msg, buffer, n + 1);
                        TTY_AddMsg(0, msg);
                  }

                  Free(buffer);
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function send kernel message with success on terminal
 *
 * @retval number of written characters
 */
//================================================================================================//
u32_t kprintOK(void)
{
      return kprint("\r\x1B[72C[\x1B[32m  OK  \x1B[0m]\n");
}


//================================================================================================//
/**
 * @brief Function send kernel message with failure on terminal
 *
 * @retval number of written characters
 */
//================================================================================================//
u32_t kprintFail(void)
{
      return kprint("\r\x1B[72C[\x1B[31m FAIL \x1B[0m]\n");
}


//================================================================================================//
/**
 * @brief Function send kernel message with error number on terminal
 *
 * @retval number of written characters
 */
//================================================================================================//
u32_t kprintErrorNo(i8_t errorNo)
{
      return kprint("\r\x1B[72C[\x1B[31m Er%d \x1B[0m]\n", errorNo);
}


//================================================================================================//
/**
 * @brief Function put character into stdout stream
 *
 * @param *stdout             stdout
 * @param c                   character
 */
//================================================================================================//
void fputChar(stdioFIFO_t *stdout, ch_t c)
{
      if (stdout)
      {
            while (TRUE)
            {
                  TaskSuspendAll();

                  if (stdout->Level < configSTDIO_BUFFER_SIZE)
                  {
                        stdout->Buffer[stdout->TxIdx++] = c;

                        if (stdout->TxIdx >= configSTDIO_BUFFER_SIZE)
                              stdout->TxIdx = 0;

                        stdout->Level++;

                        TaskResumeAll();
                        return;
                  }
                  else
                  {
                        TaskResumeAll();
                        TaskDelay(5);
                  }
            }
      }
}


//================================================================================================//
/**
 * @brief Unblocked function put character into stdout stream
 *
 * @param *stdout             stdout
 * @param c                   character
 */
//================================================================================================//
void ufputChar(stdioFIFO_t *stdout, ch_t c)
{
      if (stdout)
      {
            TaskSuspendAll();

            if (stdout->Level < configSTDIO_BUFFER_SIZE)
            {
                  stdout->Buffer[stdout->TxIdx++] = c;

                  if (stdout->TxIdx >= configSTDIO_BUFFER_SIZE)
                        stdout->TxIdx = 0;

                  stdout->Level++;

                  TaskResumeAll();
            }
      }
}


//================================================================================================//
/**
 * @brief Function get character from stdin stream
 *
 * @param *stdin            stdin
 *
 * @retval character
 */
//================================================================================================//
ch_t fgetChar(stdioFIFO_t *stdin)
{
      ch_t out = ASCII_CANCEL;

      if (stdin)
      {
            while (TRUE)
            {
                  TaskSuspendAll();

                  if (stdin->Level > 0)
                  {
                        out = stdin->Buffer[stdin->RxIdx++];

                        if (stdin->RxIdx >= configSTDIO_BUFFER_SIZE)
                              stdin->RxIdx = 0;

                        stdin->Level--;

                        TaskResumeAll();

                        goto fgetChar_end;
                  }
                  else
                  {
                        TaskResumeAll();
                        TaskDelay(5);
                  }
            }
      }

      fgetChar_end:
            return out;
}


//================================================================================================//
/**
 * @brief Function get character from stdin stream in the unblocking mode
 *
 * @param *stdin            stdin
 *
 * @retval character
 */
//================================================================================================//
ch_t ufgetChar(stdioFIFO_t *stdin)
{
      ch_t out = ASCII_CANCEL;

      if (stdin)
      {
            TaskSuspendAll();

            if (stdin->Level > 0)
            {
                  out = stdin->Buffer[stdin->RxIdx++];

                  if (stdin->RxIdx >= configSTDIO_BUFFER_SIZE)
                        stdin->RxIdx = 0;

                  stdin->Level--;
            }

            TaskResumeAll();
      }

      return out;
}


//================================================================================================//
/**
 * @brief Waiting for STDIO flush
 *
 * @param stdioFIFO_t *stdio
 */
//================================================================================================//
void fsflush(stdioFIFO_t *stdio)
{
       while (stdio->Level)
       {
             TaskDelay(1);
       }
}


//================================================================================================//
/**
 * @brief Function convert arguments to the stdio (vsnprintf)
 *
 * @param[in] stdio          TRUE to enable STDIO
 * @param[in] *streamStdout  buffer for stream or stdout
 * @param[in] size           buffer size
 * @param[in] *format        message format
 * @param[in] arg            argument list
 *
 * @return number of printed characters
 */
//================================================================================================//
static u32_t vsnprint(bool_t stdio, void *streamStdout, u32_t size, const ch_t *format, va_list arg)
{
      #define putCharacter(character)                       \
            if (stdio)                                      \
            {                                               \
                  fputChar(stdout, character);              \
            }                                               \
            else                                            \
            {                                               \
                  if (streamLen < size)                     \
                  {                                         \
                        *stream++ = character;              \
                  }                                         \
                  else                                      \
                  {                                         \
                        *stream = 0;                        \
                        goto vsnfprint_end;                 \
                  }                                         \
            }                                               \
            streamLen++


      ch_t  character;
      u32_t streamLen = 1;

      stdioFIFO_t *stdout = (stdioFIFO_t*)streamStdout;
      ch_t        *stream = (ch_t*)streamStdout;


      while ((character = *format++) != ASCII_NULL)
      {
            if (character != '%')
            {
                  if (character == ASCII_LF)
                  {
                        putCharacter(ASCII_CR);
                  }

                  putCharacter(character);
            }
            else
            {
                  character = *format++;

                  if (character == '%' || character == 'c')
                  {
                        if (character == 'c')
                              character = va_arg(arg, i32_t);

                        putCharacter(character);

                        continue;
                  }

                  if (character == 's' || character == 'd' || character == 'x' || character == 'u')
                  {
                        ch_t result[11];
                        ch_t *resultPtr;

                        if (character == 's')
                        {
                              resultPtr = va_arg(arg, ch_t*);
                        }
                        else
                        {
                              u8_t zeros = *format++;

                              if (zeros >= '0' && zeros <= '9')
                              {
                                    zeros -= '0';
                              }
                              else
                              {
                                    zeros = 0;
                                    format--;
                              }

                              u8_t   base  = ((character == 'd') || (character == 'u') ? 10 : 16);
                              bool_t uint  = ((character == 'x') || (character == 'u') ? TRUE : FALSE);

                              resultPtr = itoa(va_arg(arg, i32_t), result, base, uint, zeros);
                        }

                        while ((character = *resultPtr++))
                        {
                              putCharacter(character);
                        }

                        continue;
                  }
            }
      }

      vsnfprint_end:
            return (streamLen - 1);

      #undef putChar
}


//================================================================================================//
/**
 * @brief Function convert arguments to the stdio and gets data from stdin
 *
 * @param[in]  *stdin         stdin
 * @param[in]  *stdout        stdout
 * @param[in]  size           buffer size
 * @param[in]  *format        message format
 * @param[in]  *var           output
 *
 * @return number of printed characters
 */
//================================================================================================//
u32_t fscan(stdioFIFO_t *stdin, stdioFIFO_t *stdout, const ch_t *format, void *var)
{
      ch_t  character;
      u32_t streamLen = 1;

      while ((character = *format++) != '\0')
      {
            if (character != '%')
            {
                  if (character == ASCII_LF)
                        fputChar(stdout, ASCII_CR);

                  fputChar(stdout, character);
            }
            else
            {
                  character = *format++;

                  u8_t inCmdStep = 0;

                  if (character == 'd' || character == 'u')
                  {
                        i32_t  *dec = (i32_t*)var;
                        i32_t  sign = 1;
                        bool_t uint = (character == 'u' ? TRUE : FALSE);

                        *dec = 0;

                        while (TRUE)
                        {
                              character = fgetChar(stdin);

                              if (  (character >= '0' && character <= '9')
                                 || (character == '-' && !uint && sign == 1) )
                              {
                                    fputChar(stdout, character);
                              }
                              else if (character == ASCII_CR || character == ASCII_LF)
                              {
                                    *dec *= sign;
                                    fputChar(stdout, ASCII_CR);
                                    fputChar(stdout, ASCII_LF);
                                    goto fscan_end;
                              }
                              else if ((character == ASCII_BS) && (streamLen > 1))
                              {
                                    fprint(stdout, "%c\x1B[K", character);

                                    if (streamLen == 2 && sign == -1)
                                          sign = 1;
                                    else
                                          *dec /= 10;
                                    streamLen--;
                                    continue;
                              }
                              else
                              {
                                    continue;
                              }

                              if (character == '-' && sign == 1 && !uint)
                              {
                                    sign = -1;
                              }

                              if (character >= '0' && character <= '9')
                              {
                                    character -= '0';

                                    *dec *= 10;
                                    *dec += character;
                              }

                              streamLen++;
                        }

                        goto fscan_end;
                  }

                  if (character == 'x')
                  {
                        u32_t *hex = (u32_t *)var;

                        *hex = 0;

                        while (TRUE)
                        {
                              character = fgetChar(stdin);

                              if (  ((character >= '0') && (character <= '9'))
                                 || ((character >= 'A') && (character <= 'F'))
                                 || ((character >= 'a') && (character <= 'f')) )
                              {
                                    fputChar(stdout, character);
                              }
                              else if (character == ASCII_CR || character == ASCII_LF)
                              {
                                    fputChar(stdout, ASCII_CR);
                                    fputChar(stdout, ASCII_LF);
                                    goto fscan_end;
                              }
                              else if ((character == ASCII_BS) && (streamLen > 1))
                              {
                                    fprint(stdout, "%c\x1B[K", character);
                                    *hex >>= 4;
                                    streamLen--;
                                    continue;
                              }
                              else
                              {
                                    continue;
                              }

                              if ((character >= 'A') && (character <= 'F'))
                              {
                                    character = character - 'A' + 0x0A;
                              }
                              else if ((character >= 'a') && (character <= 'f'))
                              {
                                    character = character - 'a' + 0x0A;
                              }
                              else if ((character >= '0') && (character <= '9'))
                              {
                                    character -= '0';
                              }

                              *hex <<= 4;
                              *hex |= character;

                              streamLen++;
                        }
                  }

                  if (character == 'b')
                  {
                        u32_t *bin = (u32_t *)var;

                        *bin = 0;

                        while (TRUE)
                        {
                              character = fgetChar(stdin);

                              if (character == '0' || character == '1')
                              {
                                    fputChar(stdout, character);
                              }
                              else if (character == ASCII_CR || character == ASCII_LF)
                              {
                                    fputChar(stdout, ASCII_CR);
                                    fputChar(stdout, ASCII_LF);
                                    goto fscan_end;
                              }
                              else if ((character == ASCII_BS) && (streamLen > 1))
                              {
                                    fprint(stdout, "%c\x1B[K", character);
                                    *bin >>= 1;
                                    streamLen--;
                                    continue;
                              }
                              else
                              {
                                    continue;
                              }

                              character -= '0';

                              *bin <<= 1;
                              *bin |= character;

                              streamLen++;
                        }
                  }

                  if (character == 's')
                  {
                        u16_t free = *(format++);

                        if (free >= '1' && free <= '9')
                        {
                              free = (free - '0') * 50;
                        }
                        else
                        {
                              free = UINT16_MAX;
                        }

                        ch_t *string = (ch_t*)var;

                        while (TRUE)
                        {
                              character = fgetChar(stdin);

                              /* check command Arrow Up */
                              if ((character == ASCII_ESC) && (inCmdStep == 0))
                              {
                                    inCmdStep++;
                                    *(string++) = character;
                                    continue;
                              }
                              else if ((character == '[') && (inCmdStep == 1))
                              {
                                 inCmdStep++;
                                 *(string++) = character;
                                 continue;
                              }
                              else if ((character == 'A') && (inCmdStep == 2))
                              {
                                 *(string++) = character;
                                 goto fscan_end;
                              }
                              else
                              {
                                    inCmdStep = 0;
                              }


                              /* put character */
                              if (character == ASCII_CR || character == ASCII_LF)
                              {
                                    *(string++) = 0x00;
                                    fputChar(stdout, ASCII_CR);
                                    fputChar(stdout, ASCII_LF);
                                    goto fscan_end;
                              }
                              else if ((character == ASCII_BS) && (streamLen > 1))
                              {
                                    fprint(stdout, "%c\x1B[K", character);
                                    *(--string) = 0x00;
                                    streamLen--;
                                    free++;
                                    continue;
                              }
                              else if (character >= ' ')
                              {
                                    if (free)
                                    {
                                          fputChar(stdout, character);
                                          *(string++) = character;
                                          free--;
                                    }
                              }
                              else
                              {
                                    continue;
                              }

                              streamLen++;
                        }
                  }
            }
      }

      fscan_end:
            return (streamLen - 1);
}


//================================================================================================//
/**
 * @brief Clear stdin buffer
 *
 * @param *stdin              stdin
 */
//================================================================================================//
void fclearSTDIO(stdioFIFO_t *stdio)
{
      if (stdio)
      {
            TaskSuspendAll();
            stdio->Level = 0;
            stdio->RxIdx = 0;
            stdio->TxIdx = 0;
            TaskResumeAll();
      }
}


/*==================================================================================================
                                            End of file
==================================================================================================*/
