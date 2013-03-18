/*=========================================================================*//**
@file    tty.c

@author  Daniel Zorychta

@brief   This file support virtual terminal

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "tty.h"
#include <string.h>
#include "uart_def.h"
#include "vfs.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define TTYD_NAME                       "ttyworker"
#define TTYD_STACK_DEPTH                STACK_DEPTH_LOW
#define TTYD_PRIORITY                   0

#define FILE                            "/dev/ttyS0"

#define BLOCK_TIME                      10000

#define VT100_RESET_ATTRIBUTES          "\e[0m"
#define VT100_CLEAR_SCREEN              "\e[2J"
#define VT100_DISABLE_LINE_WRAP         "\e[7l"
#define VT100_CURSOR_HOME               "\e[H"
#define VT100_CURSOR_OFF                "\e[?25l"
#define VT100_CURSOR_ON                 "\e[?25h"
#define VT100_CARRIAGE_RETURN           "\r"
#define VT100_ERASE_LINE_FROM_CUR       "\e[K"
#define VT100_SAVE_CURSOR_POSITION      "\e7"
#define VT100_CURSOR_OFF                "\e[?25l"
#define VT100_SET_CURSOR_POSITION(r, c) "\e["#r";"#c"H"
#define VT100_QUERY_CURSOR_POSITION     "\e[6n"
#define VT100_RESTORE_CURSOR_POSITION   "\e8"
#define VT100_CURSOR_ON                 "\e[?25h"
#define VT100_SHIFT_CURSOR_RIGHT(t)     "\e["#t"C"

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/* independent TTY device */
struct tty_data {
        char    *line[TTY_MAX_LINES];
        u8_t    write_index;
        u8_t    new_msg_counter;
        flag_t  refresh_last_line;
        mutex_t *secure_resources_mtx;
        flag_t  echo_enabled;

        /* FIFO for keyboard read */
        struct input_stream {
                u16_t level;
                char  buffer[TTY_STREAM_SIZE];
                u16_t txidx;
                u16_t rxidx;
        } input;

        char edit_line[TTY_EDIT_LINE_LEN + 1];
        uint edit_line_length;
        uint cursor_position;

        u8_t number;
};

/* memory structure */
struct tty_ctrl {
        struct tty_data *tty[TTY_DEV_COUNT];
        u8_t    current_TTY;
        i8_t    change_to_TTY;
        u8_t    column_count;
        u8_t    row_count;
        task_t *tty_taskhdl;
        sem_t  *stdout_semcnt;          /* semaphore used to trigger daemon operation */
        u8_t    capture_key_step;       /* decode Fn key step */
        char    capture_key_tmp;        /* temporary value */
        uint    task_delay;             /* task delay depended by user activity */
};

