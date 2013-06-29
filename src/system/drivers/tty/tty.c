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
#include "drivers/tty.h"
#include "drivers/uart_def.h"

MODULE_NAME(TTY);

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define INPUT_SERVICE_TASK_NAME                 "ttyin"
#define OUTPUT_SERVICE_TASK_NAME                "ttyout"
#define INPUT_SERVICE_TASK_STACK_DEPTH          STACK_DEPTH_VERY_LOW
#define OUTPUT_SERVICE_TASK_STACK_DEPTH         STACK_DEPTH_VERY_LOW
#define INPUT_SERVICE_TASK_PRIORITY             0
#define OUTPUT_SERVICE_TASK_PRIORITY            0

#define TTYFILE                                 "/dev/ttyS0"

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

#define DEFAULT_COL                             80
#define DEFAULT_ROW                             24

#define current_tty_handle()                    tty_ctrl->tty[tty_ctrl->current_TTY]

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/* independent TTY device */
struct tty_data {
        char    *line[TTY_MAX_LINES];
        u8_t     write_index;
        u8_t     new_msg_counter;
        flag_t   refresh_last_line;
        mutex_t *secure_resources_mtx;
        flag_t   echo_enabled;

        /* FIFO for keyboard read */
        struct input_stream {
                u16_t level;
                char  buffer[TTY_STREAM_SIZE];
                u16_t txidx;
                u16_t rxidx;
        } input;

        char   edit_line[TTY_EDIT_LINE_LEN + 1];
        uint   edit_line_length;
        uint   cursor_position;
        u32_t  file_size;
        sem_t *edit_line_read_sem;

        u8_t device_number;
};

/* memory structure */
struct tty_ctrl {
        struct tty_data *tty[TTY_DEV_COUNT];
        u8_t    current_TTY;
        i8_t    change_to_TTY;
        u8_t    column_count;
        u8_t    row_count;
        task_t *keyboard_task;
        task_t *screen_task;
        sem_t  *do_stdout_semcnt;       /* semaphore used to trigger daemon operation */

        bool    capture_enabled;        /* VT100 cmd capture enabled */
        u8_t    capture_buffer_idx;     /* VT100 cmd capture buffer index */
        char    capture_buffer[12];     /* VT100 cmd capture buffer */

        bool    tty_size_updated;

//        uint    task_delay;           /* task delay depended by user activity */
        FILE   *io_stream;              /* IO stream used to get and set data to terminal */
};

/* key detector results */
enum vt100cmd {
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
        TERMINAL_SIZE,
        END_OF_TEXT,
        END_OF_TRANSMISSION,
        NORMAL_KEY
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void          input_service_task                   (void *arg);
static void          output_service_task                  (void *arg);
static void          switch_tty_if_requested              (void);
static void          move_cursor_to_beginning_of_editline (struct tty_data *tty);
static void          move_cursor_to_end_of_editline       (struct tty_data *tty);
static void          remove_character_from_editline       (struct tty_data *tty);
static void          delete_character_from_editline       (struct tty_data *tty);
static void          add_charater_to_editline             (struct tty_data *tty, char chr);
static void          show_new_messages                    (struct tty_data *tty);
static void          refresh_last_line                    (struct tty_data *tty);
static void          switch_tty_to                        (int tty_number);
static void          clear_tty                            (struct tty_data *tty);
static char         *new_CRLF_message                     (const char *msg, uint msg_len);
static stdret_t      free_the_oldest_line                 (struct tty_data *tty);
static char         *merge_or_create_message              (struct tty_data *tty, const char *msg_src);
static void          add_message                          (struct tty_data *tty, const char *msg, uint msg_len);
static void          strncpy_LF2CRLF                      (char *dst, const char *src, uint n);
static void          link_message                         (struct tty_data *tty, char *msg);
static void          inc_message_counter                  (struct tty_data *tty);
static enum vt100cmd capture_VT100_commands               (char character);
static u8_t          get_message_index                    (struct tty_data *tty, u8_t back);
static void          refresh_tty                          (struct tty_data *tty);
static void          read_vt100_size                      (void);
static void          write_input_stream                   (struct tty_data *tty, char chr);
static stdret_t      read_input_stream                    (struct tty_data *tty, char *chr);
static void          move_editline_to_stream              (struct tty_data *tty);

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

        _stop_if(!drvhdl);

        if (dev >= TTY_DEV_COUNT) {
                return STD_RET_ERROR;
        }

