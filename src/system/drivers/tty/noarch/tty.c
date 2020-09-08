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
struct tty_io;

typedef struct {
        struct tty_io *io;
        queue_t       *queue_out;
        mutex_t       *secure_mtx;
        ttybfr_t      *screen;
        ttyedit_t     *editline;
        ttycmd_t      *vtcmd;
        bool           flushed;
        u8_t           major;
        u8_t           minor;
} tty_t;

typedef struct tty_io {
        FILE  *infile;
        FILE  *outfile;
        tty_t *tty[_TTY_NUMBER_OF_VT];
        tid_t  service_in;
        u8_t   current_tty;
        bool   clear_at_init;
} tty_io_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int      configure               (tty_t *tty, const TTY_config_t *conf);
static void     service_in              (void *arg);
static void     vt100_init              (tty_io_t *io);
static void     vt100_analyze           (tty_io_t *io, char c);
static void     copy_string_to_queue    (const char *str, queue_t *queue, bool lfend, uint timeout);
static int      switch_terminal         (tty_io_t *io, int term_no);
static void     handle_new_line         (tty_t *tty);
static int      show_fresh_line         (tty_t *tty);
static int      clear_tty               (tty_t *tty);
static int      refresh_last_line       (tty_t *tty);
static int      dump_tty_buffer         (tty_t *tty, char *dst, size_t *size);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(TTY);

