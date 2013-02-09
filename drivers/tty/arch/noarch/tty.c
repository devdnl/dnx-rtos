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
#define TTYD_STACK_SIZE                 (3 * MINIMAL_STACK_SIZE)

#define FILE                            "/dev/ttyS0"
#define TTY_STREAM_SIZE                 80
#define TTY_EDIT_LINE_LEN               80

#define TTY(number)                     term->tty[number]
#define BLOCK_TIME                      10000

#define VT100_RESET_ATTRIBUTES          "\e[0m"
#define VT100_CLEAR_SCREEN              "\e[2J"
#define VT100_DISABLE_LINE_WRAP         "\e[7l"
#define VT100_CURSOR_HOME               "\e[H"
#define VT100_CURSOR_OFF                "\e[?25l"
#define VT100_CURSOR_ON                 "\e[?25h"
#define VT100_CARRIAGE_RETURN           "\r"
#define VT100_LINE_ERASE_FROM_CUR       "\e[K"
#define VT100_SAVE_CURSOR_POSITION      "\e[s"
#define VT100_CURSOR_OFF                "\e[?25l"
#define VT100_SET_CURSOR_POSITION(r, c) "\e["#r";"#c"H"
#define VT100_QUERY_CURSOR_POSITION     "\e[6n"
#define VT100_RESTORE_CURSOR_POSITION   "\e[u"
#define VT100_CURSOR_ON                 "\e[?25h"

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
                mutex_t  mtx;                   /* resources security */
                flag_t   echoOn;                /* echo indicator */

                struct inputstream {            /* FIFO for keyboard read */
                        u16_t level;
                        ch_t  buffer[TTY_STREAM_SIZE];
                        u16_t txidx;
                        u16_t rxidx;
                } input;

                ch_t   editLine[TTY_EDIT_LINE_LEN + 1]; /* input line */
                uint_t editLineFill;            /* edit line fill level */
                uint_t cursorPosition;          /* cursor position in line */
        } *tty[TTY_LAST];

        u8_t   currentTTY;      /* current terminal */
        i8_t   changeToTTY;     /* terminal to change */
        u8_t   col;             /* terminal column count */
        u8_t   row;             /* terminal row count */
        task_t taskHdl;         /* task handle */
        sem_t  semcnt;          /* semaphore used to trigger daemon operation */
        u8_t   decodeFnStep;    /* decode Fn key step */
};

/* key detector results */
enum keyfn {
        TTY1_SELECTED,
        TTY2_SELECTED,
        TTY3_SELECTED,
        TTY4_SELECTED,
        TTY_SEL_PEND,
        TTY_SEL_NONE
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void       task_tty(void *arg);
static void       stdout_service(FILE_t *stream, u8_t dev);
static void       stdin_service(FILE_t *stream, u8_t dev);
static void       clear_tty(u8_t dev);
static void       add_message(u8_t dev, ch_t *msg, bool_t incMsgCnt);
static enum keyfn decode_fn_keys(ch_t character);
static u8_t       get_message_index(u8_t dev, u8_t back);
static void       refresh_tty(u8_t dev, FILE_t *file);
static void       get_vt100_size(FILE_t *ttysfile);
static void       put_into_input_stream(ch_t chr, u8_t dev);
static stdRet_t   get_from_input_stream(ch_t *chr, u8_t dev);
static void       move_line_to_stream(u8_t dev);

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

        if ((term = calloc(1, sizeof(struct termHdl))) != NULL) {
                if ((term->semcnt = CreateSemCnt(10, 0)) != NULL) {
                        if (TaskCreate(task_tty, TTYD_NAME, TTYD_STACK_SIZE,
                                       NULL, -1, &term->taskHdl) == OS_OK) {

                                term->col = 80;
                                term->row = 24;

                                term->changeToTTY = -1;

                                return STD_RET_OK;
                        }
                }

                if (term->semcnt) {
                        DeleteSemCnt(term->semcnt);
                }

                free(term);
                term = NULL;
        }

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