        /* initialize control structure */
        if (!tty_ctrl) {
                if (!(tty_ctrl = calloc(1, sizeof(struct tty_ctrl)))) {
                        goto ctrl_error;
                }

                if (!(tty_ctrl->do_stdout_semcnt = new_counting_semaphore(256, 0))) {
                        goto ctrl_error;
                }

                if (!(tty_ctrl->io_stream = fopen(TTYFILE, "r+"))) {
                        goto ctrl_error;
                }

                if (!(tty_ctrl->keyboard_task = new_task(input_service_task, INPUT_SERVICE_TASK_NAME, INPUT_SERVICE_TASK_STACK_DEPTH, NULL))) {
                        goto ctrl_error;
                }

                if (!(tty_ctrl->screen_task = new_task(output_service_task, OUTPUT_SERVICE_TASK_NAME, OUTPUT_SERVICE_TASK_STACK_DEPTH, NULL))) {
                        goto ctrl_error;
                }

                tty_ctrl->column_count  = DEFAULT_COL;
                tty_ctrl->row_count     = DEFAULT_ROW;
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

        if (!(tty->edit_line_read_sem = new_counting_semaphore(DEFAULT_COL, 0))) {
                goto drv_error;
        }

        tty->echo_enabled  = SET;
        tty_ctrl->tty[dev] = tty;
        tty->device_number = dev;
        tty->file_size     = 1;
        *drvhdl            = tty;
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
                if (tty_ctrl->do_stdout_semcnt) {
                        delete_counting_semaphore(tty_ctrl->do_stdout_semcnt);
                }

                if (tty_ctrl->keyboard_task) {
                        delete_task(tty_ctrl->keyboard_task);
                }

                if (tty_ctrl->screen_task) {
                        delete_task(tty_ctrl->screen_task);
                }

                if (tty_ctrl->io_stream) {
                        fclose(tty_ctrl->io_stream);
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
        _stop_if(!drvhdl);
        _stop_if(!tty_ctrl);

        struct tty_data *tty = drvhdl;

        if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                clear_tty(tty);

                enter_critical_section();

                delete_counting_semaphore(tty_ctrl->do_stdout_semcnt);
                delete_task(tty_ctrl->keyboard_task);
                delete_task(tty_ctrl->screen_task);
                fclose(tty_ctrl->io_stream);

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
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t TTY_open(void *drvhdl)
{
        _stop_if(!drvhdl);
        _stop_if(!tty_ctrl);

        return STD_RET_OK;
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
        _stop_if(!drvhdl);
        _stop_if(!tty_ctrl);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Write data to TTY
 *
 * @param[in] *drvhdl           driver's memory handle
 * @param[in] *src              source
 * @param[in] size              size
 * @param[in] lseek             seek
 *
 * @retval number of written nitems
 */
//==============================================================================
size_t TTY_write(void *drvhdl, const void *src, size_t size, size_t nitems, u64_t lseek)
{
        (void)lseek;

        _stop_if(!drvhdl);
        _stop_if(!src);
        _stop_if(!size);
        _stop_if(!nitems);
        _stop_if(!tty_ctrl);

        struct tty_data *tty = drvhdl;

        /* if current TTY is showing wait to show refreshed line */
        while (  tty_ctrl->tty[tty_ctrl->current_TTY]->refresh_last_line
              && tty_ctrl->current_TTY == tty->device_number) {

                give_counting_semaphore(tty_ctrl->do_stdout_semcnt);
                sleep_ms(10);
        }

        /* wait for secure access to data */
        size_t n = 0;
        if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                /* check if screen is cleared */
                if (strncmp(VT100_CLEAR_SCREEN, src, 4) == 0) {
                        clear_tty(tty);
                }

                add_message(tty, src, nitems);

                n = nitems;
                tty->file_size += n;

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
 * @param[in]   size            size
 * @param[in]   lseek           seek
 *
 * @retval number of read nitems
 */
//==============================================================================
size_t TTY_read(void *drvhdl, void *dst, size_t size, size_t nitems, u64_t lseek)
{
        (void)lseek;

        _stop_if(!drvhdl);
        _stop_if(!dst);
        _stop_if(!size);
        _stop_if(!nitems);
        _stop_if(!tty_ctrl);

        struct tty_data *tty = drvhdl;

        size_t n = 0;
        while (nitems > 0) {
                if (read_input_stream(tty, dst) != STD_RET_OK) {
                        break;
                }

                nitems--;
                n++;

                const char chr = *(char *)dst;
                if (chr == '\n') {
                        break;
                }

                dst += size;
        }

        n /= size;
        tty->file_size += n;

        return n;
}

//==============================================================================
/**
 * @brief Specific settings of TTY
 *
 * @param[in]     *drvhdl       driver's memory handle
 * @param[in]     ioRq          IO reqest
 * @param[in,out] args          additional arguments
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t TTY_ioctl(void *drvhdl, int iorq, va_list args)
{
        _stop_if(!drvhdl);
        _stop_if(!tty_ctrl);

        struct tty_data *tty = drvhdl;
        int *out_ptr;

        switch (iorq) {
        /* return current TTY */
        case TTY_IORQ_GET_CURRENT_TTY:
                out_ptr = va_arg(args, int*);
                if (out_ptr == NULL) {
                        return STD_RET_ERROR;
                }
                *out_ptr = tty_ctrl->current_TTY;
                break;

        /* set active terminal */
        case TTY_IORQ_SWITCH_TTY_TO:
                switch_tty_to(va_arg(args, int));
                break;

        /* clear terminal */
        case TTY_IORQ_CLEAN_TTY:
                clear_tty(tty);
                break;

        /* terminal size - number of columns */
        case TTY_IORQ_GET_COL:
                out_ptr = va_arg(args, int*);
                if (out_ptr == NULL) {
                        return STD_RET_ERROR;
                }
                *out_ptr = tty_ctrl->column_count;
                break;

        /* terminal size - number of rows */
        case TTY_IORQ_GET_ROW:
                out_ptr = va_arg(args, int*);
                if (out_ptr == NULL) {
                        return STD_RET_ERROR;
                }
                *out_ptr = tty_ctrl->row_count;
                break;

        /* clear screen */
        case TTY_IORQ_CLEAR_SCR:
                clear_tty(tty);
                tty_ctrl->change_to_TTY = tty_ctrl->current_TTY;
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
 * @brief Function flush device
 *
 * @param[in] *drvhdl           driver's memory handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t TTY_flush(void *drvhdl)
{
        _stop_if(!drvhdl);
        _stop_if(!tty_ctrl);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function returns device informations
 *
 * @param[in]  *drvhld          driver's memory handle
 * @param[out] *info            device/file info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t TTY_info(void *drvhdl, struct vfs_dev_info *info)
{
        _stop_if(!drvhdl);
        _stop_if(!info);
        _stop_if(!tty_ctrl);

        struct tty_data *tty = drvhdl;

        info->st_size = tty->file_size;
        return STD_RET_OK;
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
                if (fread(&chr, sizeof(char), 1, tty_ctrl->io_stream) == 0) {
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

                case NORMAL_KEY:
                        switch (chr) {
                        case '\r':
                        case '\n': move_editline_to_stream(tty);        break;
                        case '\b': remove_character_from_editline(tty); break;
                        default  : add_charater_to_editline(tty, chr);  break;
                        }
                        break;

                case ARROW_LEFT_KEY:
                        if (tty->cursor_position > 0) {
                                fwrite("\b", sizeof(char), 1, tty_ctrl->io_stream);
                                tty->cursor_position--;
                        }
                        break;

                case ARROW_RIGHT_KEY:
                        if (tty->cursor_position < tty->edit_line_length) {
                                const char *msg = VT100_SHIFT_CURSOR_RIGHT(1);
                                fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);
                                tty->cursor_position++;
                        }
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

        const char *msg = VT100_RESET_ATTRIBUTES
                          VT100_CLEAR_SCREEN
                          VT100_DISABLE_LINE_WRAP
                          VT100_CURSOR_HOME;

        fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);

        read_vt100_size();

        for (;;) {
                if (current_tty_handle() == NULL) {
                        sleep_ms(100);
                        continue;
                }

                if (take_counting_semaphore(tty_ctrl->do_stdout_semcnt, MAX_DELAY) == OS_OK) {
                        struct tty_data *tty = current_tty_handle();

                        if (tty->new_msg_counter > 0) {
                                show_new_messages(tty);

                        } else if (tty->refresh_last_line == SET) {
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
        char *msg = VT100_CURSOR_OFF;
        fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);

        while (tty->cursor_position > 0) {
                fwrite("\b", sizeof(char), 1, tty_ctrl->io_stream);
                tty->cursor_position--;
        }

        msg = VT100_CURSOR_ON;
        fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);
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
        char *msg = VT100_CURSOR_OFF;
        fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);

        while (tty->cursor_position < tty->edit_line_length) {
                char *msg = VT100_SHIFT_CURSOR_RIGHT(1);
                fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);
                tty->cursor_position++;
        }

        msg = VT100_CURSOR_ON;
        fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);
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
        if (tty->cursor_position == 0 || tty->edit_line_length == 0) {
                return;
        }

        tty->cursor_position--;

        for (uint i = tty->cursor_position; i < tty->edit_line_length; i++) {
                tty->edit_line[i] = tty->edit_line[i + 1];
        }

        tty->edit_line_length--;

        char *msg = "\b"VT100_ERASE_LINE_FROM_CUR VT100_SAVE_CURSOR_POSITION;
        fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);

        msg = &tty->edit_line[tty->cursor_position];
        fwrite(msg, sizeof(char), tty->edit_line_length - tty->cursor_position, tty_ctrl->io_stream);

        msg = VT100_RESTORE_CURSOR_POSITION;
        fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);
}

//==============================================================================
/**
 * @brief Function delete right character from edit line
 *
 * @param *tty                  terminal data
 */
//==============================================================================
static void delete_character_from_editline(struct tty_data *tty)
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
        fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);

        msg = &tty->edit_line[tty->cursor_position];
        fwrite(msg, sizeof(char), tty->edit_line_length - tty->cursor_position, tty_ctrl->io_stream);

        msg = VT100_RESTORE_CURSOR_POSITION;
        fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);
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
        if (tty->edit_line_length >= TTY_EDIT_LINE_LEN - 1) {
                return;
        }

        if (tty->cursor_position < tty->edit_line_length) {
                for (uint i = tty->edit_line_length; i > tty->cursor_position; i--) {
                        tty->edit_line[i] = tty->edit_line[i - 1];
                }

                tty->edit_line[tty->cursor_position++] = chr;
                tty->edit_line_length++;

                const char *msg = VT100_SAVE_CURSOR_POSITION;
                fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);

                msg = &tty->edit_line[tty->cursor_position - 1];
                fwrite(msg, sizeof(char), tty->edit_line_length - (tty->cursor_position - 1), tty_ctrl->io_stream);

                msg = VT100_RESTORE_CURSOR_POSITION VT100_SHIFT_CURSOR_RIGHT(1);
                fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);
        } else {
                tty->edit_line[tty->cursor_position++] = chr;
                tty->edit_line_length++;

                if (tty->echo_enabled == SET) {
                        fwrite(&chr, sizeof(char), 1, tty_ctrl->io_stream);
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
                tty_ctrl->current_TTY   = tty_ctrl->change_to_TTY;
                tty_ctrl->change_to_TTY = -1;
                refresh_tty(current_tty_handle());
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
                give_counting_semaphore(tty_ctrl->do_stdout_semcnt);
        }
}

