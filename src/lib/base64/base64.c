/*=========================================================================*//**
File     base64.c

Author   Daniel Zorychta

Brief    Base64 coding library.

         Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

         Base64 encoding/decoding (RFC1341)
         Copyright (c) 2005, Jouni Malinen <jkmaline@cc.hut.fi>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License version 2 as
         published by the Free Software Foundation.

         Alternatively, this software may be distributed under the terms of BSD
         license.

         See README and COPYING for more details.

*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdlib.h>
#include <string.h>
#include "base64.h"

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
static const unsigned char base64_table[64] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief  Function encode selected binary buffer to Base64 string.
 *
 * Caller is responsible for freeing the returned buffer. Returned buffer is
 * nul terminated to make it easier to use as a C string. The nul terminator is
 * not included in out_len.
 *
 * @param  src           source buffer
 * @param  len           source buffer length
 * @param  out_len       length of encoded buffer
 * @return On success return allocated block encoded in Base64, otherwise NULL.
 */
//==============================================================================
char *base64_encode(const uint8_t *src, size_t len, size_t *out_len)
{
        char *out, *pos;
        const uint8_t *end, *in;
        size_t olen;

        olen = len * 4 / 3 + 4; /* 3-byte blocks to 4-byte */
        olen += olen / 72;      /* line feeds */
        olen++;                 /* nul termination */
        out = malloc(olen);
        if (out == NULL)
                return NULL;

        end = src + len;
        in  = src;
        pos = out;

        while (end - in >= 3) {
                *pos++ = base64_table[in[0] >> 2];
                *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
                *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
                *pos++ = base64_table[in[2] & 0x3f];
                in += 3;
        }

        if (end - in) {
                *pos++ = base64_table[in[0] >> 2];
                if (end - in == 1) {
                        *pos++ = base64_table[(in[0] & 0x03) << 4];
                        *pos++ = '=';
                } else {
                        *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
                        *pos++ = base64_table[(in[1] & 0x0f) << 2];
                }
                *pos++ = '=';
        }

        *pos = '\0';
        if (out_len)
                *out_len = pos - out;

        return out;
}

//==============================================================================
/**
 * @brief  Function decode selected string buffer coded in Base64 to binary buffer.
 *
 * Caller is responsible for freeing the returned buffer.
 *
 * @param  buf           source buffer
 * @param  len           source buffer length
 * @param  out_len       length of decoded buffer
 * @return On success return allocated decoded block, otherwise NULL.
 */
//==============================================================================
uint8_t *base64_decode(const char *buf, size_t len, size_t *out_len)
{
        const unsigned char *src = (void*)buf;
        uint8_t *out, *pos;
        char dtable[256], in[4], block[4];
        size_t i, count, olen;

        memset(dtable, 0x80, 256);
        for (i = 0; i < sizeof(base64_table); i++) {
                dtable[base64_table[i]] = i;
        }

        dtable['='] = 0;

        count = 0;
        for (i = 0; i < len; i++) {
                if (dtable[src[i]] != 0x80)
                        count++;
        }

        if (count % 4)
                return NULL;

        olen = count / 4 * 3;
        pos = out = malloc(olen);
        if (out == NULL)
                return NULL;

        count = 0;
        for (i = 0; i < len; i++) {
                char tmp = dtable[src[i]];
                if (tmp == 0x80)
                        continue;

                in[count] = src[i];
                block[count] = tmp;
                count++;
                if (count == 4) {
                        *pos++ = (block[0] << 2) | (block[1] >> 4);
                        *pos++ = (block[1] << 4) | (block[2] >> 2);
                        *pos++ = (block[2] << 6) | block[3];
                        count  = 0;
                }
        }

        if (pos > out) {
                if (in[2] == '=')
                        pos -= 2;
                else if (in[3] == '=')
                        pos--;
        }

        *out_len = pos - out;

        return out;
}

/*==============================================================================
  End of file
==============================================================================*/