        if (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) == OS_OK) {
                mutex_t mtx = TTY(dev)->mtx;

                /* free unused terminal */
                for (u8_t i = 0; i < TTY_MAX_LINES; i++) {
                        if (TTY(dev)->line[i]) {
                                free(TTY(dev)->line[i]);
                        }
                }

                free(TTY(dev));
                TTY(dev) = NULL;

                GiveRecMutex(mtx);
                DeleteRecMutex(mtx);

                /* if all terminal are unused free terminal handler */
                for (u8_t i = 0; i < TTY_LAST; i++) {
                        if (term->tty[i]) {
                                return STD_RET_OK;
                        }
                }

                DeleteSemCnt(term->semcnt);
                TaskDelete(term->taskHdl);
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
              if ((TTY(dev)->mtx = CreateRecMutex()) != NULL) {
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
                TaskDelay(10);
        }

        /* wait for secure access to data */
        if (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) == OS_OK) {
                ch_t *msg;

                /* check if screen is cleared */
                if (strncmp(VT100_CLEAR_SCREEN, src, 4) == 0) {
                        clear_tty(dev);
                }

                /* check if last message has not line ending */
                ch_t   *lstmsg  = TTY(dev)->line[get_message_index(dev, 1)];
                size_t  msgsize = strlen(lstmsg);

                if (lstmsg && (*(lstmsg + msgsize - 1) != '\n')) {
                        msgsize += nitems + 1;

                        /* create buffer for new msg */
                        msg = malloc(msgsize);

                        if (msg) {
                                /* move message pointer back */
                                TTY(dev)->wrIdx--;

                                if (TTY(dev)->wrIdx >= TTY_MAX_LINES) {
                                        TTY(dev)->wrIdx = TTY_MAX_LINES - 1;
                                }

                                /* if no message to refresh setup refresh only one */
                                if (TTY(dev)->newMsgCnt == 0) {
                                        TTY(dev)->refLstLn = SET;
                                }

                                strcpy(msg, lstmsg);
                                strncat(msg, src, nitems + 1);

                                /* free last message and add new on the same place */
                                add_message(dev, msg, FALSE);
                        }
                } else {
                        /* add new message */
                        msg = malloc(nitems);

                        if (msg) {
                                strncpy(msg, src, nitems);

                                add_message(dev, msg, TRUE);
                        }
                }

                n = nitems;

                GiveRecMutex(TTY(dev)->mtx);
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

        if (TakeRecMutex(TTY(dev)->mtx, 0) == OS_OK) {
                while (nitems > 0) {
                        if (get_from_input_stream(dst, dev) != STD_RET_OK) {
                                break;
                        }

                        dst += size;
                        nitems--;
                        n++;
                }

                n /= size;

                GiveSemCnt(term->semcnt);

                GiveRecMutex(TTY(dev)->mtx);
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
                GiveSemCnt(term->semcnt);
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
                if (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) == OS_OK) {
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

                        GiveRecMutex(TTY(dev)->mtx);
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
                TaskDelay(250);
        }

        msg = VT100_RESET_ATTRIBUTES VT100_CLEAR_SCREEN VT100_DISABLE_LINE_WRAP
              VT100_CURSOR_HOME;

        fwrite(msg, sizeof(ch_t), strlen(msg), ttys);

        get_vt100_size(ttys);

        for (;;) {
                if (TTY(term->currentTTY) == NULL) {
                        TaskDelay(100);
                        continue;
                }

                stdout_service(ttys, term->currentTTY);

                stdin_service(ttys, term->currentTTY);

                /* external trigger: terminal switch */
                if (term->changeToTTY != -1) {
                        term->currentTTY  = term->changeToTTY;
                        term->changeToTTY = -1;
                        refresh_tty(term->currentTTY, ttys);
                }

                /* check if task can go to short sleep */
                if (TTY(term->currentTTY)->newMsgCnt == 0) {
                        TakeSemCnt(term->semcnt, BLOCK_TIME);
                }
        }

        /* this should never happen */
        TaskTerminate();
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
        ch_t *msg;

        if (TTY(dev)->newMsgCnt > 0) {
                while (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) != OS_OK);

                while (TTY(dev)->newMsgCnt) {
                        if (TTY(dev)->newMsgCnt > term->row) {
                                TTY(dev)->newMsgCnt = term->row;
                        }

                        msg = TTY(dev)->line[get_message_index(term->currentTTY,
                                                       TTY(dev)->newMsgCnt)];

                        TTY(dev)->newMsgCnt--;

                        if (msg) {
                                fwrite(msg, sizeof(ch_t), strlen(msg), stream);
                        }
                }

                TTY(dev)->refLstLn = RESET;

                GiveRecMutex(TTY(dev)->mtx);
        } else if (TTY(dev)->refLstLn == SET) {
                while (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) != OS_OK);

                msg = VT100_CURSOR_OFF
                      VT100_CARRIAGE_RETURN
                      VT100_LINE_ERASE_FROM_CUR
                      VT100_RESET_ATTRIBUTES;

                fwrite(msg, sizeof(ch_t), strlen(msg), stream);

                /* refresh line */
                msg = TTY(dev)->line[get_message_index(term->currentTTY, 1)];
                fwrite(msg, sizeof(ch_t), strlen(msg), stream);

                /* cursor on */
                msg = VT100_CURSOR_ON;
                fwrite(msg, sizeof(ch_t), strlen(msg), stream);

                TTY(dev)->refLstLn = RESET;

                GiveRecMutex(TTY(dev)->mtx);
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
        ch_t chr;
        enum keyfn keyfn;

        if (ioctl(stream, UART_IORQ_GET_BYTE, &chr) != STD_RET_OK) {
                return;
        }

        keyfn = decode_fn_keys(chr);

        /* no Fn key was detected */
        if (keyfn == TTY_SEL_NONE) {
                if (chr == '\r' || chr == '\n') {
                        chr = '\n';
                        move_line_to_stream(dev);
                } else if (chr == '\b') {
                        if (  TTY(dev)->editLineFill > 0
                           && TTY(dev)->cursorPosition > 0) {
                                TTY(dev)->editLine[--TTY(dev)->cursorPosition] = '\0';
                                TTY(dev)->editLineFill--;
                        } else {
                                chr = '\0';
                        }
                } else {
                        if (TTY(dev)->editLineFill < TTY_EDIT_LINE_LEN) {
                                TTY(dev)->editLine[TTY(dev)->cursorPosition++] = chr;
                                TTY(dev)->editLineFill++;
                        }
                }

                if (TTY(dev)->echoOn == SET && chr != '\0') {
                        if (chr == '\n') {
                                chr = '\r';
                                ioctl(stream, UART_IORQ_SEND_BYTE, &chr);
                                chr = '\n';
                        }

                        ioctl(stream, UART_IORQ_SEND_BYTE, &chr);
                }
        } else if ( (keyfn <= TTY4_SELECTED) && (keyfn != term->currentTTY) ) { /* Fn key was hit */
                term->currentTTY = keyfn;

                if (TTY(keyfn) == NULL) {
                        TTY_Open(keyfn, TTY_PART_NONE);
                }

                if (TTY(keyfn)) {
                        refresh_tty(keyfn, stream);
                }
        }
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
        if (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) == OS_OK) {
                for (u8_t i = 0; i < TTY_MAX_LINES; i++) {
                        if (TTY(dev)->line[i]) {
                                free(TTY(dev)->line[i]);
                                TTY(dev)->line[i] = NULL;
                        }
                }

                TTY(dev)->newMsgCnt = 0;
                TTY(dev)->wrIdx     = 0;
                TTY(dev)->refLstLn  = RESET;

                GiveSemCnt(term->semcnt);

                GiveRecMutex(TTY(dev)->mtx);
        }
}