//==============================================================================
/**
 * @brief Function show new messages
 *
 * @param *tty          terminal data
 */
//==============================================================================
static void show_new_messages(struct tty_data *tty)
{
        if (lock_recursive_mutex(tty->secure_resources_mtx, MAX_DELAY) == MUTEX_LOCKED) {

                while (tty->new_msg_counter) {
                        if (tty->new_msg_counter > tty_ctrl->row_count) {
                                tty->new_msg_counter = tty_ctrl->row_count;
                        }

                        const char *msg = tty->line[get_message_index(tty, tty->new_msg_counter)];

                        tty->new_msg_counter--;

                        if (msg) {
                                fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);
                        }
                }

                tty->refresh_last_line = RESET;

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

                const char *msg = VT100_CURSOR_OFF
                                  VT100_CARRIAGE_RETURN
                                  VT100_ERASE_LINE_FROM_CUR
                                  VT100_RESET_ATTRIBUTES;

                fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);

                /* refresh line */
                msg = tty->line[get_message_index(tty, 1)];
                fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);

                /* cursor on */
                msg = VT100_CURSOR_ON;
                fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);

                tty->refresh_last_line = RESET;

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }
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

                give_counting_semaphore(tty_ctrl->do_stdout_semcnt);

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }
}

//==============================================================================
/**
 * @brief Convert \n to \r\n in the message
 *
 * @param[in] *msg      message data
 * @param[in]  msg_len  message length
 *
 * @return pointer to corrected message
 */