/* key detector results */
enum keycap {
        F1_KEY,
        F2_KEY,
        F3_KEY,
        F4_KEY,
        ARROW_LEFT_KEY,
        ARROW_RIGHT_KEY,
        KEY_CAPTURE_PENDING,
        END_KEY,
        HOME_KEY,
        DEL_KEY,
        NORMAL_KEY
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void  task_tty(void *arg);
static void  switch_tty_if_requested(file_t *stream);
static void  stdout_service(struct tty_data *tty, file_t *stream);
static void  move_cursor_to_beginning_of_editline(struct tty_data *tty, file_t *stream);
static void  move_cursor_to_end_of_editline(struct tty_data *tty, file_t *stream);
static void  remove_character_from_editline(struct tty_data *tty, file_t *stream);
static void  delete_character_from_editline(struct tty_data *tty, file_t *stream);
static void  add_charater_to_editline(struct tty_data *tty, char chr, file_t *stream);
static void  show_new_messages(struct tty_data *tty, file_t *stream);
static void  refresh_last_line(struct tty_data *tty, file_t *stream);
static void  stdin_service(struct tty_data *tty, file_t *stream);
static void  switch_tty_immediately(u8_t tty_number, file_t *stream);
static void  clear_tty(struct tty_data *tty);
static uint  add_message(struct tty_data *tty, char *msg, uint msg_len);
static char *create_buffer_for_message(struct tty_data *tty, char *msg_src, uint msg_len);
static u8_t  count_non_lf_ended_messages(struct tty_data *tty);
static void  strncpy_LF2CRLF(char *dst, char *src, uint n);
static void  free_non_lf_ended_messages(struct tty_data *tty, u8_t msg_count);
static void  link_message(struct tty_data *tty, char *msg);
static void  inc_message_counter(struct tty_data *tty);
static enum keycap capture_special_keys(char character);
static u8_t  get_message_index(struct tty_data *tty, u8_t back);
static void  refresh_tty(struct tty_data *tty, file_t *file);
static void  get_vt100_size(file_t *ttysfile);
static void  write_input_stream(struct tty_data *tty, char chr);
static stdret_t read_input_stream(struct tty_data *tty, char *chr);
static void  move_editline_to_stream(struct tty_data *tty, file_t *stream);

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
 * @brief Initialize TTY devices
 *
 * @param[out] **drvhdl         driver's memory handler
 * @param[in]  dev              device number
 * @param[in]  part             device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t TTY_init(void **drvhdl, uint dev, uint part)
{
        (void)part;

        struct tty_data *tty;

        if (dev >= TTY_DEV_COUNT) {
                return STD_RET_ERROR;
        }


        /* initialize control structure */
        if (!tty_ctrl) {
                if (!(tty_ctrl = calloc(1, sizeof(struct tty_ctrl)))) {
                        goto ctrl_error;
                }

                if (!(tty_ctrl->stdout_semcnt = new_counting_semaphore(10, 0))) {
                        goto ctrl_error;
                }

                if (!(tty_ctrl->tty_taskhdl = new_task(task_tty, TTYD_NAME, TTYD_STACK_DEPTH, NULL))) {
                        goto ctrl_error;
                }

                tty_ctrl->column_count  = 80;
                tty_ctrl->row_count     = 24;
                tty_ctrl->change_to_TTY = -1;
        }


        /* initialize driver data */
        if (!(tty = calloc(1, sizeof(struct tty_data)))) {
                goto drv_error;
        }

        if (!(tty->secure_resources_mtx = new_recursive_mutex())) {
                goto drv_error;
        }

        tty->echo_enabled  = SET;
        tty_ctrl->tty[dev] = tty;
        tty->number = dev;
        *drvhdl     = tty;
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
                if (tty_ctrl->stdout_semcnt) {
                        delete_counting_semaphore(tty_ctrl->stdout_semcnt);
                }

                if (tty_ctrl->tty_taskhdl) {
                        delete_task(tty_ctrl->tty_taskhdl);
                }

                free(tty_ctrl);
                tty_ctrl = NULL;
        }
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release TTY device
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t TTY_release(void *drvhdl)
{
        struct tty_data *tty = drvhdl;

        if (!tty_ctrl || !tty) {
                return STD_RET_ERROR;
        }

        if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                clear_tty(tty);
                enter_critical();
                unlock_recursive_mutex(tty->secure_resources_mtx);
                delete_recursive_mutex(tty->secure_resources_mtx);
                tty_ctrl->tty[tty->number] = NULL;
                exit_critical();
                free(tty);
                return STD_RET_OK;
        } else {
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Opens specified port and initialize default settings
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t TTY_open(void *drvhdl)
{
        struct tty_data *tty = drvhdl;

        if (!tty_ctrl || !tty) {
                return STD_RET_ERROR;
        } else {
                return STD_RET_OK;
        }
}

//==============================================================================
/**
 * @brief Function close opened port
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t TTY_close(void *drvhdl)
{
        struct tty_data *tty = drvhdl;

        if (!tty_ctrl || !tty) {
                return STD_RET_ERROR;
        } else {
                return STD_RET_OK;
        }
}

//==============================================================================
/**
 * @brief Write data to TTY
 *
 * @param[in] *drvhdl           driver's memory handle
 * @param[in] *src              source
 * @param[in] size              size
 * @param[in] seek              seek
 *
 * @retval number of written nitems
 */
//==============================================================================
size_t TTY_write(void *drvhdl, void *src, size_t size, size_t nitems, size_t seek)
{
        (void)seek;

        struct tty_data *tty = drvhdl;
        size_t n = 0;

        if (!tty_ctrl || !tty || !src || !size || !nitems) {
                return n;
        }

        /* if current TTY is showing wait to show refreshed line */
        while (  tty_ctrl->tty[tty_ctrl->current_TTY]->refresh_last_line
              && tty_ctrl->current_TTY == tty->number) {
                milisleep(10);
        }

        /* wait for secure access to data */
        if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                /* check if screen is cleared */
                if (strncmp(VT100_CLEAR_SCREEN, src, 4) == 0) {
                        clear_tty(tty);
                }

                n = add_message(tty, src, nitems);

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }

        return n;
}

//==============================================================================
/**
 * @brief Write data to TTY
 *
 * @param[in]  *drvhdl          driver's memory handle
 * @param[out] *dst             destination
 * @param[in]  size             size
 * @param[in]  seek             seek
 *
 * @retval number of read nitems
 */
//==============================================================================
size_t TTY_read(void *drvhdl, void *dst, size_t size, size_t nitems, size_t seek)
{
        (void)seek;

        struct tty_data *tty = drvhdl;
        size_t n = 0;

        if (!tty_ctrl || !tty || !dst || !size || !nitems) {
                return n;
        }

        if (lock_recursive_mutex(tty->secure_resources_mtx, 0) == MUTEX_LOCKED) {
                while (nitems > 0) {
                        if (read_input_stream(tty, dst) != STD_RET_OK) {
                                break;
                        }

                        dst += size;
                        nitems--;
                        n++;
                }

                n /= size;

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }

        return n;
}

//==============================================================================
/**
 * @brief Specific settings of TTY
 *
 * @param[in]     *drvhdl       driver's memory handle
 * @param[in]     ioRq          IO reqest
 * @param[in,out] data          data pointer
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t TTY_ioctl(void *drvhdl, iorq_t iorq, void *data)
{
        struct tty_data *tty = drvhdl;

        if (!tty_ctrl || !tty) {
                return STD_RET_ERROR;
        }

        switch (iorq) {
        /* return current TTY */
        case TTY_IORQ_GET_CURRENT_TTY:
                if (data == NULL) {
                        return STD_RET_ERROR;
                }
                *((u8_t*)data) = tty_ctrl->current_TTY;
                break;