static const thread_attr_t SERVICE_IN_ATTR = {
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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(TTY, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        int err = ENODEV;

        if (minor >= _TTY_NUMBER_OF_VT) {
                return err;
        }

        /* initialize module base */
        tty_io_t *io  = NULL;
        tty_t    *tty = NULL;

        if (minor == 0) {
                err = sys_zalloc(sizeof(tty_io_t), cast(void**, &io));

                if (!err) {
                        err = sys_thread_create(service_in, &SERVICE_IN_ATTR, io, &io->service_in);
                }

                if (err) {
                        if (sys_thread_is_valid(io->service_in)) {
                                sys_thread_destroy(io->service_in);
                        }

                        if (io) {
                                sys_free(cast(void**, &io));
                        }
                } else {
                        printk("TTY: thread ID: %u", io->service_in);
                }

        } else {
                tty_t *tty0 = NULL;
                err = sys_module_get_instance(major, 0, (void**)&tty0);

                if (!err) {
                        io = tty0->io;

                        if (io == NULL) {
                                err = EFAULT;
                        }
                }
        }

        if (!err && io) {
                err = sys_zalloc(sizeof(tty_t), device_handle);
                if (err) {
                        goto tty_alloc_finish;
                }

                tty = *device_handle;

                err = sys_queue_create(_TTY_STREAM_SIZE, sizeof(char), &tty->queue_out);
                if (err) {
                        goto tty_alloc_finish;
                }

                err = sys_mutex_create(MUTEX_TYPE_RECURSIVE, &tty->secure_mtx);
                if (err) {
                        goto tty_alloc_finish;
                }

                err = ttybfr_create(&tty->screen);
                if (err) {
                        goto tty_alloc_finish;
                }

                err = ttyedit_create(&io->outfile, &tty->editline);
                if (err) {
                        goto tty_alloc_finish;
                }

                err = ttycmd_create(&tty->vtcmd);
                if (err) {
                        goto tty_alloc_finish;
                }

                tty->io        = io;
                tty->major     = major;
                tty->minor     = minor;
                io->tty[minor] = tty;

                if (!err && config) {
                        err = configure(tty, config);
                }

                tty_alloc_finish:
                if (err && tty) {
                        if (tty->vtcmd) {
                                ttycmd_destroy(tty->vtcmd);
                        }

                        if (tty->editline) {
                                ttyedit_destroy(tty->editline);
                        }

                        if (tty->screen) {
                                ttybfr_destroy(tty->screen);
                        }

                        if (tty->secure_mtx) {
                                sys_mutex_destroy(tty->secure_mtx);
                        }

                        if (tty->queue_out) {
                                sys_queue_destroy(tty->queue_out);
                        }

                        if (device_handle) {
                                sys_free(device_handle);
                        }

                        if (minor == 0) {
                                sys_thread_destroy(io->service_in);
                                sys_free(cast(void**, &io));
                        }
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
                tty->io->tty[tty->minor] = NULL;

                /* release IO if all TTYs are released */
                bool release_TTY = true;
                for (int i = 0; i < _TTY_NUMBER_OF_VT && release_TTY; i++) {
                        release_TTY = !tty->io->tty[i];
                }

                if (release_TTY) {
                        sys_thread_destroy(tty->io->service_in);

                        if (tty->io->infile) {
                                sys_fclose(tty->io->infile);
                        }

                        if (tty->io->outfile) {
                                sys_fclose(tty->io->outfile);
                        }

                        sys_free(cast(void**, &tty->io));
                }

                sys_free(&device_handle);
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
        if (!err) {
                ttybfr_put(tty->screen, cast(const char *, src), count);
                err = show_fresh_line(tty);
                sys_mutex_unlock(tty->secure_mtx);

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
        case IOCTL_TTY__CONFIGURE:
                if (arg) {
                        err = configure(tty, arg);
                }
                break;

        case IOCTL_TTY__GET_CURRENT_TTY:
                if (arg) {
                        *cast(int*, arg) = tty->io->current_tty;
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
                                ttyedit_set_value(tty->editline, arg, tty->io->current_tty == tty->minor);
                                sys_mutex_unlock(tty->secure_mtx);
                        }
                }
                break;

        case IOCTL_TTY__SWITCH_TTY_TO:
                if (arg) {
                        err = switch_terminal(tty->io, *cast(int*, arg));
                }
                break;

        case IOCTL_TTY__CLEAR_SCR:
                err = clear_tty(tty);
                break;

        case IOCTL_TTY__ECHO_ON:
                err = ttyedit_enable_echo(tty->editline);
                break;

        case IOCTL_TTY__ECHO_OFF:
                err = ttyedit_disable_echo(tty->editline);
                break;

        case IOCTL_TTY__GET_NUMBER_OF_TTYS:
                if (arg) {
                        *cast(int*, arg) = _TTY_NUMBER_OF_VT;
                }
                break;

        case IOCTL_TTY__REFRESH_LAST_LINE:
                err = refresh_last_line(tty);
                break;

        case IOCTL_TTY__READ_BUFFER:
                err = EINVAL;
                TTY_buffer_t *buf = arg;
                if (buf && buf->ptr && buf->size) {
                        err = dump_tty_buffer(tty, buf->ptr, &buf->size);
                }
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
        if (!err) {
                ttybfr_flush(tty->screen);
                err = refresh_last_line(tty);
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
        device_stat->st_minor = tty->minor;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function configure TTY IO.
 *
 * @param  tty          tty instance
 * @param  conf         configuration
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int configure(tty_t *tty, const TTY_config_t *conf)
{
        FILE *in, *out;

        int err = sys_fopen(conf->input_file, "r+", &in);

        if (!err) {
                if (strcmp(conf->input_file, conf->output_file) == 0) {
                        out = in;
                } else {
                        err = sys_fopen(conf->output_file, "r", &out);
                }
        }

        if (!err && tty->io->infile) {
                sys_fclose(tty->io->infile);
        }

        if (!err && tty->io->outfile && (tty->io->outfile != tty->io->infile)) {
                sys_fclose(tty->io->outfile);
        }

        if (!err) {
                tty->io->infile  = in;
                tty->io->outfile = out;
                tty->io->clear_at_init = conf->clear_screen;

                vt100_init(tty->io);

        } else {
                if (in) {
                        sys_fclose(in);
                }

                if (out && (out != in)) {
                        sys_fclose(out);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief TTY input service (helper task)
 *
 * @param arg           tty io
 */
//==============================================================================
static void service_in(void *arg)
{
        tty_io_t *io = arg;

        bool last_CR = false;

        for (;;) {
                char c = '\0'; size_t rdcnt;
                if (sys_fread(&c, 1, &rdcnt, io->infile) == ESUCC) {
                        /*
                         * Standard TTY protocol is to sent a CR from the keyboard
                         * when the user has finished typing and for the computer
                         * to echo LF when it's ready for the next line.
                         */
                        if (last_CR && c == '\n') continue;

                        last_CR = (c == '\r');

                        vt100_analyze(io, c);
                } else {
                        sys_sleep_ms(100);
                }
        }
}

//==============================================================================
/**
 * @brief Configure VT100 terminal
 *
 * @param io    TTY io
 */
//==============================================================================
static void vt100_init(tty_io_t *io)
{
        size_t wrcnt;

        const char *cmd = VT100_RESET_ATTRIBUTES;
        sys_fwrite(cmd, strlen(cmd), &wrcnt, io->outfile);

        if (io->clear_at_init) {
                cmd = VT100_CLEAR_SCREEN VT100_CURSOR_HOME;
                sys_fwrite(cmd, strlen(cmd), &wrcnt, io->outfile);
        }

        cmd = VT100_DISABLE_LINE_WRAP;
        sys_fwrite(cmd, strlen(cmd), &wrcnt, io->outfile);
}

//==============================================================================
/**
 * @brief Control analysis of VT100 input stream
 *
 * @param io            tty io
 * @param c             input character
 */
//==============================================================================
static void vt100_analyze(tty_io_t *io, char c)
{
        tty_t *tty = io->tty[io->current_tty];
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
                switch_terminal(tty->io, resp - TTYCMD_KEY_F1);
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
 * @param io            tty io
 * @param term_no       terminal number
 *
 * @return One of errno value.
 */
//==============================================================================
static int switch_terminal(tty_io_t *io, int term_no)
{
        int err = EINVAL;

        if (term_no < _TTY_NUMBER_OF_VT && io->tty[term_no] && (io->current_tty != term_no)) {

                err = EBUSY;

                tty_t *tty = io->tty[io->current_tty];

                u64_t tref = sys_time_get_reference();

                while (not ttycmd_is_idle(tty->vtcmd) && not sys_time_is_expired(tref, 1000)) {
                        sys_sleep_ms(50);
                }

                if (not sys_time_is_expired(tref, 1000)) {

                        io->current_tty = term_no;

                        tty_t *tty = io->tty[io->current_tty];

                        vt100_init(io);

                        err = sys_mutex_lock(tty->secure_mtx, MAX_DELAY_MS);
                        if (!err) {
                                size_t      wrcnt;
                                const char *str;

                                for (int i = _TTY_TERMINAL_ROWS - 1; i >= 0; i--) {
                                        str = ttybfr_get_line(tty->screen, i);

                                        if (str) {
                                                sys_fwrite(str, strlen(str), &wrcnt,
                                                           io->outfile);
                                        }
                                }

                                str = ttyedit_get_value(tty->editline);
                                sys_fwrite(str, strlen(str), &wrcnt, io->outfile);
                                ttybfr_clear_fresh_line_counter(tty->screen);

                                sys_mutex_unlock(tty->secure_mtx);
                        }
                }
        }

        return err;
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
                        sys_fwrite(crlf, strlen(crlf), &wrcnt, tty->io->outfile);

                }

                copy_string_to_queue(str, tty->queue_out, true, 0);

                ttyedit_clear(tty->editline);

                sys_mutex_unlock(tty->secure_mtx);
        }
}

//==============================================================================
/**
 * @brief  Function show fresh line.
 *
 * @param  tty          TTY instance
 *
 * @return One of errno value.
 */
//==============================================================================
static int show_fresh_line(tty_t *tty)
{
        int err = ESUCC;

        if (tty->minor == tty->io->current_tty) {
                err = sys_mutex_lock(tty->secure_mtx, 100);
                if (!err) {

                        const char *str;
                        while ((str = ttybfr_get_fresh_line(tty->screen))) {
                                size_t wrcnt;

                                if (tty->flushed) {
                                        sys_fwrite(VT100_CLEAR_LINE,
                                                   strlen(VT100_CLEAR_LINE),
                                                   &wrcnt,
                                                   tty->io->outfile);

                                        tty->flushed = false;
                                }

                                sys_fwrite(str, strlen(str), &wrcnt, tty->io->outfile);
                        }

                        sys_mutex_unlock(tty->secure_mtx);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Clear TTY output.
 *
 * @param  tty          TTY instance
 *
 * @return One of errno value.
 */
//==============================================================================
static int clear_tty(tty_t *tty)
{
        int err = sys_mutex_lock(tty->secure_mtx, 100);
        if (!err) {
                ttybfr_clear(tty->screen);

                if (tty->minor == tty->io->current_tty) {
                        vt100_init(tty->io);
                }

                sys_mutex_unlock(tty->secure_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Refresh last line.
 *
 * @param  tty          TTY instance
 *
 * @return One of errno value.
 */
//==============================================================================
static int refresh_last_line(tty_t *tty)
{
        int err = EINVAL;

        if (tty->minor == tty->io->current_tty) {

                err = sys_mutex_lock(tty->secure_mtx, MAX_DELAY_MS);
                if (!err) {
                        size_t wrcnt;

                        sys_fwrite(VT100_CLEAR_LINE, strlen(VT100_CLEAR_LINE),
                                   &wrcnt, tty->io->outfile);

                        const char *last_line = ttybfr_get_line(tty->screen, 0);
                        sys_fwrite(last_line, strlen(last_line),
                                   &wrcnt, tty->io->outfile);

                        const char *editline = ttyedit_get_value(tty->editline);
                        sys_fwrite(editline, strlen(editline),
                                   &wrcnt, tty->io->outfile);

                        tty->flushed = true;

                        sys_mutex_unlock(tty->secure_mtx);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Dump TTY buffer.
 *
 * @param  tty          TTY instance
 * @param  dst          destination buffer
 * @param  size         destination buffer size
 *
 * @return One of errno value.
 */
//==============================================================================
static int dump_tty_buffer(tty_t *tty, char *dst, size_t *size)
{
        int err = sys_mutex_lock(tty->secure_mtx, MAX_DELAY_MS);
        if (!err) {

                dst[0] = '\0';

                for (int i = _TTY_TERMINAL_ROWS - 1; i >= 0; i--) {

                        const char *str = ttybfr_get_line(tty->screen, i);
                        if (str) {

                                size_t len = min(*size, strlen(str));
                                if (len) {
                                        strlcat(dst, str, len);
                                        dst   += len - 2;
                                        *dst++ = '\n';
                                        *dst   = '\0';
                                        *size -= len - 1;

                                } else {
                                        break;
                                }
                        }
                }

                sys_mutex_unlock(tty->secure_mtx);
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
