/*=========================================================================*//**
@file    tty.c

@author  Daniel Zorychta

@brief   This file support virtual terminal

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "tty_cfg.h"
#include "tty_def.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define INPUT_SERVICE_TASK_NAME                 "tty:key"
#define OUTPUT_SERVICE_TASK_NAME                "tty:scr"
#define INPUT_SERVICE_TASK_STACK_DEPTH          STACK_DEPTH_VERY_LOW - 20
#define OUTPUT_SERVICE_TASK_STACK_DEPTH         STACK_DEPTH_VERY_LOW - 32
#define INPUT_SERVICE_TASK_PRIORITY             0
#define OUTPUT_SERVICE_TASK_PRIORITY            0

#define BLOCK_TIME                              10000

#define VT100_RESET_ATTRIBUTES                  "\e[0m"
#define VT100_CLEAR_SCREEN                      "\e[2J"
#define VT100_DISABLE_LINE_WRAP                 "\e[7l"
#define VT100_CURSOR_HOME                       "\e[H"
#define VT100_CURSOR_OFF                        "\e[?25l"
#define VT100_CURSOR_ON                         "\e[?25h"
#define VT100_CARRIAGE_RETURN                   "\r"
#define VT100_ERASE_LINE_FROM_CUR               "\e[K"
#define VT100_SAVE_CURSOR_POSITION              "\e7"
#define VT100_CURSOR_OFF                        "\e[?25l"
#define VT100_SET_CURSOR_POSITION(r, c)         "\e["#r";"#c"H"
#define VT100_QUERY_CURSOR_POSITION             "\e[6n"
#define VT100_RESTORE_CURSOR_POSITION           "\e8"
#define VT100_CURSOR_ON                         "\e[?25h"
#define VT100_SHIFT_CURSOR_RIGHT(t)             "\e["#t"C"

#ifndef EOF
#define EOF                                     (-1)
#endif

#ifndef ETX
#define ETX                                     0x03
#endif

#ifndef EOT
#define EOT                                     0x04
#endif

#define DEFAULT_COLUMN_NUMBER                   80
#define DEFAULT_ROW_NUMBER                      24

#define VT100_CMD_TIMEOUT                       5

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/* independent TTY device */
struct tty_data {
        struct screen_struct{
                char    *line[TTY_MAX_LINES];
                u16_t    write_index;
                u16_t    new_line_counter;
                flag_t   refresh_last_line;
        } screen;

        struct key_stream_struct {
                u16_t    level;
                char     buffer[TTY_STREAM_SIZE];
                u16_t    write_index;
                u16_t    read_index;
        } key_stream;

        struct edit_line_stream {
                sem_t   *read_sem;
                char     buffer[TTY_EDIT_LINE_LEN + 1];
                u16_t    length;
                u16_t    cursor_position;
                flag_t   echo_enabled;
        } edit_line;

        mutex_t *secure_resources_mtx;
        u8_t     device_number;
};

/* memory structure */
struct tty_ctrl {
        struct tty_data *tty[TTY_DEV_COUNT];            /* pointer to started TTYs */
        task_t          *input_service_handle;
        task_t          *output_service_handle;
        sem_t           *process_output_sem;            /* semaphore used to trigger output service */
        FILE            *io_stream;                     /* IO stream used to read/write data from/to VT100 */
        bool             tty_size_updated;
        u16_t            column_count;
        u16_t            row_count;
        u8_t             current_TTY;
        i8_t             change_to_TTY;

        struct VT100_cmd_capture_struct {
                bool            is_pending;             /* VT100 cmd capture enabled */
                u8_t            buffer_index;           /* VT100 cmd capture buffer index */
                char            buffer[12];             /* VT100 cmd capture buffer */
                int             timer;                  /* VT100 cmd capture timeout */
        } VT100_cmd_capture;
};