        /* set active terminal */
        case TTY_IORQ_SET_ACTIVE_TTY:
                if (data == NULL) {
                        return STD_RET_ERROR;
                }
                tty_ctrl->change_to_TTY = *((u8_t*)data);
                break;

        /* clear terminal */
        case TTY_IORQ_CLEAR_TTY:
                clear_tty(tty);
                break;

        /* terminal size - number of columns */
        case TTY_IORQ_GET_COL:
                if (data == NULL) {
                        return STD_RET_ERROR;
                }
                *((u8_t*)data) = tty_ctrl->column_count;
                break;

        /* terminal size - number of rows */
        case TTY_IORQ_GET_ROW:
                if (data == NULL) {
                        return STD_RET_ERROR;
                }
                *((u8_t*)data) = tty_ctrl->row_count;
                break;

        /* clear screen */
        case TTY_IORQ_CLEAR_SCR:
                tty_ctrl->change_to_TTY = tty_ctrl->current_TTY;
                clear_tty(tty);
                break;

        /* clear last line */
        case TTY_IORQ_CLEAR_LAST_LINE:
                if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                        char *msg = malloc(2);

                        if (msg) {
                                msg[0] = '\r';
                                msg[1] = '\0';

                                /* move message pointer back */
                                tty->write_index--;

                                if (tty->write_index >= TTY_MAX_LINES) {
                                        tty->write_index = TTY_MAX_LINES - 1;
                                }

                                /* if no message to refresh setup refresh only one */
                                if (tty->new_msg_counter == 0) {
                                        tty->refresh_last_line = SET;
                                }

                                add_message(tty, msg, FALSE);
                        }

                        unlock_recursive_mutex(tty->secure_resources_mtx);
                }

                break;

        /* turn on terminal echo */
        case TTY_IORQ_ECHO_ON:
                tty->echo_enabled = SET;
                break;

        /* turn off terminal echo */
        case TTY_IORQ_ECHO_OFF:
                tty->echo_enabled = RESET;
                break;

        default:
                return STD_RET_ERROR;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief TTY daemon
 */
//==============================================================================
static void task_tty(void *arg)
{
        (void) arg;
        char   *msg;
        file_t *io_file;

        set_priority(TTYD_PRIORITY);

        /* try open selected file */
        while ((io_file = fopen(FILE, "r+")) == NULL) {
                milisleep(250);
        }

        msg = VT100_RESET_ATTRIBUTES VT100_CLEAR_SCREEN VT100_DISABLE_LINE_WRAP
              VT100_CURSOR_HOME;

        fwrite(msg, sizeof(char), strlen(msg), io_file);

        get_vt100_size(io_file);

        tty_ctrl->task_delay = 10;

        for (;;) {
                if (tty_ctrl->tty[tty_ctrl->current_TTY] == NULL) {
                        milisleep(100);
                        continue;
                }

                stdout_service(tty_ctrl->tty[tty_ctrl->current_TTY], io_file);
                stdin_service(tty_ctrl->tty[tty_ctrl->current_TTY], io_file);
                switch_tty_if_requested(io_file);

                milisleep(tty_ctrl->task_delay);
        }

        /* this should never happen */
        task_exit();
}

//==============================================================================
/**
 * @brief Function provide STDOUT service
 *
 * @param *tty          terminal data
 * @param *stream       file
 */
//==============================================================================
static void stdout_service(struct tty_data *tty, file_t *stream)
{
        if (take_counting_semaphore(tty_ctrl->stdout_semcnt, 0) == OS_OK) {
                if (tty->new_msg_counter > 0) {
                        show_new_messages(tty, stream);

                        if (tty_ctrl->task_delay > 40){
                                tty_ctrl->task_delay = 40;
                        }
                } else if (tty->refresh_last_line == SET) {
                        refresh_last_line(tty, stream);
                }
        }
}

