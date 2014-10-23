/*=========================================================================*//**
@file    tty_editln.c

@author  Daniel Zorychta

@brief

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
#include "core/module.h"
#include <string.h>
#include "tty.h"
#include "tty_cfg.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define EDITLINE_LEN _TTY_DEFAULT_TERMINAL_COLUMNS

/*==============================================================================
  Local object types
==============================================================================*/
struct ttyedit {
        FILE           *out_file;
        void           *self;
        char            buffer[EDITLINE_LEN + 1];
        u16_t           length;
        u16_t           cursor_position;
        bool            echo_enabled;
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
 * @param out_file      output file object
 *
 * @param edit line object address if success, NULL on error
 */
//==============================================================================
ttyedit_t *ttyedit_new(FILE *out_file)
{
        ttyedit_t *edit = calloc(1, sizeof(ttyedit_t));
        if (edit) {
                edit->self         = edit;
                edit->out_file     = out_file;
                edit->echo_enabled = true;
        }

        return edit;
}

//==============================================================================
/**
 * @brief Destroy editline
 *
 * @param this          editline object
 */
//==============================================================================
void ttyedit_delete(ttyedit_t *this)
{
        if (is_valid(this)) {
                this->self = NULL;
                free(this);
        }
}

//==============================================================================
/**
 * @brief Enable editline echo
 *
 * @param this          editline object
 */
//==============================================================================
void ttyedit_echo_enable(ttyedit_t *this)
{
        if (is_valid(this)) {
                this->echo_enabled = true;
        }
}

//==============================================================================
/**
 * @brief Disable editline echo
 *
 * @param this          editline object
 */
//==============================================================================
void ttyedit_echo_disable(ttyedit_t *this)
{
        if (is_valid(this)) {
                this->echo_enabled = false;
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
char *ttyedit_get(ttyedit_t *this)
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
void ttyedit_set(ttyedit_t *this, const char *str, bool show)
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
                                vfs_fwrite(str, sizeof(char), strlen(str), this->out_file);

                                static const char *erase_line_end = ERASE_LINE_END;
                                vfs_fwrite(erase_line_end, sizeof(char), strlen(erase_line_end), this->out_file);
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
                                const char *cmd = VT100_SAVE_CURSOR_POSITION;
                                vfs_fwrite(cmd, sizeof(char), strlen(cmd), this->out_file);

                                cmd = &this->buffer[this->cursor_position - 1];
                                vfs_fwrite(cmd, sizeof(char), this->length - (this->cursor_position - 1), this->out_file);

                                cmd = VT100_RESTORE_CURSOR_POSITION VT100_SHIFT_CURSOR_RIGHT(1);
                                vfs_fwrite(cmd, sizeof(char), strlen(cmd), this->out_file);
                        }
                } else {
                        this->buffer[this->cursor_position++] = c;
                        this->length++;

                        if (this->echo_enabled) {
                                vfs_fwrite(&c, sizeof(char), 1, this->out_file);
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

                const char *cmd = "\b"VT100_ERASE_LINE_FROM_CUR VT100_SAVE_CURSOR_POSITION;
                vfs_fwrite(cmd, sizeof(char), strlen(cmd), this->out_file);

                cmd = &this->buffer[this->cursor_position];
                vfs_fwrite(cmd, sizeof(char), this->length - this->cursor_position, this->out_file);

                cmd = VT100_RESTORE_CURSOR_POSITION;
                vfs_fwrite(cmd, sizeof(char), strlen(cmd), this->out_file);
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

                const char *cmd = VT100_SAVE_CURSOR_POSITION VT100_ERASE_LINE_FROM_CUR;
                vfs_fwrite(cmd, sizeof(char), strlen(cmd), this->out_file);

                cmd = &this->buffer[this->cursor_position];
                vfs_fwrite(cmd, sizeof(char), this->length - this->cursor_position, this->out_file);

                cmd = VT100_RESTORE_CURSOR_POSITION;
                vfs_fwrite(cmd, sizeof(char), strlen(cmd), this->out_file);
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
                        vfs_fwrite("\b", sizeof(char), 1, this->out_file);
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
                        const char *cmd = VT100_SHIFT_CURSOR_RIGHT(1);
                        vfs_fwrite(cmd, sizeof(char), strlen(cmd), this->out_file);
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
                const char *cmd = VT100_CURSOR_OFF;
                vfs_fwrite(cmd, sizeof(char), strlen(cmd), this->out_file);

                while (this->cursor_position > 0) {
                        vfs_fwrite("\b", sizeof(char), 1, this->out_file);
                        this->cursor_position--;
                }

                cmd = VT100_CURSOR_ON;
                vfs_fwrite(cmd, sizeof(char), strlen(cmd), this->out_file);
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
                const char *cmd = VT100_CURSOR_OFF;
                vfs_fwrite(cmd, sizeof(char), strlen(cmd), this->out_file);

                while (this->cursor_position < this->length) {
                        char *cmd = VT100_SHIFT_CURSOR_RIGHT(1);
                        vfs_fwrite(cmd, sizeof(char), strlen(cmd), this->out_file);
                        this->cursor_position++;
                }

                cmd = VT100_CURSOR_ON;
                vfs_fwrite(cmd, sizeof(char), strlen(cmd), this->out_file);
        }
}

/*==============================================================================
  End of file
==============================================================================*/
