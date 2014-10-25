/*=========================================================================*//**
@file    printx.c

@author  Daniel Zorychta

@brief   Basic print functions

@note    Copyright (C) 2013, 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
==============================================================================*/
#include "config.h"
#include "core/printx.h"
#include "core/sysmoni.h"
#include "core/progman.h"
#include "kernel/kwrapper.h"
#include "dnx/misc.h"
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

/*==============================================================================
  Local macros
==============================================================================*/
#define TO_STR(str)                     #str
#define NUMBER_TO_STR(val)              TO_STR(val)

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
#if (CONFIG_PRINTF_ENABLE > 0)
static char *itoa(i32_t val, char *buf, u8_t base, bool usign_val, u8_t zeros_req);
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
        i32_t  sign        = 0;
        u8_t   zero_cnt    = 0;

        if (base >= 2 && base <= 16) {

                if ((base == 10) && ((sign = val) < 0) && !usign_val) {
                        val = -val;
                }

                i32_t quot;
                i32_t rem;
                do {
                        if (usign_val) {
                                quot = static_cast(u32_t, val) / base;
                                rem  = static_cast(u32_t, val) % base;
                        } else {
                                quot = val / base;
                                rem  = val % base;
                        }

                        *buf++ = digits[rem];
                        zero_cnt++;
                } while ((val = quot));

                while (zeros_req > zero_cnt) {
                        *buf++ = '0';
                        zero_cnt++;
                }

                if (sign < 0 && !usign_val) {
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
        }

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
 * @param[in]   n               buffer size
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
                int size = sys_vsnprintf(NULL, 0, format, args) + 1;
                va_end(args);

                char *buffer = sysm_syscalloc(size, sizeof(char));
                if (buffer) {
                        va_start(args, format);
                        int n = sys_vsnprintf(buffer, size, format, args);
                        va_end(args);

                        vfs_fwrite(buffer, sizeof(char), n, sys_printk_file);

                        if (LAST_CHARACTER(buffer) != '\n') {
                                vfs_fflush(sys_printk_file);
                        }

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
#if (CONFIG_PRINTF_ENABLE > 0)
        if (stream) {
                char ch = (char)c;
                if (vfs_fwrite(&ch, sizeof(char), 1, stream) == 1) {
                        return c;
                }
        }
#else
        UNUSED_ARG(c);
        UNUSED_ARG(stream);
#endif
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
 * @return number of characters written to the stream
 */
//==============================================================================
int sys_f_puts(const char *s, FILE *file, bool puts)
{
#if (CONFIG_PRINTF_ENABLE > 0)
        if (file) {
                int n = vfs_fwrite(s, sizeof(char), strlen(s), file);

                if (puts) {
                        n += vfs_fwrite("\n", sizeof(char), 1, file);
                }

                if (n != 0)
                        return n;
        }
#else
        UNUSED_ARG(s);
        UNUSED_ARG(file);
        UNUSED_ARG(puts);
#endif
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
#if (CONFIG_PRINTF_ENABLE > 0)
        if (!stream) {
                return EOF;
        }

        int chr = 0;
        if (vfs_fread(&chr, sizeof(char), 1, stream) != 0) {
                if (vfs_ferror(stream) || vfs_feof(stream)) {
                        return EOF;
                }
        } else {
                return EOF;
        }

        return chr;
#else
        UNUSED_ARG(stream);
        return EOF;
#endif
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
#if (CONFIG_PRINTF_ENABLE > 0)
        if (!str || size < 2 || !stream) {
                return NULL;
        }

        struct stat file_stat;
        if (vfs_fstat(stream, &file_stat) == 0) {
                if (file_stat.st_type == FILE_TYPE_PIPE || file_stat.st_type == FILE_TYPE_DRV) {
                        int n = 0;
                        for (int i = 0; i < size - 1; i++) {
                                int m = vfs_fread(str + i, sizeof(char), 1, stream);
                                if (m == 0) {
                                        str[i] = '\0';
                                        return str;
                                } else {
                                        n += m;
                                }

                                if (vfs_ferror(stream) || vfs_feof(stream)) {
                                        if (n == 0) {
                                                return NULL;
                                        } else {
                                                str[i + 1] = '\0';
                                                return str;
                                        }
                                }

                                if (str[i] == '\n') {
                                        str[i + 1] = '\0';
                                        break;
                                }
                        }

                        return str;
                } else {
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

                        if (len != 0 && len < n && vfs_feof(stream))
                                vfs_clearerr(stream);

                        if (len == 0)
                                len = 1;

                        vfs_fseek(stream, fpos + len, SEEK_SET);

                        return str;
                }
        }
#else
        UNUSED_ARG(str);
        UNUSED_ARG(size);
        UNUSED_ARG(stream);
#endif
        return NULL;
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
 * @brief Function write to file formatted string
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
        int n = 0;

        if (file && format) {
                va_list args;
                va_start(args, format);
                n = sys_vfprintf(file, format, args);
                va_end(args);
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
 * @brief Function write to file formatted string
 *
 * @param file                file
 * @param format              formated text
 * @param arg                 arguments
 *
 * @retval number of written characters
 */
//==============================================================================
int sys_vfprintf(FILE *file, const char *format, va_list arg)
{
#if (CONFIG_PRINTF_ENABLE > 0)
        int n = 0;

        if (file && format) {
                va_list carg;
                va_copy(carg, arg);
                u32_t size = sys_vsnprintf(NULL, 0, format, carg) + 1;

                char *str = sysm_syscalloc(1, size);
                if (str) {
                        n = sys_vsnprintf(str, size, format, arg);
                        vfs_fwrite(str, sizeof(char), n, file);
                        sysm_sysfree(str);
                }
        }

        return n;
#else
        UNUSED_ARG(file);
        UNUSED_ARG(format);
        UNUSED_ARG(arg);
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
#if (CONFIG_ERRNO_STRING_LEN == 0)
                /* empty */
#elif (CONFIG_ERRNO_STRING_LEN == 1)
                [ESUCC       ] = NUMBER_TO_STR(ESUCC),
                [EPERM       ] = NUMBER_TO_STR(EPERM),
                [ENOENT      ] = NUMBER_TO_STR(ENOENT),
                [ESRCH       ] = NUMBER_TO_STR(ESRCH),
                [EIO         ] = NUMBER_TO_STR(EIO),
                [ENXIO       ] = NUMBER_TO_STR(ENXIO),
                [E2BIG       ] = NUMBER_TO_STR(E2BIG),
                [ENOEXEC     ] = NUMBER_TO_STR(ENOEXEC),
                [EAGAIN      ] = NUMBER_TO_STR(EAGAIN),
                [ENOMEM      ] = NUMBER_TO_STR(ENOMEM),
                [EACCES      ] = NUMBER_TO_STR(EACCES),
                [EFAULT      ] = NUMBER_TO_STR(EFAULT),
                [EBUSY       ] = NUMBER_TO_STR(EBUSY),
                [EEXIST      ] = NUMBER_TO_STR(EEXIST),
                [ENODEV      ] = NUMBER_TO_STR(ENODEV),
                [ENOTDIR     ] = NUMBER_TO_STR(ENOTDIR),
                [EISDIR      ] = NUMBER_TO_STR(EISDIR),
                [EINVAL      ] = NUMBER_TO_STR(EINVAL),
                [EMFILE      ] = NUMBER_TO_STR(EMFILE),
                [EFBIG       ] = NUMBER_TO_STR(EFBIG),
                [ENOSPC      ] = NUMBER_TO_STR(ENOSPC),
                [ESPIPE      ] = NUMBER_TO_STR(ESPIPE),
                [EROFS       ] = NUMBER_TO_STR(EROFS),
                [EDOM        ] = NUMBER_TO_STR(EDOM),
                [ERANGE      ] = NUMBER_TO_STR(ERANGE),
                [EILSEQ      ] = NUMBER_TO_STR(EILSEQ),
                [ENAMETOOLONG] = NUMBER_TO_STR(ENAMETOOLONG),
                [ENOTEMPTY   ] = NUMBER_TO_STR(ENOTEMPTY),
                [EBADRQC     ] = NUMBER_TO_STR(EBADRQC),
                [ETIME       ] = NUMBER_TO_STR(ETIME),
                [ENONET      ] = NUMBER_TO_STR(ENONET),
                [EUSERS      ] = NUMBER_TO_STR(EUSERS),
                [EADDRINUSE  ] = NUMBER_TO_STR(EADDRINUSE),
                [ENOMEDIUM   ] = NUMBER_TO_STR(ENOMEDIUM),
                [EMEDIUMTYPE ] = NUMBER_TO_STR(EMEDIUMTYPE),
                [ECANCELED   ] = NUMBER_TO_STR(ECANCELED)
#elif (CONFIG_ERRNO_STRING_LEN == 2)
                [ESUCC       ] = TO_STR(ESUCC),
                [EPERM       ] = TO_STR(EPERM),
                [ENOENT      ] = TO_STR(ENOENT),
                [ESRCH       ] = TO_STR(ESRCH),
                [EIO         ] = TO_STR(EIO),
                [ENXIO       ] = TO_STR(ENXIO),
                [E2BIG       ] = TO_STR(E2BIG),
                [ENOEXEC     ] = TO_STR(ENOEXEC),
                [EAGAIN      ] = TO_STR(EAGAIN),
                [ENOMEM      ] = TO_STR(ENOMEM),
                [EACCES      ] = TO_STR(EACCES),
                [EFAULT      ] = TO_STR(EFAULT),
                [EBUSY       ] = TO_STR(EBUSY),
                [EEXIST      ] = TO_STR(EEXIST),
                [ENODEV      ] = TO_STR(ENODEV),
                [ENOTDIR     ] = TO_STR(ENOTDIR),
                [EISDIR      ] = TO_STR(EISDIR),
                [EINVAL      ] = TO_STR(EINVAL),
                [EMFILE      ] = TO_STR(EMFILE),
                [EFBIG       ] = TO_STR(EFBIG),
                [ENOSPC      ] = TO_STR(ENOSPC),
                [ESPIPE      ] = TO_STR(ESPIPE),
                [EROFS       ] = TO_STR(EROFS),
                [EDOM        ] = TO_STR(EDOM),
                [ERANGE      ] = TO_STR(ERANGE),
                [EILSEQ      ] = TO_STR(EILSEQ),
                [ENAMETOOLONG] = TO_STR(ENAMETOOLONG),
                [ENOTEMPTY   ] = TO_STR(ENOTEMPTY),
                [EBADRQC     ] = TO_STR(EBADRQC),
                [ETIME       ] = TO_STR(ETIME),
                [ENONET      ] = TO_STR(ENONET),
                [EUSERS      ] = TO_STR(EUSERS),
                [EADDRINUSE  ] = TO_STR(EADDRINUSE),
                [ENOMEDIUM   ] = TO_STR(ENOMEDIUM),
                [EMEDIUMTYPE ] = TO_STR(EMEDIUMTYPE),
                [ECANCELED   ] = TO_STR(ECANCELED)
#elif (CONFIG_ERRNO_STRING_LEN == 3)
                [ESUCC       ] = "Success",
                [EPERM       ] = "Operation not permitted",
                [ENOENT      ] = "No such file or directory",
                [ESRCH       ] = "No such process",
                [EIO         ] = "I/O error",
                [ENXIO       ] = "No such device or address",
                [E2BIG       ] = "Argument list too long",
                [ENOEXEC     ] = "Exec format error",
                [EAGAIN      ] = "Try again",
                [ENOMEM      ] = "Out of memory",
                [EACCES      ] = "Permission denied",
                [EFAULT      ] = "Bad address",
                [EBUSY       ] = "Device or resource busy",
                [EEXIST      ] = "File exists",
                [ENODEV      ] = "No such device",
                [ENOTDIR     ] = "Not a directory",
                [EISDIR      ] = "Is a directory",
                [EINVAL      ] = "Invalid argument",
                [EMFILE      ] = "Too many open files",
                [EFBIG       ] = "File too large",
                [ENOSPC      ] = "No space left on device",
                [ESPIPE      ] = "Illegal seek",
                [EROFS       ] = "Read-only file system",
                [EDOM        ] = "Math argument out of domain of function",
                [ERANGE      ] = "Math result not representable",
                [EILSEQ      ] = "Illegal byte sequence",
                [ENAMETOOLONG] = "File name too long",
                [ENOTEMPTY   ] = "Directory not empty",
                [EBADRQC     ] = "Invalid request code",
                [ETIME       ] = "Timer expired",
                [ENONET      ] = "Machine is not on the network",
                [EUSERS      ] = "Too many users",
                [EADDRINUSE  ] = "Address already in use",
                [ENOMEDIUM   ] = "No medium found",
                [EMEDIUMTYPE ] = "Wrong medium type",
                [ECANCELED   ] = "Operation Canceled"
#else
#error "CONFIG_ERRNO_STRING_LEN should be in range 0 - 3!"
#endif
        };

        if (CONFIG_ERRNO_STRING_LEN == 0) {
                return "";
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
                sys_fprintf(stderr, "%s: %s\n", str, sys_strerror(errno));
        } else {
                sys_fprintf(stderr, "%s\n", sys_strerror(errno));
        }
#else
        (void) str;
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
        char   chr;
        int    arg_size;
        size_t scan_len     = 1;
        bool   leading_zero = false;
        bool   loop_break   = false;
        bool   long_long    = false;

        /// @brief  Function break loop
        /// @param  None
        /// @return None
        inline void break_loop()
        {
                loop_break = true;
        }

        /// @brief  Put character to the buffer
        /// @param  c    character to put
        /// @return On success true is returned, otherwise false and loop is break
        bool put_char(const char c)
        {
                if (buf) {
                        if (scan_len < size) {
                                *buf++ = c;
                        } else {
                                break_loop();
                                return false;
                        }
                }

                scan_len++;
                return true;
        }

        /// @brief  Get char from format string
        /// @param  None
        /// @return Load next character from format string
        bool get_format_char()
        {
                chr = *format++;

                if (chr == '\0') {
                        break_loop();
                        return false;
                } else {
                        return true;
                }
        }

        /// @brief  Analyze modifiers (%0, %.*, %<num>, %xxlx)
        /// @param  None
        /// @return If modifiers are set or not then true is returned. On error false.
        bool check_modifiers()
        {
                arg_size     = -1;
                leading_zero = false;

                // check leading zero enable
                if (chr == '0') {
                        leading_zero = true;
                        if (!get_format_char()) {
                                return false;
                        }
                }

                // check argument size modifier
                if (chr == '.') {
                        if (!get_format_char()) {
                                return false;
                        }

                        if (chr == '*') {
                                arg_size = va_arg(arg, int);

                                if (!get_format_char()) {
                                        return false;
                                }
                        } else {
                                loop_break = true;
                                return false;
                        }

                // check numeric size modifier
                } else {
                        arg_size = 0;
                        while (chr >= '0' && chr <= '9') {
                                arg_size *= 10;
                                arg_size += chr - '0';

                                if (!get_format_char()) {
                                        return false;
                                }
                        }
                }

                // check long long values
                if (chr == 'l') {
                        long_long = true;

                        if (!get_format_char()) {
                                return false;
                        }
                }

                return true;
        }

        /// @brief  Put percent or character
        /// @param  None
        /// @return If format was found then true is returned, otherwise false.
        bool put_percent_or_char()
        {
                if (chr == '%' || chr == 'c') {
                        if (chr == 'c') {
                                chr = va_arg(arg, int);
                                if (chr == '\0') {
                                        chr = 0xFF;
                                }
                        }

                        put_char(chr);

                        return true;
                }

                return false;
        }

        /// @brief  Put string
        /// @param  None
        /// @return If format was found then true is returned, otherwise false.
        bool put_string()
        {
                if (chr == 's') {
                        char *str = va_arg(arg, char*);
                        if (!str) {
                                str = "";
                        }

                        if (arg_size <= 0) {
                                arg_size = UINT16_MAX;
                        }

                        while ((chr = *str++) && arg_size--) {
                                if (chr == '\0') {
                                        break;
                                }

                                if (!put_char(chr)) {
                                        break;
                                }
                        }

                        return true;
                } else {
                        return false;
                }
        }

        /// @brief  Put integer
        /// @param  None
        /// @return If format was found then true is returned, otherwise false.
        bool put_integer()
        {
                if (chr == 'd' || chr == 'u' || chr == 'i' || chr == 'x' || chr == 'X') {
                        char result[24];
                        memset(result, 0, sizeof(result));

                        bool upper  = chr == 'X';
                        bool spaces = false;
                        bool expand = false;
                        bool unsign = chr == 'u';
                        int  base   = chr == 'x' || chr == 'X' ? 16 : 10;

                        if (arg_size == -1 && leading_zero == false) {
                                expand = false;
                                spaces = false;

                        } else if (arg_size == -1 && leading_zero == true) {
                                expand = false;
                                spaces = false;

                        } else if (arg_size >= 0 && leading_zero == false) {
                                expand = true;
                                spaces = true;

                        } else if (arg_size >= 0 && leading_zero == true) {
                                expand = true;
                                spaces = false;
                        }

                        if (arg_size > static_cast(int, sizeof(result) - 1)) {
                                arg_size = sizeof(result) - 1;

                        }

                        /* NOTE: 64-bit integers are not supported */
                        i32_t val;
                        if (long_long) {
                                val = va_arg(arg, i32_t);
                        } else {
                                val = va_arg(arg, i32_t);
                        }

                        char *result_ptr = itoa(val, result, base, unsign, expand ? arg_size : 0);

                        if (static_cast(int, strlen(result_ptr)) > arg_size) {
                                arg_size = strlen(result_ptr);
                        }

                        while ((chr = *result_ptr++) && arg_size--) {
                                if (spaces && chr == '0') {
                                        chr = ' ';
                                } else {
                                        spaces = false;
                                }

                                if (upper) {
                                        chr = toupper(static_cast(int, chr));
                                } else {
                                        chr = tolower(static_cast(int, chr));
                                }

                                if (!put_char(chr)) {
                                        loop_break = true;
                                        break;
                                }
                        }

                        return true;
                } else {
                        return false;
                }
        }

        /// @brief  Put float value
        /// @param  None
        /// @return If format was found then true is returned, otherwise false.
        bool put_float()
        {
                if (chr == 'f') {
                        char result[12];
                        memset(result, 0, sizeof(result));

                        int len = dtoa(va_arg(arg, double), result, 6, 12);

                        for (int i = 0; i < len; i++) {
                                if (!put_char(result[i])) {
                                        loop_break = true;
                                        break;
                                }
                        }

                        return true;
                } else {
                        return false;
                }
        }

        // read characters from format string
        while (loop_break == false) {

                if (!get_format_char())
                        continue;

                if (chr != '%') {
                        put_char(chr);
                        continue;

                } else {
                        if (!get_format_char())
                                continue;

                        if (!check_modifiers())
                                continue;

                        if (put_percent_or_char())
                                continue;

                        if (put_string())
                                continue;

                        if (put_integer())
                                continue;

                        if (put_float())
                                continue;
                }
        }

        if (buf)
                *buf = 0;

        return (scan_len - 1);
#else
        UNUSED_ARG(buf);
        UNUSED_ARG(size);
        UNUSED_ARG(format);
        UNUSED_ARG(arg);
        return 0;
#endif
}

/*==============================================================================
  End of file
==============================================================================*/