//==============================================================================
/**
 * @brief Function provide STDIN service
 *
 * @param *tty          terminal data
 * @param *stream       file
 */
//==============================================================================
static void stdin_service(struct tty_data *tty, file_t *stream)
{
        char  chr;
        char *msg;
        enum keycap keydec;

        if (ioctl(stream, UART_IORQ_GET_BYTE, &chr) != STD_RET_OK) {
                if (tty_ctrl->task_delay < 200) {
                        tty_ctrl->task_delay += 10;
                }
                return;
        }

        tty_ctrl->task_delay = 10;

        switch ((keydec = capture_special_keys(chr))) {
        case F1_KEY:
        case F2_KEY:
        case F3_KEY:
        case F4_KEY:
                switch_tty_immediately(keydec - F1_KEY, stream);
                break;

        case NORMAL_KEY:
                switch (chr) {
                case '\r':
                case '\n':
                        move_editline_to_stream(tty, stream);
                        break;

                case '\b':
                        remove_character_from_editline(tty, stream);
                        break;

                default:
                        add_charater_to_editline(tty, chr, stream);
                        break;
                }
                break;

        case ARROW_LEFT_KEY:
                if (tty->cursor_position > 0) {
                        chr = '\b';
                        ioctl(stream, UART_IORQ_SEND_BYTE, &chr);
                        tty->cursor_position--;
                }
                break;

        case ARROW_RIGHT_KEY:
                if (tty->cursor_position < tty->edit_line_length) {
                        msg = VT100_SHIFT_CURSOR_RIGHT(1);
                        fwrite(msg, sizeof(char), strlen(msg), stream);
                        tty->cursor_position++;
                }
                break;

        case HOME_KEY:
                move_cursor_to_beginning_of_editline(tty, stream);
                break;

        case END_KEY:
                move_cursor_to_end_of_editline(tty, stream);
                break;

        case DEL_KEY:
                delete_character_from_editline(tty, stream);
                break;

        default:
                break;
        }
}

//==============================================================================
/**
 * @brief Function move the cursor to the beginning of edit line
 *
 * @param *tty                  terminal data
 * @param *stream               required stream to refresh changes
 */
//==============================================================================
static void move_cursor_to_beginning_of_editline(struct tty_data *tty, file_t *stream)
{
        char *msg = VT100_CURSOR_OFF;
        fwrite(msg, sizeof(char), strlen(msg), stream);

        while (tty->cursor_position > 0) {
                char chr = '\b';
                ioctl(stream, UART_IORQ_SEND_BYTE, &chr);
                tty->cursor_position--;
        }

        msg = VT100_CURSOR_ON;
        fwrite(msg, sizeof(char), strlen(msg), stream);
}

//==============================================================================
/**
 * @brief Function move the cursor to the end of edit line
 *
 * @param *tty                  terminal data
 * @param *stream               required stream to refresh changes
 */
//==============================================================================
static void move_cursor_to_end_of_editline(struct tty_data *tty, file_t *stream)
{
        char *msg = VT100_CURSOR_OFF;
        fwrite(msg, sizeof(char), strlen(msg), stream);

        while (tty->cursor_position < tty->edit_line_length) {
                char *msg = VT100_SHIFT_CURSOR_RIGHT(1);
                fwrite(msg, sizeof(char), strlen(msg), stream);
                tty->cursor_position++;
        }

        msg = VT100_CURSOR_ON;
        fwrite(msg, sizeof(char), strlen(msg), stream);
}

//==============================================================================
/**
 * @brief Function remove character at cursor position from edit line
 *
 * @param *tty                  terminal data
 * @param *stream               required stream to refresh changes
 */
//==============================================================================
static void remove_character_from_editline(struct tty_data *tty, file_t *stream)
{
        if (tty->cursor_position == 0 || tty->edit_line_length == 0) {
                return;
        }

        tty->cursor_position--;

        for (uint i = tty->cursor_position; i < tty->edit_line_length; i++) {
                tty->edit_line[i] = tty->edit_line[i + 1];
        }

        tty->edit_line_length--;

        char *msg = "\b"VT100_ERASE_LINE_FROM_CUR VT100_SAVE_CURSOR_POSITION;
        fwrite(msg, sizeof(char), strlen(msg), stream);

        msg = &tty->edit_line[tty->cursor_position];
        fwrite(msg, sizeof(char), tty->edit_line_length - tty->cursor_position, stream);

        msg = VT100_RESTORE_CURSOR_POSITION;
        fwrite(msg, sizeof(char), strlen(msg), stream);
}

//==============================================================================
/**
 * @brief Function delete right character from edit line
 *
 * @param *tty                  terminal data
 * @param *stream               required stream to refresh changes
 */