//==============================================================================
static char *new_CRLF_message(const char *msg, uint msg_len)
{
        /* calculate how many '\n' exist in string */
        uint LF_count = 0;
        for (uint i = 0; i < msg_len; i++) {
                if (msg[i] == '\n') {
                        LF_count++;
                }
        }

        char *new_msg = malloc(msg_len + (2 * LF_count) + 1);
        if (new_msg) {
                strncpy_LF2CRLF(new_msg, msg, msg_len + (2 * LF_count) + 1);
        }

        return new_msg;
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
                int line_index = get_message_index(tty, i);
                if (tty->line[line_index]) {
                        free(tty->line[line_index]);
                        tty->line[line_index] = NULL;
                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function create new buffer for new message or merge new message with
 *        latest
 *
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
static char *merge_or_create_message(struct tty_data *tty, const char *msg_src)
{
        char   *msg         = NULL;
        char   *lst_msg     = tty->line[get_message_index(tty, 1)];
        size_t  lst_msg_len = strlen(lst_msg);

        if (lst_msg && (*(lst_msg + lst_msg_len - 1) != '\n')) {
                lst_msg_len += 1;

                if (msg_src[0] == '\r' && strncmp(msg_src, "\r\n", 2)) {
                        msg = malloc(strlen(msg_src + 1) + 1);
                } else {
                        msg = malloc(lst_msg_len + strlen(msg_src) + 1);
                }

                if (msg) {
                        if (tty->write_index == 0)
                                tty->write_index = TTY_MAX_LINES - 1;
                        else
                                tty->write_index--;

                        if (tty->new_msg_counter == 0) {
                                tty->refresh_last_line = SET;
                        }

                        if (msg_src[0] == '\r' && strncmp(msg_src, "\r\n", 2)) {
                                strcpy(msg, msg_src + 1);
                        } else {
                                strcpy(msg, lst_msg);
                                strcat(msg, msg_src);
                        }
                }
        } else {
                msg = malloc(strlen(msg_src) + 1);
                if (msg) {
                        strcpy(msg, msg_src);
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
 * @brief Function link prepared message to TTY
 *
 * @param *tty          terminal address
 * @param *msg          message
 */
//==============================================================================
static void link_message(struct tty_data *tty, char *msg)
{
        if (tty->line[tty->write_index]) {
                free(tty->line[tty->write_index]);
        }

        tty->line[tty->write_index++] = msg;

        if (tty->write_index >= TTY_MAX_LINES) {
                tty->write_index = 0;
        }
}

//==============================================================================
/**
 * @brief Add new message or modify existing
 *
 * @param *tty          terminal address
 * @param *msg          message
 * @param  msgLen       message length
 */
//==============================================================================
static void add_message(struct tty_data *tty, const char *msg, uint msg_len)
{
        if (!msg_len || !msg) {
                return;
        }

        char *crlf_msg;
        while (!(crlf_msg = new_CRLF_message(msg, msg_len))) {
                if (free_the_oldest_line(tty) == STD_RET_ERROR) {
                        break;
                }
        }

        if (crlf_msg) {
                char *new_msg = merge_or_create_message(tty, crlf_msg);
                link_message(tty, new_msg);
                free(crlf_msg);
        }

        give_counting_semaphore(tty_ctrl->do_stdout_semcnt);
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
 * @return decoded command
 */
//==============================================================================
static enum vt100cmd capture_VT100_commands(char character)
{
        enum vt100cmd vt100cmd = KEY_CAPTURE_PENDING;

        if (strchr("cnRPQSDCF~", character) != NULL && tty_ctrl->capture_enabled == true) {
                tty_ctrl->capture_enabled = false;

                if (  tty_ctrl->capture_buffer[tty_ctrl->capture_buffer_idx - 1] == 'O'
                   || tty_ctrl->capture_buffer[tty_ctrl->capture_buffer_idx - 1] == '[') {
                        switch (character) {
                        case 'P': vt100cmd = F1_KEY; break;
                        case 'Q': vt100cmd = F2_KEY; break;
                        case 'R': vt100cmd = F3_KEY; break;
                        case 'S': vt100cmd = F4_KEY; break;
                        case 'D': vt100cmd = ARROW_LEFT_KEY; break;
                        case 'C': vt100cmd = ARROW_RIGHT_KEY; break;
                        case 'F': vt100cmd = END_KEY; break;
                        }
                } else {
                        switch (character) {
                        /* calculate terminal size */
                        case 'R': {
                                const char *data = strchr(tty_ctrl->capture_buffer, '[');
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
                                switch (tty_ctrl->capture_buffer[tty_ctrl->capture_buffer_idx - 1]) {
                                case '1': vt100cmd = HOME_KEY; break;
                                case '3': vt100cmd = DEL_KEY ; break;
                                }

                                break;
                        }
                }

                memset(tty_ctrl->capture_buffer, 0, sizeof(tty_ctrl->capture_buffer));
        } else {
                if (character == '\e') {
                        tty_ctrl->capture_enabled    = true;
                        tty_ctrl->capture_buffer_idx = 0;
                }

                if (tty_ctrl->capture_enabled == false) {
                        switch (character) {
                        case 0x03: vt100cmd = END_OF_TEXT; break;
                        case 0x04: vt100cmd = END_OF_TRANSMISSION; break;
                        default  : vt100cmd = NORMAL_KEY; break;
                        }
                } else {
                        if (tty_ctrl->capture_buffer_idx < sizeof(tty_ctrl->capture_buffer))
                                tty_ctrl->capture_buffer[tty_ctrl->capture_buffer_idx++] = character;
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

        const char *msg = VT100_RESET_ATTRIBUTES
                          VT100_CLEAR_SCREEN
                          VT100_DISABLE_LINE_WRAP
                          VT100_CURSOR_HOME;

        fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);

        if (lock_recursive_mutex(tty->secure_resources_mtx, BLOCK_TIME) == MUTEX_LOCKED) {
                int rows;

                if (tty_ctrl->row_count < TTY_MAX_LINES) {
                        rows = tty_ctrl->row_count;
                } else {
                        rows = TTY_MAX_LINES;
                }

                for (int i = rows - 1; i > 0; i--) {
                        msg = tty->line[get_message_index(tty, i)];

                        if (msg) {
                                fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);
                        }
                }

                msg = tty->edit_line;
                fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);

                tty->new_msg_counter = 0;

                unlock_recursive_mutex(tty->secure_resources_mtx);
        }
}

//==============================================================================
/**
 * @brief Function gets terminal size and write value to configuration
 */
//==============================================================================
static void read_vt100_size(void)
{
        tty_ctrl->column_count = DEFAULT_COL;
        tty_ctrl->row_count    = DEFAULT_ROW;

#if (TTY_TERM_SIZE_CHECK != 0)
        /*
         * Function send request to the terminal to get the termnal's size.
         * Response of this request will be received by input handler.
         */

        const char *msg = VT100_SAVE_CURSOR_POSITION
                          VT100_CURSOR_OFF
                          VT100_SET_CURSOR_POSITION(250, 250)
                          VT100_QUERY_CURSOR_POSITION;
        fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);

        /* waiting for response from input function */
        tty_ctrl->tty_size_updated = false;
        while (tty_ctrl->tty_size_updated == false) {
                sleep_ms(5);
        }

        /* restore cursor position, cursor on */
        msg = VT100_RESTORE_CURSOR_POSITION
              VT100_CURSOR_ON;
        fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);
#endif
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

                give_counting_semaphore(tty->edit_line_read_sem);

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
        if (take_counting_semaphore(tty->edit_line_read_sem, MAX_DELAY) == OS_OK) {

                if (lock_recursive_mutex(tty->secure_resources_mtx, MAX_DELAY) == MUTEX_LOCKED) {
                        *chr = tty->input.buffer[tty->input.rxidx++];

                        if (tty->input.rxidx >= TTY_STREAM_SIZE) {
                                tty->input.rxidx = 0;
                        }

                        tty->input.level--;

                        unlock_recursive_mutex(tty->secure_resources_mtx);
                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function move edit line to stream
 *
 * @param[in] *tty              terminal address
 */
//==============================================================================
static void move_editline_to_stream(struct tty_data *tty)
{
        tty->edit_line[tty->edit_line_length++] = '\n';

        uint line_len = tty->edit_line_length;

        for (uint i = 0; i < line_len; i++) {
                write_input_stream(tty, tty->edit_line[i]);
        }

        if (lock_recursive_mutex(tty->secure_resources_mtx, MAX_DELAY) == MUTEX_LOCKED) {
                add_message(tty, tty->edit_line, line_len);
                tty->refresh_last_line = RESET;
                unlock_recursive_mutex(tty->secure_resources_mtx);
        }

        memset(tty->edit_line, '\0', TTY_EDIT_LINE_LEN);
        tty->edit_line_length = 0;
        tty->cursor_position  = 0;

        const char *msg;
        if (tty->new_msg_counter == 0) {
                msg = "\r\n";
        } else {
                msg = "\r"VT100_ERASE_LINE_FROM_CUR;
        }

        fwrite(msg, sizeof(char), strlen(msg), tty_ctrl->io_stream);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
