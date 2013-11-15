/*=========================================================================*//**
@file    printx.c

@author  Daniel Zorychta

@brief   Basic print functions

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "config.h"
#include "core/printx.h"
#include "core/sysmoni.h"
#include "core/progman.h"
#include "kernel/kwrapper.h"
#include <unistd.h>
#include <errno.h>

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
#if (CONFIG_PRINTF_ENABLE > 0)
static char *itoa(i32_t val, char *buf, u8_t base, bool usign_val, u8_t zeros_req);
static int   calc_format_size(const char *format, va_list arg);
#endif

/*==============================================================================
  Local objects
==============================================================================*/
#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
static FILE *sys_printk_file;
#endif

/*==============================================================================
  Exported objects
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

        char  *buffer_copy = buf;
        i32_t  sign     = 0;
        u8_t   zero_cnt = 0;
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
                        zero_cnt++;
                } while ((val = quot));
        } else {
                if ((base == 10) && ((sign = val) < 0)) {
                        val = -val;
                }

                do {
                        quot   = val / base;
                        rem    = val % base;
                        *buf++ = digits[rem];
                        zero_cnt++;
                } while ((val = quot));
        }

        while (zeros_req > zero_cnt) {
                *buf++ = '0';
                zero_cnt++;
        }

        if (sign < 0) {
                *buf++ = '-';
        }

        /* reverse buffer */
        char *begin = buffer_copy;
        char *end   = (buf - 1);

        while (end > begin) {
                char temp = *end;
                *end--    = *begin;
                *begin++  = temp;
        }

        itoa_exit:
        *buf = '\0';
        return buffer_copy;
}
#endif

//==============================================================================
/**
 * @brief Function convert double to string
 *
 * @note used software: nickgsuperstar@gmail.com & nickg@client9.com
 *                      https://code.google.com/p/stringencoders/
 *
 * @param[in]   value           input value
 * @param[out] *str             string - result
 * @param[in]   prec            precision
 *
 * @return number of characters
 */