//==============================================================================
static void delete_character_from_editline(struct tty_data *tty, file_t *stream)
{
        if (tty->edit_line_length == 0) {
                return;
        }

        if (tty->cursor_position == tty->edit_line_length) {
                return;
        }

        for (uint i = tty->cursor_position; i <= tty->edit_line_length; i++) {
                tty->edit_line[i] = tty->edit_line[i + 1];
        }

        tty->edit_line_length--;

        char *msg = VT100_SAVE_CURSOR_POSITION VT100_ERASE_LINE_FROM_CUR;
        fwrite(msg, sizeof(char), strlen(msg), stream);

        msg = &tty->edit_line[tty->cursor_position];
        fwrite(msg, sizeof(char), tty->edit_line_length - tty->cursor_position, stream);

        msg = VT100_RESTORE_CURSOR_POSITION;
        fwrite(msg, sizeof(char), strlen(msg), stream);
}

//==============================================================================
/**
 * @brief Function add character at cursor position in edit line
 *
 * @param *tty                  terminal data
 * @param chr                   character added to edit line
 * @param *stream               required stream to refresh changes
 */
//==============================================================================
static void add_charater_to_editline(struct tty_data *tty, char chr, file_t *stream)
{
        char *msg;

        if (tty->edit_line_length >= TTY_EDIT_LINE_LEN - 1) {
                return;
        }

        if (tty->cursor_position < tty->edit_line_length) {
                for (uint i = tty->edit_line_length; i > tty->cursor_position; i--) {
                        tty->edit_line[i] = tty->edit_line[i - 1];
                }

                tty->edit_line[tty->cursor_position++] = chr;
                tty->edit_line_length++;

                msg = VT100_SAVE_CURSOR_POSITION;
                fwrite(msg, sizeof(char), strlen(msg), stream);

                msg = &tty->edit_line[tty->cursor_position - 1];
                fwrite(msg, sizeof(char), tty->edit_line_length - (tty->cursor_position - 1), stream);

                msg = VT100_RESTORE_CURSOR_POSITION VT100_SHIFT_CURSOR_RIGHT(1);
                fwrite(msg, sizeof(char), strlen(msg), stream);
        } else {
                tty->edit_line[tty->cursor_position++] = chr;
                tty->edit_line_length++;

                if (tty->echo_enabled == SET) {
                        ioctl(stream, UART_IORQ_SEND_BYTE, &chr);
                }
        }
}

//==============================================================================
/**
 * @brief Function change active terminal if requested
 *
 * @param *stream       required stream to refresh changed terminal
 */
//==============================================================================
static void switch_tty_if_requested(file_t *stream)
{
        if (tty_ctrl->change_to_TTY != -1) {
                tty_ctrl->current_TTY   = tty_ctrl->change_to_TTY;
                tty_ctrl->change_to_TTY = -1;
                refresh_tty(tty_ctrl->tty[tty_ctrl->current_TTY], stream);
        }
}

//==============================================================================
/**
 * @brief Function switch terminal immediately
 *
 * @param  dev          terminal to switch
 * @param *stream       required stream to refresh changed terminal
 */
//==============================================================================
static void switch_tty_immediately(u8_t tty_number, file_t *stream)
{
        if (tty_number != tty_ctrl->current_TTY) {
                tty_ctrl->current_TTY = tty_number;

                if (tty_ctrl->tty[tty_ctrl->current_TTY]) {
                        refresh_tty(tty_ctrl->tty[tty_ctrl->current_TTY], stream);
                }
        }
}

//==============================================================================
/**
 * @brief Function show new messages
 *
 * @param *tty          terminal data
 * @param *stream       required stream to refresh new messages
 */
//==============================================================================
static void show_new_messages(struct tty_data *tty, file_t *stream)
{
        while (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) != MUTEX_LOCKED);

        while (tty->new_msg_counter) {
                if (tty->new_msg_counter > tty_ctrl->row_count) {
                        tty->new_msg_counter = tty_ctrl->row_count;
                }

                char *msg = tty->line[get_message_index(tty, tty->new_msg_counter)];

                tty->new_msg_counter--;

                if (msg) {
                        fwrite(msg, sizeof(char), strlen(msg), stream);
                }
        }

        tty->refresh_last_line = RESET;

        unlock_recursive_mutex(tty->secure_resources_mtx);
}

//==============================================================================
/**
 * @brief Function refresh last line
 *
 * @param *tty          terminal data
 * @param *stream       required stream to refresh new messages
 */
