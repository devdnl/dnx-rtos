/*=========================================================================*//**
@file    io.c

@author  Daniel Zorychta

@brief   This file support standard io functions

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


*//*==========================================================================*/

/*==============================================================================
 Include files
 =============================================================================*/
#include "io.h"
#include <string.h>
#include "vfs.h"
#include "memman.h"
#include "oswrap.h"

/*==============================================================================
 Local symbolic constants/macros
==============================================================================*/
#define calloc(nmemb, msize)              memman_calloc(nmemb, msize)
#define malloc(size)                      memman_malloc(size)
#define free(mem)                         memman_free(mem)

#define fopen(path, mode)                 vfs_fopen(path, mode)
#define fclose(file)                      vfs_fclose(file)
#define fwrite(ptr, isize, nitems, file)  vfs_fwrite(ptr, isize, nitems, file)
#define fread(ptr, isize, nitems, file)   vfs_fread(ptr, isize, nitems, file)
#define fseek(file, offset, mode)         vfs_fseek(file, offset, mode)
#define ftell(file)                       vfs_ftell(file)
#define ioctl(file, rq, data)             vfs_ioctl(file, rq, data)

/*==============================================================================
 Local types, enums definitions
==============================================================================*/

/*==============================================================================
 Local function prototypes
==============================================================================*/
#if (CONFIG_PRINTF_ENABLE > 0)
static void  reverseBuffer(ch_t *begin, ch_t *end);
static ch_t *itoa(i32_t val, ch_t *buf, u8_t base, bool_t usign_val, u8_t zeros_req);
static int_t CalcFormatSize(const ch_t *format, va_list arg);
#endif

/*==============================================================================
 Local object definitions
==============================================================================*/
#if (CONFIG_SYSTEM_MSG_ENABLE > 0 && CONFIG_PRINTF_ENABLE > 0)
static FILE_t *kprintFile;
#endif

/*==============================================================================
 Exported object definitions
==============================================================================*/

/*==============================================================================
 Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function reverse buffer
 *
 * @param *begin     buffer begin
 * @param *end       buffer end
 */
//==============================================================================
#if (CONFIG_PRINTF_ENABLE > 0)
static void reverseBuffer(ch_t *begin, ch_t *end)
{
        ch_t temp;

        while (end > begin) {
                temp     = *end;
                *end--   = *begin;
                *begin++ = temp;
        }
}
#endif

//==============================================================================
/**
 * @brief Function convert value to the character
 *
 * @param  val          converted value
 * @param *buf          result buffer
 * @param  base         conversion base
 * @param  usign_val    unsigned value conversion
 * @param  zeros_req    zeros required (added zeros to conversion)
 *
 * @return pointer in the buffer
 */
//==============================================================================
#if (CONFIG_PRINTF_ENABLE > 0)
static ch_t *itoa(i32_t val, ch_t *buf, u8_t base, bool_t usign_val, u8_t zeros_req)
{
        static const ch_t digits[] = "0123456789ABCDEF";

        ch_t  *bufferCopy = buf;
        i32_t  sign    = 0;
        u8_t   zeroCnt = 0;
        i32_t  quot;
        i32_t  rem;

        if (base < 2 || base > 16) {
                goto itoa_exit;
        }

        if (usign_val) {
                do {
                        quot   = (u32_t) ((u32_t) val / (u32_t) base);
                        rem    = (u32_t) ((u32_t) val % (u32_t) base);
                        *buf++ = digits[rem];
                        zeroCnt++;
                } while ((val = quot));
        } else {
                if ((base == 10) && ((sign = val) < 0)) {
                        val = -val;
                }

                do {
                        quot   = val / base;
                        rem    = val % base;
                        *buf++ = digits[rem];
                        zeroCnt++;
                } while ((val = quot));
        }

        while (zeros_req > zeroCnt) {
                *buf++ = '0';
                zeroCnt++;
        }

        if (sign < 0) {
                *buf++ = '-';
        }

        reverseBuffer(bufferCopy, (buf - 1));

        itoa_exit:
        *buf = '\0';
        return bufferCopy;
}
#endif