//==============================================================================
#if (CONFIG_PRINTF_ENABLE > 0)
static int dtoa(double value, char* str, int prec, int n)
{
#define push_char(c) if (conv < n) {*wstr++ = c; conv++;}

        const double pow10[] = {1, 10, 100, 1000, 10000, 100000, 1000000,
                                10000000, 100000000, 1000000000};

        void strreverse(char* begin, char* end)
        {
                char aux;
                while (end > begin) {
                        aux = *end, *end-- = *begin, *begin++ = aux;
                }
        }

        int  conv  = 0;
        char *wstr = str;

        /*
         * Hacky test for NaN
         * under -fast-math this won't work, but then you also won't
         * have correct nan values anyways.  The alternative is
         * to link with libmath (bad) or hack IEEE double bits (bad)
         */
        if (!(value == value)) {
                push_char('n');
                push_char('a');
                push_char('n');
                return conv;
        }

        /* if input is larger than thres_max, revert to exponential */
        const double thres_max = (double) (0x7FFFFFFF);

        double diff  = 0.0;

        if (prec < 0) {
                prec = 0;
        } else if (prec > 9) {
                /* precision of >= 10 can lead to overflow errors */
                prec = 9;
        }

        /* we'll work in positive values and deal with the negative sign issue later */
        int neg = 0;
        if (value < 0) {
                neg = 1;
                value = -value;
        }

        int    whole = (int) value;
        double tmp   = (value - whole) * pow10[prec];
        u32_t  frac  = (u32_t)tmp;

        diff = tmp - frac;

        if (diff > 0.5) {
                ++frac;

                /* handle rollover, e.g.  case 0.99 with prec 1 is 1.0  */
                if (frac >= pow10[prec]) {
                        frac = 0;
                        ++whole;
                }
        } else if (diff == 0.5 && ((frac == 0) || (frac & 1))) {
                /* if halfway, round up if odd, OR if last digit is 0.  That last part is strange */
                ++frac;
        }

        /* for very large numbers switch back to native sprintf for exponentials.
         anyone want to write code to replace this? */
        /*
         normal printf behavior is to print EVERY whole number digit
         which can be 100s of characters overflowing your buffers == bad
         */
        if (value > thres_max) {
                push_char('E');
                push_char('?');
                return conv;
        }

        if (prec == 0) {
                diff = value - whole;
                if (diff > 0.5) {
                        /* greater than 0.5, round up, e.g. 1.6 -> 2 */
                        ++whole;
                } else if (diff == 0.5 && (whole & 1)) {
                        /* exactly 0.5 and ODD, then round up */
                        /* 1.5 -> 2, but 2.5 -> 2 */
                        ++whole;
                }
        } else {
                int count = prec;

                /* now do fractional part, as an unsigned number */
                do {
                        --count;
                        push_char((char) (48 + (frac % 10)));
                } while (frac /= 10);

                /* add extra 0s */
                while (count-- > 0) {
                        push_char('0');
                }

                /* add decimal */
                push_char('.');
        }

        /* Do whole part. Take care of sign. Conversion. Number is reversed. */
        do {
                push_char((char) (48 + (whole % 10)));
        } while (whole /= 10);

        if (neg) {
                push_char('-');
        }

        /* remove ending zeros */
        int m   = n < conv ? n : conv;
        int del = 0;

        for (int i = 0; i < m; i++) {
                if (str[i] == '0' && m > 1) {
                        str[i] = ' ';
                        del++;
                } else if (str[i] == '.') {
                        str[i] = ' ';
                        del++;
                        break;
                } else {
                        break;
                }
        }
        conv -= del;

        strreverse(str, wstr - 1);

        return conv;
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

                        while (chr >= '0' && chr <= '9') {
                                chr = *format++;
                        }

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

                        if (chr == 'd' || chr == 'x' || chr == 'u' || chr == 'i' || chr == 'X') {
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
 * @brief Enable printk functionality
 *
 * @param filename      path to file used to write kernel log
 */
//==============================================================================
void printk_enable(char *filename)
{
#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
        /* close file if opened */
        if (sys_printk_file) {
                vfs_fclose(sys_printk_file);
                sys_printk_file = NULL;
        }

        /* open new file */
        if (sys_printk_file == NULL) {
                sys_printk_file = vfs_fopen(filename, "w");
        }
#else
        UNUSED_ARG(filename);
#endif
}

//==============================================================================
/**
 * @brief Disable printk functionality
 */
//==============================================================================
void printk_disable(void)
{
#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
        if (sys_printk_file) {
                vfs_fclose(sys_printk_file);
                sys_printk_file = NULL;
        }
#endif
}

//==============================================================================
/**
 * @brief Function send kernel message on terminal
 *
 * @param *format             formated text
 * @param ...                 format arguments
 */
//==============================================================================
void printk(const char *format, ...)
{
#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
        va_list args;

        if (sys_printk_file) {
                va_start(args, format);
                int size = calc_format_size(format, args);
                va_end(args);

                char *buffer = sysm_syscalloc(size, sizeof(char));

                if (buffer) {
                        va_start(args, format);
                        sys_vsnprintf(buffer, size, format, args);
                        va_end(args);

                        vfs_fwrite(buffer, sizeof(char), size, sys_printk_file);

                        sysm_sysfree(buffer);
                }
        }
#else
        UNUSED_ARG(format);
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
int sys_fputc(int c, FILE *stream)
{
        if (stream) {
                char ch = (char)c;
                if (vfs_fwrite(&ch, sizeof(char), 1, stream) == 1) {
                        return c;
                }
        }

        return EOF;
}

//==============================================================================
/**
 * @brief Function puts string to selected file (fputs & puts)
 *
 * @param[in] *s        string
 * @param[in] *file     file
 * @param[in]  puts     puts functionality (true: add \n at the end of string)
 *
 * @return number of puts characters
 */
//==============================================================================
int sys_f_puts(const char *s, FILE *file, bool puts)
{
        if (file) {
                int n = vfs_fwrite(s, sizeof(char), strlen(s), file);

                if (puts && n) {
                        n += vfs_fwrite("\n", sizeof(char), 1, file);
                }

                if (n != 0)
                        return n;
        }

        return EOF;
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
int sys_getc(FILE *stream)
{
        if (!stream) {
                return EOF;
        }

        int chr = 0;
        while (vfs_fread(&chr, sizeof(char), 1, stream) < 1) {
                if (vfs_ferror(stream) || vfs_feof(stream))
                        return EOF;
                else
                        sleep_ms(10);
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
char *sys_fgets(char *str, int size, FILE *stream)
{
        if (!str || size < 2 || !stream) {
                return NULL;
        }

        u64_t fpos = vfs_ftell(stream);

        int n;
        while ((n = vfs_fread(str, sizeof(char), size - 1, stream)) == 0) {
                if (vfs_ferror(stream) || vfs_feof(stream)) {
                        return NULL;
                }
        }

        char *end;
        if ((end = strchr(str, '\n'))) {
                end++;
                *end = '\0';
        } else {
                str[n] = '\0';
        }

        int len = strlen(str);

        if (len == 0)
                len = 1;

        vfs_fseek(stream, fpos + len, SEEK_SET);

        return str;
}

//==============================================================================
/**
 * @brief Function send to buffer formated output string
 *
 * @param *bfr                output buffer
 * @param  size               buffer size
 * @param *format             formated text
 * @param  ...                format arguments
 *
 * @retval number of written characters
 */
//==============================================================================
int sys_snprintf(char *bfr, size_t size, const char *format, ...)
{
#if (CONFIG_PRINTF_ENABLE > 0)
        va_list args;
        int n = 0;

        if (bfr && size && format) {
                va_start(args, format);
                n = sys_vsnprintf(bfr, size, format, args);
                va_end(args);
        }

        return n;
#else
        UNUSED_ARG(bfr);
        UNUSED_ARG(size);
        UNUSED_ARG(format);
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
int sys_fprintf(FILE *file, const char *format, ...)
{
#if (CONFIG_PRINTF_ENABLE > 0)
        va_list args;
        int n = 0;

        if (file && format) {
                va_start(args, format);
                u32_t size = calc_format_size(format, args);
                va_end(args);

                char *str = sysm_syscalloc(1, size);

                if (str) {
                        va_start(args, format);
                        n = sys_vsnprintf(str, size, format, args);
                        va_end(args);

                        vfs_fwrite(str, sizeof(char), n, file);

                        sysm_sysfree(str);
                }
        }

        return n;
#else
        UNUSED_ARG(file);
        UNUSED_ARG(format);
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Function returns error string
 *
 * @param errnum        error number
 *
 * @return error number string
 */
//==============================================================================
const char *sys_strerror(int errnum)
{
#if (CONFIG_PRINTF_ENABLE > 0)
        static const char *errstr[] = {
#if   (CONFIG_ERRNO_STRING_LEN == 0)
                "",
#elif (CONFIG_ERRNO_STRING_LEN == 1)
                "1",
                "2",
                "3",
                "4",
                "5",
                "6",
                "7",
                "8",
                "9",
                "10",
                "11",
                "12",
                "13",
                "14",
                "15",
                "16",
                "17",
                "18",
                "19",
                "20",
                "21",
                "22",
                "23",
                "24",
                "25",
                "26",
                "27",
                "28",
                "29",
                "30",
                "31",
                "32",
                "33",
                "34",
                "35"
#elif (CONFIG_ERRNO_STRING_LEN == 2)
                "ESUCC",
                "EPERM"
                "ENOENT",
                "ESRCH",
                "EIO",
                "ENXIO",
                "E2BIG",
                "ENOEXEC",
                "EAGAIN",
                "ENOMEM",
                "EACCES",
                "EFAULT",
                "EBUSY",
                "EEXIST",
                "ENODEV",
                "ENOTDIR",
                "EISDIR",
                "EINVAL",
                "EMFILE",
                "EFBIG",
                "ENOSPC",
                "ESPIPE",
                "EROFS",
                "EDOM",
                "ERANGE",
                "EILSEQ",
                "ENAMETOOLONG",
                "ENOTEMPTY",
                "EBADRQC",
                "ETIME",
                "ENONET",
                "EUSERS",
                "EADDRINUSE",
                "ENOMEDIUM",
                "EMEDIUMTYPE",
                "ECANCELED"
#elif (CONFIG_ERRNO_STRING_LEN == 3)
                "Success",
                "Operation not permitted",
                "No such file or directory",
                "No such process",
                "I/O error",
                "No such device or address",
                "Argument list too long",
                "Exec format error",
                "Try again",
                "Out of memory",
                "Permission denied",
                "Bad address",
                "Device or resource busy",
                "File exists",
                "No such device",
                "Not a directory",
                "Is a directory",
                "Invalid argument",
                "Too many open files",
                "File too large",
                "No space left on device",
                "Illegal seek",
                "Read-only file system",
                "Math argument out of domain of func",
                "Math result not representable",
                "Illegal byte sequence",
                "File name too long",
                "Directory not empty",
                "Invalid request code",
                "Timer expired",
                "Machine is not on the network",
                "Too many users",
                "Address already in use",
                "No medium found",
                "Wrong medium type",
                "Operation Canceled"
#else
#error "CONFIG_ERRNO_STRING_LEN should be in range 0 - 3!"
#endif
        };

        if (CONFIG_ERRNO_STRING_LEN == 0) {
                return errstr[0];
        } else if (errnum < _ENUMBER) {
                return errstr[errnum];
        } else {
                return "Unknown error";
        }
#else
        (void) errnum;
        return "";
#endif
}

//==============================================================================
/**
 * @brief Function prints error string
 *
 * @param str           string to print or NULL
 */
//==============================================================================
void sys_perror(const char *str)
{
#if (CONFIG_PRINTF_ENABLE > 0)
        if (str) {
                sys_fprintf(stderr, "%s: %s\n", str, sys_strerror(_task_get_data()->f_errno));
        } else {
                sys_fprintf(stderr, "%s\n", sys_strerror(_task_get_data()->f_errno));
        }
#else
        (void) str
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
//===============================================================================
int sys_vsnprintf(char *buf, size_t size, const char *format, va_list arg)
{
#if (CONFIG_PRINTF_ENABLE > 0)
        #define put_character(character) {      \
                if ((size_t)slen < size)  {     \
                        *buf++ = character;     \
                        slen++;                 \
                }  else {                       \
                        goto vsnprint_end;      \
                }                               \
        }

        char chr;
        int  slen = 1;
        int  arg_size;

        while ((chr = *format++) != '\0') {
                if (chr != '%') {
                        put_character(chr);
                        continue;
                }

                chr = *format++;

                arg_size = 0;
                while (chr >= '0' && chr <= '9') {
                        arg_size *= 10;
                        arg_size += chr - '0';
                        chr       = *format++;
                }

                if (chr == '%' || chr == 'c') {
                        if (chr == 'c') {
                                chr = va_arg(arg, int);
                        }

                        put_character(chr);
                        continue;
                }

                if (chr == 's' || chr == 'd' || chr == 'x' || chr == 'u' || chr == 'X' || chr == 'i') {
                        char result[12];
                        memset(result, 0, sizeof(result));
                        char *resultPtr;

                        if (chr == 's') {
                                resultPtr = va_arg(arg, char*);
                                if (!resultPtr) {
                                        resultPtr = "(null)";
                                }
                        } else {
                                if (arg_size > 9) {
                                        arg_size = 9;
                                }

                                u8_t base    = (chr == 'd' || chr == 'u' || chr == 'i' ? 10 : 16);
                                bool uint_en = (chr == 'x' || chr == 'u' || chr == 'X' ? TRUE : FALSE);

                                resultPtr = itoa(va_arg(arg, i32_t), result,
                                                 base, uint_en, arg_size);
                        }

                        while ((chr = *resultPtr++)) {
                                put_character(chr);
                        }

                        continue;
                }

                if (chr == 'f') {
                        slen += dtoa(va_arg(arg, double), buf, 6, size - slen);
                        buf  += slen;
                        continue;
                }
        }

vsnprint_end:
        *buf = 0;
        return (slen - 1);
        #undef put_character
#else
        UNUSED_ARG(buf);
        UNUSED_ARG(size);
        UNUSED_ARG(format);
        UNUSED_ARG(arg);
        return 0;
#endif
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