//==============================================================================
/**
 * @brief Add new message or modify existing
 *
 * @param dev           number of terminal
 * @param *msg          message
 * @param incMsgCnt     TRUE: increase msg counter; FALSE no increase
 */
//==============================================================================
static void add_message(u8_t dev, ch_t *msg, bool_t incMsgCnt)
{
        u8_t wridx = term->tty[dev]->wrIdx;
        u8_t mgcnt = 1;

        /* find all messages which are not ended by LF (this is common line) */
        for (u8_t i = 0; i < TTY_MAX_LINES; i++) {
                ch_t *lastmsg = TTY(dev)->line[wridx++];

                if (wridx >= TTY_MAX_LINES) {
                        wridx = 0;
                }

                if (lastmsg == NULL) {
                        break;
                }

                if (*(lastmsg + strlen(lastmsg) - 1) != '\n') {
                        mgcnt++;
                } else {
                        break;
                }
        }

        /* free current line and group if exist */
        wridx = TTY(dev)->wrIdx;
        while (mgcnt--) {
                free(TTY(dev)->line[wridx]);
                TTY(dev)->line[wridx++] = NULL;

                if (wridx >= TTY_MAX_LINES) {
                        wridx = 0;
                }
        }

        /* add new message */
        TTY(dev)->line[TTY(dev)->wrIdx++] = msg;

        if (TTY(dev)->wrIdx >= TTY_MAX_LINES) {
                TTY(dev)->wrIdx = 0;
        }

        /* increase new message counter */
        if (incMsgCnt) {
                if (TTY(dev)->newMsgCnt < TTY_MAX_LINES) {
                        TTY(dev)->newMsgCnt++;
                }
        }

        GiveSemCnt(term->semcnt);
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
 * @brief Check if pushed button is F1-F4
 *
 * @param character     button part of code
 */
//==============================================================================
static enum keyfn decode_fn_keys(ch_t character)
{
        switch (term->decodeFnStep) {
        case 0:
                if (character == '\e') {
                        term->decodeFnStep++;
                        return TTY_SEL_PEND;
                } else {
                        term->decodeFnStep = 0;
                }
                break;

        case 1:
                if (character == 'O') {
                        term->decodeFnStep++;
                        return TTY_SEL_PEND;
                } else {
                        term->decodeFnStep = 0;
                }
                break;

        case 2:
                term->decodeFnStep = 0;

                if (character == 'P') {
                        return TTY1_SELECTED;
                } else if (character == 'Q') {
                        return TTY2_SELECTED;
                } else if (character == 'R') {
                        return TTY3_SELECTED;
                } else if (character == 'S') {
                        return TTY4_SELECTED;
                }

                break;

        default:
                term->decodeFnStep = 0;
                break;
        }

        return TTY_SEL_NONE;
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

        if (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) == OS_OK) {
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

                GiveRecMutex(TTY(dev)->mtx);
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
                        TaskDelay(100);
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
                        TaskDelay(10);
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
static void put_into_input_stream(ch_t chr, u8_t dev)
{
        if (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) == OS_OK) {
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

                GiveRecMutex(TTY(dev)->mtx);
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
static stdRet_t get_from_input_stream(ch_t *chr, u8_t dev)
{
        if (TTY(dev)->input.level == 0) {
                return STD_RET_ERROR;
        }

        if (TakeRecMutex(TTY(dev)->mtx, 0) == OS_OK) {
                *chr = TTY(dev)->input.buffer[TTY(dev)->input.rxidx++];

                if (TTY(dev)->input.rxidx >= TTY_STREAM_SIZE) {
                        TTY(dev)->input.rxidx = 0;
                }

                TTY(dev)->input.level--;

                GiveRecMutex(TTY(dev)->mtx);
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function move edit line to stream
 *
 * @param [in]  dev             device
 */
//==============================================================================
static void move_line_to_stream(u8_t dev)
{
        uint_t line_len = strlen(TTY(dev)->editLine);

        for (uint_t i = 0; i < line_len; i++) {
                put_into_input_stream(TTY(dev)->editLine[i], dev);
        }

        put_into_input_stream('\n', dev);

        ch_t *msg = calloc(line_len + 2, sizeof(ch_t));

        if (msg) {
                strcpy(msg, TTY(dev)->editLine);
                strcat(msg, "\r\n");
                add_message(dev, msg, FALSE);
        }

        memset(TTY(dev)->editLine, '\0', TTY_EDIT_LINE_LEN);
        TTY(dev)->editLineFill   = 0;
        TTY(dev)->cursorPosition = 0;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
