/*=========================================================================*//**
@file    vsnprintf.c

@author  Daniel Zorychta

@brief   Print functions.

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <dnx/misc.h>
#include <ctype.h>

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/
extern char *_libc_itoa(i64_t val, char *buf, u8_t base, bool usign_val, u8_t zeros_req);
extern int _libc_dtoa(double value, char *str, int prec, int n);

/*==============================================================================
  Function definitions
==============================================================================*/

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
 *
 * Supported flags:
 *   %%         - print % character
 *                printf("%%"); => %
 *
 *   %c         - print selected character (the \0 character is skipped)
 *                printf("_%c_", 'x');  => _x_
 *                printf("_%c_", '\0'); => __
 *
 *   %s         - print selected string
 *                printf("%s", "Foobar"); => Foobar
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                printf("%.*s\n", 3, "Foobar"); => Foo
 *
 *   %.ns       - print selected string but only the n length
 *                printf("%.3s\n", "Foobar"); => Foo
 *
 *   %d, %i     - print decimal integer values
 *                printf("%d, %i", -5, 10); => -5, 10
 *
 *   %u         - print unsigned decimal integer values
 *                printf("%u, %u", -1, 10); => 4294967295, 10
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                printf("0x%x, 0x%X", 0x5A, 0xfa); => 0x5a, 0xFA
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                printf("0x02X, 0x03X", 0x5, 0x1F43); => 0x05, 0x1F43
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                printf("Foobar: %f", 1.0); => Foobar: 1.000000
 *
 *   %l?        - print long values, where ? means d, i, u, x, or X.
 *                printf("32-bit value: %lu\n", 4561); => 32-bit value: 4561
 *
 *   %ll?       - print long long values, where ? means d, i, u, x, or X.
 *                printf("64-bit value: %llu\n", 4561); => 64-bit value: 4561
 *
 *   %p         - print pointer
 *                printf("Pointer: %p", main); => Pointer: 0x4028B4
 */
//==============================================================================
int vsnprintf(char *buf, size_t size, const char *format, va_list arg)
{
         char   chr;
         int    arg_size;
         size_t scan_len     = 1;
         int    long_ctr     = 0;
         bool   leading_zero = false;
         bool   loop_break   = false;
         bool   arg_size_str = false;
         bool   point        = false;

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
                 arg_size_str = false;

                 // check leading zero enable
                 if (chr == '0') {
                         leading_zero = true;
                         if (!get_format_char()) {
                                 return false;
                         }
                 }

                 // check argument size modifier
                 if (chr == '.') {
                         point = true;

                         if (!get_format_char()) {
                                 return false;
                         }

                         if (chr == '*') {
                                 arg_size     = va_arg(arg, int);
                                 arg_size_str = true;

                                 if (!get_format_char()) {
                                         return false;
                                 }
                         } else if (chr >= '0' && chr <= '9') {
                                 arg_size     = 0;
                                 arg_size_str = true;
                                 while (chr >= '0' && chr <= '9') {
                                         arg_size *= 10;
                                         arg_size += chr - '0';

                                         if (!get_format_char()) {
                                                 return false;
                                         }
                                 }
                         } else {
                                 break_loop();
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
                         long_ctr++;

                         if (!get_format_char()) {
                                 return false;
                         }

                         if (chr == 'l') {
                                 long_ctr++;

                                 if (!get_format_char()) {
                                         return false;
                                 }
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
                                 if (chr != '\0') {
                                         put_char(chr);
                                 }
                         } else {
                                 put_char(chr);
                         }

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

                         if (arg_size == 0 && arg_size_str == true) {
                                 return true;
                         }

                         if (arg_size <= 0 || arg_size_str == false) {
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
                         char result[65];
                         bool upper  = chr == 'X';
                         bool spaces = false;
                         bool expand = false;
                         bool unsign = chr == 'u' || chr == 'x' || chr =='X';
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

                         if (arg_size > cast(int, sizeof(result) - 1)) {
                                 arg_size = sizeof(result) - 1;

                         }

                         i64_t val;
                         if (long_ctr == 2) {
                                 val = va_arg(arg, i64_t);

                         } else {
                                 val = va_arg(arg, i32_t);

                                 if (unsign) {
                                         val &= 0xFFFFFFFFUL;
                                 }
                         }

                         char *result_ptr = _libc_itoa(val, result, base, unsign, expand ? arg_size : 0);

                         if (cast(int, strlen(result_ptr)) > arg_size) {
                                 arg_size = strlen(result_ptr);
                         }

                         while ((chr = *result_ptr++) && arg_size--) {
                                 if (spaces && chr == '0' && arg_size > 0) {
                                         chr = ' ';
                                 } else {
                                         spaces = false;
                                 }

                                 if (upper) {
                                         chr = toupper(cast(int, chr));
                                 } else {
                                         chr = tolower(cast(int, chr));
                                 }

                                 if (!put_char(chr)) {
                                         break;
                                 }
                         }

                         long_ctr = 0;

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
                 if (chr == 'f' || chr == 'F' || chr == 'g' || chr == 'G') {
                         char result[65];
                         arg_size = !point ? 10 : arg_size;
                         if (arg_size > 0) {
                                 int prec = (arg_size <= 0) ? 10 : arg_size;
                                 int len  = _libc_dtoa(va_arg(arg, double), result, prec, sizeof(result));
                                 for (int i = 0; i < len; i++) {
                                         if (!put_char(result[i])) {
                                                 break;
                                         }
                                 }
                         } else {
                                 double val = va_arg(arg, double);
                                 char *result_ptr = _libc_itoa(val, result, 10, false, 0);
                                 while ((chr = *result_ptr++)) {
                                         if (!put_char(chr)) {
                                                 break;
                                         }
                                 }
                         }
                         return true;
                 } else {
                         return false;
                 }
         }

         /// @brief  Put pointer value
         /// @param  None
         /// @return If format was found then true is returned, otherwise false.
         bool put_pointer()
         {
                 if (chr == 'p') {
                         i64_t val = va_arg(arg, uintptr_t);
                         val &= UINTPTR_MAX;
                         char  result[16];
                         char *result_ptr = _libc_itoa(val, result, 16, true, 0);

                         if (!put_char('0'))
                                 return true;

                         if (!put_char('x'))
                                 return true;

                         while ((chr = *result_ptr++)) {
                                 if (!put_char(chr)) {
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

                         if (put_pointer())
                                 continue;
                 }
         }

         if (buf)
                 *buf = 0;

         return (scan_len - 1);
}

//==============================================================================
/**
 * @brief Function convert arguments to stream
 *
 * @param[in] *buf           buffer for stream
 * @param[in]  size          buffer size
 * @param[in] *format        message format
 * @param[in]  ...           argument list
 *
 * @return number of printed characters
 */
//==============================================================================
int _libc_snprintf(char *buf, size_t size, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = vsnprintf(buf, size, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief Function convert arguments to stream
 *
 * @param[in] *buf           buffer for stream
 * @param[in] *format        message format
 * @param[in]  ...           argument list
 *
 * @return number of printed characters
 */
//==============================================================================
int _libc_sprintf(char *s, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = vsnprintf(s, UINT16_MAX, format, arg);
        va_end(arg);
        return status;
}

/*==============================================================================
  End of file
==============================================================================*/