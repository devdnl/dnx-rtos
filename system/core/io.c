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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 Include files
 =============================================================================*/
#include "core/io.h"
#include "core/vfs.h"
#include "core/sysmoni.h"
#include "kernel/kwrapper.h"

/*==============================================================================
 Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
 Local types, enums definitions
==============================================================================*/

/*==============================================================================
 Local function prototypes
==============================================================================*/
#if (CONFIG_PRINTF_ENABLE > 0)
static char *itoa(i32_t val, char *buf, u8_t base, bool usign_val, u8_t zeros_req);
static int   calc_format_size(const char *format, va_list arg);
#endif

/*==============================================================================
 Local object definitions
==============================================================================*/
#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
static file_t *io_printk_file;
#endif

/*==============================================================================
 Exported object definitions
==============================================================================*/

/*==============================================================================
 Function definitions
==============================================================================*/

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
static char *itoa(i32_t val, char *buf, u8_t base, bool usign_val, u8_t zeros_req)
{
        static const char digits[] = "0123456789ABCDEF";

        char  *bufferCopy = buf;
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

        /* reverse buffer */
        char  temp;
        char *begin = bufferCopy;
        char *end   = (buf - 1);

        while (end > begin) {
                temp     = *end;
                *end--   = *begin;
                *begin++ = temp;
        }

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
static int calc_format_size(const char *format, va_list arg)
{
        char chr;
        int size = 1;

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
                                size += strlen(va_arg(arg, char*));
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
char *io_atoi(char *string, u8_t base, i32_t *value)
{
        char  character;
        i32_t sign = 1;
        bool  charFound = FALSE;

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
 * @brief Enable printk functionality
 *
 * @param filename      path to file used to write kernel log
 */
//==============================================================================
#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
void io_enable_printk(char *filename)
{
        /* close file if opened */
        if (io_printk_file) {
                vfs_fclose(io_printk_file);
                io_printk_file = NULL;
        }

        /* open new file */
        if (io_printk_file == NULL) {
                io_printk_file = vfs_fopen(filename, "w");
        }
}
#endif

//==============================================================================
/**
 * @brief Disable printk functionality
 */
//==============================================================================
#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
void io_disable_printk(void)
{
        if (io_printk_file) {
                vfs_fclose(io_printk_file);
                io_printk_file = NULL;
        }
}
#endif

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
#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
void io_printk(const char *format, ...)
{
        va_list args;

        if (io_printk_file) {
                va_start(args, format);
                int size = calc_format_size(format, args);
                va_end(args);

                char *buffer = sysm_syscalloc(size, sizeof(char));

                if (buffer) {
                        va_start(args, format);
                        io_vsnprintf(buffer, size, format, args);
                        va_end(args);

                        vfs_fwrite(buffer, sizeof(char), size, io_printk_file);

                        sysm_sysfree(buffer);
                }
        }
}
#endif

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
int io_fputc(int c, file_t *stream)
{
        if (stream) {
                char ch = (char)c;
                if (vfs_fwrite(&ch, sizeof(char), 1, stream) < 1) {
                        return EOF;
                } else {
                        return c;
                }
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
int io_getc(file_t *stream)
{
        int chr  = EOF;
        u16_t dcnt = 0;

        if (!stream) {
                return EOF;
        }

        while (vfs_fread(&chr, sizeof(char), 1, stream) < 1) {
                if (dcnt >= 60000) {
                        milisleep(200);
                } else if (dcnt >= 5000) {
                        dcnt += 100;
                        milisleep(100);
                } else {
                        dcnt += 20;
                        milisleep(20);
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
char *io_fgets(char *str, int size, file_t *stream)
{
        if (!str || !size || !stream) {
                return NULL;
        }

        for (int i = 0; i < size - 1; i++) {
                str[i] = io_getc(stream);

                if (str[i] == (char)EOF && i == 0) {
                        return NULL;
                } else if (str[i] == '\n' || str[i] == (char)EOF) {
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
#if (CONFIG_PRINTF_ENABLE > 0)
int io_snprintf(char *bfr, u32_t size, const char *format, ...)
{
        va_list args;
        int n = 0;

        if (bfr) {
                va_start(args, format);
                n = io_vsnprintf(bfr, size, format, args);
                va_end(args);
        }

        return n;
}
#endif

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
#if (CONFIG_PRINTF_ENABLE > 0)
int io_fprintf(file_t *file, const char *format, ...)
{
        va_list args;
        int n = 0;

        if (file) {
                va_start(args, format);
                u32_t size = calc_format_size(format, args);
                va_end(args);

                char *str = sysm_syscalloc(1, size);

                if (str) {
                        va_start(args, format);
                        n = io_vsnprintf(str, size, format, args);
                        va_end(args);

                        vfs_fwrite(str, sizeof(char), size, file);

                        sysm_sysfree(str);
                }
        }

        return n;
}
#endif

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
#if (CONFIG_PRINTF_ENABLE > 0)
int io_vsnprintf(char *buf, size_t size, const char *format, va_list arg)
{
#define putCharacter(character)                 \
        {                                       \
                if ((size_t)slen < size)  {     \
                        *buf++ = character;     \
                        slen++;                 \
                }  else {                       \
                        goto vsnprint_end;      \
                }                               \
        }

        char  chr;
        int slen = 1;

        while ((chr = *format++) != '\0') {
                if (chr != '%') {
                        putCharacter(chr);
                        continue;
                }

                chr = *format++;

                if (chr == '%' || chr == 'c') {
                        if (chr == 'c') {
                                chr = va_arg(arg, int);
                        }

                        putCharacter(chr);
                        continue;
                }

                if (chr == 's' || chr == 'd' || chr == 'x' || chr == 'u') {
                        char result[11];
                        char *resultPtr;

                        if (chr == 's') {
                                resultPtr = va_arg(arg, char*);
                        } else {
                                u8_t zeros = *format++;

                                if (zeros >= '0' && zeros <= '9') {
                                        zeros -= '0';
                                } else {
                                        zeros = 0;
                                        format--;
                                }

                                u8_t base = (chr == 'd' || chr == 'u' ? 10 : 16);
                                bool uint = (chr == 'x' || chr == 'u' ? TRUE : FALSE);

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
}
#endif

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
#if (CONFIG_SCANF_ENABLE > 0)
int io_fscanf(file_t *stream, const char *format, ...)
{
        int n = 0;
        va_list args;

        char *str = sysm_syscalloc(BUFSIZ, sizeof(char));

        if (str == NULL) {
                return 0;
        }

        if (io_fgets(str, BUFSIZ, stream) == str) {
                for(uint i = 0; i < strlen(str); i++) {
                        if (str[i] == '\n') {
                                str[i] = '\0';
                                break;
                        }
                }

                va_start(args, format);
                n = io_vsscanf(str, format, args);
                va_end(args);
        }

        sysm_sysfree(str);
        return n;
}
#endif

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
#if (CONFIG_SCANF_ENABLE > 0)
int io_sscanf(const char *str, const char *format, ...)
{
        va_list args;
        va_start(args, format);
        int n = io_vsscanf(str, format, args);
        va_end(args);
        return n;
}
#endif

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
#if (CONFIG_SCANF_ENABLE > 0)
int io_vsscanf(const char *str, const char *format, va_list args)
{
        int   read_fields = 0;
        char    chr;
        int   value;
        char   *strs;
        int   sign;
        char   *string;
        uint  bfr_size;

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

                                strs  = (char*)str;

                                while (*str >= '0' && *str <= '9') {
                                        value *= 10;
                                        value += *str - '0';
                                        str++;
                                }

                                if (str != strs) {
                                        int *var = va_arg(args, int*);

                                        if (var) {
                                                *var = value * sign;
                                                read_fields++;
                                        }
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

                                strs  = (char*)str;

                                while (  (*str >= '0' && *str <= '9')
                                      || (*str >= 'a' && *str <= 'f')
                                      || (*str >= 'A' && *str <= 'F') ) {

                                        uint var;

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
                                        int *var = va_arg(args, int*);

                                        if (var) {
                                                *var = value * sign;
                                                read_fields++;
                                        }
                                }
                                break;

                        case 'o':
                                value = 0;
                                sign  = 1;

                                if (*str == '-') {
                                        sign = -1;
                                        str++;
                                }

                                strs  = (char*)str;

                                while (*str >= '0' && *str <= '7') {
                                        value *= 8;
                                        value += *str - '0';
                                        str++;
                                }

                                if (str != strs) {
                                        int *var = va_arg(args, int*);

                                        if (var) {
                                                *var = value * sign;
                                                read_fields++;
                                        }
                                }
                                break;

                        case 'c':
                                if (*str >= ' ') {
                                        char *var = va_arg(args, char*);

                                        if (var) {
                                                *var = *str;
                                                read_fields++;
                                        }
                                        str++;
                                }
                                break;

                        case 's':
                                string = va_arg(args, char*);
                                if (string) {
                                        strcpy(string, str);
                                        read_fields++;
                                }
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
}
#endif

#ifdef __cplusplus
}
#endif

/*==============================================================================
 End of file
==============================================================================*/