//==============================================================================
static void refresh_last_line(struct tty_data *tty, file_t *stream)
{
        while (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) != MUTEX_LOCKED);

        char *msg = VT100_CURSOR_OFF
                    VT100_CARRIAGE_RETURN
                    VT100_ERASE_LINE_FROM_CUR
                    VT100_RESET_ATTRIBUTES;

        fwrite(msg, sizeof(char), strlen(msg), stream);

        /* refresh line */
        msg = tty->line[get_message_index(tty, 1)];
        fwrite(msg, sizeof(char), strlen(msg), stream);

        /* cursor on */
        msg = VT100_CURSOR_ON;
        fwrite(msg, sizeof(char), strlen(msg), stream);

        tty->refresh_last_line = RESET;

        unlock_recursive_mutex(tty->secure_resources_mtx);
}

//==============================================================================
/**
 * @brief Clear all messages from terminal and free memory
 *
 * @param *tty          terminal data
 */
//==============================================================================
static void clear_tty(struct tty_data *tty)
{
        if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                for (uint i = 0; i < TTY_MAX_LINES; i++) {
                        if (tty->line[i]) {
                                free(tty->line[i]);
                                tty->line[i] = NULL;
                        }
                }

                tty->new_msg_counter   = 0;
                tty->write_index       = 0;
                tty->refresh_last_line = RESET;

                give_counting_semaphore(tty_ctrl->stdout_semcnt);

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }
}

//==============================================================================
/**
 * @brief Add new message or modify existing
 *
 * @param *tty          terminal address
 * @param *msg          message
 * @param  msgLen       message length
 *
 * @return msgLen if message created, otherwise 0
 */
//==============================================================================
static uint add_message(struct tty_data *tty, char *msg, uint msg_len)
{
        uint  msg_cnt;
        char *new_msg;

        if (!msg_len || !msg) {
                return 0;
        }

        msg_cnt = count_non_lf_ended_messages(tty);

        free_non_lf_ended_messages(tty, msg_cnt);

        new_msg = create_buffer_for_message(tty, msg, msg_len);

        link_message(tty, new_msg);

        give_counting_semaphore(tty_ctrl->stdout_semcnt);

        if (new_msg) {
                return msg_len;
        } else {
                return 0;
        }
}

//==============================================================================
/**
 * @brief Function create new buffer for new message
 * Function create new buffer for new message if latest message is LF ended,
 * otherwise function merge latest message with new message. Function returns
 * pointer to new buffer.
 *
 * @param [in] *tty             terminal address
 * @param [in] *msg_src         message source pointer
 * @param [in]  msg_len         message length
 *
 * @return pointer to new message buffer
 */