/* key detector results */
enum vt100cmd {
        F1_KEY,
        F2_KEY,
        F3_KEY,
        F4_KEY,
        ARROW_LEFT_KEY,
        ARROW_RIGHT_KEY,
        ARROW_UP_KEY,
        ARROW_DOWN_KEY,
        KEY_CAPTURE_PENDING,
        END_KEY,
        HOME_KEY,
        DEL_KEY,
        ESC_KEY,
        TERMINAL_SIZE_CMD,
        END_OF_TEXT,
        END_OF_TRANSMISSION,
        NORMAL_KEY
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static inline struct tty_data *current_tty_handle                   (void);
static void                    input_service_task                   (void *arg);
static void                    output_service_task                  (void *arg);
static void                    switch_tty_if_requested              (void);
static void                    move_cursor_to_beginning_of_editline (struct tty_data *tty);
static void                    move_cursor_to_end_of_editline       (struct tty_data *tty);
static void                    remove_character_from_editline       (struct tty_data *tty);
static void                    delete_character_from_editline       (struct tty_data *tty);
static void                    add_charater_to_editline             (struct tty_data *tty, char chr);
static void                    show_new_lines                       (struct tty_data *tty);
static void                    refresh_last_line                    (struct tty_data *tty);
static void                    switch_tty_to                        (int tty_number);
static void                    clear_tty                            (struct tty_data *tty);
static char                   *new_CRLF_line                        (const char *line, uint line_len);
static stdret_t                free_the_oldest_line                 (struct tty_data *tty);
static char                   *merge_or_create_line                 (struct tty_data *tty, const char *line_src);
static void                    add_line                             (struct tty_data *tty, const char *line, uint line_len);
static void                    strncpy_LF2CRLF                      (char *dst, const char *src, uint n);
static void                    link_line                            (struct tty_data *tty, char *line);
static void                    increase_line_counter                (struct tty_data *tty);
static enum vt100cmd           capture_VT100_commands               (char chr);
static u8_t                    get_line_index                       (struct tty_data *tty, u8_t go_back);
static void                    refresh_tty                          (struct tty_data *tty);
static void                    read_vt100_size                      (void);
static void                    write_key_stream                     (struct tty_data *tty, char chr);
static stdret_t                read_key_stream                      (struct tty_data *tty, char *chr);
static void                    move_editline_to_streams             (struct tty_data *tty, bool flush);

/*==============================================================================
  Local object definitions
==============================================================================*/
/* memory used by driver */
static struct tty_ctrl *tty_ctrl;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize TTY device
 */
//==============================================================================
API_MOD_INIT(TTY, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG(minor);

        STOP_IF(device_handle == NULL);

        if (major >= TTY_DEV_COUNT) {
                return STD_RET_ERROR;
        }

        /* initialize control structure */
        if (!tty_ctrl) {
                if (!(tty_ctrl = calloc(1, sizeof(struct tty_ctrl)))) {
                        goto ctrl_error;
                }

                if (!(tty_ctrl->process_output_sem = new_semaphore())) {
                        goto ctrl_error;
                }

                if (!(tty_ctrl->io_stream = vfs_fopen(TTYFILE, "r+"))) {
                        goto ctrl_error;
                }

                if (!(tty_ctrl->input_service_handle = new_task(input_service_task, INPUT_SERVICE_TASK_NAME, INPUT_SERVICE_TASK_STACK_DEPTH, NULL))) {
                        goto ctrl_error;
                }

                if (!(tty_ctrl->output_service_handle = new_task(output_service_task, OUTPUT_SERVICE_TASK_NAME, OUTPUT_SERVICE_TASK_STACK_DEPTH, NULL))) {
                        goto ctrl_error;
                }

                tty_ctrl->column_count  = DEFAULT_COLUMN_NUMBER;
                tty_ctrl->row_count     = DEFAULT_ROW_NUMBER;
                tty_ctrl->change_to_TTY = -1;
        }


        /* initialize driver data */
        struct tty_data *tty;
        if (!(tty = calloc(1, sizeof(struct tty_data)))) {
                goto drv_error;
        }

        if (!(tty->secure_resources_mtx = new_recursive_mutex())) {
                goto drv_error;
        }

        if (!(tty->edit_line.read_sem = new_counting_semaphore(DEFAULT_COLUMN_NUMBER, 0))) {
                goto drv_error;
        }

        tty->edit_line.echo_enabled = SET;
        tty_ctrl->tty[major]        = tty;
        tty->device_number          = major;
        *device_handle              = tty;

        return STD_RET_OK;


drv_error:
        if (tty) {
                if (tty->secure_resources_mtx) {
                        delete_recursive_mutex(tty->secure_resources_mtx);
                }

                free(tty);
        }
        return STD_RET_ERROR;


ctrl_error:
        if (tty_ctrl) {
                if (tty_ctrl->process_output_sem) {
                        delete_semaphore(tty_ctrl->process_output_sem);
                }

                if (tty_ctrl->input_service_handle) {
                        delete_task(tty_ctrl->input_service_handle);
                }

                if (tty_ctrl->output_service_handle) {
                        delete_task(tty_ctrl->output_service_handle);
                }

                if (tty_ctrl->io_stream) {
                        vfs_fclose(tty_ctrl->io_stream);
                }

                free(tty_ctrl);
                tty_ctrl = NULL;
        }
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release TTY device
 */
//==============================================================================
API_MOD_RELEASE(TTY, void *device_handle)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(tty_ctrl == NULL);

        struct tty_data *tty = device_handle;

        if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                clear_tty(tty);

                enter_critical_section();

                delete_semaphore(tty_ctrl->process_output_sem);
                delete_task(tty_ctrl->input_service_handle);
                delete_task(tty_ctrl->output_service_handle);
                vfs_fclose(tty_ctrl->io_stream);

                unlock_recursive_mutex(tty->secure_resources_mtx);
                delete_recursive_mutex(tty->secure_resources_mtx);

                tty_ctrl->tty[tty->device_number] = NULL;

                exit_critical_section();
                free(tty);
                return STD_RET_OK;
        } else {
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Opens specified port and initialize default settings
 */
//==============================================================================
API_MOD_OPEN(TTY, void *device_handle, int flags)
{
        UNUSED_ARG(flags);

        STOP_IF(device_handle == NULL);
        STOP_IF(tty_ctrl == NULL);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Close opened port
 */
//==============================================================================
API_MOD_CLOSE(TTY, void *device_handle, bool force, task_t *opened_by_task)
{
        UNUSED_ARG(force);
        UNUSED_ARG(opened_by_task);

        STOP_IF(device_handle == NULL);
        STOP_IF(tty_ctrl == NULL);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Write data to TTY
 */
//==============================================================================
API_MOD_WRITE(TTY, void *device_handle, const u8_t *src, size_t count, u64_t *fpos)
{
        UNUSED_ARG(fpos);

        STOP_IF(device_handle == NULL);
        STOP_IF(src == NULL);
        STOP_IF(count == 0);
        STOP_IF(tty_ctrl == NULL);

        struct tty_data *tty = device_handle;

        /* if current TTY is showing wait to show refreshed line */
        while (  tty_ctrl->tty[tty_ctrl->current_TTY]->screen.refresh_last_line
              && tty_ctrl->current_TTY == tty->device_number) {

                give_semaphore(tty_ctrl->process_output_sem);
                sleep_ms(10);
        }

        /* wait for secure access to data */
        size_t n = 0;
        if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                /* check if screen is cleared */
                if (strncmp(VT100_CLEAR_SCREEN, (char *)src, 4) == 0) {
                        clear_tty(tty);
                }

                add_line(tty, (char *)src, count);

                n = count;

                *fpos = -n;

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }

        return n;
}

//==============================================================================
/**
 * @brief Read data from TTY
 */
//==============================================================================
API_MOD_READ(TTY, void *device_handle, u8_t *dst, size_t count, u64_t *fpos)
{
        UNUSED_ARG(fpos);

        STOP_IF(device_handle == NULL);
        STOP_IF(dst == NULL);
        STOP_IF(count == 0);
        STOP_IF(tty_ctrl == NULL);

        struct tty_data *tty = device_handle;

        size_t n   = 0;
        char  *str = (char *)dst;
        while (count > 0) {
                while (read_key_stream(tty, str) != STD_RET_OK);

                n++;

                if (*str == '\n') {
                        break;
                }

                count--;
                str++;
        }

        *fpos = -n;

        return n;
}

//==============================================================================
/**
 * @brief Specific settings of TTY
 */
//==============================================================================
API_MOD_IOCTL(TTY, void *device_handle, int iorq, void *arg)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(tty_ctrl == NULL);

        struct tty_data *tty = device_handle;

        switch (iorq) {
        /* return current TTY */
        case TTY_IORQ_GET_CURRENT_TTY:
                if (arg == NULL) {
                        return STD_RET_ERROR;
                }
                *((int*)arg) = tty_ctrl->current_TTY;
                break;

        /* set active terminal */
        case TTY_IORQ_SWITCH_TTY_TO:
                switch_tty_to((int)arg);
                break;

        /* clear terminal */
        case TTY_IORQ_CLEAN_TTY:
                clear_tty(tty);
                break;

        /* terminal size - number of columns */
        case TTY_IORQ_GET_COL:
                if (arg == NULL) {
                        return STD_RET_ERROR;
                }
                *((int*)arg) = tty_ctrl->column_count;
                break;

        /* terminal size - number of rows */
        case TTY_IORQ_GET_ROW:
                if (arg == NULL) {
                        return STD_RET_ERROR;
                }
                *((int*)arg) = tty_ctrl->row_count;
                break;

        /* clear screen */
        case TTY_IORQ_CLEAR_SCR:
                clear_tty(tty);
                tty_ctrl->change_to_TTY = tty_ctrl->current_TTY;
                break;

        /* turn on terminal echo */
        case TTY_IORQ_ECHO_ON:
                tty->edit_line.echo_enabled = SET;
                break;

        /* turn off terminal echo */
        case TTY_IORQ_ECHO_OFF:
                tty->edit_line.echo_enabled = RESET;
                break;

        default:
                return STD_RET_ERROR;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Flush device
 */
//==============================================================================
API_MOD_FLUSH(TTY, void *device_handle)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(tty_ctrl == NULL);

        struct tty_data *tty = device_handle;

        if (lock_recursive_mutex(tty->secure_resources_mtx, MAX_DELAY) == MUTEX_LOCKED) {
                move_editline_to_streams(tty, true);
                unlock_recursive_mutex(tty->secure_resources_mtx);
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Interface returns device informations
 */
//==============================================================================
API_MOD_STAT(TTY, void *device_handle, struct vfs_dev_stat *device_stat)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(device_stat == NULL);
        STOP_IF(tty_ctrl == NULL);

        struct tty_data *tty = device_handle;

        device_stat->st_size  = 1;
        device_stat->st_major = tty->device_number;
        device_stat->st_minor = 0;
        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function returns handle of current TTY
 */
//==============================================================================
static inline struct tty_data *current_tty_handle(void)
{
        return tty_ctrl->tty[tty_ctrl->current_TTY];
}

//==============================================================================
/**
 * @brief TTY input service task
 *
 * @param[in] *arg      task arguments
 */
//==============================================================================
static void input_service_task(void *arg)
{
        (void) arg;

        set_priority(INPUT_SERVICE_TASK_PRIORITY);

        for (;;) {
                char chr = 0;
                if (vfs_fread(&chr, sizeof(char), 1, tty_ctrl->io_stream) == 0) {
                        continue;
                }

                struct tty_data *tty = current_tty_handle();
                enum   vt100cmd  cmd = capture_VT100_commands(chr);

                switch (cmd) {
                case F1_KEY:
                case F2_KEY:
                case F3_KEY:
                case F4_KEY:
                        switch_tty_to(cmd - F1_KEY);
                        break;

                case ESC_KEY:
                case NORMAL_KEY:
                        switch (chr) {
                        case '\r':
                        case '\n': move_editline_to_streams(tty, false); break;
                        case '\b': remove_character_from_editline(tty); break;
                        default  : add_charater_to_editline(tty, chr); break;
                        }
                        break;

                case ARROW_LEFT_KEY:
                        if (tty->edit_line.cursor_position > 0) {
                                vfs_fwrite("\b", sizeof(char), 1, tty_ctrl->io_stream);
                                tty->edit_line.cursor_position--;
                        }
                        break;

                case ARROW_RIGHT_KEY:
                        if (tty->edit_line.cursor_position < tty->edit_line.length) {
                                const char *cmd = VT100_SHIFT_CURSOR_RIGHT(1);
                                vfs_fwrite(cmd, sizeof(char), strlen(cmd), tty_ctrl->io_stream);
                                tty->edit_line.cursor_position++;
                        }
                        break;

                case ARROW_UP_KEY:
                        add_charater_to_editline(tty, '^');
                        add_charater_to_editline(tty, 'O');
                        add_charater_to_editline(tty, 'A');
                        move_editline_to_streams(tty, false);
                        break;

                case ARROW_DOWN_KEY:
                        add_charater_to_editline(tty, '^');
                        add_charater_to_editline(tty, '0');
                        add_charater_to_editline(tty, 'B');
                        move_editline_to_streams(tty, false);
                        break;

                case HOME_KEY:
                        move_cursor_to_beginning_of_editline(tty);
                        break;

                case END_KEY:
                        move_cursor_to_end_of_editline(tty);
                        break;

                case DEL_KEY:
                        delete_character_from_editline(tty);
                        break;

                case END_OF_TEXT:
                        add_charater_to_editline(tty, ETX);
                        move_editline_to_streams(tty, false);
                        break;

                case END_OF_TRANSMISSION:
                        add_charater_to_editline(tty, EOT);
                        move_editline_to_streams(tty, false);
                        break;

                default:
                        break;
                }
        }

        /* this should never happen */
        task_exit();
}

//==============================================================================
/**
 * @brief TTY screen task
 *
 * @param[in] *arg      task arguments
 */
//==============================================================================
static void output_service_task(void *arg)
{
        (void) arg;

        set_priority(OUTPUT_SERVICE_TASK_PRIORITY);

        const char *cmd = VT100_RESET_ATTRIBUTES
                          VT100_CLEAR_SCREEN
                          VT100_DISABLE_LINE_WRAP
                          VT100_CURSOR_HOME;

        vfs_fwrite(cmd, sizeof(char), strlen(cmd), tty_ctrl->io_stream);

        for (;;) {
                if (current_tty_handle() == NULL) {
                        sleep_ms(100);
                        continue;
                }

                if (take_semaphore(tty_ctrl->process_output_sem, MAX_DELAY) == SEMAPHORE_TAKEN) {
                        struct tty_data *tty = current_tty_handle();

                        if (tty->screen.new_line_counter > 0) {
                                show_new_lines(tty);

                        } else if (tty->screen.refresh_last_line == SET) {
                                refresh_last_line(tty);
                        }

                        switch_tty_if_requested();
                }
        }

        /* this should never happen */
        task_exit();
}

//==============================================================================
/**
 * @brief Function move the cursor to the beginning of edit line
 *
 * @param *tty                  terminal data
 */
//==============================================================================
static void move_cursor_to_beginning_of_editline(struct tty_data *tty)
{
        const char *cmd = VT100_CURSOR_OFF;
        vfs_fwrite(cmd, sizeof(char), strlen(cmd), tty_ctrl->io_stream);

        while (tty->edit_line.cursor_position > 0) {
                vfs_fwrite("\b", sizeof(char), 1, tty_ctrl->io_stream);
                tty->edit_line.cursor_position--;
        }

        cmd = VT100_CURSOR_ON;
        vfs_fwrite(cmd, sizeof(char), strlen(cmd), tty_ctrl->io_stream);
}

//==============================================================================
/**
 * @brief Function move the cursor to the end of edit line
 *
 * @param *tty                  terminal data
 */
//==============================================================================
static void move_cursor_to_end_of_editline(struct tty_data *tty)
{
        const char *cmd = VT100_CURSOR_OFF;
        vfs_fwrite(cmd, sizeof(char), strlen(cmd), tty_ctrl->io_stream);

        while (tty->edit_line.cursor_position < tty->edit_line.length) {
                char *cmd = VT100_SHIFT_CURSOR_RIGHT(1);
                vfs_fwrite(cmd, sizeof(char), strlen(cmd), tty_ctrl->io_stream);
                tty->edit_line.cursor_position++;
        }

        cmd = VT100_CURSOR_ON;
        vfs_fwrite(cmd, sizeof(char), strlen(cmd), tty_ctrl->io_stream);
}

//==============================================================================
/**
 * @brief Function remove character at cursor position from edit line
 *
 * @param *tty                  terminal data
 */
//==============================================================================
static void remove_character_from_editline(struct tty_data *tty)
{
        if (tty->edit_line.cursor_position == 0 || tty->edit_line.length == 0) {
                return;
        }

        tty->edit_line.cursor_position--;

        for (uint i = tty->edit_line.cursor_position; i < tty->edit_line.length; i++) {
                tty->edit_line.buffer[i] = tty->edit_line.buffer[i + 1];
        }

        tty->edit_line.length--;

        const char *cmd = "\b"VT100_ERASE_LINE_FROM_CUR VT100_SAVE_CURSOR_POSITION;
        vfs_fwrite(cmd, sizeof(char), strlen(cmd), tty_ctrl->io_stream);

        cmd = &tty->edit_line.buffer[tty->edit_line.cursor_position];
        vfs_fwrite(cmd, sizeof(char), tty->edit_line.length - tty->edit_line.cursor_position, tty_ctrl->io_stream);

        cmd = VT100_RESTORE_CURSOR_POSITION;
        vfs_fwrite(cmd, sizeof(char), strlen(cmd), tty_ctrl->io_stream);
}

//==============================================================================
/**
 * @brief Function delete a right character from edit line
 *
 * @param *tty                  terminal data
 */
//==============================================================================
static void delete_character_from_editline(struct tty_data *tty)
{
        if (tty->edit_line.length == 0) {
                return;
        }

        if (tty->edit_line.cursor_position == tty->edit_line.length) {
                return;
        }

        for (uint i = tty->edit_line.cursor_position; i <= tty->edit_line.length; i++) {
                tty->edit_line.buffer[i] = tty->edit_line.buffer[i + 1];
        }

        tty->edit_line.length--;

        const char *cmd = VT100_SAVE_CURSOR_POSITION VT100_ERASE_LINE_FROM_CUR;
        vfs_fwrite(cmd, sizeof(char), strlen(cmd), tty_ctrl->io_stream);

        cmd = &tty->edit_line.buffer[tty->edit_line.cursor_position];
        vfs_fwrite(cmd, sizeof(char), tty->edit_line.length - tty->edit_line.cursor_position, tty_ctrl->io_stream);

        cmd = VT100_RESTORE_CURSOR_POSITION;
        vfs_fwrite(cmd, sizeof(char), strlen(cmd), tty_ctrl->io_stream);
}

//==============================================================================
/**
 * @brief Function add character at cursor position in edit line
 *
 * @param *tty                  terminal data
 * @param  chr                  character added to edit line
 */
//==============================================================================
static void add_charater_to_editline(struct tty_data *tty, char chr)
{
        if (tty->edit_line.length >= TTY_EDIT_LINE_LEN - 1) {
                return;
        }

        if (tty->edit_line.cursor_position < tty->edit_line.length) {
                for (uint i = tty->edit_line.length; i > tty->edit_line.cursor_position; i--) {
                        tty->edit_line.buffer[i] = tty->edit_line.buffer[i - 1];
                }

                tty->edit_line.buffer[tty->edit_line.cursor_position++] = chr;
                tty->edit_line.length++;

                if (tty->edit_line.echo_enabled == SET) {
                        const char *cmd = VT100_SAVE_CURSOR_POSITION;
                        vfs_fwrite(cmd, sizeof(char), strlen(cmd), tty_ctrl->io_stream);

                        cmd = &tty->edit_line.buffer[tty->edit_line.cursor_position - 1];
                        vfs_fwrite(cmd, sizeof(char), tty->edit_line.length - (tty->edit_line.cursor_position - 1), tty_ctrl->io_stream);

                        cmd = VT100_RESTORE_CURSOR_POSITION VT100_SHIFT_CURSOR_RIGHT(1);
                        vfs_fwrite(cmd, sizeof(char), strlen(cmd), tty_ctrl->io_stream);
                }
        } else {
                tty->edit_line.buffer[tty->edit_line.cursor_position++] = chr;
                tty->edit_line.length++;

                if (tty->edit_line.echo_enabled == SET) {
                        vfs_fwrite(&chr, sizeof(char), 1, tty_ctrl->io_stream);
                }
        }
}

//==============================================================================
/**
 * @brief Function change active terminal if requested
 */
//==============================================================================
static void switch_tty_if_requested(void)
{
        if (tty_ctrl->change_to_TTY >= TTY_DEV_COUNT) {
                tty_ctrl->change_to_TTY = -1;
        } else if (tty_ctrl->change_to_TTY != -1) {
                if (tty_ctrl->tty[tty_ctrl->change_to_TTY] != NULL) {
                        tty_ctrl->current_TTY   = tty_ctrl->change_to_TTY;
                        tty_ctrl->change_to_TTY = -1;
                        refresh_tty(current_tty_handle());
                } else {
                        tty_ctrl->change_to_TTY = -1;
                }
        }
}

//==============================================================================
/**
 * @brief Function switch terminal
 *
 * @param tty_number    terminal to switch
 */
//==============================================================================
static void switch_tty_to(int tty_number)
{
        if (tty_number < TTY_DEV_COUNT && tty_ctrl->current_TTY != tty_number) {
                tty_ctrl->change_to_TTY = tty_number;
                give_semaphore(tty_ctrl->process_output_sem);
        }
}

//==============================================================================
/**
 * @brief Function show new lines
 *
 * @param *tty          terminal data
 */
//==============================================================================
static void show_new_lines(struct tty_data *tty)
{
        if (lock_recursive_mutex(tty->secure_resources_mtx, MAX_DELAY) == MUTEX_LOCKED) {

                while (tty->screen.new_line_counter) {
                        if (tty->screen.new_line_counter > tty_ctrl->row_count) {
                                tty->screen.new_line_counter = tty_ctrl->row_count;
                        }

                        const char *line = tty->screen.line[get_line_index(tty, tty->screen.new_line_counter)];

                        tty->screen.new_line_counter--;

                        if (line) {
                                vfs_fwrite(line, sizeof(char), strlen(line), tty_ctrl->io_stream);
                        }
                }

                tty->screen.refresh_last_line = RESET;

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }
}

//==============================================================================
/**
 * @brief Function refresh last line
 *
 * @param *tty          terminal data
 */
//==============================================================================
static void refresh_last_line(struct tty_data *tty)
{
        if (lock_recursive_mutex(tty->secure_resources_mtx, MAX_DELAY) == MUTEX_LOCKED) {

                const char *data = VT100_CURSOR_OFF
                                   VT100_CARRIAGE_RETURN
                                   VT100_ERASE_LINE_FROM_CUR
                                   VT100_RESET_ATTRIBUTES;

                vfs_fwrite(data, sizeof(char), strlen(data), tty_ctrl->io_stream);

                /* refresh line */
                data = tty->screen.line[get_line_index(tty, 1)];
                vfs_fwrite(data, sizeof(char), strlen(data), tty_ctrl->io_stream);

                /* cursor on */
                data = VT100_CURSOR_ON;
                vfs_fwrite(data, sizeof(char), strlen(data), tty_ctrl->io_stream);

                tty->screen.refresh_last_line = RESET;

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }
}

//==============================================================================
/**
 * @brief Clear all lines from terminal and free memory
 *
 * @param *tty          terminal data
 */
//==============================================================================
static void clear_tty(struct tty_data *tty)
{
        if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                for (uint i = 0; i < TTY_MAX_LINES; i++) {
                        if (tty->screen.line[i]) {
                                free(tty->screen.line[i]);
                                tty->screen.line[i] = NULL;
                        }
                }

                tty->screen.new_line_counter  = 0;
                tty->screen.write_index       = 0;
                tty->screen.refresh_last_line = RESET;

                give_semaphore(tty_ctrl->process_output_sem);

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }
}

//==============================================================================
/**
 * @brief Convert \n to \r\n in the line
 *
 * @param[in] *line             line data
 * @param[in]  line_len         line length
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

        char *new_line = malloc(line_len + (2 * LF_count) + 1);
        if (new_line) {
                strncpy_LF2CRLF(new_line, line, line_len + (2 * LF_count) + 1);
        }

        return new_line;
}

//==============================================================================
/**
 * @brief Function free the oldest line in terminal
 *
 * @param [in] *tty             terminal address
 *
 * @retval STD_RET_OK           successfully freed the oldest line
 * @retval STD_RET_ERROR        freeing not possible
 */
//==============================================================================
static stdret_t free_the_oldest_line(struct tty_data *tty)
{
        for (int i = TTY_MAX_LINES - 1; i >= 0; i--) {
                int line_index = get_line_index(tty, i);
                if (tty->screen.line[line_index]) {
                        free(tty->screen.line[line_index]);
                        tty->screen.line[line_index] = NULL;
                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
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
 * @param [in] *tty             terminal address
 * @param [in] *line_src        source line
 *
 * @return pointer to new line
 */
//==============================================================================
static char *merge_or_create_line(struct tty_data *tty, const char *line_src)
{
        char   *line          = NULL;
        char   *last_line     = tty->screen.line[get_line_index(tty, 1)];
        size_t  last_line_len = strlen(last_line);

        if (last_line && (*(last_line + last_line_len - 1) != '\n')) {
                last_line_len += 1;

                if (line_src[0] == '\r' && strncmp(line_src, "\r\n", 2)) {
                        line = malloc(strlen(line_src + 1) + 1);
                } else {
                        line = malloc(last_line_len + strlen(line_src) + 1);
                }

                if (line) {
                        if (tty->screen.write_index == 0)
                                tty->screen.write_index = TTY_MAX_LINES - 1;
                        else
                                tty->screen.write_index--;

                        if (tty->screen.new_line_counter == 0) {
                                tty->screen.refresh_last_line = SET;
                        }

                        if (line_src[0] == '\r' && strncmp(line_src, "\r\n", 2)) {
                                strcpy(line, line_src + 1);
                        } else {
                                strcpy(line, last_line);
                                strcat(line, line_src);
                        }
                }
        } else {
                line = malloc(strlen(line_src) + 1);
                if (line) {
                        strcpy(line, line_src);
                        increase_line_counter(tty);
                }
        }

        return line;
}

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
        if (!dst || !src || !n) {
                return;
        }

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
 * @brief Function link prepared line to TTY buffer
 *
 * @param *tty          terminal address
 * @param *line         line
 */
//==============================================================================
static void link_line(struct tty_data *tty, char *line)
{
        if (tty->screen.line[tty->screen.write_index]) {
                free(tty->screen.line[tty->screen.write_index]);
        }

        tty->screen.line[tty->screen.write_index++] = line;

        if (tty->screen.write_index >= TTY_MAX_LINES) {
                tty->screen.write_index = 0;
        }
}

//==============================================================================
/**
 * @brief Add new line or modify existing
 *
 * @param *tty          terminal address
 * @param *line         line
 * @param  line_len     line length
 */
//==============================================================================
static void add_line(struct tty_data *tty, const char *line, uint line_len)
{
        if (!line_len || !line) {
                return;
        }

        char *crlf_line;
        while (!(crlf_line = new_CRLF_line(line, line_len))) {
                if (free_the_oldest_line(tty) == STD_RET_ERROR) {
                        break;
                }
        }

        if (crlf_line) {
                char *new_line = merge_or_create_line(tty, crlf_line);
                link_line(tty, new_line);
                free(crlf_line);
        }

        give_semaphore(tty_ctrl->process_output_sem);
}

//==============================================================================
/**
 * @brief Function increase line counter
 *
 * @param *tty          terminal address
 */
//==============================================================================
static void increase_line_counter(struct tty_data *tty)
{
        if (tty->screen.new_line_counter < TTY_MAX_LINES) {
                tty->screen.new_line_counter++;
        }
}

//==============================================================================
/**
 * @brief Get last or selected line
 *
 * @param *tty          terminal address
 * @param  go_back      number of lines from current index
 *
 * @return line's index
 */
//==============================================================================
static u8_t get_line_index(struct tty_data *tty, u8_t go_back)
{
        /* check if index underflow when calculating with back */
        if (tty->screen.write_index < go_back) {
                return TTY_MAX_LINES - (go_back - tty->screen.write_index);
        } else {
                return tty->screen.write_index - go_back;
        }
}

//==============================================================================
/**
 * @brief Function capture VT100 commands and keys
 *
 * @param chr           button part of code
 *
 * @return decoded command/key
 */
//==============================================================================
static enum vt100cmd capture_VT100_commands(char chr)
{
        enum vt100cmd vt100cmd = KEY_CAPTURE_PENDING;

        if (  get_OS_time_ms() - tty_ctrl->VT100_cmd_capture.timer >= VT100_CMD_TIMEOUT
           && tty_ctrl->VT100_cmd_capture.is_pending == true) {

                if (  tty_ctrl->VT100_cmd_capture.buffer_index == 1
                   && tty_ctrl->VT100_cmd_capture.buffer[0] == '\e') {

                        vt100cmd = ESC_KEY;
                }

                tty_ctrl->VT100_cmd_capture.is_pending = false;
                memset(tty_ctrl->VT100_cmd_capture.buffer, 0, sizeof(tty_ctrl->VT100_cmd_capture.buffer));

                if (vt100cmd != KEY_CAPTURE_PENDING) {
                        return vt100cmd;
                }
        }

        if (strchr("\ecnRPQSDCFAB~", chr) != NULL && tty_ctrl->VT100_cmd_capture.is_pending == true) {

                tty_ctrl->VT100_cmd_capture.is_pending = false;

                if (  tty_ctrl->VT100_cmd_capture.buffer[tty_ctrl->VT100_cmd_capture.buffer_index - 1] == 'O'
                   || tty_ctrl->VT100_cmd_capture.buffer[tty_ctrl->VT100_cmd_capture.buffer_index - 1] == '[') {
                        switch (chr) {
                        case 'P': vt100cmd = F1_KEY; break;
                        case 'Q': vt100cmd = F2_KEY; break;
                        case 'R': vt100cmd = F3_KEY; break;
                        case 'S': vt100cmd = F4_KEY; break;
                        case 'D': vt100cmd = ARROW_LEFT_KEY; break;
                        case 'C': vt100cmd = ARROW_RIGHT_KEY; break;
                        case 'A': vt100cmd = ARROW_UP_KEY; break;
                        case 'B': vt100cmd = ARROW_DOWN_KEY; break;
                        case 'F': vt100cmd = END_KEY; break;
                        }
                } else {
                        switch (chr) {
                        /* calculate terminal size */
                        case 'R': {
                                const char *data = strchr(tty_ctrl->VT100_cmd_capture.buffer, '[');
                                int row_no = 0;
                                int column_no = 0;

                                if (data) {
                                        data++;
                                        while (*data != ';' && *data >= '0' && *data <= '9') {
                                                row_no *= 10;
                                                row_no += *data - '0';
                                                data++;
                                        }

                                        data++;
                                        while (*data != 'R' && *data >= '0' && *data <= '9') {
                                                column_no *= 10;
                                                column_no += *data - '0';
                                                data++;
                                        }
                                }

                                if (column_no > 0)
                                        tty_ctrl->column_count = column_no;

                                if (row_no > 0)
                                        tty_ctrl->row_count = row_no;

                                tty_ctrl->tty_size_updated = true;
                                break;
                        }

                        /* HOME and DELETE keys */
                        case '~':
                                switch (tty_ctrl->VT100_cmd_capture.buffer[tty_ctrl->VT100_cmd_capture.buffer_index - 1]) {
                                case '1': vt100cmd = HOME_KEY; break;
                                case '3': vt100cmd = DEL_KEY ; break;
                                }

                                break;
                        }
                }

                memset(tty_ctrl->VT100_cmd_capture.buffer, 0, sizeof(tty_ctrl->VT100_cmd_capture.buffer));
        } else {
                if (chr == '\e') {
                        tty_ctrl->VT100_cmd_capture.is_pending   = true;
                        tty_ctrl->VT100_cmd_capture.buffer_index = 0;
                        tty_ctrl->VT100_cmd_capture.timer        = get_OS_time_ms();
                }

                if (tty_ctrl->VT100_cmd_capture.is_pending == false) {
                        switch (chr) {
                        case 0x03: vt100cmd = END_OF_TEXT; break;
                        case 0x04: vt100cmd = END_OF_TRANSMISSION; break;
                        default  : vt100cmd = NORMAL_KEY; break;
                        }
                } else {
                        if (tty_ctrl->VT100_cmd_capture.buffer_index < ARRAY_SIZE(tty_ctrl->VT100_cmd_capture.buffer))
                                tty_ctrl->VT100_cmd_capture.buffer[tty_ctrl->VT100_cmd_capture.buffer_index++] = chr;
                }
        }

        return vt100cmd;
}

//==============================================================================
/**
 * @brief Refresh selected TTY
 *
 * @param *tty          terminal address
 */
//==============================================================================
static void refresh_tty(struct tty_data *tty)
{
        read_vt100_size();

        const char *data = VT100_RESET_ATTRIBUTES
                           VT100_CLEAR_SCREEN
                           VT100_DISABLE_LINE_WRAP
                           VT100_CURSOR_HOME;

        vfs_fwrite(data, sizeof(char), strlen(data), tty_ctrl->io_stream);

        if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                int rows;

                if (tty_ctrl->row_count < TTY_MAX_LINES) {
                        rows = tty_ctrl->row_count;
                } else {
                        rows = TTY_MAX_LINES;
                }

                for (int i = rows - 1; i > 0; i--) {
                        data = tty->screen.line[get_line_index(tty, i)];

                        if (data) {
                                vfs_fwrite(data, sizeof(char), strlen(data), tty_ctrl->io_stream);
                        }
                }

                data = tty->edit_line.buffer;
                vfs_fwrite(data, sizeof(char), strlen(data), tty_ctrl->io_stream);

                tty->screen.new_line_counter = 0;

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }
}

//==============================================================================
/**
 * @brief Function gets terminal size. Function receive analyze parameters in
 *        input service.
 */
//==============================================================================
static void read_vt100_size(void)
{
        tty_ctrl->column_count = DEFAULT_COLUMN_NUMBER;
        tty_ctrl->row_count    = DEFAULT_ROW_NUMBER;

#if (TTY_TERM_SIZE_CHECK != 0)
        /*
         * Function send request to the terminal to get the termnal's size.
         * Response of this request will be received by input handler.
         */

        const char *data = VT100_SAVE_CURSOR_POSITION
                           VT100_CURSOR_OFF
                           VT100_SET_CURSOR_POSITION(250, 250)
                           VT100_QUERY_CURSOR_POSITION;
        vfs_fwrite(data, sizeof(char), strlen(data), tty_ctrl->io_stream);

        /* waiting for response from input function */
        tty_ctrl->tty_size_updated = false;
        while (tty_ctrl->tty_size_updated == false) {
                sleep_ms(5);
        }

        /* restore cursor position, cursor on */
        data = VT100_RESTORE_CURSOR_POSITION
               VT100_CURSOR_ON;
        vfs_fwrite(data, sizeof(char), strlen(data), tty_ctrl->io_stream);
#endif
}

//==============================================================================
/**
 * @brief Function puts data to key stream
 *
 * @param *tty          terminal address
 * @param  chr          character
 */
//==============================================================================
static void write_key_stream(struct tty_data *tty, char chr)
{
        if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                tty->key_stream.buffer[tty->key_stream.write_index++] = chr;

                if (tty->key_stream.write_index >= TTY_STREAM_SIZE) {
                        tty->key_stream.write_index = 0;
                }

                if (tty->key_stream.level < TTY_STREAM_SIZE) {
                        tty->key_stream.level++;
                } else {
                        tty->key_stream.read_index++;

                        if (tty->key_stream.read_index >= TTY_STREAM_SIZE) {
                                tty->key_stream.read_index = 0;
                        }
                }

                give_counting_semaphore(tty->edit_line.read_sem);

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }
}

//==============================================================================
/**
 * @brief Function gets character from key stream
 *
 * @param[in]  *tty             terminal address
 * @param[out] *chr             pointer to character
 *
 * @retval STD_RET_OK           loaded character from stream
 * @retval STD_RET_ERROR        stream is empty
 */
//==============================================================================
static stdret_t read_key_stream(struct tty_data *tty, char *chr)
{
        if (take_counting_semaphore(tty->edit_line.read_sem, MAX_DELAY) == SEMAPHORE_TAKEN) {
                if (tty->key_stream.level == 0)
                        return STD_RET_ERROR;

                if (lock_recursive_mutex(tty->secure_resources_mtx, MAX_DELAY) == MUTEX_LOCKED) {
                        *chr = tty->key_stream.buffer[tty->key_stream.read_index++];

                        if (tty->key_stream.read_index >= TTY_STREAM_SIZE) {
                                tty->key_stream.read_index = 0;
                        }

                        tty->key_stream.level--;

                        unlock_recursive_mutex(tty->secure_resources_mtx);
                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function move edit line to output and key stream
 *
 * @param[in] *tty              terminal address
 * @param[in]  flush            move editline to streams at file flush
 */
//==============================================================================
static void move_editline_to_streams(struct tty_data *tty, bool flush)
{
        if (lock_recursive_mutex(tty->secure_resources_mtx, MAX_DELAY) == MUTEX_LOCKED) {
                tty->edit_line.buffer[tty->edit_line.length++] = flush ? ' ' : '\n';

                uint line_len = tty->edit_line.length;
                for (uint i = 0; i < line_len; i++) {
                        write_key_stream(tty, tty->edit_line.buffer[i]);
                }

                if (tty->edit_line.echo_enabled == SET) {
                        add_line(tty, tty->edit_line.buffer, line_len);
                        tty->screen.refresh_last_line = RESET;

                        const char *data;
                        if (tty->screen.new_line_counter == 0) {
                                data = "\r\n";
                        } else {
                                data = "\r"VT100_ERASE_LINE_FROM_CUR;
                        }

                        vfs_fwrite(data, sizeof(char), strlen(data), tty_ctrl->io_stream);
                }

                memset(tty->edit_line.buffer, '\0', TTY_EDIT_LINE_LEN);
                tty->edit_line.length = 0;
                tty->edit_line.cursor_position = 0;

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
