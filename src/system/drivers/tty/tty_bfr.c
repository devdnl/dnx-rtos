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
#define BUFFER_VALIDATION                       (u32_t)0xFF49421D
#define EDITLINE_VALIDATION                     (u32_t)0x6921363E

/*==============================================================================
  Local object types
==============================================================================*/
struct tty_buffer {
        char    *line[_TTY_DEFAULT_TERMINAL_HEIGHT];
        u32_t    valid;
        u16_t    write_index;
        u16_t    read_index;
        u16_t    new_line_cnt;
};

struct tty_editline {
        FILE    *out_file;
        char    *buffer[_TTY_EDIT_LINE_LEN + 1];
        u32_t    valid;
        u16_t    length;
        u16_t    cursor_position;
        bool     echo_enabled;
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
 * @brief Initialize buffer
 *
 * @param bfr           buffer address
 *
 * @return if success buffer object, NULL on error
 */
//==============================================================================
tty_buffer_t *ttybfr_new()
{
        tty_buffer_t *bfr = calloc(1, sizeof(tty_buffer_t));
        if (bfr) {
                bfr->valid = BUFFER_VALIDATION;
                return bfr;
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief Destroy buffer object
 *
 * @param bfr           buffer object
 */
//==============================================================================
void ttybfr_delete(tty_buffer_t *bfr)
{
        if (bfr) {
                if (bfr->valid == BUFFER_VALIDATION) {
                        bfr->valid = 0;
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
void ttybfr_add_line(tty_buffer_t *bfr, const char *src, size_t len)
{

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
tty_editline_t *ttyedit_new(FILE *out_file)
{
        tty_editline_t *edit = calloc(1, sizeof(tty_editline_t));
        if (edit) {
                edit->valid        = EDITLINE_VALIDATION;
                edit->out_file     = out_file;
                edit->echo_enabled = true;
                return edit;
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief Destroy editline
 *
 * @param edit          editline object
 */
//==============================================================================
void ttyedit_delete(tty_editline_t *edit)
{
        if (edit) {
                if (edit->valid == EDITLINE_VALIDATION) {
                        edit->valid = 0;
                        free(edit);
                }
        }
}

//==============================================================================
/**
 * @brief Enable editline echo
 *
 * @param edit          editline object
 */
//==============================================================================
void ttyedit_echo_enable(tty_editline_t *edit)
{
        edit->echo_enabled = true;
}

//==============================================================================
/**
 * @brief Disable editline echo
 *
 * @param edit          editline object
 */
//==============================================================================
void ttyedit_echo_disable(tty_editline_t *edit)
{
        edit->echo_enabled = false;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
