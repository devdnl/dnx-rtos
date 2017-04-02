/*=========================================================================*//**
File     base64.h

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

#ifndef _BASE64_H_
#define _BASE64_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported functions
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
extern char *base64_encode(const uint8_t *src, size_t len, size_t *out_len);

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
extern uint8_t *base64_decode(const char *src, size_t len, size_t *out_len);

#ifdef __cplusplus
}
#endif

#endif /* _BASE64_H_ */
/*==============================================================================
  End of file
==============================================================================*/
