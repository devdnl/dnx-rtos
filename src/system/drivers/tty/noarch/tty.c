/*=========================================================================*//**
@file    tty.c

@author  Daniel Zorychta

@brief   This file support virtual terminal.

@note    Copyright (C) 2012, 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "tty_cfg.h"
#include "tty.h"
#include "../tty_ioctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
enum cmd {
        CMD_INPUT,
        CMD_SWITCH_TTY,
        CMD_CLEAR_TTY,
        CMD_LINE_ADDED,
        CMD_REFRESH_LAST_LINE
};

typedef struct {
        enum cmd        cmd : 8;
        u8_t            arg;
} tty_cmd_t;

typedef struct {
       queue_t         *queue_out;
       mutex_t         *secure_mtx;
       ttybfr_t        *screen;
       ttyedit_t       *editline;
       ttycmd_t        *vtcmd;
       bool             flushed;
       u8_t             major;
} tty_t;

struct module {
        FILE           *infile;
        FILE           *outfile;
        queue_t        *queue_cmd;
        tty_t          *tty[_TTY_NUMBER_OF_VT];
        tid_t           service_out;
        tid_t           service_in;
        int             current_tty;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void     service_out             (void *arg);
static void     service_in              (void *arg);
static void     send_cmd                (enum cmd cmd, u8_t arg);
static void     vt100_init              ();
static void     vt100_analyze           (const char c);
static void     copy_string_to_queue    (const char *str, queue_t *queue, bool lfend, uint timeout);
static void     switch_terminal         (int term_no);
static void     handle_new_line         (tty_t *tty);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(TTY);

static struct module *tty_module;

static const int    SERVICE_IN_PRIORITY  = PRIORITY_NORMAL;
static const int    SERVICE_OUT_PRIORITY = PRIORITY_NORMAL;
static const size_t QUEUE_CMD_LEN        = _TTY_TERMINAL_ROWS;

static const thread_attr_t SERVICE_IN_ATTR = {
        .stack_depth = STACK_DEPTH_LOW,
        .priority    = PRIORITY_NORMAL,
        .detached    = true
};

static const thread_attr_t SERVICE_OUT_ATTR = {
        .stack_depth = STACK_DEPTH_LOW,
        .priority    = PRIORITY_NORMAL,
        .detached    = true
};

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(TTY, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG1(minor);

        int err = ENODEV;

        if (major >= _TTY_NUMBER_OF_VT || minor != 0) {
                return err;
        }

        /* initialize module base */
        if (!tty_module) {
                err = sys_zalloc(sizeof(struct module), cast(void**, &tty_module));
                if (err != ESUCC)
                        return err;

                err = sys_fopen(_TTY_IN_FILE, "r", &tty_module->infile);
                if (err != ESUCC)
                        goto module_alloc_finish;

                err = sys_fopen(_TTY_OUT_FILE, "w", &tty_module->outfile);
                if (err != ESUCC)
                        goto module_alloc_finish;

                err = sys_thread_create(service_in, &SERVICE_IN_ATTR, NULL, &tty_module->service_in);
                if (err != ESUCC)
                        goto module_alloc_finish;

                err = sys_thread_create(service_out, &SERVICE_OUT_ATTR, NULL, &tty_module->service_out);
                if (err != ESUCC)
                        goto module_alloc_finish;

                err = sys_queue_create(QUEUE_CMD_LEN, sizeof(tty_cmd_t), &tty_module->queue_cmd);

                module_alloc_finish:
                if (err != ESUCC) {
                        if (tty_module->infile)
                                sys_fclose(tty_module->infile);

                        if (tty_module->outfile)
                                sys_fclose(tty_module->outfile);

                        if (sys_thread_is_valid(tty_module->service_in))
                                sys_thread_destroy(tty_module->service_in);

                        if (sys_thread_is_valid(tty_module->service_out))
                                sys_thread_destroy(tty_module->service_out);

                        if (tty_module->queue_cmd)
                                sys_queue_destroy(tty_module->queue_cmd);

                        if (tty_module)
                                sys_free(cast(void**, &tty_module));
                }
        } else {
                err = ESUCC;
        }

        /* initialize selected TTY */
        if (err == ESUCC) {
                err = sys_zalloc(sizeof(tty_t), device_handle);
                if (err != ESUCC)
                        return err;

                tty_t *tty = *device_handle;

                err = sys_queue_create(_TTY_STREAM_SIZE, sizeof(char), &tty->queue_out);
                if (err != ESUCC)
                        goto tty_alloc_finish;

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &tty->secure_mtx);
                if (err != ESUCC)
                        goto tty_alloc_finish;

                err = ttybfr_create(&tty->screen);
                if (err != ESUCC)
                        goto tty_alloc_finish;

                err = ttyedit_create(tty_module->outfile, &tty->editline);
                if (err != ESUCC)
                        goto tty_alloc_finish;

                err = ttycmd_create(&tty->vtcmd);
                if (err != ESUCC)
                        goto tty_alloc_finish;

                tty->major             = major;
                tty_module->tty[major] = tty;

                tty_alloc_finish:
                if (err != ESUCC) {
                        if (tty->vtcmd)
                                ttycmd_destroy(tty->vtcmd);

                        if (tty->editline)
                                ttyedit_destroy(tty->editline);

                        if (tty->screen)
                                ttybfr_destroy(tty->screen);

                        if (tty->secure_mtx)
                                sys_mutex_destroy(tty->secure_mtx);

                        if (tty->queue_out)
                                sys_queue_destroy(tty->queue_out);

                        if (device_handle)
                                sys_free(device_handle);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_RELEASE(TTY, void *device_handle)
{
        tty_t *tty = device_handle;

        int err = sys_mutex_trylock(tty->secure_mtx);
        if (!err) {
                sys_mutex_destroy(tty->secure_mtx);
                sys_queue_destroy(tty->queue_out);
                ttybfr_destroy(tty->screen);
                ttyedit_destroy(tty->editline);
                ttycmd_destroy(tty->vtcmd);
                tty_module->tty[tty->major] = NULL;
                sys_free(&device_handle);

                /* de-initialize entire module if all TTYs are released */
                bool release_TTY = true;
                for (int i = 0; i < _TTY_NUMBER_OF_VT && release_TTY; i++) {
                        release_TTY = !tty_module->tty[i];
                }

                if (release_TTY) {
                        sys_thread_destroy(tty_module->service_in);
                        sys_thread_destroy(tty_module->service_out);
                        sys_fclose(tty_module->infile);
                        sys_fclose(tty_module->outfile);
                        sys_queue_destroy(tty_module->queue_cmd);
                        sys_free(cast(void**, &tty_module));
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_OPEN(TTY, void *device_handle, u32_t flags)
{
        UNUSED_ARG2(device_handle, flags);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_CLOSE(TTY, void *device_handle, bool force)
{
        UNUSED_ARG2(device_handle, force);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Write data to device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_WRITE(TTY,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG2(fpos, fattr);

        tty_t *tty = device_handle;

        int err = sys_mutex_lock(tty->secure_mtx, MAX_DELAY_MS);
        if (err == ESUCC) {
                ttybfr_put(tty->screen, cast(const char *, src), count);
                sys_mutex_unlock(tty->secure_mtx);
                send_cmd(CMD_LINE_ADDED, tty->major);

                *wrcnt = count;
        }

        return err;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_READ(TTY,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        tty_t *tty = device_handle;

        *rdcnt = 0;

        while (count--) {
                if (fattr.non_blocking_rd) {
                        if (sys_mutex_lock(tty->secure_mtx, MAX_DELAY_MS) == ESUCC) {
                                const char *str = ttyedit_get_value(tty->editline);
                                copy_string_to_queue(str, tty->queue_out, false, MAX_DELAY_MS);
                                ttyedit_clear(tty->editline);
                                sys_mutex_unlock(tty->secure_mtx);
                        } else {
                                return ETIME;
                        }
                }

                if (sys_queue_receive(tty->queue_out, dst, fattr.non_blocking_rd ? 0 : MAX_DELAY_MS) == ESUCC) {
                        (*rdcnt)++;

                        if (*dst == '\n')
                                break;

                        dst++;
                }
        }

        *fpos = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_IOCTL(TTY, void *device_handle, int request, void *arg)
{
        tty_t *tty = device_handle;
        int    err = EINVAL;

        switch (request) {
        case IOCTL_TTY__GET_CURRENT_TTY:
                if (arg) {
                        *cast(int*, arg) = tty_module->current_tty;
                        err = ESUCC;
                }
                break;

        case IOCTL_TTY__GET_COL:
                if (arg) {
                        *cast(int*, arg) = _TTY_TERMINAL_COLUMNS;
                        err = ESUCC;
                }
                break;

        case IOCTL_TTY__GET_ROW:
                if (arg) {
                        *cast(int*, arg) = _TTY_TERMINAL_ROWS;
                        err = ESUCC;
                }
                break;

        case IOCTL_TTY__SET_EDITLINE:
                if (arg) {
                        err = sys_mutex_lock(tty->secure_mtx, MAX_DELAY_MS);
                        if (err == ESUCC) {
                                ttyedit_set_value(tty->editline, arg, tty_module->current_tty == tty->major);
                                sys_mutex_unlock(tty->secure_mtx);
                        }
                }
                break;

        case IOCTL_TTY__SWITCH_TTY_TO:
                send_cmd(CMD_SWITCH_TTY, *cast(int*, arg));
                err = ESUCC;
                break;

        case IOCTL_TTY__CLEAR_SCR:
                send_cmd(CMD_CLEAR_TTY, tty->major);
                err = ESUCC;
                break;

        case IOCTL_TTY__ECHO_ON:
                ttyedit_enable_echo(tty->editline);
                err = ESUCC;
                break;

        case IOCTL_TTY__ECHO_OFF:
                ttyedit_disable_echo(tty->editline);
                err = ESUCC;
                break;

        case IOCTL_TTY__GET_NUMBER_OF_TTYS:
                if (arg) {
                        *cast(int*, arg) = _TTY_NUMBER_OF_VT;
                }
                break;

        case IOCTL_TTY__REFRESH_LAST_LINE:
                send_cmd(CMD_REFRESH_LAST_LINE, tty->major);
                err = ESUCC;
                break;

        default:
                err = EBADRQC;
                break;
        }

        return err;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_FLUSH(TTY, void *device_handle)
{
        tty_t *tty = device_handle;

        int err = sys_mutex_lock(tty->secure_mtx, MAX_DELAY_MS);
        if (err == ESUCC) {
                ttybfr_flush(tty->screen);
                send_cmd(CMD_REFRESH_LAST_LINE, tty->major);
                sys_mutex_unlock(tty->secure_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_STAT(TTY, void *device_handle, struct vfs_dev_stat *device_stat)
{
        tty_t *tty = device_handle;

        device_stat->st_size  = 0;
        device_stat->st_major = tty->major;
        device_stat->st_minor = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief TTY input service (helper task)
 */
//==============================================================================
static void service_in(void *arg)
{
        UNUSED_ARG1(arg);

        bool last_CR = false;

        sys_thread_set_priority(SERVICE_IN_PRIORITY);

        for (;;) {
                char c = '\0'; size_t rdcnt;
                if (sys_fread(&c, 1, &rdcnt, tty_module->infile) == ESUCC) {
                        /*
                         * Standard TTY protocol is to sent a CR from the keyboard
                         * when the user has finished typing and for the computer
                         * to echo LF when it's ready for the next line.
                         */
                        if (last_CR && c == '\n') continue;

                        last_CR = (c == '\r');

                        sys_thread_set_priority(PRIORITY_HIGHEST);
                        send_cmd(CMD_INPUT, c);
                        sys_thread_set_priority(SERVICE_IN_PRIORITY);
                }
        }
}

//==============================================================================
/**
 * @brief TTY output service (main task)
 */
//==============================================================================
static void service_out(void *arg)
{
        UNUSED_ARG1(arg);

        sys_thread_set_priority(SERVICE_OUT_PRIORITY);

        vt100_init();

        for (;;) {
                tty_cmd_t rq;

                if (sys_queue_receive(tty_module->queue_cmd, &rq, MAX_DELAY_MS) == ESUCC) {
                        switch (rq.cmd) {
                        case CMD_INPUT: {
                                vt100_analyze(rq.arg);
                                break;
                        }

                        case CMD_CLEAR_TTY: {
                                if (rq.arg < _TTY_NUMBER_OF_VT && tty_module->tty[rq.arg]) {
                                        tty_t *tty = tty_module->tty[rq.arg];

                                        if (sys_mutex_lock(tty->secure_mtx, 100) == ESUCC) {
                                                ttybfr_clear(tty->screen);

                                                if (tty_module->current_tty == tty->major) {
                                                        vt100_init();
                                                }

                                                sys_mutex_unlock(tty->secure_mtx);
                                        }
                                }
                                break;
                        }

                        case CMD_SWITCH_TTY: {
                                switch_terminal(rq.arg);
                                break;
                        }

                        case CMD_LINE_ADDED: {
                                if (rq.arg < _TTY_NUMBER_OF_VT && tty_module->tty[rq.arg] && rq.arg == tty_module->current_tty) {
                                        tty_t *tty = tty_module->tty[rq.arg];

                                        if (sys_mutex_lock(tty->secure_mtx, 100) == ESUCC) {
                                                const char *str;
                                                while ((str = ttybfr_get_fresh_line(tty->screen))) {
                                                        size_t wrcnt;

                                                        if (tty->flushed) {
                                                                sys_fwrite(VT100_CLEAR_LINE,
                                                                           strlen(VT100_CLEAR_LINE),
                                                                           &wrcnt,
                                                                           tty_module->outfile);

                                                                tty->flushed = false;
                                                        }

                                                        sys_fwrite(str,
                                                                   strlen(str),
                                                                   &wrcnt,
                                                                   tty_module->outfile);
                                                }

                                                sys_mutex_unlock(tty->secure_mtx);
                                        }
                                }
                                break;
                        }

                        case CMD_REFRESH_LAST_LINE: {
                                tty_t *tty = tty_module->tty[rq.arg];

                                if (  rq.arg == tty_module->current_tty
                                   && sys_mutex_lock(tty->secure_mtx, MAX_DELAY_MS) == ESUCC) {
                                        size_t wrcnt;

                                        sys_fwrite(VT100_CLEAR_LINE,
                                                   strlen(VT100_CLEAR_LINE),
                                                   &wrcnt,
                                                   tty_module->outfile);

                                        const char *last_line = ttybfr_get_line(tty->screen, 0);
                                        sys_fwrite(last_line,
                                                   strlen(last_line),
                                                   &wrcnt,
                                                   tty_module->outfile);

                                        const char *editline = ttyedit_get_value(tty->editline);
                                        sys_fwrite(editline,
                                                   strlen(editline),
                                                   &wrcnt,
                                                   tty_module->outfile);

                                        tty->flushed = true;

                                        sys_mutex_unlock(tty->secure_mtx);
                                }
                                break;
                        }

                        default:
                                break;
                        }
                }
        }
}

//==============================================================================
/**
 * @brief Send command to main service
 *
 * @param cmd           a command
 * @param arg           an argument
 */
//==============================================================================
static void send_cmd(enum cmd cmd, u8_t arg)
{
        tty_cmd_t rq;
        rq.cmd = cmd;
        rq.arg = arg;

        sys_queue_send(tty_module->queue_cmd, &rq, MAX_DELAY_MS);
}

//==============================================================================
/**
 * @brief Configure VT100 terminal
 */
//==============================================================================
static void vt100_init()
{
        const char *cmd = VT100_RESET_ATTRIBUTES
                        #if _TTY_CLR_SCR_AT_INIT == _YES_
                          VT100_CLEAR_SCREEN
                          VT100_CURSOR_HOME
                        #endif
                          VT100_DISABLE_LINE_WRAP;

        size_t wrcnt;
        sys_fwrite(cmd, strlen(cmd), &wrcnt, tty_module->outfile);
}

//==============================================================================
/**
 * @brief Control analyzis of VT100 input stream
 *
 * @param c             input character
 */
//==============================================================================
static void vt100_analyze(const char c)
{
        tty_t *tty = tty_module->tty[tty_module->current_tty];
        if (!tty)
                return;

        ttycmd_resp_t resp = ttycmd_analyze(tty->vtcmd, c);
        switch (resp) {
        case TTYCMD_KEY_ENTER:
                handle_new_line(tty);
                break;

        case TTYCMD_KEY_CHAR:
                ttyedit_insert_char(tty->editline, c);
                break;

        case TTYCMD_KEY_ENDTEXT:
                ttyedit_insert_char(tty->editline, c);
                handle_new_line(tty);
                break;

        case TTYCMD_KEY_BACKSPACE:
                ttyedit_remove_char(tty->editline);
                break;

        case TTYCMD_KEY_DELETE:
                ttyedit_delete_char(tty->editline);
                break;

        case TTYCMD_KEY_ARROW_LEFT:
                ttyedit_move_cursor_left(tty->editline);
                break;

        case TTYCMD_KEY_ARROW_RIGHT:
                ttyedit_move_cursor_right(tty->editline);
                break;

        case TTYCMD_KEY_ARROW_UP:
                copy_string_to_queue(VT100_ARROW_UP_STDOUT, tty->queue_out, true, 0);
                break;

        case TTYCMD_KEY_ARROW_DOWN:
                copy_string_to_queue(VT100_ARROW_DOWN_STDOUT, tty->queue_out, true, 0);
                break;

        case TTYCMD_KEY_TAB:
                copy_string_to_queue(ttyedit_get_value(tty->editline), tty->queue_out, false, 0);
                copy_string_to_queue(VT100_TAB, tty->queue_out, true, 0);
                break;

        case TTYCMD_KEY_HOME:
                ttyedit_move_cursor_home(tty->editline);
                break;

        case TTYCMD_KEY_END:
                ttyedit_move_cursor_end(tty->editline);
                break;

        case TTYCMD_KEY_F1...TTYCMD_KEY_F12:
                switch_terminal(resp - TTYCMD_KEY_F1);
                break;

        default:
                break;
        }
}

//==============================================================================
/**
 * @brief Copy string to queue
 *
 * @param str           string
 * @param queue         queue
 * @param lfend         true: adds LF, false: without LF
 * @param timeout       operation timeout [ms]
 */
//==============================================================================
static void copy_string_to_queue(const char *str, queue_t *queue, bool lfend, uint timeout)
{
        for (uint i = 0; i < strlen(str); i++) {
                if (sys_queue_send(queue, &str[i], timeout) != ESUCC) {
                        break;
                }
        }

        if (lfend) {
                const char lf = '\n';
                sys_queue_send(queue, &lf, timeout);
        }
}

//==============================================================================
/**
 * @brief Switch terminal
 *
 * @param term_no       terminal number
 */
//==============================================================================
static void switch_terminal(int term_no)
{
        if (term_no < _TTY_NUMBER_OF_VT && tty_module->tty[term_no] && tty_module->current_tty != term_no) {
                tty_t *tty = tty_module->tty[tty_module->current_tty];

                if (ttycmd_is_idle(tty->vtcmd)) {
                        tty_module->current_tty = term_no;

                        tty_t *tty = tty_module->tty[tty_module->current_tty];

                        vt100_init();

                        if (sys_mutex_lock(tty->secure_mtx, MAX_DELAY_MS) == ESUCC) {
                                size_t      wrcnt;
                                const char *str;

                                for (int i = _TTY_TERMINAL_ROWS - 1; i >= 0; i--) {
                                        str = ttybfr_get_line(tty->screen, i);

                                        if (str) {
                                                sys_fwrite(str,
                                                            strlen(str),
                                                            &wrcnt,
                                                            tty_module->outfile);
                                        }
                                }

                                str = ttyedit_get_value(tty->editline);
                                sys_fwrite(str, strlen(str), &wrcnt, tty_module->outfile);
                                ttybfr_clear_fresh_line_counter(tty->screen);

                                sys_mutex_unlock(tty->secure_mtx);
                        }
                } else {
                        /* try later */
                        send_cmd(CMD_SWITCH_TTY, term_no);
                }
        }
}

//==============================================================================
/**
 * @brief Handle adding a new line to buffer and prepare editline.
 *
 * @param tty           terminal object
 */
//==============================================================================
static void handle_new_line(tty_t *tty)
{
        if (sys_mutex_lock(tty->secure_mtx, MAX_DELAY_MS) == ESUCC) {
                const char *str  = ttyedit_get_value(tty->editline);
                const char *lf   = "\n";

                if (ttyedit_is_echo_enabled(tty->editline)) {

                        ttybfr_put(tty->screen, str, strlen(str));
                        ttybfr_put(tty->screen, lf, strlen(lf));
                        ttybfr_clear_fresh_line_counter(tty->screen);

                        const char *crlf = "\r\n";
                        size_t      wrcnt;
                        sys_fwrite(crlf, strlen(crlf), &wrcnt, tty_module->outfile);

                }

                copy_string_to_queue(str, tty->queue_out, true, 0);

                ttyedit_clear(tty->editline);

                sys_mutex_unlock(tty->secure_mtx);
        }
}

/*==============================================================================
  End of file
==============================================================================*/
