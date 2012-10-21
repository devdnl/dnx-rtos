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
static void  reverseBuffer(ch_t *begin, ch_t *end);
static u32_t vsnprint(ch_t *stream, u32_t size, const ch_t *format, va_list arg);
static ch_t *itoa(i32_t value, ch_t *buffer, u8_t base, bool_t unsignedValue, u8_t zerosRequired);
static ch_t *atoi(ch_t *string, u8_t base, i32_t *value);
static u32_t CalcFormatSize(const ch_t *format, va_list arg);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static bool_t kprintEnabled = FALSE;
static u8_t   kprintTTY;


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Enable kprint functionality
 */
//================================================================================================//
void EnableKprint(void)
{
      kprintEnabled = TRUE;
}


//================================================================================================//
/**
 * @brief Disable kprint functionality
 */
//================================================================================================//
void DisableKprint(void)
{
      kprintEnabled = FALSE;
}


//================================================================================================//
/**
 * @brief Change kprint terminal
 *
 * @param tty     kprint terminal number
 */
//================================================================================================//
void ChangekprintTTY(u8_t tty)
{
      kprintTTY = tty;
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
static ch_t *itoa(i32_t value, ch_t *buffer, u8_t base, bool_t unsignedValue, u8_t zerosRequired)
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
static ch_t *atoi(ch_t *string, u8_t base, i32_t *value)
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
 * @brief Function convert arguments to stream
 *
 * @param[in] *format        message format
 * @param[in] arg            argument list
 *
 * @return size of snprint result
 */
//================================================================================================//
static u32_t CalcFormatSize(const ch_t *format, va_list arg)
{
      ch_t  chr;
      u32_t size = 1;

      while ((chr = *format++) != ASCII_NULL)
      {
            if (chr != '%')
            {
                  if (chr == '\n')
                  {
                        size += 2;
                  }
                  else
                  {
                        size++;
                  }
            }
            else
            {
                  chr = *format++;

                  if (chr == '%' || chr == 'c')
                  {
                        chr = va_arg(arg, i32_t);
                        size++;
                        continue;
                  }

                  if (chr == 's')
                  {
                        size += strlen(va_arg(arg, ch_t*));
                        continue;
                  }

                  if (chr == 'd' || chr == 'x' || chr == 'u')
                  {
                        chr = va_arg(arg, i32_t);
                        size += 11;
                        continue;
                  }
            }
      }

      return size;
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
            n = vsnprint(stream, size, format, args);
            va_end(args);
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function send on a standard output string
 *
 * @param tty                 virtual terminal number
 * @param *format             formated text
 * @param ...                 format arguments
 *
 * @retval number of written characters
 */
//================================================================================================//
u32_t printt(u8_t tty, const ch_t *format, ...)
{
      va_list args;
      u32_t n    = 0;

      va_start(args, format);
      u32_t size = CalcFormatSize(format, args);
      va_end(args);

      ch_t *str  = Calloc(1, size);

      if (str)
      {
            va_start(args, format);
            n = vsnprint(str, size, format, args);
            va_end(args);

            TTY_AddMsg(tty, str);

            Free(str);
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
            va_start(args, format);
            u32_t size = CalcFormatSize(format, args);
            va_end(args);

            ch_t *buffer = Calloc(size, sizeof(ch_t));

            if (buffer)
            {
                  va_start(args, format);
                  n = vsnprint(buffer, size, format, args);
                  va_end(args);

                  TTY_AddMsg(kprintTTY, buffer);

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
 * @brief Function put character into virtual terminal
 *
 * @param tty                 virtual terminal
 * @param c                   character
 */
//================================================================================================//
void putChart(u8_t tty, ch_t c)
{
      ch_t chr[2] = {c, 0};

      TTY_AddMsg(tty, chr);
}


//================================================================================================//
/**
 * @brief Function get character from virtual terminal
 *
 * @param tty            virtual terminal number
 *
 * @retval character
 */
//================================================================================================//
ch_t getChart(u8_t tty)
{
      u8_t chr = 0;

      while (TRUE)
      {
            chr = TTY_GetChr(tty);

            if (chr)
            {
                  break;
            }
            else
            {
                  Sleep(25);
            }
      }

      return chr;
}


//================================================================================================//
/**
 * @brief Function get character from virtual terminal in unblocking mode
 *
 * @param tty            virtual terminal number
 *
 * @retval character
 */
//================================================================================================//
ch_t ugetChart(u8_t tty)
{
      return TTY_GetChr(tty);
}


//================================================================================================//
/**
 * @brief Function convert arguments to stream
 *
 * @param[in] *stream        buffer for stream
 * @param[in] size           buffer size
 * @param[in] *format        message format
 * @param[in] arg            argument list
 *
 * @return number of printed characters
 */
//================================================================================================//
static u32_t vsnprint(ch_t *stream, u32_t size, const ch_t *format, va_list arg)
{
      #define putCharacter(character)           \
      {                                         \
            if (slen < size)                    \
            {                                   \
                  *stream++ = character;        \
                  slen++;                       \
            }                                   \
            else                                \
            {                                   \
                  goto vsnprint_end;            \
            }                                   \
      }


      ch_t  chr;
      u32_t slen = 1;

      /* analyze format */
      while ((chr = *format++) != ASCII_NULL)
      {
            if (chr != '%')
            {
                  if (chr == ASCII_LF)
                  {
                        putCharacter(ASCII_CR);
                  }

                  putCharacter(chr);
            }
            else
            {
                  chr = *format++;

                  if (chr == '%' || chr == 'c')
                  {
                        if (chr == 'c')
                              chr = va_arg(arg, i32_t);

                        putCharacter(chr);

                        continue;
                  }

                  if (chr == 's' || chr == 'd' || chr == 'x' || chr == 'u')
                  {
                        ch_t result[11];
                        ch_t *resultPtr;

                        if (chr == 's')
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

                              u8_t   base  = ((chr == 'd') || (chr == 'u') ? 10 : 16);
                              bool_t uint  = ((chr == 'x') || (chr == 'u') ? TRUE : FALSE);

                              resultPtr = itoa(va_arg(arg, i32_t), result, base, uint, zeros);
                        }

                        while ((chr = *resultPtr++))
                        {
                              putCharacter(chr);
                        }

                        continue;
                  }
            }
      }

      vsnprint_end:
      *stream = 0;
      return (slen - 1);

      #undef putChar
}


//================================================================================================//
/**
 * @brief Function convert arguments to the stdio and gets data from stdin
 *
 * @param[in]  tty            virtual terminal number
 * @param[in]  size           buffer size
 * @param[in]  *format        message format
 * @param[in]  *var           output
 *
 * @return number of printed characters
 */
//================================================================================================//
u32_t scant(u8_t tty, const ch_t *format, void *var)
{
      ch_t  chr;
      u32_t streamLen = 1;

      while ((chr = *format++) != '\0')
      {
            if (chr != '%')
            {
                  if (chr == ASCII_LF)
                        putChart(tty, ASCII_CR);

                  putChart(tty, chr);
            }
            else
            {
                  chr = *format++;

                  u8_t inCmdStep = 0;

                  if (chr == 'd' || chr == 'u')
                  {
                        i32_t  *dec = (i32_t*)var;
                        i32_t  sign = 1;
                        bool_t uint = (chr == 'u' ? TRUE : FALSE);

                        *dec = 0;

                        while (TRUE)
                        {
                              chr = getChart(tty);

                              if (  (chr >= '0' && chr <= '9')
                                 || (chr == '-' && !uint && sign == 1) )
                              {
                                    putChart(tty, chr);
                              }
                              else if (chr == ASCII_CR || chr == ASCII_LF)
                              {
                                    *dec *= sign;
                                    putChart(tty, ASCII_CR);
                                    putChart(tty, ASCII_LF);
                                    goto tscan_end;
                              }
                              else if ((chr == ASCII_BS) && (streamLen > 1))
                              {
                                    printt(tty, "%c\x1B[K", chr);

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

                              if (chr == '-' && sign == 1 && !uint)
                              {
                                    sign = -1;
                              }

                              if (chr >= '0' && chr <= '9')
                              {
                                    chr -= '0';

                                    *dec *= 10;
                                    *dec += chr;
                              }

                              streamLen++;
                        }

                        goto tscan_end;
                  }

                  if (chr == 'x')
                  {
                        u32_t *hex = (u32_t *)var;

                        *hex = 0;

                        while (TRUE)
                        {
                              chr = getChart(tty);

                              if (  ((chr >= '0') && (chr <= '9'))
                                 || ((chr >= 'A') && (chr <= 'F'))
                                 || ((chr >= 'a') && (chr <= 'f')) )
                              {
                                    putChart(tty, chr);
                              }
                              else if (chr == ASCII_CR || chr == ASCII_LF)
                              {
                                    putChart(tty, ASCII_CR);
                                    putChart(tty, ASCII_LF);
                                    goto tscan_end;
                              }
                              else if ((chr == ASCII_BS) && (streamLen > 1))
                              {
                                    printt(tty, "%c\x1B[K", chr);
                                    *hex >>= 4;
                                    streamLen--;
                                    continue;
                              }
                              else
                              {
                                    continue;
                              }

                              if ((chr >= 'A') && (chr <= 'F'))
                              {
                                    chr = chr - 'A' + 0x0A;
                              }
                              else if ((chr >= 'a') && (chr <= 'f'))
                              {
                                    chr = chr - 'a' + 0x0A;
                              }
                              else if ((chr >= '0') && (chr <= '9'))
                              {
                                    chr -= '0';
                              }

                              *hex <<= 4;
                              *hex |= chr;

                              streamLen++;
                        }
                  }

                  if (chr == 'b')
                  {
                        u32_t *bin = (u32_t *)var;

                        *bin = 0;

                        while (TRUE)
                        {
                              chr = getChart(tty);

                              if (chr == '0' || chr == '1')
                              {
                                    putChart(tty, chr);
                              }
                              else if (chr == ASCII_CR || chr == ASCII_LF)
                              {
                                    putChart(tty, ASCII_CR);
                                    putChart(tty, ASCII_LF);
                                    goto tscan_end;
                              }
                              else if ((chr == ASCII_BS) && (streamLen > 1))
                              {
                                    printt(tty, "%c\x1B[K", chr);
                                    *bin >>= 1;
                                    streamLen--;
                                    continue;
                              }
                              else
                              {
                                    continue;
                              }

                              chr -= '0';

                              *bin <<= 1;
                              *bin |= chr;

                              streamLen++;
                        }
                  }

                  if (chr == 's')
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
                              chr = getChart(tty);

                              /* check command Arrow Up */
                              if ((chr == ASCII_ESC) && (inCmdStep == 0))
                              {
                                    inCmdStep++;
                                    *(string++) = chr;
                                    continue;
                              }
                              else if ((chr == '[') && (inCmdStep == 1))
                              {
                                 inCmdStep++;
                                 *(string++) = chr;
                                 continue;
                              }
                              else if ((chr == 'A') && (inCmdStep == 2))
                              {
                                 *(string++) = chr;
                                 goto tscan_end;
                              }
                              else
                              {
                                    inCmdStep = 0;
                              }


                              /* put character */
                              if (chr == ASCII_CR || chr == ASCII_LF)
                              {
                                    *(string++) = 0x00;
                                    putChart(tty, ASCII_CR);
                                    putChart(tty, ASCII_LF);
                                    goto tscan_end;
                              }
                              else if ((chr == ASCII_BS) && (streamLen > 1))
                              {
                                    printt(tty, "%c\x1B[K", chr);
                                    *(--string) = 0x00;
                                    streamLen--;
                                    free++;
                                    continue;
                              }
                              else if (chr >= ' ')
                              {
                                    if (free)
                                    {
                                          putChart(tty, chr);
                                          *(string++) = chr;
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

      tscan_end:
            return (streamLen - 1);
}


/*==================================================================================================
                                            End of file
==================================================================================================*/
