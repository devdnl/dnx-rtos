/*=========================================================================*//**
@file    tty_bfr.c

@author  Daniel Zorychta

@brief   Code in this file is responsible for buffer support.

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

/*==============================================================================
  Local macros
==============================================================================*/
#define VALIDATION_TOKEN                        (u32_t)0xFF49421D
#define SET_VALIDATION(_bfr, _val)              *(u32_t *)&_bfr->valid = _val;

/*==============================================================================
  Local object types
==============================================================================*/
struct ttybfr {
        char           *line[_TTY_DEFAULT_TERMINAL_ROWS];
        const u32_t     valid;
        u16_t           write_index;
        u16_t           read_index;
        u16_t           fresh_line_cnt;
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
 * @brief Function copy string and replace \n to \r\n
 *
 * @param *dst          destination string
 * @param *src          source string
 * @param  n            destination length
 */
//==============================================================================
static void strncpy_LF2CRLF(char *dst, const char *src, uint n)
{
        for (uint i = 0; i < (n - 1); i++) {
                if (*src == '\n') {
                        *(dst++) = '\r';
                        *(dst++) = *(src++);
                        i++;
                } else if (*src == '\0') {
                        break;
                } else {
                        *(dst++) = *(src++);
                }
        }

        *dst = '\0';
}

//==============================================================================
/**
 * @brief Convert \n to \r\n
 *
 * @param[in] line              line data
 * @param[in] line_len          line length
 *
 * @return pointer to new corrected line
 */
//==============================================================================
static char *new_CRLF_line(const char *line, uint line_len)
{
        /* calculate how many '\n' exist in string */
        uint LF_count = 0;
        for (uint i = 0; i < line_len; i++) {
                if (line[i] == '\n') {
                        LF_count++;
                }
        }

        char *new_line = malloc(line_len + LF_count + 1);
        if (new_line) {
                strncpy_LF2CRLF(new_line, line, line_len + LF_count + 1);
        }

        return new_line;
}

//==============================================================================
/**
 * @brief Get last or selected line
 *
 * @param this          buffer object
 * @param go_back       number of lines from current index
 *
 * @return line's index
 */
//==============================================================================
static uint get_line_index(ttybfr_t *this, uint go_back)
{
        if (this->write_index < go_back) {
                return _TTY_DEFAULT_TERMINAL_ROWS - (go_back - this->write_index);
        } else {
                return this->write_index - go_back;
        }
}

//==============================================================================
/**
 * @brief Function free the oldest line
 *
 * @param this          buffer object
 *
 * @return 0 if success, 1 on error
 */
//==============================================================================
static int free_the_oldest_line(ttybfr_t *this)
{
        for (int i = _TTY_DEFAULT_TERMINAL_ROWS - 1; i >= 0; i--) {
                int line_index = get_line_index(this, i);
                if (this->line[line_index]) {
                        free(this->line[line_index]);
                        this->line[line_index] = NULL;
                        return 0;
                }
        }

        return 1;
}

//==============================================================================
/**
 * @brief Function create new buffer for new line or merge new line with the
 *        latest
 *
 * Function create new buffer for new line if latest line is LF ended,
 * otherwise function merge latest line with new line. Function returns
 * pointer to new buffer (if created) or to source buffer if no changes was made.
 *
 * @param [in]  this            buffer object
 * @param [in]  src             source line
 * @param [out] new             if new string created set to true
 *
 * @return pointer to new line
 */
//==============================================================================
static char *merge_or_create_line(ttybfr_t *this, const char *src, bool *new)
{
        char   *line          = NULL;
        char   *last_line     = this->line[get_line_index(this, 1)];
        size_t  last_line_len = strlen(last_line);

        if (last_line && LAST_CHARACTER(last_line) != '\n') {
                last_line_len += 1;

                if (FIRST_CHARACTER(src) == '\r' && strncmp(src, "\r\n", 2) != 0) {
                        line = malloc(strlen(src + 1) + 1);
                        if (line) {
                                strcpy(line, src + 1);
                        } else {
                                return NULL;
                        }
                } else {
                        line = malloc(last_line_len + strlen(src) + 1);
                        if (line) {
                                strcpy(line, last_line);
                                strcat(line, src);
                        } else {
                                return NULL;
                        }
                }

                if (this->write_index == 0)
                        this->write_index = _TTY_DEFAULT_TERMINAL_ROWS - 1;
                else
                        this->write_index--;

                *new = true;
        } else {
                line = (char *)src;
                *new = false;
        }

        if (this->fresh_line_cnt < _TTY_DEFAULT_TERMINAL_ROWS) {
                u16_t total_lines;
                if (this->write_index > this->read_index) {
                        total_lines = this->write_index - this->read_index + 1;
                } else {
                        total_lines = (_TTY_DEFAULT_TERMINAL_ROWS - this->read_index) + this->write_index;
                }

                if (this->fresh_line_cnt < total_lines) {
                        this->fresh_line_cnt++;
                }
        }

        return line;
}

//==============================================================================
/**
 * @brief Function link prepared line to buffer
 *
 * @param this          buffer object
 * @param line          line
 */
//==============================================================================
static void link_line(ttybfr_t *this, char *line)
{
        if (this->line[this->write_index]) {
                free(this->line[this->write_index]);
        }

        this->line[this->write_index++] = line;

        if (this->write_index >= _TTY_DEFAULT_TERMINAL_ROWS) {
                this->write_index = 0;
        }
}

/*------------------------------------------------------------------------------
 * INTERFACES
 *----------------------------------------------------------------------------*/

//==============================================================================
/**
 * @brief Initialize buffer
 *
 * @return if success buffer object, NULL on error
 */
//==============================================================================
ttybfr_t *ttybfr_new()
{
        ttybfr_t *bfr = calloc(1, sizeof(ttybfr_t));
        if (bfr) {
                SET_VALIDATION(bfr, VALIDATION_TOKEN);
        }

        return bfr;
}

//==============================================================================
/**
 * @brief Destroy buffer object
 *
 * @param this          buffer object
 */
//==============================================================================
void ttybfr_delete(ttybfr_t *this)
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
 * @brief Add new line to buffer
 *
 * @param this          buffer object
 * @param src           source
 * @param len           length
 */
//==============================================================================
void ttybfr_add_line(ttybfr_t *this, const char *src, size_t len)
{
        if (this) {
                if (this->valid == VALIDATION_TOKEN) {
                        while (len) {
                                /* find line in buffer */
                                const char *line = src;
                                size_t      llen = 0;

                                while (llen++, --len && *src++ != '\n');

                                /* add line to buffer */
                                if (strncmp(VT100_CLEAR_SCREEN, (char *)line, 4) == 0) {
                                        ttybfr_clear(this);
                                }

                                char *crlf_line;
                                while (!(crlf_line = new_CRLF_line(line, llen))) {
                                        if (free_the_oldest_line(this) != 0) {
                                                break;
                                        }
                                }

                                if (crlf_line) {
                                        bool  new;
                                        char *new_line = merge_or_create_line(this, crlf_line, &new);
                                        link_line(this, new_line);

                                        if (new || new_line == NULL) {
                                                free(crlf_line);
                                        }
                                }
                        }
                }
        }
}

//==============================================================================
/**
 * @brief Clear whole terminal
 *
 * @param this          buffer object
 */
//==============================================================================
void ttybfr_clear(ttybfr_t *this)
{
        if (this) {
                if (this->valid == VALIDATION_TOKEN) {
                        for (int i = 0; i < _TTY_DEFAULT_TERMINAL_ROWS; i++) {
                                if (this->line[i]) {
                                        free(this->line[i]);
                                        this->line[i] = NULL;
                                }
                        }

                        this->fresh_line_cnt = 0;
                        this->read_index     = 0;
                        this->write_index    = 0;
                }
        }
}

//==============================================================================
/**
 * @brief Return n-line
 *
 * @param this          buffer object
 * @param n             n-line from head
 *
 * @return pointer to line or NULL if line doesn't exist
 */
//==============================================================================
const char *ttybfr_get_line(ttybfr_t *this, int n)
{
        if (this && n > 0 && n <= _TTY_DEFAULT_TERMINAL_ROWS) {
                if (this->valid == VALIDATION_TOKEN) {
                        return this->line[get_line_index(this, n)];
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Return fresh line
 *
 * @param this          buffer object
 *
 * @return pointer to new line or NULL if no new line
 */
//==============================================================================
const char *ttybfr_get_fresh_line(ttybfr_t *this)
{
        if (this) {
                if (this->valid == VALIDATION_TOKEN) {
                        if (this->fresh_line_cnt) {
                                return this->line[get_line_index(this, this->fresh_line_cnt--)];
                        }
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Clear fresh line counter
 *
 * @param this          buffer object
 */
//==============================================================================
void ttybfr_clear_fresh_line_counter(ttybfr_t *this)
{
        if (this) {
                if (this->valid == VALIDATION_TOKEN) {
                        this->fresh_line_cnt = 0;
                }
        }
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
