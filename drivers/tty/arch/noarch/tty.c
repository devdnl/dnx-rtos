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
#define TTYD_STACK_SIZE                 STACK_LOW_SIZE
#define TTYD_PRIORITY                   0

#define FILE                            "/dev/ttyS0"

#define TTY(number)                     term->tty[number]
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
/* memory structure */
struct termHdl {
        struct ttyEntry {
                ch_t    *line[TTY_MAX_LINES];   /* line buffer */
                u8_t     wrIdx;                 /* write index */
                u8_t     newMsgCnt;             /* new message counter */
                flag_t   refLstLn;              /* request to refresh last line */
                mutex_t *mtx;                   /* resources security */
                flag_t   echoOn;                /* echo indicator */

                struct inputstream {            /* FIFO for keyboard read */
                        u16_t level;
                        ch_t  buffer[TTY_STREAM_SIZE];
                        u16_t txidx;
                        u16_t rxidx;
                } input;

                char editLine[TTY_EDIT_LINE_LEN + 1]; /* input line */
                uint editLineLen;             /* edit line fill level */
                uint cursorPosition;          /* cursor position in line */
        } *tty[TTY_LAST];

        u8_t    currentTTY;     /* current terminal */
        i8_t    changeToTTY;    /* terminal to change */
        u8_t    col;            /* terminal column count */
        u8_t    row;            /* terminal row count */
        task_t *taskhdl;        /* TTY worker */
        sem_t  *semcnt_stdout;  /* semaphore used to trigger daemon operation */
        u8_t    captureKeyStep; /* decode Fn key step */
        ch_t    captureKeyTmp;  /* temporary value */
        uint    taskDelay;      /* task delay depended by user activity */
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
static void        task_tty(void *arg);
static void        switch_tty_if_requested(FILE_t *stream);
static void        stdout_service(FILE_t *stream, u8_t dev);
static void        move_cursor_to_beginning_of_editline(u8_t dev, FILE_t *stream);
static void        move_cursor_to_end_of_editline(u8_t dev, FILE_t *stream);
static void        remove_character_from_editline(u8_t dev, FILE_t *stream);
static void        delete_character_from_editline(u8_t dev, FILE_t *stream);
static void        add_charater_to_editline(u8_t dev, ch_t chr, FILE_t *stream);
static void        show_new_messages(u8_t dev, FILE_t *stream);
static void        refresh_last_line(u8_t dev, FILE_t *stream);
static void        stdin_service(FILE_t *stream, u8_t dev);
static void        switch_tty_immediately(u8_t dev, FILE_t *stream);
static void        clear_tty(u8_t dev);
static uint        add_message(u8_t dev, ch_t *msg, uint msgLen);
static ch_t       *create_buffer_for_message(u8_t dev, ch_t *msg, uint msgLen);
static u8_t        count_non_lf_ended_messages(u8_t dev);
static void        strncpy_LF2CRLF(ch_t *dst, ch_t *src, uint n);
static void        free_non_lf_ended_messages(u8_t dev, u8_t mgscount);
static void        link_message(ch_t *msg, u8_t dev);
static void        inc_message_counter(u8_t dev);
static enum keycap capture_special_keys(ch_t character);
static u8_t        get_message_index(u8_t dev, u8_t back);
static void        refresh_tty(u8_t dev, FILE_t *file);
static void        get_vt100_size(FILE_t *ttysfile);
static void        write_input_stream(ch_t chr, u8_t dev);
static stdRet_t    read_input_stream(ch_t *chr, u8_t dev);
static void        move_editline_to_stream(u8_t dev, FILE_t *stream);

/*==============================================================================
  Local object definitions
==============================================================================*/
/* memory used by driver */
static struct termHdl *term;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize USART devices
 *
 * @param dev     device number
 * @param part    device part
 *
 * @retval STD_RET_OK
 */
//==============================================================================
stdRet_t TTY_Init(devx_t dev, fd_t part)
{
        (void)part;

        if (dev >= TTY_LAST) {
                return STD_RET_ERROR;
        }

        if (term != NULL) {
                return STD_RET_OK;
        }

        if ((term = calloc(1, sizeof(struct termHdl))) == NULL) {
                return STD_RET_ERROR;
        }

        if ((term->semcnt_stdout = new_semaphore_counting(10, 0)) != NULL) {

                term->taskhdl = new_task(task_tty, TTYD_NAME, TTYD_STACK_SIZE, NULL, TTYD_PRIORITY);
                if (term->taskhdl) {
                        term->col = 80;
                        term->row = 24;

                        term->changeToTTY = -1;

                        return STD_RET_OK;
                }
        }

        if (term->semcnt_stdout) {
                delete_semaphore_counting(term->semcnt_stdout);
        }

        free(term);
        term = NULL;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release TTY device
 *
 * @param dev     device number
 * @param part    device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t TTY_Release(devx_t dev, fd_t part)
{
        (void)part;

        if (!term || dev >= TTY_LAST) {
                return STD_RET_ERROR;
        }

        if (TTY(dev) == NULL) {
                return STD_RET_OK;
        }

        if (mutex_recursive_lock(TTY(dev)->mtx, BLOCK_TIME) == OS_OK) {
                mutex_t *mtx = TTY(dev)->mtx;

                /* free unused terminal */
                for (u8_t i = 0; i < TTY_MAX_LINES; i++) {
                        if (TTY(dev)->line[i]) {
                                free(TTY(dev)->line[i]);
                        }
                }

                free(TTY(dev));
                TTY(dev) = NULL;

                mutex_recursive_unlock(mtx);
                delete_mutex_recursive(mtx);

                /* if all terminal are unused free terminal handler */
                for (u8_t i = 0; i < TTY_LAST; i++) {
                        if (term->tty[i]) {
                                return STD_RET_OK;
                        }
                }

                delete_semaphore_counting(term->semcnt_stdout);
                delete_task(term->taskhdl);
                free(term);
                term = NULL;

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Opens specified port and initialize default settings
 *
 * @param[in]  dev                        TTY number
 * @param[in]  part                       device part
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  operation error
 */
//==============================================================================
stdRet_t TTY_Open(devx_t dev, fd_t part)
{
      (void)part;

      if (dev >= TTY_LAST || term == NULL) {
              return STD_RET_ERROR;
      }

      if (TTY(dev) != NULL) {
              return STD_RET_OK;
      }

      if ((TTY(dev) = calloc(1, sizeof(struct ttyEntry))) != NULL) {
              if ((TTY(dev)->mtx = new_recursive_mutex()) != NULL) {
                      TTY(dev)->echoOn = SET;
                      return STD_RET_OK;
              }

              free(TTY(dev));
              TTY(dev) = NULL;
      }

      return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close opened port
 *
 * @param[in]  dev                        TTY number
 * @param[in]  part                       device part
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  operation error
 */
//==============================================================================
stdRet_t TTY_Close(devx_t dev, fd_t part)
{
        (void)part;

        if (dev < TTY_LAST) {
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Write data to TTY
 *
 * @param[in]  dev                        TTY number
 * @param[in]  part                       device part
 * @param[in]  *src                       source buffer
 * @param[in]  size                       buffer size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @retval number of written nitems
 */
//==============================================================================
size_t TTY_Write(devx_t dev,  fd_t   part,   void   *src,
                 size_t size, size_t nitems, size_t  seek)
{
        (void)seek;
        (void)part;

        size_t n = 0;

        if (!term && dev >= TTY_LAST && !src && !size && !nitems) {
                return n;
        }

        /* if current TTY is showing wait to show refreshed line */
        while (TTY(dev)->refLstLn == SET && dev == term->currentTTY) {
                milisleep(10);
        }

        /* wait for secure access to data */
        if (mutex_recursive_lock(TTY(dev)->mtx, BLOCK_TIME) == OS_OK) {
                /* check if screen is cleared */
                if (strncmp(VT100_CLEAR_SCREEN, src, 4) == 0) {
                        clear_tty(dev);
                }

                n = add_message(dev, src, nitems);

                mutex_recursive_unlock(TTY(dev)->mtx);
        }

        return n;
}

//==============================================================================
/**
 * @brief Write data to TTY
 *
 * @param[in]  dev                        TTY number
 * @param[in]  part                       device part
 * @param[in]  *dst                       destination buffer
 * @param[in]  size                       item size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @retval number of read nitems
 */
//==============================================================================
size_t TTY_Read(devx_t dev,  fd_t   part,   void   *dst,
                size_t size, size_t nitems, size_t  seek)
{
        (void)seek;
        (void)part;

        size_t n = 0;

        if (!term && dev >= TTY_LAST && !dst && !size && !nitems) {
                return 0;
        }

        if (mutex_recursive_lock(TTY(dev)->mtx, 0) == OS_OK) {
                while (nitems > 0) {
                        if (read_input_stream(dst, dev) != STD_RET_OK) {
                                break;
                        }

                        dst += size;
                        nitems--;
                        n++;
                }

                n /= size;

                mutex_recursive_unlock(TTY(dev)->mtx);
        }

        return n;
}

//==============================================================================
/**
 * @brief Specific settings of TTY
 *
 * @param[in    ] dev           TTY device
 * @param[in    ]  part         device part
 * @param[in    ] ioRQ          input/output request
 * @param[in,out] *data         input/output data
 *
 * @retval STD_RET_OK           operation success
 * @retval STD_RET_ERROR        operation error
 */
//==============================================================================
stdRet_t TTY_IOCtl(devx_t dev, fd_t part, IORq_t ioRQ, void *data)
{
        (void)part;

        if (!term || dev >= TTY_LAST) {
                return STD_RET_ERROR;
        }

        switch (ioRQ) {
        /* return current TTY */
        case TTY_IORQ_GETCURRENTTTY:
                if (data == NULL) {
                        return STD_RET_ERROR;
                }
                *((u8_t*)data) = term->currentTTY;
                break;

        /* set active terminal */
        case TTY_IORQ_SETACTIVETTY:
                if (data == NULL) {
                        return STD_RET_ERROR;
                }
                term->changeToTTY = *((u8_t*)data);
                break;

        /* clear terminal */
        case TTY_IORQ_CLEARTTY:
                clear_tty(dev);
                break;

        /* terminal size - number of columns */
        case TTY_IORQ_GETCOL:
                if (data == NULL) {
                        return STD_RET_ERROR;
                }
                *((u8_t*)data) = term->col;
                break;

        /* terminal size - number of rows */
        case TTY_IORQ_GETROW:
                if (data == NULL) {
                        return STD_RET_ERROR;
                }
                *((u8_t*)data) = term->row;
                break;

        /* clear screen */
        case TTY_IORQ_CLEARSCR:
                term->changeToTTY = term->currentTTY;
                clear_tty(dev);
                break;

        /* clear last line */
        case TTY_IORQ_CLEARLASTLINE:
                if (mutex_recursive_lock(TTY(dev)->mtx, BLOCK_TIME) == OS_OK) {
                        ch_t *msg = malloc(2);

                        if (msg) {
                                msg[0] = '\r';
                                msg[1] = '\0';

                                /* move message pointer back */
                                TTY(dev)->wrIdx--;

                                if (TTY(dev)->wrIdx >= TTY_MAX_LINES) {
                                        TTY(dev)->wrIdx = TTY_MAX_LINES - 1;
                                }

                                /* if no message to refresh setup refresh only one */
                                if (TTY(dev)->newMsgCnt == 0) {
                                        TTY(dev)->refLstLn = SET;
                                }

                                add_message(dev, msg, FALSE);
                        }

                        mutex_recursive_unlock(TTY(dev)->mtx);
                }

                break;

        /* turn on terminal echo */
        case TTY_IORQ_ECHO_ON:
                TTY(dev)->echoOn = SET;
                break;

        /* turn off terminal echo */
        case TTY_IORQ_ECHO_OFF:
                TTY(dev)->echoOn = RESET;
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

        ch_t   *msg;
        FILE_t *ttys;

        /* try open selected file */
        while ((ttys = fopen(FILE, "r+")) == NULL) {
                milisleep(250);
        }

        msg = VT100_RESET_ATTRIBUTES VT100_CLEAR_SCREEN VT100_DISABLE_LINE_WRAP
              VT100_CURSOR_HOME;

        fwrite(msg, sizeof(ch_t), strlen(msg), ttys);

        get_vt100_size(ttys);

        term->taskDelay = 10;

        for (;;) {
                if (TTY(term->currentTTY) == NULL) {
                        milisleep(100);
                        continue;
                }

                stdout_service(ttys, term->currentTTY);

                stdin_service(ttys, term->currentTTY);

                switch_tty_if_requested(ttys);

                milisleep(term->taskDelay);
        }

        /* this should never happen */
        task_exit();
}

//==============================================================================
/**
 * @brief Function provide STDOUT service
 *
 * @param *stream       file
 * @param  dev          terminal number
 */
//==============================================================================
static void stdout_service(FILE_t *stream, u8_t dev)
{
        if (semaphore_counting_take(term->semcnt_stdout, 0) == OS_OK) {
                if (TTY(dev)->newMsgCnt > 0) {
                        show_new_messages(dev, stream);

                        if (term->taskDelay > 40){
                                term->taskDelay = 40;
                        }
                } else if (TTY(dev)->refLstLn == SET) {
                        refresh_last_line(dev, stream);
                }
        }
}

//==============================================================================
/**
 * @brief Function provide STDIN service
 *
 * @param *stream       file
 * @param  dev          terminal number
 */
//==============================================================================
static void stdin_service(FILE_t *stream, u8_t dev)
{
        ch_t  chr;
        ch_t *msg;
        enum keycap keydec;

        if (ioctl(stream, UART_IORQ_GET_BYTE, &chr) != STD_RET_OK) {
                if (term->taskDelay < 200) {
                        term->taskDelay += 10;
                }
                return;
        }

        term->taskDelay = 10;

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
                        move_editline_to_stream(dev, stream);
                        break;

                case '\b':
                        remove_character_from_editline(dev, stream);
                        break;

                default:
                        add_charater_to_editline(dev, chr, stream);
                        break;
                }
                break;

        case ARROW_LEFT_KEY:
                if (TTY(dev)->cursorPosition > 0) {
                        chr = '\b';
                        ioctl(stream, UART_IORQ_SEND_BYTE, &chr);
                        TTY(dev)->cursorPosition--;
                }
                break;

        case ARROW_RIGHT_KEY:
                if (TTY(dev)->cursorPosition < TTY(dev)->editLineLen) {
                        msg = VT100_SHIFT_CURSOR_RIGHT(1);
                        fwrite(msg, sizeof(ch_t), strlen(msg), stream);
                        TTY(dev)->cursorPosition++;
                }
                break;

        case HOME_KEY:
                move_cursor_to_beginning_of_editline(dev, stream);
                break;

        case END_KEY:
                move_cursor_to_end_of_editline(dev, stream);
                break;

        case DEL_KEY:
                delete_character_from_editline(dev, stream);
                break;

        default:
                break;
        }
}

//==============================================================================
/**
 * @brief Function move the cursor to the beginning of edit line
 *
 * @param  dev                  number of terminal
 * @param *stream               required stream to refresh changes
 */
//==============================================================================
static void move_cursor_to_beginning_of_editline(u8_t dev, FILE_t *stream)
{
        ch_t *msg = VT100_CURSOR_OFF;
        fwrite(msg, sizeof(ch_t), strlen(msg), stream);

        while (TTY(dev)->cursorPosition > 0) {
                ch_t chr = '\b';
                ioctl(stream, UART_IORQ_SEND_BYTE, &chr);
                TTY(dev)->cursorPosition--;
        }

        msg = VT100_CURSOR_ON;
        fwrite(msg, sizeof(ch_t), strlen(msg), stream);
}

//==============================================================================
/**
 * @brief Function move the cursor to the end of edit line
 */
//==============================================================================
static void move_cursor_to_end_of_editline(u8_t dev, FILE_t *stream)
{
        ch_t *msg = VT100_CURSOR_OFF;
        fwrite(msg, sizeof(ch_t), strlen(msg), stream);

        while (TTY(dev)->cursorPosition < TTY(dev)->editLineLen) {
                ch_t *msg = VT100_SHIFT_CURSOR_RIGHT(1);
                fwrite(msg, sizeof(ch_t), strlen(msg), stream);
                TTY(dev)->cursorPosition++;
        }

        msg = VT100_CURSOR_ON;
        fwrite(msg, sizeof(ch_t), strlen(msg), stream);
}

//==============================================================================
/**
 * @brief Function remove character at cursor position from edit line
 *
 * @param  dev                  number of terminal
 * @param *stream               required stream to refresh changes
 */
//==============================================================================
static void remove_character_from_editline(u8_t dev, FILE_t *stream)
{
        if (TTY(dev)->cursorPosition == 0 || TTY(dev)->editLineLen == 0) {
                return;
        }

        TTY(dev)->cursorPosition--;

        for (u8_t i = TTY(dev)->cursorPosition; i < TTY(dev)->editLineLen; i++) {
                TTY(dev)->editLine[i] = TTY(dev)->editLine[i + 1];
        }

        TTY(dev)->editLineLen--;

        ch_t *msg = "\b"VT100_ERASE_LINE_FROM_CUR VT100_SAVE_CURSOR_POSITION;
        fwrite(msg, sizeof(ch_t), strlen(msg), stream);

        msg = &TTY(dev)->editLine[TTY(dev)->cursorPosition];
        fwrite(msg, sizeof(ch_t),
               TTY(dev)->editLineLen - TTY(dev)->cursorPosition, stream);

        msg = VT100_RESTORE_CURSOR_POSITION;
        fwrite(msg, sizeof(ch_t), strlen(msg), stream);
}

//==============================================================================
/**
 * @brief Function delete right character from edit line
 *
 * @param  dev                  number of terminal
 * @param *stream               required stream to refresh changes
 */
//==============================================================================
static void delete_character_from_editline(u8_t dev, FILE_t *stream)
{
        if (TTY(dev)->editLineLen == 0) {
                return;
        }

        if (TTY(dev)->cursorPosition == TTY(dev)->editLineLen) {
                return;
        }

        for (u8_t i = TTY(dev)->cursorPosition; i <= TTY(dev)->editLineLen; i++) {
                TTY(dev)->editLine[i] = TTY(dev)->editLine[i + 1];
        }

        TTY(dev)->editLineLen--;

        ch_t *msg = VT100_SAVE_CURSOR_POSITION VT100_ERASE_LINE_FROM_CUR;
        fwrite(msg, sizeof(ch_t), strlen(msg), stream);

        msg = &TTY(dev)->editLine[TTY(dev)->cursorPosition];
        fwrite(msg, sizeof(ch_t),
               TTY(dev)->editLineLen - TTY(dev)->cursorPosition, stream);

        msg = VT100_RESTORE_CURSOR_POSITION;
        fwrite(msg, sizeof(ch_t), strlen(msg), stream);
}

//==============================================================================
/**
 * @brief Function add character at cursor position in edit line
 */
//==============================================================================
static void add_charater_to_editline(u8_t dev, ch_t chr, FILE_t *stream)
{
        ch_t *msg;

        if (TTY(dev)->editLineLen >= TTY_EDIT_LINE_LEN - 1) {
                return;
        }

        if (TTY(dev)->cursorPosition < TTY(dev)->editLineLen) {
                for (u8_t i = TTY(dev)->editLineLen; i > TTY(dev)->cursorPosition; i--) {
                        TTY(dev)->editLine[i] = TTY(dev)->editLine[i - 1];
                }

                TTY(dev)->editLine[TTY(dev)->cursorPosition++] = chr;
                TTY(dev)->editLineLen++;

                msg = VT100_SAVE_CURSOR_POSITION;
                fwrite(msg, sizeof(ch_t), strlen(msg), stream);

                msg = &TTY(dev)->editLine[TTY(dev)->cursorPosition - 1];
                fwrite(msg, sizeof(ch_t),
                       TTY(dev)->editLineLen - (TTY(dev)->cursorPosition - 1), stream);

                msg = VT100_RESTORE_CURSOR_POSITION VT100_SHIFT_CURSOR_RIGHT(1);
                fwrite(msg, sizeof(ch_t), strlen(msg), stream);
        } else {
                TTY(dev)->editLine[TTY(dev)->cursorPosition++] = chr;
                TTY(dev)->editLineLen++;

                if (TTY(dev)->echoOn == SET) {
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
static void switch_tty_if_requested(FILE_t *stream)
{
        if (term->changeToTTY != -1) {
                term->currentTTY  = term->changeToTTY;
                term->changeToTTY = -1;
                refresh_tty(term->currentTTY, stream);
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
static void switch_tty_immediately(u8_t dev, FILE_t *stream)
{
        if (dev != term->currentTTY) {
                term->currentTTY = dev;

                if (TTY(term->currentTTY) == NULL) {
                        TTY_Open(term->currentTTY, TTY_PART_NONE);
                }

                if (TTY(term->currentTTY)) {
                        refresh_tty(term->currentTTY, stream);
                }
        }
}

//==============================================================================
/**
 * @brief Function show new messages
 *
 * @param  dev          number of terminal
 * @param *stream       required stream to refresh new messages
 */
//==============================================================================
static void show_new_messages(u8_t dev, FILE_t *stream)
{
        while (mutex_recursive_lock(TTY(dev)->mtx, BLOCK_TIME) != OS_OK);

        while (TTY(dev)->newMsgCnt) {
                if (TTY(dev)->newMsgCnt > term->row) {
                        TTY(dev)->newMsgCnt = term->row;
                }

                ch_t *msg = TTY(dev)->line[get_message_index(term->currentTTY,
                                                             TTY(dev)->newMsgCnt)];

                TTY(dev)->newMsgCnt--;

                if (msg) {
                        fwrite(msg, sizeof(ch_t), strlen(msg), stream);
                }
        }

        TTY(dev)->refLstLn = RESET;

        mutex_recursive_unlock(TTY(dev)->mtx);
}

//==============================================================================
/**
 * @brief Function refresh last line
 *
 * @param  dev          number of terminal
 * @param *stream       required stream to refresh new messages
 */
//==============================================================================
static void refresh_last_line(u8_t dev, FILE_t *stream)
{
        while (mutex_recursive_lock(TTY(dev)->mtx, BLOCK_TIME) != OS_OK);

        ch_t *msg = VT100_CURSOR_OFF
                    VT100_CARRIAGE_RETURN
                    VT100_ERASE_LINE_FROM_CUR
                    VT100_RESET_ATTRIBUTES;

        fwrite(msg, sizeof(ch_t), strlen(msg), stream);

        /* refresh line */
        msg = TTY(dev)->line[get_message_index(term->currentTTY, 1)];
        fwrite(msg, sizeof(ch_t), strlen(msg), stream);

        /* cursor on */
        msg = VT100_CURSOR_ON;
        fwrite(msg, sizeof(ch_t), strlen(msg), stream);

        TTY(dev)->refLstLn = RESET;

        mutex_recursive_unlock(TTY(dev)->mtx);
}

//==============================================================================
/**
 * @brief Clear all messages from terminal and free memory
 *
 * @param dev        terminal number
 */
//==============================================================================
static void clear_tty(u8_t dev)
{
        if (mutex_recursive_lock(TTY(dev)->mtx, BLOCK_TIME) == OS_OK) {
                for (u8_t i = 0; i < TTY_MAX_LINES; i++) {
                        if (TTY(dev)->line[i]) {
                                free(TTY(dev)->line[i]);
                                TTY(dev)->line[i] = NULL;
                        }
                }

                TTY(dev)->newMsgCnt = 0;
                TTY(dev)->wrIdx     = 0;
                TTY(dev)->refLstLn  = RESET;

                semaphore_counting_give(term->semcnt_stdout);

                mutex_recursive_unlock(TTY(dev)->mtx);
        }
}

//==============================================================================
/**
 * @brief Add new message or modify existing
 *
 * @param  dev          number of terminal
 * @param *msg          message
 * @param  msgLen       message length
 *
 * @return msgLen if message created, otherwise 0
 */
//==============================================================================
static uint add_message(u8_t dev, ch_t *msg, uint msgLen)
{
        u8_t  mgcnt;
        ch_t *newMsg;

        if (!msgLen || !msg) {
                return 0;
        }

        mgcnt = count_non_lf_ended_messages(dev);

        free_non_lf_ended_messages(dev, mgcnt);

        newMsg = create_buffer_for_message(dev, msg, msgLen);

        link_message(newMsg, dev);

        semaphore_counting_give(term->semcnt_stdout);

        if (newMsg) {
                return msgLen;
        } else {
                return 0;
        }
}

//==============================================================================
/**
 * @brief Function create new buffer for new message
 * Function create new buffer for new message if lates message is LF ended,
 * otherwise function merge latest message with new message. Function returns
 * pointer to new buffer.
 *
 * @param [in]  dev             number of terminal
 * @param [in] *msgSrc          message source pointer
 * @param [in]  msgLen          message length
 *
 * @return pointer to new message buffer
 */
//==============================================================================
static ch_t *create_buffer_for_message(u8_t dev, ch_t *msgSrc, uint msgLen)
{
        ch_t   *msg;
        ch_t   *lstMsg     = TTY(dev)->line[get_message_index(dev, 1)];
        size_t  lstMsgSize = strlen(lstMsg);
        uint  LF_count   = 0;

        /* calculate how many '\n' exist in string */
        for (uint i = 0; i < msgLen; i++) {
                if (msgSrc[i] == '\n') {
                        LF_count++;
                }
        }

        if (lstMsg && (*(lstMsg + lstMsgSize - 1) != '\n')) {
                lstMsgSize += 1;

                msg = malloc(lstMsgSize + msgLen + (2 * LF_count));
                if (msg) {
                        TTY(dev)->wrIdx--;
                        if (TTY(dev)->wrIdx >= TTY_MAX_LINES) {
                                TTY(dev)->wrIdx = TTY_MAX_LINES - 1;
                        }

                        /* if no message to refresh setup refresh only one */
                        if (TTY(dev)->newMsgCnt == 0) {
                                TTY(dev)->refLstLn = SET;
                        }

                        strcpy(msg, lstMsg);
                        strncpy_LF2CRLF(msg + strlen(msg), msgSrc, msgLen + LF_count);
                }
        } else {
                msg = malloc(msgLen + (2 * LF_count));
                if (msg) {
                        strncpy_LF2CRLF(msg, msgSrc, msgLen + (2 * LF_count));
                        inc_message_counter(dev);
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
static void strncpy_LF2CRLF(ch_t *dst, ch_t *src, uint n)
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
 * @param  dev          number of terminal
 */
//==============================================================================
static u8_t count_non_lf_ended_messages(u8_t dev)
{
        u8_t wridx = TTY(dev)->wrIdx;
        u8_t mgcnt = 1;

        /* find all messages which are not ended by LF (this is common line) */
        for (u8_t i = 0; i < TTY_MAX_LINES; i++) {
                ch_t *lastmsg = TTY(dev)->line[wridx++];

                if (lastmsg == NULL) {
                        break;
                }

                if (wridx >= TTY_MAX_LINES) {
                        wridx = 0;
                }

                if (*(lastmsg + strlen(lastmsg) - 1) != '\n') {
                        mgcnt++;
                } else {
                        break;
                }
        }

        return mgcnt;
}

//==============================================================================
/**
 * @brief Function free all old non-LF ended messages
 * Function does not change tty's wrIdx. Function free all non-LF ended messages
 * from current write index -- function clear the latest messages.
 *
 * @param  dev          number of terminal
 * @param  msgcnt       message count to free
 */
//==============================================================================
static void free_non_lf_ended_messages(u8_t dev, u8_t mgscount)
{
        u8_t wridx = TTY(dev)->wrIdx;

        while (mgscount--) {
                if (TTY(dev)->line[wridx]) {
                        free(TTY(dev)->line[wridx]);
                        TTY(dev)->line[wridx] = NULL;
                }

                wridx++;

                if (wridx >= TTY_MAX_LINES) {
                        wridx = 0;
                }
        }
}

//==============================================================================
/**
 * @brief Function link prepared message to TTY
 *
 * @param *msg          message
 * @param  dev          number of terminal
 */
//==============================================================================
static void link_message(ch_t *msg, u8_t dev)
{
        TTY(dev)->line[TTY(dev)->wrIdx++] = msg;

        if (TTY(dev)->wrIdx >= TTY_MAX_LINES) {
                TTY(dev)->wrIdx = 0;
        }
}

//==============================================================================
/**
 * @brief Function increase message counter
 *
 * @param  dev          number of terminal
 */
//==============================================================================
static void inc_message_counter(u8_t dev)
{
        if (TTY(dev)->newMsgCnt < TTY_MAX_LINES) {
                TTY(dev)->newMsgCnt++;
        }
}

//==============================================================================
/**
 * @brief Get last or selected message
 *
 * @param dev           number of terminal
 * @param back          number of lines from current index
 *
 * @return index to message
 */
//==============================================================================
static u8_t get_message_index(u8_t dev, u8_t back)
{
        /* check if index underflow when calculating with back */
        if (TTY(dev)->wrIdx < back) {
                return TTY_MAX_LINES - (back - TTY(dev)->wrIdx);
        } else {
                return TTY(dev)->wrIdx - back;
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
static enum keycap capture_special_keys(ch_t character)
{
        switch (term->captureKeyStep) {
        case 0:
                if (character == '\e') {
                        term->captureKeyStep++;
                        return KEY_CAPTURE_PENDING;
                }
                break;

        case 1:
                if (character == 'O' || character == '[') {
                        term->captureKeyStep++;
                        return KEY_CAPTURE_PENDING;
                } else {
                        term->captureKeyStep = 0;
                }
                break;

        case 2:
                term->captureKeyStep = 0;

                switch (character) {
                case 'P': return F1_KEY;
                case 'Q': return F2_KEY;
                case 'R': return F3_KEY;
                case 'S': return F4_KEY;
                case 'D': return ARROW_LEFT_KEY;
                case 'C': return ARROW_RIGHT_KEY;
                case 'F': return END_KEY;
                case '1': term->captureKeyTmp  = '1';
                          term->captureKeyStep = 3;
                          return KEY_CAPTURE_PENDING;
                case '3': term->captureKeyTmp  = '3';
                          term->captureKeyStep = 3;
                          return KEY_CAPTURE_PENDING;
                }

                break;

        case 3:
                term->captureKeyStep = 0;

                if (character == '~') {
                        switch (term->captureKeyTmp) {
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        }
                }

                break;

        default:
                term->captureKeyStep = 0;
                break;
        }

        return NORMAL_KEY;
}

//==============================================================================
/**
 * @brief Refresh selected TTY
 *
 * @param dev     number of terminal
 */
//==============================================================================
static void refresh_tty(u8_t dev, FILE_t *file)
{
        get_vt100_size(file);

        ch_t *msg = VT100_RESET_ATTRIBUTES  VT100_CLEAR_SCREEN
                    VT100_DISABLE_LINE_WRAP VT100_CURSOR_HOME;

        fwrite(msg, sizeof(ch_t), strlen(msg), file);

        if (mutex_recursive_lock(TTY(dev)->mtx, BLOCK_TIME) == OS_OK) {
                i8_t rows;

                if (term->row < TTY_MAX_LINES) {
                        rows = term->row;
                } else {
                        rows = TTY_MAX_LINES;
                }

                for (i8_t i = rows - 1; i > 0; i--) {
                        msg = TTY(dev)->line[get_message_index(term->currentTTY, i)];

                        if (msg) {
                                fwrite(msg, sizeof(ch_t), strlen(msg), file);
                        }
                }

                msg = TTY(dev)->editLine;
                fwrite(msg, sizeof(ch_t), strlen(msg), file);

                TTY(dev)->newMsgCnt = 0;

                mutex_recursive_unlock(TTY(dev)->mtx);
        }
}

//==============================================================================
/**
 * @brief Function gets terminal size
 *
 * @param *ttysfile     tty file
 */
//==============================================================================
static void get_vt100_size(FILE_t *ttysfile)
{
        ch_t chr = 0;

        /* set default values */
        term->col = 80;
        term->row = 24;

        ch_t *rq = VT100_SAVE_CURSOR_POSITION
                   VT100_CURSOR_OFF
                   VT100_SET_CURSOR_POSITION(250, 250)
                   VT100_QUERY_CURSOR_POSITION;

        fwrite(rq, sizeof(ch_t), strlen(rq), ttysfile);

        /* buffer for response */
        ch_t resp[10];
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
        ch_t *respPtr = &resp[0];
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
        fwrite(rq, sizeof(ch_t), strlen(rq), ttysfile);

        /* find response begin */
        ch_t *seek = strchr(resp, '[');
        seek++;

        u8_t row = 0;
        u8_t col = 0;

        /* calculate row */
        while ((chr = *(seek++)) != ';') {
                row *= 10;
                row += chr - '0';
        }

        /* calculate columns */
        while ((chr = *(seek++)) != 'R') {
                col *= 10;
                col += chr - '0';
        }

        term->row = row;
        term->col = col;
}

//==============================================================================
/**
 * @brief Function puts data to input stream
 *
 * @param chr           character
 * @param dev           device
 */
//==============================================================================
static void write_input_stream(ch_t chr, u8_t dev)
{
        if (mutex_recursive_lock(TTY(dev)->mtx, BLOCK_TIME) == OS_OK) {
                TTY(dev)->input.buffer[TTY(dev)->input.txidx++] = chr;

                if (TTY(dev)->input.txidx >= TTY_STREAM_SIZE) {
                        TTY(dev)->input.txidx = 0;
                }

                if (TTY(dev)->input.level < TTY_STREAM_SIZE) {
                        TTY(dev)->input.level++;
                } else {
                        TTY(dev)->input.rxidx++;

                        if (TTY(dev)->input.rxidx >= TTY_STREAM_SIZE) {
                                TTY(dev)->input.rxidx = 0;
                        }
                }

                mutex_recursive_unlock(TTY(dev)->mtx);
        }
}

//==============================================================================
/**
 * @brief Function gets character from input stream
 *
 * @param[out] *chr             pointer to character
 * @param [in]  dev             device
 *
 * @retval STD_RET_OK           loaded character from stream
 * @retval STD_RET_ERROR        stream is empty
 */
//==============================================================================
static stdRet_t read_input_stream(ch_t *chr, u8_t dev)
{
        if (TTY(dev)->input.level == 0) {
                return STD_RET_ERROR;
        }

        if (mutex_recursive_lock(TTY(dev)->mtx, 0) == OS_OK) {
                *chr = TTY(dev)->input.buffer[TTY(dev)->input.rxidx++];

                if (TTY(dev)->input.rxidx >= TTY_STREAM_SIZE) {
                        TTY(dev)->input.rxidx = 0;
                }

                TTY(dev)->input.level--;

                mutex_recursive_unlock(TTY(dev)->mtx);
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function move edit line to stream
 *
 * @param [in]  dev             device
 * @param [in] *stream          terminal control stream
 */
//==============================================================================
static void move_editline_to_stream(u8_t dev, FILE_t *stream)
{
        TTY(dev)->editLine[TTY(dev)->editLineLen++] = '\n';

        uint line_len = TTY(dev)->editLineLen;

        for (uint i = 0; i < line_len; i++) {
                write_input_stream(TTY(dev)->editLine[i], dev);
        }

        while (mutex_recursive_lock(TTY(dev)->mtx, BLOCK_TIME) != OS_OK);
        add_message(dev, TTY(dev)->editLine, line_len);
        TTY(dev)->refLstLn = RESET;
        mutex_recursive_unlock(TTY(dev)->mtx);

        memset(TTY(dev)->editLine, '\0', TTY_EDIT_LINE_LEN);
        TTY(dev)->editLineLen    = 0;
        TTY(dev)->cursorPosition = 0;

        ch_t *msg = "\r\n";
        fwrite(msg, sizeof(ch_t), strlen(msg), stream);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