//==============================================================================
/**
 * @brief Function convert arguments to stream
 *
 * @param[in] *format        message format
 * @param[in]  arg           argument list
 *
 * @return size of snprintf result
 */
//==============================================================================
#if (CONFIG_PRINTF_ENABLE > 0)
static int_t CalcFormatSize(const ch_t *format, va_list arg)
{
        ch_t chr;
        int_t size = 1;

        while ((chr = *format++) != '\0') {
                if (chr != '%') {
                        if (chr == '\n') {
                                size += 2;
                        } else {
                                size++;
                        }
                } else {
                        chr = *format++;

                        if (chr == '%' || chr == 'c') {
                                if (chr == 'c') {
                                        chr = va_arg(arg, i32_t);
                                }

                                size++;
                                continue;
                        }

                        if (chr == 's') {
                                size += strlen(va_arg(arg, ch_t*));
                                continue;
                        }

                        if (chr == 'd' || chr == 'x' || chr == 'u') {
                                chr = va_arg(arg, i32_t);
                                size += 11;
                                continue;
                        }
                }
        }

        return size;
}
#endif

//==============================================================================
/**
 * @brief Function convert ASCII to the number
 * When function find any other character than number (depended of actual base)
 * immediately finished operation and return pointer when bad character was
 * found
 *
 * @param[in]  *string       string to decode
 * @param[in]   base         decode base
 * @param[out] *value        pointer to result
 *
 * @return pointer in string when operation was finished
 */