//==============================================================================
static char *create_buffer_for_message(struct tty_data *tty, char *msg_src, uint msg_len)
{
        char   *msg;
        char   *lst_msg     = tty->line[get_message_index(tty, 1)];
        size_t  lst_msg_len = strlen(lst_msg);
        uint    LF_count    = 0;

        /* calculate how many '\n' exist in string */
        for (uint i = 0; i < msg_len; i++) {
                if (msg_src[i] == '\n') {
                        LF_count++;
                }
        }

        if (lst_msg && (*(lst_msg + lst_msg_len - 1) != '\n')) {
                lst_msg_len += 1;

                msg = malloc(lst_msg_len + msg_len + (2 * LF_count));
                if (msg) {
                        tty->write_index--;
                        if (tty->write_index >= TTY_MAX_LINES) {
                                tty->write_index = TTY_MAX_LINES - 1;
                        }

                        /* if no message to refresh setup refresh only one */
                        if (tty->new_msg_counter == 0) {
                                tty->refresh_last_line = SET;
                        }

                        strcpy(msg, lst_msg);
                        strncpy_LF2CRLF(msg + strlen(msg), msg_src, msg_len + LF_count);
                }
        } else {
                msg = malloc(msg_len + (2 * LF_count));
                if (msg) {
                        strncpy_LF2CRLF(msg, msg_src, msg_len + (2 * LF_count));
                        inc_message_counter(tty);
                }
        }

        return msg;
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
static void strncpy_LF2CRLF(char *dst, char *src, uint n)
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
 * @brief Function find and count number of non-LF ended lines
 * Function count the number of non-LF ended messages from current write index.
 *
 * @param *tty          terminal address
 */
//==============================================================================
static u8_t count_non_lf_ended_messages(struct tty_data *tty)
{
        u8_t write_index = tty->write_index;
        u8_t msg_cnt = 1;

        /* find all messages which are not ended by LF (this is common line) */
        for (uint i = 0; i < TTY_MAX_LINES; i++) {
                char *last_msg = tty->line[write_index++];

                if (last_msg == NULL) {
                        break;
                }

                if (write_index >= TTY_MAX_LINES) {
                        write_index = 0;
                }

                if (*(last_msg + strlen(last_msg) - 1) != '\n') {
                        msg_cnt++;
                } else {
                        break;
                }
        }

        return msg_cnt;
}

//==============================================================================
/**
 * @brief Function free all old non-LF ended messages
 * Function does not change tty's wrIdx. Function free all non-LF ended messages
 * from current write index -- function clear the latest messages.
 *
 * @param  *tty         terminal address
 * @param  msg_count    message count to free
 */
//==============================================================================
static void free_non_lf_ended_messages(struct tty_data *tty, u8_t msg_count)
{
        u8_t write_idx = tty->write_index;

        while (msg_count--) {
                if (tty->line[write_idx]) {
                        free(tty->line[write_idx]);
                        tty->line[write_idx] = NULL;
                }

                write_idx++;

                if (write_idx >= TTY_MAX_LINES) {
                        write_idx = 0;
                }
        }
}

//==============================================================================
/**
 * @brief Function link prepared message to TTY
 *
 * @param *tty          terminal address
 * @param *msg          message
 */
//==============================================================================
static void link_message(struct tty_data *tty, char *msg)
{
        tty->line[tty->write_index++] = msg;

        if (tty->write_index >= TTY_MAX_LINES) {
                tty->write_index = 0;
        }
}

//==============================================================================
/**
 * @brief Function increase message counter
 *
 * @param *tty          terminal address
 */
//==============================================================================
static void inc_message_counter(struct tty_data *tty)
{
        if (tty->new_msg_counter < TTY_MAX_LINES) {
                tty->new_msg_counter++;
        }
}

//==============================================================================
/**
 * @brief Get last or selected message
 *
 * @param *tty          terminal address
 * @param back          number of lines from current index
 *
 * @return index to message
 */
//==============================================================================
static u8_t get_message_index(struct tty_data *tty, u8_t back)
{
        /* check if index underflow when calculating with back */
        if (tty->write_index < back) {
                return TTY_MAX_LINES - (back - tty->write_index);
        } else {
                return tty->write_index - back;
        }
}

//==============================================================================
/**
 * @brief Function capture special keys
 *
 * @param character     button part of code
 *
 * @return decoded key
 */
//==============================================================================
static enum keycap capture_special_keys(char character)
{
        switch (tty_ctrl->capture_key_step) {
        case 0:
                if (character == '\e') {
                        tty_ctrl->capture_key_step++;
                        return KEY_CAPTURE_PENDING;
                }
                break;

        case 1:
                if (character == 'O' || character == '[') {
                        tty_ctrl->capture_key_step++;
                        return KEY_CAPTURE_PENDING;
                } else {
                        tty_ctrl->capture_key_step = 0;
                }
                break;

        case 2:
                tty_ctrl->capture_key_step = 0;

                switch (character) {
                case 'P': return F1_KEY;
                case 'Q': return F2_KEY;
                case 'R': return F3_KEY;
                case 'S': return F4_KEY;
                case 'D': return ARROW_LEFT_KEY;
                case 'C': return ARROW_RIGHT_KEY;
                case 'F': return END_KEY;
                case '1': tty_ctrl->capture_key_tmp  = '1';
                          tty_ctrl->capture_key_step = 3;
                          return KEY_CAPTURE_PENDING;
                case '3': tty_ctrl->capture_key_tmp  = '3';
                          tty_ctrl->capture_key_step = 3;
                          return KEY_CAPTURE_PENDING;
                }

                break;

        case 3:
                tty_ctrl->capture_key_step = 0;

                if (character == '~') {
                        switch (tty_ctrl->capture_key_tmp) {
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        }
                }

                break;

        default:
                tty_ctrl->capture_key_step = 0;
                break;
        }

        return NORMAL_KEY;
}

//==============================================================================
/**
 * @brief Refresh selected TTY
 *
 * @param *tty          terminal address
 */
//==============================================================================
static void refresh_tty(struct tty_data *tty, file_t *file)
{
        get_vt100_size(file);

        char *msg = VT100_RESET_ATTRIBUTES  VT100_CLEAR_SCREEN
                    VT100_DISABLE_LINE_WRAP VT100_CURSOR_HOME;

        fwrite(msg, sizeof(char), strlen(msg), file);

        if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                i8_t rows;

                if (tty_ctrl->row_count < TTY_MAX_LINES) {
                        rows = tty_ctrl->row_count;
                } else {
                        rows = TTY_MAX_LINES;
                }

                for (i8_t i = rows - 1; i > 0; i--) {
                        msg = tty->line[get_message_index(tty, i)];

                        if (msg) {
                                fwrite(msg, sizeof(char), strlen(msg), file);
                        }
                }

                msg = tty->edit_line;
                fwrite(msg, sizeof(char), strlen(msg), file);

                tty->new_msg_counter = 0;

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }
}

//==============================================================================
/**
 * @brief Function gets terminal size
 *
 * @param *ttysfile     tty file
 */
