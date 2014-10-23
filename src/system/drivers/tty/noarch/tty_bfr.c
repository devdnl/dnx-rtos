/*=========================================================================*//**
@file    tty_bfr.c

@author  Daniel Zorychta

@brief   Code in this file is responsible for buffer support.

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
#include "core/module.h"
#include "dnx/misc.h"
#include <string.h>
#include "tty.h"
#include "tty_cfg.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define CR_LF_LEN       2
#define CR_LF_NUL_LEN   3

/*==============================================================================
  Local object types
==============================================================================*/
struct ttybfr {
        void  *self;
        char  *line[_TTY_TERMINAL_ROWS];
        char   new_line_bfr[_TTY_TERMINAL_COLUMNS + CR_LF_NUL_LEN];
        size_t new_line_bfr_idx;
        int    carriage;
        u16_t  write_index;
        bool   fresh_line[_TTY_TERMINAL_ROWS];
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(TTY);

static const char *CR_LF = "\r\n";

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
 * @brief  Check if given object is valid
 * @param  this         object to examine
 * @return On success true is returned, otherwise false.
 */
//==============================================================================
static bool is_valid(ttybfr_t *this)
{
        return this && this->self == this;
}

//==============================================================================
/**
 * @brief  Get last or selected line
 * @param  this          buffer object
 * @param  go_back       number of lines from current index
 * @return line's index
 */
//==============================================================================
static uint get_line_index(ttybfr_t *this, uint go_back)
{
        if (this->write_index < go_back) {
                return _TTY_TERMINAL_ROWS - (go_back - this->write_index);
        } else {
                return this->write_index - go_back;
        }
}

//==============================================================================
/**
 * @brief  Function link prepared line to buffer
 * @param  this          buffer object
 * @param  line          line
 * @return None
 */
//==============================================================================
static void link_line(ttybfr_t *this, char *line)
{
        if (this->line[this->write_index]) {
                free(this->line[this->write_index]);
        }

        this->line[this->write_index]       = line;
        this->fresh_line[this->write_index] = true;

        if (LAST_CHARACTER(line) == '\n') {
                this->write_index = (this->write_index + 1) % _TTY_TERMINAL_ROWS;
        }
}

//==============================================================================
/**
 * @brief  Clear the new line buffer
 * @param  this         buffer object
 * @return None
 */
//==============================================================================
static void clear_new_line_buffer(ttybfr_t *this)
{
        memset(this->new_line_bfr, '\0', sizeof(this->new_line_bfr));
        this->new_line_bfr_idx = 0;
}

//==============================================================================
/**
 * @brief  Clear last line
 * @param  this          buffer object
 * @return None
 */
//==============================================================================
static void clear_last_line(ttybfr_t *this)
{
        if (this->line[this->write_index]) {
                free(this->line[this->write_index]);
                this->line[this->write_index] = NULL;
        }
}

//==============================================================================
/**
 * @brief  Put the new line buffer to the main buffer
 * @param  this         buffer object
 * @return None
 */
//==============================================================================
static void put_new_line_buffer(ttybfr_t *this)
{
        /* function link a new line to the buffer */
        void link_new_line(const char *str, size_t len)
        {
                char *line = malloc(len + 1);
                if (line) {
                        strcpy(line, str);
                        link_line(this, line);
                }
        }

        /* check if in buffer is VT100 clear command */
        if (strncmp(VT100_CLEAR_SCREEN, this->new_line_bfr, 4) == 0) {
                ttybfr_clear(this);
        }

        /* add line to the buffer */
        size_t new_line_len = strlen(this->new_line_bfr);
        char *last_line     = this->line[get_line_index(this, 0)];
        if (last_line) {
                if (LAST_CHARACTER(last_line) == '\n') {
                        link_new_line(this->new_line_bfr, new_line_len);
                } else {
                        size_t last_line_len = strlen(last_line);
                        char *line = malloc(last_line_len + new_line_len + 1);
                        if (line) {
                                strcpy(line, last_line);
                                strcat(line, this->new_line_bfr);
                                link_line(this, line);
                        }
                }
        } else {
                link_new_line(this->new_line_bfr, new_line_len);
        }

        clear_new_line_buffer(this);
}


/*------------------------------------------------------------------------------
 * INTERFACES
 *----------------------------------------------------------------------------*/

//==============================================================================
/**
 * @brief  Initialize buffer
 * @param  None
 * @return If success buffer object, NULL on error
 */
//==============================================================================
ttybfr_t *ttybfr_new()
{
        ttybfr_t *bfr = calloc(1, sizeof(ttybfr_t));
        if (bfr) {
                bfr->self = bfr;
        }

        return bfr;
}

//==============================================================================
/**
 * @brief  Destroy buffer object
 * @param  this          buffer object
 * @return None
 */
//==============================================================================
void ttybfr_delete(ttybfr_t *this)
{
        if (is_valid(this)) {
                this->self = NULL;
                free(this);
        }
}

//==============================================================================
/**
 * @brief  Put string buffer to the line
 * @param  this          buffer object
 * @param  src           source
 * @param  len           length
 * @return None
 */
//==============================================================================
void ttybfr_put(ttybfr_t *this, const char *src, size_t len)
{
        void put_char(const char chr)
        {
                this->new_line_bfr[this->new_line_bfr_idx++] = chr;

                if (this->new_line_bfr_idx > _TTY_TERMINAL_COLUMNS) {
                        put_new_line_buffer(this);
                }
        }

        if (is_valid(this)) {
                bool VT100_cmd = false;

                while (len--) {
                        char chr = *src++;

                        if (chr == '\r') {
                                this->carriage = 0;
                                this->new_line_bfr_idx = 0;

                                if (LAST_CHARACTER(this->line[this->write_index]) != '\n') {
                                        clear_last_line(this);
                                }

                        } else if (chr == '\n') {
                                strcat(this->new_line_bfr, CR_LF);
                                put_new_line_buffer(this);
                                this->carriage = 0;

                        } else if (chr == '\e') {
                                VT100_cmd = true;
                                put_char(chr);

                        } else if (VT100_cmd) {
                                put_char(chr);

                                if (strchr("hlHKCDm", chr)) {
                                        VT100_cmd = false;
                                }

                        } else if (this->carriage >= _TTY_TERMINAL_COLUMNS) {
                                strcat(this->new_line_bfr, "\r\n");
                                put_new_line_buffer(this);
                                this->carriage = 0;

                        } else {
                                if (!(chr == '\t' || chr >= ' ')) {
                                        chr = 0xFF;
                                }

                                put_char(chr);
                                this->carriage++;
                        }
                }
        }
}

//==============================================================================
/**
 * @brief  Clear whole terminal
 * @param  this          buffer object
 * @return None
 */
//==============================================================================
void ttybfr_clear(ttybfr_t *this)
{
        if (is_valid(this)) {
                for (int i = 0; i < _TTY_TERMINAL_ROWS; i++) {
                        if (this->line[i]) {
                                free(this->line[i]);
                                this->line[i] = NULL;
                        }

                        this->fresh_line[i] = false;
                }

                this->write_index = 0;
        }
}

//==============================================================================
/**
 * @brief  Return n-line
 * @param  this          buffer object
 * @param  n             n-line from head
 * @return pointer to line or NULL if line doesn't exist
 */
//==============================================================================
const char *ttybfr_get_line(ttybfr_t *this, int n)
{
        if (is_valid(this) && n >= 0 && n < _TTY_TERMINAL_ROWS) {
                return this->line[get_line_index(this, n)];
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Return fresh line
 * @param  this          buffer object
 * @return pointer to new line or NULL if no new line
 */
//==============================================================================
const char *ttybfr_get_fresh_line(ttybfr_t *this)
{
        if (is_valid(this)) {
                for (int i = _TTY_TERMINAL_ROWS - 1; i >= 0; i--) {
                        uint idx = get_line_index(this, i);
                        if (this->fresh_line[idx]) {
                                this->fresh_line[idx] = false;
                                return this->line[idx];
                        }
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Clear fresh line counter
 * @param  this          buffer object
 * @return None
 */
//==============================================================================
void ttybfr_clear_fresh_line_counter(ttybfr_t *this)
{
        if (is_valid(this)) {
                for (int i = 0; i < _TTY_TERMINAL_ROWS; i++) {
                        this->fresh_line[i] = false;
                }
        }
}

//==============================================================================
/**
 * @brief  Flush prepare buffer
 * @param  this          buffer object
 * @return None
 */
//==============================================================================
void ttybfr_flush(ttybfr_t *this)
{
        if (is_valid(this)) {
                if (strlen(this->new_line_bfr)) {
                        put_new_line_buffer(this);
                        clear_new_line_buffer(this);
                }
        }
}

/*==============================================================================
  End of file
==============================================================================*/