//==============================================================================
ch_t *io_atoi(ch_t *string, u8_t base, i32_t *value)
{
        ch_t character;
        i32_t sign = 1;
        bool_t charFound = FALSE;

        *value = 0;

        if (base < 2 && base > 16) {
                goto atoi_end;
        }

        while ((character = *string) != '\0') {
                /* if space exist, atoi continue finding correct character */
                if ((character == ' ') && (charFound == FALSE)) {
                        string++;
                        continue;
                } else {
                        charFound = TRUE;
                }

                /* check signum */
                if (character == '-') {
                        if (base == 10) {
                                if (sign == 1) {
                                        sign = -1;
                                }

                                string++;
                                continue;
                        } else {
                                goto atoi_sign;
                        }
                }

                /* check character range */
                if (character >= 'a') {
                        character -= 'a' - 10;
                } else if (character >= 'A') {
                        character -= 'A' - 10;
                } else if (character >= '0') {
                        character -= '0';
                } else {
                        goto atoi_sign;
                }

                /* check character range according to actual base */
                if (character >= base) {
                        break;
                }

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

//==============================================================================
/**
 * @brief Enable kprint functionality
 *
 * @param filename      path to file used to write kernel log
 */
//==============================================================================
void io_kprintEnable(ch_t *filename)
{
#if (CONFIG_SYSTEM_MSG_ENABLE > 0 && CONFIG_PRINTF_ENABLE > 0)
        /* close file if opened */
        if (kprintFile) {
                fclose(kprintFile);
                kprintFile = NULL;
        }

        /* open new file */
        if (kprintFile == NULL) {
                kprintFile = fopen(filename, "w");
        }
#else
        (void)filename;
#endif
}

//==============================================================================
/**
 * @brief Disable kprint functionality
 */
//==============================================================================
void io_kprintDisable(void)
{
#if (CONFIG_SYSTEM_MSG_ENABLE > 0 && CONFIG_PRINTF_ENABLE > 0)
        if (kprintFile) {
                fclose(kprintFile);
                kprintFile = NULL;
        }
#endif
}

//==============================================================================
/**
 * @brief Function send kernel message on terminal
 *
 * @param *format             formated text
 * @param ...                 format arguments
 *
 * @retval number of written characters
 */
//==============================================================================
int_t io_kprint(const ch_t *format, ...)
{
#if (CONFIG_SYSTEM_MSG_ENABLE > 0 && CONFIG_PRINTF_ENABLE > 0)
        va_list args;
        int_t n = 0;

        if (kprintFile) {
                va_start(args, format);
                int_t size = CalcFormatSize(format, args);
                va_end(args);

                ch_t *buffer = calloc(size, sizeof(ch_t));

                if (buffer) {
                        va_start(args, format);
                        n = io_vsnprintf(buffer, size, format, args);
                        va_end(args);

                        fwrite(buffer, sizeof(ch_t), size, kprintFile);

                        free(buffer);
                }
        }

        return n;
#else
        (void)format;
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Function put character into file
 *
 * @param  c                   character
 * @param *stream              file
 *
 * @retval c if OK otherwise EOF
 */
//==============================================================================
int_t io_fputc(int_t c, FILE_t *stream)
{
        if (stream) {
                fwrite(&c, sizeof(ch_t), 1, stream);
                return c;
        } else {
                return EOF;
        }
}

//==============================================================================
/**
 * @brief Function get character from file
 *
 * @param *stream            source file
 *
 * @retval character
 */
//==============================================================================
int_t io_getc(FILE_t *stream)
{
        int_t chr  = EOF;
        u16_t dcnt = 0;

        if (!stream) {
                return EOF;
        }

        while (fread(&chr, sizeof(ch_t), 1, stream) < 1) {
                if (dcnt >= 60000) {
                        TaskDelay(200);
                } else if (dcnt >= 5000) {
                        dcnt += 100;
                        TaskDelay(100);
                } else {
                        dcnt += 20;
                        TaskDelay(20);
                }
        }

        return chr;
}

//==============================================================================
/**
 * @brief Function gets number of bytes from file
 *
 * @param[out] *str          buffer with string
 * @param[in]   size         buffer size
 * @param[in]  *stream       source stream
 *
 * @retval NULL if error, otherwise pointer to str
 */
//==============================================================================
ch_t *io_fgets(ch_t *str, int_t size, FILE_t *stream)
{
        if (!str || !size || !stream) {
                return NULL;
        }

        for (int_t i = 0; i < size - 1; i++) {
                str[i] = io_getc(stream);

                if (str[i] == (ch_t)EOF && i == 0) {
                        return NULL;
                } else if (str[i] == '\n' || str[i] == (ch_t)EOF) {
                        str[i + 1] = '\0';
                        break;
                }
        }

        str[size - 1] = '\0';

        return str;
}

//==============================================================================
/**
 * @brief Function send to buffer formated output string
 *
 * @param *bfr                output buffer
 * @param  size               buffer size
 * @param *format             formated text
 * @param ...                 format arguments
 *
 * @retval number of written characters
 */
//==============================================================================
int_t io_snprintf(ch_t *bfr, u32_t size, const ch_t *format, ...)
{
#if (CONFIG_PRINTF_ENABLE > 0)
        va_list args;
        int_t n = 0;

        if (bfr) {
                va_start(args, format);
                n = io_vsnprintf(bfr, size, format, args);
                va_end(args);
        }

        return n;
#else
        (void)bfr;
        (void)size;
        (void)format;
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Function send on a standard output string
 *
 * @param *file               file
 * @param *format             formated text
 * @param ...                 format arguments
 *
 * @retval number of written characters
 */
//==============================================================================
int_t io_fprintf(FILE_t *file, const ch_t *format, ...)
{
#if (CONFIG_PRINTF_ENABLE > 0)
        va_list args;
        int_t n = 0;

        if (file) {
                va_start(args, format);
                u32_t size = CalcFormatSize(format, args);
                va_end(args);

                ch_t *str = calloc(1, size);

                if (str) {
                        va_start(args, format);
                        n = io_vsnprintf(str, size, format, args);
                        va_end(args);

                        fwrite(str, sizeof(ch_t), size, file);

                        free(str);
                }
        }

        return n;
#else
        (void)file;
        (void)format;
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Function convert arguments to stream
 *
 * @param[in] *buf           buffer for stream
 * @param[in]  size          buffer size
 * @param[in] *format        message format
 * @param[in]  arg           argument list
 *
 * @return number of printed characters
 */
//==============================================================================
int_t io_vsnprintf(ch_t *buf, size_t size, const ch_t *format, va_list arg)
{
#if (CONFIG_PRINTF_ENABLE > 0)
#define putCharacter(character)                 \
      {                                         \
            if ((size_t)slen < size)  {         \
                  *buf++ = character;           \
                  slen++;                       \
            }  else {                           \
                  goto vsnprint_end;            \
            }                                   \
      }

        ch_t  chr;
        int_t slen = 1;

        while ((chr = *format++) != '\0') {
                if (chr != '%') {
                        if (chr == '\n') {
                                putCharacter('\r');
                        }

                        putCharacter(chr);

                        continue;
                }

                chr = *format++;

                if (chr == '%' || chr == 'c') {
                        if (chr == 'c') {
                                chr = va_arg(arg, i32_t);
                        }

                        putCharacter(chr);

                        continue;
                }

                if (chr == 's' || chr == 'd' || chr == 'x' || chr == 'u') {
                        ch_t result[11];
                        ch_t *resultPtr;

                        if (chr == 's') {
                                resultPtr = va_arg(arg, ch_t*);
                        } else {
                                u8_t zeros = *format++;

                                if (zeros >= '0' && zeros <= '9') {
                                        zeros -= '0';
                                } else {
                                        zeros = 0;
                                        format--;
                                }

                                u8_t base = (
                                (chr == 'd') || (chr == 'u') ? 10 : 16);

                                bool_t uint = (
                                (chr == 'x') || (chr == 'u') ? TRUE : FALSE);

                                resultPtr = itoa(va_arg(arg, i32_t), result,
                                                 base, uint, zeros);
                        }

                        while ((chr = *resultPtr++)) {
                                putCharacter(chr);
                        }

                        continue;
                }
        }

        vsnprint_end:
        *buf = 0;
        return (slen - 1);

#undef putChar
#else
        (void)buf;
        (void)size;
        (void)format;
        (void)arg;
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Function scan stream
 *
 * @param[in]  *stream        file
 * @param[in]  *format        message format
 * @param[out]  ...           output
 *
 * @return number of scanned elements
 */
//==============================================================================
int_t io_fscanf(FILE_t *stream, const ch_t *format, ...)
{
#if (CONFIG_SCANF_ENABLE > 0)
        int_t n = 0;
        va_list args;

        ch_t *str = calloc(CONFIG_FSCANF_STREAM_BUFFER_SIZE, sizeof(ch_t));

        if (str == NULL) {
                return 0;
        }

        if (io_fgets(str, CONFIG_FSCANF_STREAM_BUFFER_SIZE, stream) == str) {
                for(uint_t i = 0; i < strlen(str); i++) {
                        if (str[i] == '\n') {
                                str[i] = '\0';
                                break;
                        }
                }

                va_start(args, format);
                n = io_vsscanf(str, format, args);
                va_end(args);
        }

        free(str);
        return n;
#else
        (void)stream;
        (void)format;
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Function scan arguments defined by format (multiple argument version)
 *
 * @param[in]  *str           data buffer
 * @param[in]  *format        scan format
 * @param[out]  ...           output
 *
 * @return number of printed characters
 */
//==============================================================================
int_t io_sscanf(const ch_t *str, const ch_t *format, ...)
{
#if (CONFIG_SCANF_ENABLE > 0)
        va_list args;
        va_start(args, format);
        int_t n = io_vsscanf(str, format, args);
        va_end(args);
        return n;
#else
        (void)str;
        (void)format;
        return 0;
#endif
}

//============================================================================//
/**
 * @brief Function scan arguments defined by format (argument list version)
 *
 * @param[in]  *str           data buffer
 * @param[in]  *format        scan format
 * @param[out]  ...           output
 *
 * @return number of printed characters
 */
//============================================================================//
int_t io_vsscanf(const ch_t *str, const ch_t *format, va_list args)
{
#if (CONFIG_SCANF_ENABLE > 0)
        int_t   read_fields = 0;
        ch_t    chr;
        int_t   value;
        ch_t   *strs;
        int_t   sign;
        ch_t   *string;
        uint_t  bfr_size;

        if (!str || !format) {
                return EOF;
        }

        if (str[0] == '\0') {
                return EOF;
        }

        while ((chr = *format++) != '\0') {
                if (chr == '%') {
                        chr = *format++;

                        /* calculate buffer size */
                        bfr_size = 0;
                        while (chr >= '0' && chr <= '9') {
                                bfr_size *= 10;
                                bfr_size += chr - '0';
                                chr       = *format++;
                        }

                        switch (chr) {
                        case '%':
                                if (*str == '%') {
                                        str++;
                                        continue;
                                } else {
                                        goto io_sscanf_end;
                                }
                                break;

                        case 'd':
                                value = 0;
                                sign  = 1;

                                if (*str == '-') {
                                        sign = -1;
                                        str++;
                                }

                                strs  = (ch_t*)str;

                                while (*str >= '0' && *str <= '9') {
                                        value *= 10;
                                        value += *str - '0';
                                        str++;
                                }

                                if (str != strs) {
                                        int_t *var = va_arg(args, int_t*);
                                        *var = value * sign;
                                        read_fields++;
                                }
                                break;

                        case 'x':
                        case 'X':
                                value = 0;
                                sign  = 1;

                                if (*str == '-') {
                                        sign = -1;
                                        str++;
                                }

                                strs  = (ch_t*)str;

                                while (  (*str >= '0' && *str <= '9')
                                      || (*str >= 'a' && *str <= 'f')
                                      || (*str >= 'A' && *str <= 'F') ) {

                                        uint_t var;

                                        if (*str >= 'a') {
                                                var = *str - 'a' + 10;
                                        } else if (*str >= 'A') {
                                                var = *str - 'A' + 10;
                                        } else if (*str >= '0') {
                                                var = *str - '0';
                                        } else {
                                                var = 0;
                                        }

                                        value *= 16;
                                        value += var;
                                        str++;
                                }

                                if (strs != str) {
                                        int_t *var = va_arg(args, int_t*);
                                        *var = value * sign;
                                        read_fields++;
                                }
                                break;

                        case 'o':
                                value = 0;
                                sign  = 1;

                                if (*str == '-') {
                                        sign = -1;
                                        str++;
                                }

                                strs  = (ch_t*)str;

                                while (*str >= '0' && *str <= '7') {
                                        value *= 8;
                                        value += *str - '0';
                                        str++;
                                }

                                if (str != strs) {
                                        int_t *var = va_arg(args, int_t*);
                                        *var = value * sign;
                                        read_fields++;
                                }
                                break;

                        case 'c':
                                if (*str >= ' ') {
                                        ch_t *var = va_arg(args, ch_t*);
                                        *var = *str;
                                        read_fields++;
                                        str++;
                                }
                                break;

                        case 's':
                                string = va_arg(args, ch_t*);
                                strcpy(string, str);
                                read_fields++;
                                goto io_sscanf_end;
                        }
                } else if (chr <= ' ') {
                        while (*str <= ' ' && *str != '\0') {
                                str++;
                        }

                        if (*str == '\0') {
                                break;
                        }
                } else {
                        while (  *str == chr && chr != '%'
                              &&  chr > ' '  && chr != '\0') {
                                str++;
                                chr = *format++;
                        }

                        if (chr == '%' || chr <= ' ') {
                                format--;
                                continue;
                        } else {
                                break;
                        }
                }
        }

        io_sscanf_end:
        return read_fields;
#else
        (void)str;
        (void)format;
        (void)args;
        return 0;
#endif
}

/*==============================================================================
 End of file
==============================================================================*/
