/*=========================================================================*//**
@file    tty_editln.c

@author  Daniel Zorychta

@brief   TTY edit line (input line).

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
#define EDITLINE_LEN    _TTY_TERMINAL_COLUMNS

/*==============================================================================
  Local object types
==============================================================================*/
struct ttyedit {
        FILE  **out_file;
        void  *self;
        char   buffer[EDITLINE_LEN + 1];
        u16_t  length;
        u16_t  cursor_position;
        bool   echo_enabled;
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
 * @param  this         edit line object
 * @return If valid then true is returned, otherwise false.
 */
//==============================================================================
static inline bool is_valid(ttyedit_t *this)
{
    return this && this->self == this;
}

//==============================================================================
/**
 * @brief Initialize editline
 *
 * @param[in]  out_file      output file object
 * @param[put] edit          pointer to pointer of created object
 *
 * @param On of errno value.
 */
//==============================================================================
int ttyedit_create(FILE **out_file, ttyedit_t **edit)
{
        int err = sys_zalloc(sizeof(ttyedit_t), cast(void**, edit));
        if (err == ESUCC) {
                (*edit)->self         = *edit;
                (*edit)->out_file     = out_file;
                (*edit)->echo_enabled = true;
        }

        return err;
}

//==============================================================================
/**
 * @brief Destroy editline
 *
 * @param this          editline object
 *
 * @return One of errno value.
 */
//==============================================================================
int ttyedit_destroy(ttyedit_t *this)
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
 * @brief Enable editline echo
 *
 * @param this          editline object
 *
 * @return One of errno value.
 */
//==============================================================================
int ttyedit_enable_echo(ttyedit_t *this)
{
        if (is_valid(this)) {
                this->echo_enabled = true;
                return ESUCC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Disable editline echo
 *
 * @param this          editline object
 *
 * @return One of errno value.
 */
//==============================================================================
int ttyedit_disable_echo(ttyedit_t *this)
{
        if (is_valid(this)) {
                this->echo_enabled = false;
                return ESUCC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Return echo status
 *
 * @param this          editline object
 *
 * @return true if echo enabled, false if not
 */
//==============================================================================
bool ttyedit_is_echo_enabled(ttyedit_t *this)
{
        if (is_valid(this)) {
                return this->echo_enabled;
        }

        return false;
}

//==============================================================================
/**
 * @brief Return collected string
 *
 * @param this          editline object
 *
 * @return edit line string, NULL on error
 */
//==============================================================================
char *ttyedit_get_value(ttyedit_t *this)
{
        if (is_valid(this)) {
                return this->buffer;
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Set editline string
 *
 * @param this          editline object
 * @param str           string
 * @param show          true: new buffer is show
 */
//==============================================================================
void ttyedit_set_value(ttyedit_t *this, const char *str, bool show)
{
        if (is_valid(this) && str) {
                if (strlen(str) <= EDITLINE_LEN) {
                        if (show) {
                                ttyedit_move_cursor_home(this);
                        }

                        ttyedit_clear(this);
                        strcpy(this->buffer, str);
                        this->length          = strlen(str);
                        this->cursor_position = this->length;

                        if (show) {
                                size_t wrcnt;
                                sys_fwrite(str, strlen(str), &wrcnt, *this->out_file);

                                static const char *erase_line_end = VT100_ERASE_LINE_END;
                                sys_fwrite(erase_line_end, strlen(erase_line_end), &wrcnt, *this->out_file);
                        }
                }
        }
}

//==============================================================================
/**
 * @brief Clear collected string
 *
 * @param this          editline object
 */
//==============================================================================
void ttyedit_clear(ttyedit_t *this)
{
        if (is_valid(this)) {
                memset(this->buffer, 0, EDITLINE_LEN + 1);
                this->cursor_position = 0;
                this->length          = 0;
        }
}

//==============================================================================
/**
 * @brief Insert character at cursor position
 *
 * @param this          editline object
 * @param c             character to insert
 */
//==============================================================================
void ttyedit_insert_char(ttyedit_t *this, const char c)
{
        if (is_valid(this)) {
                if (this->length >= EDITLINE_LEN - 1) {
                        return;
                }

                if (this->cursor_position < this->length) {
                        for (uint i = this->length; i > this->cursor_position; i--) {
                                this->buffer[i] = this->buffer[i - 1];
                        }

                        this->buffer[this->cursor_position++] = c;
                        this->length++;

                        if (this->echo_enabled) {
                                size_t wrcnt;

                                const char *cmd = VT100_SAVE_CURSOR_POSITION;
                                sys_fwrite(cmd, strlen(cmd), &wrcnt, *this->out_file);

                                cmd = &this->buffer[this->cursor_position - 1];
                                sys_fwrite(cmd, this->length - (this->cursor_position - 1), &wrcnt, *this->out_file);

                                cmd = VT100_RESTORE_CURSOR_POSITION VT100_SHIFT_CURSOR_RIGHT(1);
                                sys_fwrite(cmd, strlen(cmd), &wrcnt, *this->out_file);
                        }
                } else {
                        this->buffer[this->cursor_position++] = c;
                        this->length++;

                        if (this->echo_enabled) {
                                size_t wrcnt;
                                sys_fwrite(&c, 1, &wrcnt, *this->out_file);
                        }
                }
        }
}

//==============================================================================
/**
 * @brief Remove character from edit line in front of cursor
 *
 * @param this          editline object
 */
//==============================================================================
void ttyedit_remove_char(ttyedit_t *this)
{
        if (is_valid(this)) {
                if (this->cursor_position == 0 || this->length == 0) {
                        return;
                }

                this->cursor_position--;

                for (uint i = this->cursor_position; i < this->length; i++) {
                        this->buffer[i] = this->buffer[i + 1];
                }

                this->length--;

                size_t wrcnt;

                const char *cmd = "\b"VT100_ERASE_LINE_FROM_CUR VT100_SAVE_CURSOR_POSITION;
                sys_fwrite(cmd, strlen(cmd), &wrcnt, *this->out_file);

                cmd = &this->buffer[this->cursor_position];
                sys_fwrite(cmd, this->length - this->cursor_position, &wrcnt, *this->out_file);

                cmd = VT100_RESTORE_CURSOR_POSITION;
                sys_fwrite(cmd, strlen(cmd), &wrcnt, *this->out_file);
        }
}

//==============================================================================
/**
 * @brief Delete character from edit line at cursor position
 *
 * @param this          editline object
 */
//==============================================================================
void ttyedit_delete_char(ttyedit_t *this)
{
        if (is_valid(this)) {
                if (this->length == 0 || this->cursor_position == this->length) {
                        return;
                }

                for (uint i = this->cursor_position; i <= this->length; i++) {
                        this->buffer[i] = this->buffer[i + 1];
                }

                this->length--;

                size_t wrcnt;

                const char *cmd = VT100_SAVE_CURSOR_POSITION VT100_ERASE_LINE_FROM_CUR;
                sys_fwrite(cmd, strlen(cmd), &wrcnt, *this->out_file);

                cmd = &this->buffer[this->cursor_position];
                sys_fwrite(cmd, this->length - this->cursor_position, &wrcnt, *this->out_file);

                cmd = VT100_RESTORE_CURSOR_POSITION;
                sys_fwrite(cmd, strlen(cmd), &wrcnt, *this->out_file);
        }
}

//==============================================================================
/**
 * @brief Move cursor left
 *
 * @param this          editline object
 */
//==============================================================================
void ttyedit_move_cursor_left(ttyedit_t *this)
{
        if (is_valid(this)) {
                if (this->cursor_position > 0) {
                        size_t wrcnt;
                        sys_fwrite("\b", 1, &wrcnt, *this->out_file);
                        this->cursor_position--;
                }
        }
}

//==============================================================================
/**
 * @brief Move cursor right
 *
 * @param this          editline object
 */
//==============================================================================
void ttyedit_move_cursor_right(ttyedit_t *this)
{
        if (is_valid(this)) {
                if (this->cursor_position < this->length) {
                        size_t      wrcnt;
                        const char *cmd = VT100_SHIFT_CURSOR_RIGHT(1);
                        sys_fwrite(cmd, strlen(cmd), &wrcnt, *this->out_file);
                        this->cursor_position++;
                }
        }
}

//==============================================================================
/**
 * @brief Move cursor home
 *
 * @param this          editline object
 */
//==============================================================================
void ttyedit_move_cursor_home(ttyedit_t *this)
{
        if (is_valid(this)) {
                size_t      wrcnt;
                const char *cmd = VT100_CURSOR_OFF;
                sys_fwrite(cmd, strlen(cmd), &wrcnt, *this->out_file);

                while (this->cursor_position > 0) {
                        sys_fwrite("\b", 1, &wrcnt, *this->out_file);
                        this->cursor_position--;
                }

                cmd = VT100_CURSOR_ON;
                sys_fwrite(cmd, strlen(cmd), &wrcnt, *this->out_file);
        }
}

//==============================================================================
/**
 * @brief Move cursor end
 *
 * @param this          editline object
 */
//==============================================================================
void ttyedit_move_cursor_end(ttyedit_t *this)
{
        if (is_valid(this)) {
                size_t      wrcnt;
                const char *cmd = VT100_CURSOR_OFF;
                sys_fwrite(cmd, strlen(cmd), &wrcnt, *this->out_file);

                while (this->cursor_position < this->length) {
                        char *cmd = VT100_SHIFT_CURSOR_RIGHT(1);
                        sys_fwrite(cmd, strlen(cmd), &wrcnt, *this->out_file);
                        this->cursor_position++;
                }

                cmd = VT100_CURSOR_ON;
                sys_fwrite(cmd, strlen(cmd), &wrcnt, *this->out_file);
        }
}

/*==============================================================================
  End of file
==============================================================================*/
