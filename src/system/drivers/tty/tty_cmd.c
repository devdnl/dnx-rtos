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

/*==============================================================================
  Include files
==============================================================================*/
#include <dnx/module.h>
#include <dnx/timer.h>
#include <string.h>
#include "tty.h"
#include "tty_cfg.h"
#include "core/scanx.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define VALIDATION_TOKEN                        (u32_t)0x7D8498F1
#define VT100_TOKEN_LEN                         15
#define VT100_TOKEN_READ_TIMEOUT                250

#define VT100_ARROW_LEFT                        "\033[D"
#define VT100_ARROW_RIGHT                       "\033[C"
#define VT100_ARROW_UP                          "\033[A"
#define VT100_ARROW_DOWN                        "\033[B"
#define VT100_HOME                              "\033[1~"
#define VT100_INS                               "\033[2~"
#define VT100_DEL                               "\033[3~"
#define VT100_END_1                             "\033[4~"
#define VT100_END_2                             "\033OF"
#define VT100_PGUP                              "\033[5~"
#define VT100_PGDN                              "\033[6~"
#define VT100_F1                                "\033OP"
#define VT100_F2                                "\033OQ"
#define VT100_F3                                "\033OR"
#define VT100_F4                                "\033OS"
#define VT100_F5                                "\033[16~"
#define VT100_F6                                "\033[17~"
#define VT100_F7                                "\033[18~"
#define VT100_F8                                "\033[19~"
#define VT100_F9                                "\033[20~"
#define VT100_F10                               "\033[21~"
#define VT100_F11                               "\033[23~"
#define VT100_F12                               "\033[24~"

/*==============================================================================
  Local object types
==============================================================================*/
struct ttycmd {
        u32_t           valid;
        char            token[VT100_TOKEN_LEN + 1];
        u8_t            token_cnt;
        u16_t           colums;
        u16_t           rows;
        timer_t         timer;
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
                ttycmd->valid = VALIDATION_TOKEN;
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
                        this->valid = 0;
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

                        if (c == '\033') {
                                memset(this->token, 0, VT100_TOKEN_LEN);
                                this->token[0]  = c;
                                this->token_cnt = 1;
                                this->timer     = timer_reset();
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
                                        else if (strcmp(VT100_END_1      , this->token) == 0) resp = TTYCMD_KEY_END;
                                        else if (strcmp(VT100_END_2      , this->token) == 0) resp = TTYCMD_KEY_END;
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
                                                if (sys_sscanf(this->token, "\033[%d;%dR", &row, &col) == 2) {
                                                        if (col >= 32 && row >= 10) {
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
                                        if (  timer_is_expired(this->timer, VT100_TOKEN_READ_TIMEOUT)
                                           || this->token_cnt >= VT100_TOKEN_LEN ) {

                                                this->token_cnt = 0;
                                        } else {
                                                this->token[this->token_cnt++] = c;
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

/*==============================================================================
  End of file
==============================================================================*/
