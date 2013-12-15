/*=========================================================================*//**
@file    tty_cmd.c

@author  Daniel Zorychta

@brief   Code in this file is responsible for decode VT100 commands.

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
#include "system/dnxmodule.h"
#include "tty.h"
#include "tty_cfg.h"
#include "system/timer.h"
#include "core/scanx.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define VALIDATION_TOKEN                        (u32_t)0x7D8498F1
#define SET_VALIDATION(_bfr, _val)              *(u32_t *)&_bfr->valid = _val;
#define VT100_TOKEN_LEN                         15
#define VT100_DEL                               "\e[3~"
#define VT100_ARROW_LEFT                        "\e[D"
#define VT100_ARROW_RIGHT                       "\e[C"
#define VT100_ARROW_UP                          "\e[A"
#define VT100_ARROW_DOWN                        "\e[B"
#define VT100_HOME                              "\e[1~"
#define VT100_END                               "\eOF"
#define VT100_F1                                "\eOP"
#define VT100_F2                                "\eOQ"
#define VT100_F3                                "\eOR"
#define VT100_F4                                "\eOS"
#define VT100_F5                                "\e[16~"
#define VT100_F6                                "\e[17~"
#define VT100_F7                                "\e[18~"
#define VT100_F8                                "\e[19~"
#define VT100_F9                                "\e[20~"
#define VT100_F10                               "\e[21~"
#define VT100_F11                               "\e[23~"
#define VT100_F12                               "\e[24~"

/*==============================================================================
  Local object types
==============================================================================*/
struct ttycmd {
        const u32_t     valid;
        char            token[VT100_TOKEN_LEN + 1];
        u8_t            token_cnt;
        u16_t           colums;
        u16_t           rows;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME("TTY");

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
 * @brief Initialize command object
 *
 * @return pointer to object if success, NULL on error
 */
//==============================================================================
ttycmd_t *ttycmd_new()
{
        ttycmd_t *ttycmd = calloc(1, sizeof(ttycmd_t));
        if (ttycmd) {
                SET_VALIDATION(ttycmd, VALIDATION_TOKEN);
        }

        return ttycmd;
}

//==============================================================================
/**
 * @brief Delete command object
 *
 * @param this          command analyze object
 */
//==============================================================================
void ttycmd_delete(ttycmd_t *this)
{
        if (this) {
                if (this->valid == VALIDATION_TOKEN) {
                        SET_VALIDATION(this, 0);
                        free(this);
                }
        }
}

//==============================================================================
/**
 * @brief Function analyze input stream
 *
 * @param this          command analyze object
 * @param c             input character
 *
 * @return analyzed command
 */
//==============================================================================
ttycmd_resp_t ttycmd_analyze(ttycmd_t *this, const char c)
{
        if (this) {
                if (this->valid == VALIDATION_TOKEN) {
                        if (strchr("\r\n", c)) {
                                return TTYCMD_KEY_ENTER;
                        }

                        if (c == '\b') {
                                return TTYCMD_KEY_BACKSPACE;
                        }

                        if (c == '\e') {
                                memset(this->token, 0, VT100_TOKEN_LEN);
                                this->token[0]  = c;
                                this->token_cnt = 1;
                                return TTYCMD_BUSY;

                        } else if (this->token_cnt) {
                                if (strchr("~ABCDFPQRS", c)) {
                                        if (this->token_cnt < VT100_TOKEN_LEN) {
                                                this->token[this->token_cnt++] = c;
                                        }

                                        ttycmd_resp_t resp;
                                        if      (strcmp(VT100_DEL        , this->token) == 0) resp = TTYCMD_KEY_DELETE;
                                        else if (strcmp(VT100_ARROW_LEFT , this->token) == 0) resp = TTYCMD_KEY_ARROW_LEFT;
                                        else if (strcmp(VT100_ARROW_RIGHT, this->token) == 0) resp = TTYCMD_KEY_ARROW_RIGHT;
                                        else if (strcmp(VT100_ARROW_UP   , this->token) == 0) resp = TTYCMD_KEY_ARROW_UP;
                                        else if (strcmp(VT100_ARROW_DOWN , this->token) == 0) resp = TTYCMD_KEY_ARROW_DOWN;
                                        else if (strcmp(VT100_HOME       , this->token) == 0) resp = TTYCMD_KEY_HOME;
                                        else if (strcmp(VT100_END        , this->token) == 0) resp = TTYCMD_KEY_END;
                                        else if (strcmp(VT100_F1         , this->token) == 0) resp = TTYCMD_KEY_F1;
                                        else if (strcmp(VT100_F2         , this->token) == 0) resp = TTYCMD_KEY_F2;
                                        else if (strcmp(VT100_F3         , this->token) == 0) resp = TTYCMD_KEY_F3;
                                        else if (strcmp(VT100_F4         , this->token) == 0) resp = TTYCMD_KEY_F4;
                                        else if (strcmp(VT100_F5         , this->token) == 0) resp = TTYCMD_KEY_F5;
                                        else if (strcmp(VT100_F6         , this->token) == 0) resp = TTYCMD_KEY_F6;
                                        else if (strcmp(VT100_F7         , this->token) == 0) resp = TTYCMD_KEY_F7;
                                        else if (strcmp(VT100_F8         , this->token) == 0) resp = TTYCMD_KEY_F8;
                                        else if (strcmp(VT100_F9         , this->token) == 0) resp = TTYCMD_KEY_F9;
                                        else if (strcmp(VT100_F10        , this->token) == 0) resp = TTYCMD_KEY_F10;
                                        else if (strcmp(VT100_F11        , this->token) == 0) resp = TTYCMD_KEY_F11;
                                        else if (strcmp(VT100_F12        , this->token) == 0) resp = TTYCMD_KEY_F12;
                                        else {
                                                int col = 0;
                                                int row = 0;
                                                if (sys_sscanf(this->token, "\e[%d;%dR", &row, &col) == 2) {
                                                        if (col >= 16 && row >= 2) {
                                                                this->colums = col;
                                                                this->rows   = row;
                                                                resp = TTYCMD_SIZE_CAPTURED;
                                                        } else {
                                                                resp = TTYCMD_BUSY;
                                                        }
                                                } else {
                                                        resp = TTYCMD_BUSY;
                                                }
                                        }

                                        this->token_cnt = 0;

                                        return resp;
                                } else {
                                        if (this->token_cnt < VT100_TOKEN_LEN) {
                                                this->token[this->token_cnt++] = c;
                                        } else {
                                                this->token_cnt = 0;
                                        }

                                        return TTYCMD_BUSY;
                                }
                        } else {
                                return TTYCMD_KEY_CHAR;
                        }
                }
        }

        return TTYCMD_BUSY;
}

//==============================================================================
/**
 * @brief Check if command decoder is in idle state
 *
 * @param this          command analyze object
 *
 * @return true if idle, false if operation in progress
 */
//==============================================================================
bool ttycmd_is_idle(ttycmd_t *this)
{
        if (this) {
                if (this->valid == VALIDATION_TOKEN) {
                        return this->token_cnt == 0 ? true : false;
                }
        }

        return false;
}

//==============================================================================
/**
 * @brief Return terminal size
 *
 * @param this          command analyze object
 * @param col           columns number (can be NULL)
 * @param row           rows number (can be NULL)
 */
//==============================================================================
void ttycmd_get_size(ttycmd_t *this, u16_t *col, u16_t *row)
{
        if (this) {
                if (this->valid == VALIDATION_TOKEN) {
                        if (col)
                                *col = this->colums;

                        if (row)
                                *row = this->rows;
                }
        }
}
#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