//==============================================================================
static void get_vt100_size(file_t *ttysfile)
{
        char chr = 0;

        /* set default values */
        tty_ctrl->column_count = 80;
        tty_ctrl->row_count = 24;

        char *rq = VT100_SAVE_CURSOR_POSITION
                   VT100_CURSOR_OFF
                   VT100_SET_CURSOR_POSITION(250, 250)
                   VT100_QUERY_CURSOR_POSITION;

        fwrite(rq, sizeof(char), strlen(rq), ttysfile);

        /* buffer for response */
        char resp[10];
        resp[sizeof(resp) - 1] = '\0';

        /* waiting for ESC byte */
        u8_t try = 10;
        do {
                if (ioctl(ttysfile, UART_IORQ_GET_BYTE, &chr) != STD_RET_OK) {
                        milisleep(100);
                }
        } while (--try && chr != '\x1B');

        if (try == 0) {
                return;
        }

        /* get data */
        char *respPtr = &resp[0];
        flag_t finded = RESET;
        for (u8_t i = 0; i < sizeof(resp) - 1; i++) {
                try = 10;

                while ((ioctl(ttysfile, UART_IORQ_GET_BYTE, respPtr) != STD_RET_OK) && try) {
                        milisleep(10);
                        try--;
                }

                if (*respPtr++ == 'R') {
                        finded = SET;
                        break;
                }
        }

        if (finded == RESET) {
                return;
        }

        /* restore cursor position, cursor on */
        rq = VT100_RESTORE_CURSOR_POSITION VT100_CURSOR_ON;
        fwrite(rq, sizeof(char), strlen(rq), ttysfile);

        /* find response begin */
        char *seek = strchr(resp, '[');
        seek++;

        u8_t row = 0;
        u8_t col = 0;

        /* calculate row_count */
        while ((chr = *(seek++)) != ';') {
                row *= 10;
                row += chr - '0';
        }

        /* calculate columns */
        while ((chr = *(seek++)) != 'R') {
                col *= 10;
                col += chr - '0';
        }

        tty_ctrl->row_count    = row;
        tty_ctrl->column_count = col;
}

//==============================================================================
/**
 * @brief Function puts data to input stream
 *
 * @param *tty          terminal address
 * @param chr           character
 */
//==============================================================================
static void write_input_stream(struct tty_data *tty, char chr)
{
        if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                tty->input.buffer[tty->input.txidx++] = chr;

                if (tty->input.txidx >= TTY_STREAM_SIZE) {
                        tty->input.txidx = 0;
                }

                if (tty->input.level < TTY_STREAM_SIZE) {
                        tty->input.level++;
                } else {
                        tty->input.rxidx++;

                        if (tty->input.rxidx >= TTY_STREAM_SIZE) {
                                tty->input.rxidx = 0;
                        }
                }

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }
}

//==============================================================================
/**
 * @brief Function gets character from input stream
 *
 * @param[in]  *tty             terminal address
 * @param[out] *chr             pointer to character
 *
 * @retval STD_RET_OK           loaded character from stream
 * @retval STD_RET_ERROR        stream is empty
 */
//==============================================================================
static stdret_t read_input_stream(struct tty_data *tty, char *chr)
{
        if (tty->input.level == 0) {
                return STD_RET_ERROR;
        }

        if (lock_recursive_mutex(tty->secure_resources_mtx, 0) == MUTEX_LOCKED) {
                *chr = tty->input.buffer[tty->input.rxidx++];

                if (tty->input.rxidx >= TTY_STREAM_SIZE) {
                        tty->input.rxidx = 0;
                }

                tty->input.level--;

                unlock_recursive_mutex(tty->secure_resources_mtx);
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function move edit line to stream
 *
 * @param[in] *tty              terminal address
 * @param[in] *stream           terminal control stream
 */
//==============================================================================
static void move_editline_to_stream(struct tty_data *tty, file_t *stream)
{
        char *msg;

        tty->edit_line[tty->edit_line_length++] = '\n';

        uint line_len = tty->edit_line_length;

        for (uint i = 0; i < line_len; i++) {
                write_input_stream(tty, tty->edit_line[i]);
        }

        while (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) != MUTEX_LOCKED);
        add_message(tty, tty->edit_line, line_len);
        tty->refresh_last_line = RESET;
        unlock_recursive_mutex(tty->secure_resources_mtx);

        memset(tty->edit_line, '\0', TTY_EDIT_LINE_LEN);
        tty->edit_line_length = 0;
        tty->cursor_position  = 0;

        if (tty->new_msg_counter == 0) {
                msg = "\r\n";
        } else {
                msg = "\r"VT100_ERASE_LINE_FROM_CUR;
        }

        fwrite(msg, sizeof(char), strlen(msg), stream);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
