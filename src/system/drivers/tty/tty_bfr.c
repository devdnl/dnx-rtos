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

#define VT100_CLEAR_SCREEN                      "\e[2J"

/*==============================================================================
  Local object types
==============================================================================*/
struct ttybfr {
        char           *line[_TTY_DEFAULT_TERMINAL_HEIGHT];
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
 * @param bfr           buffer object
 * @param go_back       number of lines from current index
 *
 * @return line's index
 */
//==============================================================================
static uint get_line_index(ttybfr_t *bfr, uint go_back)
{
        if (bfr->write_index < go_back) {
                return _TTY_DEFAULT_TERMINAL_HEIGHT - (go_back - bfr->write_index);
        } else {
                return bfr->write_index - go_back;
        }
}

//==============================================================================
/**
 * @brief Function free the oldest line
 *
 * @param bfr           buffer object
 *
 * @return 0 if success, 1 on error
 */
//==============================================================================
static int free_the_oldest_line(ttybfr_t *bfr)
{
        for (int i = _TTY_DEFAULT_TERMINAL_HEIGHT - 1; i >= 0; i--) {
                int line_index = get_line_index(bfr, i);
                if (bfr->line[line_index]) {
                        free(bfr->line[line_index]);
                        bfr->line[line_index] = NULL;
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
 * pointer to new buffer.
 *
 * @param [in]  bfr             buffer object
 * @param [in]  src             source line
 * @param [out] new             if new string created set to true TODO
 *
 * @return pointer to new line
 */
//==============================================================================
static char *merge_or_create_line(ttybfr_t *bfr, const char *src, bool *new)
{
        char   *line          = NULL;
        char   *last_line     = bfr->line[get_line_index(bfr, 1)];
        size_t  last_line_len = strlen(last_line);

        if (last_line && (*(last_line + last_line_len - 1) != '\n')) {
                last_line_len += 1;

                if (src[0] == '\r' && strncmp(src, "\r\n", 2)) {
                        line = malloc(strlen(src + 1) + 1);
                } else {
                        line = malloc(last_line_len + strlen(src) + 1);
                }

                if (line) {
                        if (bfr->write_index == 0)
                                bfr->write_index = _TTY_DEFAULT_TERMINAL_HEIGHT - 1;
                        else
                                bfr->write_index--;

//                        if (bfr->new_line_cnt == 0) { FIXME
//                                tty->screen.refresh_last_line = SET;
//                        }

                        if (src[0] == '\r' && strncmp(src, "\r\n", 2)) {
                                strcpy(line, src + 1);
                        } else {
                                strcpy(line, last_line);
                                strcat(line, src);
                        }
                }
        } else {
                line = malloc(strlen(src) + 1);
                if (line) {
                        strcpy(line, src);

                        if (bfr->fresh_line_cnt < _TTY_DEFAULT_TERMINAL_HEIGHT)
                                bfr->fresh_line_cnt++;
                }
        }

        return line;
}

//==============================================================================
/**
 * @brief Function link prepared line to buffer
 *
 * @param bfr           buffer object
 * @param line          line
 */
//==============================================================================
static void link_line(ttybfr_t *bfr, char *line)
{
        if (bfr->line[bfr->write_index]) {
                free(bfr->line[bfr->write_index]);
        }

        bfr->line[bfr->write_index++] = line;

        if (bfr->write_index >= _TTY_DEFAULT_TERMINAL_HEIGHT) {
                bfr->write_index = 0;
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
 * @param bfr           buffer object
 */
//==============================================================================
void ttybfr_delete(ttybfr_t *bfr)
{
        if (bfr) {
                if (bfr->valid == VALIDATION_TOKEN) {
                        SET_VALIDATION(bfr, 0);
                        free(bfr);
                }
        }
}

//==============================================================================
/**
 * @brief Add new line to buffer
 *
 * @param bfr           buffer object
 * @param src           source
 * @param len           length
 */
//==============================================================================
void ttybfr_add_line(ttybfr_t *bfr, const char *src, size_t len)
{
        if (bfr) {
                if (bfr->valid == VALIDATION_TOKEN) {
                        if (strncmp(VT100_CLEAR_SCREEN, (char *)src, 4) == 0) {
                                ttybfr_clear(bfr);
                        }

                        char *crlf_line;
                        while (!(crlf_line = new_CRLF_line(src, len))) {
                                if (free_the_oldest_line(bfr) != 0) {
                                        break;
                                }
                        }

                        if (crlf_line) {
                                bool  new; /* TODO */
                                char *new_line = merge_or_create_line(bfr, crlf_line, &new);
                                link_line(bfr, new_line);
                                free(crlf_line);
                        }
                }
        }
}

//==============================================================================
/**
 * @brief Clear whole terminal
 *
 * @param bfr           buffer object
 */
//==============================================================================
void ttybfr_clear(ttybfr_t *bfr)
{
        if (bfr) {
                if (bfr->valid == VALIDATION_TOKEN) {
                        for (int i = 0; i < _TTY_DEFAULT_TERMINAL_HEIGHT; i++) {
                                if (bfr->line[i]) {
                                        free(bfr->line[i]);
                                        bfr->line[i] = NULL;
                                }
                        }

                        bfr->fresh_line_cnt = 0;
                        bfr->read_index   = 0;
                        bfr->write_index  = 0;
                }
        }
}

//==============================================================================
/**
 * @brief Return fresh line
 *
 * @param bfr           buffer object
 *
 * @return pointer to new line or NULL if no new line
 */
//==============================================================================
const char *ttybfr_get_fresh_line(ttybfr_t *bfr)
{
        if (bfr) {
                if (bfr->valid == VALIDATION_TOKEN) {
                        if (bfr->fresh_line_cnt) {
                                const char *str = bfr->line[get_line_index(bfr, bfr->fresh_line_cnt)];
                                if (str) {
                                        bfr->fresh_line_cnt--;
                                }

                                return str;
                        }
                }
        }

        return NULL;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
