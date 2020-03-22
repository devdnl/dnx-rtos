/*=========================================================================*//**
@file    tty_cmd.c

@author  Daniel Zorychta

@brief   Code in this file is responsible for decode VT100 commands.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "drivers/driver.h"
#include "tty.h"
#include "tty_cfg.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define VT100_TOKEN_LEN                         15
#define VT100_TOKEN_READ_TIMEOUT                250

/*==============================================================================
  Local object types
==============================================================================*/
struct ttycmd {
        void           *self;
        char            token[VT100_TOKEN_LEN + 1];
        u8_t            token_cnt;
        u32_t           timer;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(TTY);

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
 * @brief  Check if object is valid
 * @param  this         command object
 * @return If valid then true is returned, otherwise false.
 */
//==============================================================================
static inline bool is_valid(ttycmd_t *this)
{
    return this && this->self == this;
}

//==============================================================================
/**
 * @brief  Initialize command object
 *
 * @param  ttycmd        pointer to target pointer of created object
 *
 * @return One of errno value.
 */
//==============================================================================
int ttycmd_create(ttycmd_t **ttycmd)
{
        int err = sys_zalloc(sizeof(ttycmd_t), cast(void**, ttycmd));
        if (err == ESUCC) {
                (*ttycmd)->self = *ttycmd;
        }

        return err;
}

//==============================================================================
/**
 * @brief Delete command object
 *
 * @param this          command analyze object
 */
//==============================================================================
int ttycmd_destroy(ttycmd_t *this)
{
        if (is_valid(this)) {
                this->self = NULL;
                sys_free(cast(void**, &this));
                return ESUCC;
        } else {
                return EINVAL;
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
        if (is_valid(this)) {
                if (strchr("\r\n", c)) {
                        return TTYCMD_KEY_ENTER;
                }

                if (c == '\b') {
                        return TTYCMD_KEY_BACKSPACE;
                }

                if (c == '\t') {
                        return TTYCMD_KEY_TAB;
                }

                if (c == ETX) {
                        return TTYCMD_KEY_ENDTEXT;
                }

                if (c == '\033') {
                        memset(this->token, 0, VT100_TOKEN_LEN);
                        this->token[0]  = c;
                        this->token_cnt = 1;
                        this->timer     = sys_time_get_reference();
                        return TTYCMD_BUSY;

                } else if (this->token_cnt) {
                        if (strchr("~ABCDFPQRS", c)) {
                                if (this->token_cnt < VT100_TOKEN_LEN) {
                                        this->token[this->token_cnt++] = c;
                                }

                                this->token_cnt = 0;

                                if      (strcmp(VT100_DEL        , this->token) == 0) return TTYCMD_KEY_DELETE;
                                else if (strcmp(VT100_ARROW_LEFT , this->token) == 0) return TTYCMD_KEY_ARROW_LEFT;
                                else if (strcmp(VT100_ARROW_RIGHT, this->token) == 0) return TTYCMD_KEY_ARROW_RIGHT;
                                else if (strcmp(VT100_ARROW_UP   , this->token) == 0) return TTYCMD_KEY_ARROW_UP;
                                else if (strcmp(VT100_ARROW_DOWN , this->token) == 0) return TTYCMD_KEY_ARROW_DOWN;
                                else if (strcmp(VT100_HOME       , this->token) == 0) return TTYCMD_KEY_HOME;
                                else if (strcmp(VT100_END_1      , this->token) == 0) return TTYCMD_KEY_END;
                                else if (strcmp(VT100_END_2      , this->token) == 0) return TTYCMD_KEY_END;
                                else if (strcmp(VT100_F1         , this->token) == 0) return TTYCMD_KEY_F1;
                                else if (strcmp(VT100_F2         , this->token) == 0) return TTYCMD_KEY_F2;
                                else if (strcmp(VT100_F3         , this->token) == 0) return TTYCMD_KEY_F3;
                                else if (strcmp(VT100_F4         , this->token) == 0) return TTYCMD_KEY_F4;
                                else if (strcmp(VT100_F5         , this->token) == 0) return TTYCMD_KEY_F5;
                                else if (strcmp(VT100_F6         , this->token) == 0) return TTYCMD_KEY_F6;
                                else if (strcmp(VT100_F7         , this->token) == 0) return TTYCMD_KEY_F7;
                                else if (strcmp(VT100_F8         , this->token) == 0) return TTYCMD_KEY_F8;
                                else if (strcmp(VT100_F9         , this->token) == 0) return TTYCMD_KEY_F9;
                                else if (strcmp(VT100_F10        , this->token) == 0) return TTYCMD_KEY_F10;
                                else if (strcmp(VT100_F11        , this->token) == 0) return TTYCMD_KEY_F11;
                                else if (strcmp(VT100_F12        , this->token) == 0) return TTYCMD_KEY_F12;
                                else return TTYCMD_BUSY;
                        } else {
                                if (  sys_time_is_expired(this->timer, VT100_TOKEN_READ_TIMEOUT)
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
        if (is_valid(this)) {
                return this->token_cnt == 0;
        }

        return false;
}

/*==============================================================================
  End of file
==============================================================================*/
