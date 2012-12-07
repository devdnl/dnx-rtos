/*=============================================================================================*//**
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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "tty.h"
#include <string.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define FILE                        "/dev/ttyS0"
#define TTY_OUT_BFR_SIZE            16

#define TTY(number)                 term->tty[number]
#define BLOCK_TIME                  10000


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
/* memory structure */
struct termHdl
{
      struct ttyEntry
      {
            ch_t    *line[TTY_MAX_LINES];        /* line buffer */
            u8_t    wrIdx;                       /* write index */
            u8_t    newMsgCnt;                   /* new message counter */
            flag_t  refLstLn;                    /* request to refresh last line */
            mutex_t mtx;                        /* resources security */

            /* FIFO used in keyboard read */
            struct inputfifo
            {
                  u16_t level;
                  ch_t  buffer[TTY_OUT_BFR_SIZE];
                  u16_t txidx;
                  u16_t rxidx;
            } input;
      } *tty[TTY_LAST];

      u8_t curTTY;
      i8_t chTTY;
      u8_t col;
      u8_t row;
      task_t taskHdl;
};


/* key detector results */
typedef enum
{
      TTY1_SELECTED,
      TTY2_SELECTED,
      TTY3_SELECTED,
      TTY4_SELECTED,
      TTY_SEL_PEND,
      TTY_SEL_NONE
} dfn_t;


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static void  ttyd(void *arg);
static void  ClearTTY(u8_t dev);
static void  AddMsg(u8_t dev, ch_t *msg, bool_t incMsgCnt);
static dfn_t decFn(ch_t character);
static u8_t  GetMsgIdx(u8_t dev, u8_t back);
static void  RefreshTTY(u8_t dev, FILE_t *file);
static void  GetTermSize(FILE_t *ttysfile);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
/* memory used by driver */
static struct termHdl *term;


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize USART devices
 *
 * @param dev     device number
 * @param part    device part
 *
 * @retval STD_RET_OK
 */
//================================================================================================//
stdRet_t TTY_Init(devx_t dev, fd_t part)
{
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (dev < TTY_LAST)
      {

            if (term == NULL)
            {
                  term = calloc(1, sizeof(struct termHdl));

                  if (term)
                  {
                        if (TaskCreate(ttyd, TTYD_NAME,    TTYD_STACK_SIZE,
                                       NULL, Priority(-1), &term->taskHdl) == OS_OK )
                        {
                              term->col   = 80;
                              term->row   = 24;
                              term->chTTY = -1;

                              status = STD_RET_OK;
                        }
                        else
                        {
                              free(term);
                        }
                  }
            }
            else
            {
                  status = STD_RET_OK;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Opens specified port and initialize default settings
 *
 * @param[in]  dev                        TTY number
 * @param[in]  part                       device part
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  operation error
 */
//================================================================================================//
stdRet_t TTY_Open(devx_t dev, fd_t part)
{
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (dev < TTY_LAST)
      {
            if (term)
            {
                  if (TTY(dev) == NULL)
                  {
                        TTY(dev) = calloc(1, sizeof(struct ttyEntry));

                        if (TTY(dev))
                        {
                              CreateRecMutex(TTY(dev)->mtx);

                              if (TTY(dev)->mtx != NULL)
                              {
                                    status = STD_RET_OK;
                              }
                              else
                              {
                                    free(TTY(dev));
                              }
                        }
                  }
                  else
                  {
                        status = STD_RET_OK;
                  }
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function close opened port
 *
 * @param[in]  dev                        TTY number
 * @param[in]  part                       device part
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  operation error
 */
//================================================================================================//
stdRet_t TTY_Close(devx_t dev, fd_t part)
{
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (dev < TTY_LAST)
      {
            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
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
//================================================================================================//
size_t TTY_Write(devx_t dev, fd_t part, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)seek;
      (void)part;

      size_t n = 0;

      if (dev < TTY_LAST && src && size && nitems)
      {
            /* if current TTY is showing wait to show refreshed line */
            while (TTY(dev)->refLstLn == SET && dev == term->curTTY)
                  TaskDelay(10);

            /* wait for secure access to data */
            if (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) == OS_OK)
            {
                  ch_t *msg;

                  /* check if screen is cleared */
                  if (strncmp("\x1B[2J", src, 4) == 0)
                  {
                        ClearTTY(dev);
                  }

                  /* check that last message had no line end */
                  ch_t   *lstmsg = TTY(dev)->line[GetMsgIdx(dev, 1)];
                  size_t msgsize = strlen(lstmsg);

                  if (lstmsg && (*(lstmsg + msgsize - 1) != '\n'))
                  {
                        msgsize += nitems + 1;

                        /* create buffer for new msg */
                        msg = malloc(msgsize);

                        if (msg)
                        {
                              /* move message pointer back */
                              TTY(dev)->wrIdx--;

                              if (TTY(dev)->wrIdx >= TTY_MAX_LINES)
                              {
                                    TTY(dev)->wrIdx = TTY_MAX_LINES - 1;
                              }

                              /* if no message to refresh setup refresh only one */
                              if (TTY(dev)->newMsgCnt == 0)
                              {
                                    TTY(dev)->refLstLn = SET;
                              }

                              strcpy(msg, lstmsg);
                              strncat(msg, src, nitems + 1);

                              /* free last message and add new on the same place */
                              AddMsg(dev, msg, FALSE);
                        }
                  }
                  else
                  {
                        /* add new message */
                        msg = malloc(nitems);

                        if (msg)
                        {
                              strncpy(msg, src, nitems);

                              AddMsg(dev, msg, TRUE);
                        }
                  }

                  n = nitems;

                  GiveRecMutex(TTY(dev)->mtx);
            }
      }

      return n;
}


//================================================================================================//
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
//================================================================================================//
size_t TTY_Read(devx_t dev, fd_t part, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)seek;
      (void)part;

      size_t n = 0;

      if (dev < TTY_LAST && dst && size && nitems)
      {
            if (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) == OS_OK)
            {
                  struct inputfifo *input = &TTY(dev)->input;

                  while ((input->level > 0) && (nitems > 0))
                  {
                        *((ch_t*)dst) = input->buffer[input->rxidx++];

                        dst += size;

                        if (input->rxidx >= TTY_OUT_BFR_SIZE)
                        {
                              input->rxidx = 0;
                        }

                        input->level--;

                        nitems--;

                        n++;
                  }

                  n /= size;

                  GiveRecMutex(TTY(dev)->mtx);
            }
      }

      return n;
}


//================================================================================================//
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
//================================================================================================//
stdRet_t TTY_IOCtl(devx_t dev, fd_t part, IORq_t ioRQ, void *data)
{
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (dev < TTY_LAST)
      {
            status = STD_RET_OK;

            switch (ioRQ)
            {
                  /* return current TTY */
                  case TTY_IORQ_GETCURRENTTTY:
                  {
                        *((u8_t*)data) = term->curTTY;
                        break;
                  }

                  /* set active terminal */
                  case TTY_IORQ_SETACTIVETTY:
                  {
                        term->chTTY = *((u8_t*)data);
                        break;
                  }

                  /* clear terminal */
                  case TTY_IORQ_CLEARTTY:
                  {
                        ClearTTY(dev);
                        break;
                  }

                  /* terminal size - number of columns */
                  case TTY_IORQ_GETCOL:
                  {
                        *((u8_t*)data) = term->col;
                        break;
                  }

                  /* terminal size - number of rows */
                  case TTY_IORQ_GETROW:
                  {
                        *((u8_t*)data) = term->row;
                        break;
                  }

                  /* clear screen */
                  case TTY_IORQ_CLEARSCR:
                  {
                        ClearTTY(dev);

                        /* this forces screen refresh */
                        term->chTTY = term->curTTY;

                        break;
                  }

                  /* clear last line */
                  case TTY_IORQ_CLEARLASTLINE:
                  {
                        if (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) == OS_OK)
                        {
                              ch_t *msg = malloc(2);

                              if (msg)
                              {
                                    msg[0] = '\r';
                                    msg[1] = '\0';

                                    /* move message pointer back */
                                    TTY(dev)->wrIdx--;

                                    if (TTY(dev)->wrIdx >= TTY_MAX_LINES)
                                    {
                                          TTY(dev)->wrIdx = TTY_MAX_LINES - 1;
                                    }

                                    /* if no message to refresh setup refresh only one */
                                    if (TTY(dev)->newMsgCnt == 0)
                                    {
                                          TTY(dev)->refLstLn = SET;
                                    }

                                    AddMsg(dev, msg, FALSE);
                              }

                              GiveRecMutex(TTY(dev)->mtx);
                        }

                        break;
                  }

                  default:
                  {
                        status = STD_RET_ERROR;
                        break;
                  }
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Release TTY device
 *
 * @param dev     device number
 * @param part    device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t TTY_Release(devx_t dev, fd_t part)
{
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (dev < TTY_LAST)
      {
            if (TTY(dev))
            {
                  if (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) == OS_OK)
                  {
                        mutex_t mtx = TTY(dev)->mtx;

                        /* free unused terminal */
                        if (TTY(dev))
                        {
                              for (u8_t i = 0; i < TTY_MAX_LINES; i++)
                              {
                                    free(TTY(dev)->line[i]);
                              }
                        }

                        free(TTY(dev));

                        GiveRecMutex(mtx);
                        DeleteRecMutex(mtx);

                        status = STD_RET_OK;

                        /* if all terminal are unused free terminal handler */
                        for (u8_t i = 0; i < TTY_LAST; i++)
                        {
                              if (term->tty[i])
                              {
                                    goto TTY_Release_End;
                              }
                        }

                        TaskDelete(term->taskHdl);
                        free(term);
                  }
            }
            else
            {
                  status = STD_RET_OK;
            }
      }

      TTY_Release_End:
      return status;
}


//================================================================================================//
/**
 * @brief TTY daemon
 */
//================================================================================================//
static void ttyd(void *arg)
{
      (void) arg;

      ch_t   chr;
      bool_t rxbfrempty = FALSE;
      ch_t   *msg;
      FILE_t *ttys;
      dfn_t  keyfn;

      /* try open selected file */
      while (TRUE)
      {
            ttys = fopen(FILE, "r+");

            if (ttys != NULL)
            {
                  break;
            }

            TaskDelay(250);
      }

      /* configure terminal VT100: reset attributes, clear screen, line wrap, cursor HOME */
      ch_t *termCfg = "\x1B[0m\x1B[2J\x1B[?7h\x1B[H";
      fwrite(termCfg, sizeof(ch_t), strlen(termCfg), ttys);

      GetTermSize(ttys);

      /* main daemon loop */
      for (;;)
      {
            /* pointer to current terminal */
            struct ttyEntry *ttyPtr = TTY(term->curTTY);

            /* STDOUT support ------------------------------------------------------------------- */
            if ((ttyPtr->newMsgCnt > 0) && ttyPtr)
            {
                  if (TakeRecMutex(ttyPtr->mtx, BLOCK_TIME) == OS_OK)
                  {
                        if (ttyPtr->newMsgCnt > term->row)
                              ttyPtr->newMsgCnt = term->row;

                        msg = ttyPtr->line[GetMsgIdx(term->curTTY, ttyPtr->newMsgCnt)];

                        ttyPtr->newMsgCnt--;

                        if (msg)
                        {
                              fwrite(msg, sizeof(ch_t), strlen(msg), ttys);
                        }

                        ttyPtr->refLstLn = RESET;

                        GiveRecMutex(ttyPtr->mtx);
                  }
            }
            else if ((ttyPtr->refLstLn == SET) && ttyPtr)
            {
                  if (TakeRecMutex(ttyPtr->mtx, BLOCK_TIME) == OS_OK)
                  {
                        /* cursor off, carriage return, line erase from cursor, font attributes reset */
                        msg = "\x1B[?25l\r\x1B[K\x1B[0m";
                        fwrite(msg, sizeof(ch_t), strlen(msg), ttys);

                        /* refresh line */
                        msg = ttyPtr->line[GetMsgIdx(term->curTTY, 1)];
                        fwrite(msg, sizeof(ch_t), strlen(msg), ttys);

                        /* cursor on */
                        msg = "\x1B[?25h";
                        fwrite(msg, sizeof(ch_t), strlen(msg), ttys);

                        ttyPtr->refLstLn = RESET;

                        GiveRecMutex(ttyPtr->mtx);
                  }
            }

            /* STDIN support -------------------------------------------------------------------- */
            if (ioctl(ttys, UART_IORQ_GET_BYTE, &chr) == STD_RET_OK)
            {
                  keyfn = decFn(chr);

                  /* no Fn key was detected */
                  if (keyfn == TTY_SEL_NONE && ttyPtr)
                  {
                        if (TakeRecMutex(ttyPtr->mtx, BLOCK_TIME) == OS_OK)
                        {
                              if (ttyPtr->input.level < TTY_OUT_BFR_SIZE)
                              {
                                    ttyPtr->input.buffer[ttyPtr->input.txidx++] = chr;

                                    if (ttyPtr->input.txidx >= TTY_OUT_BFR_SIZE)
                                    {
                                          ttyPtr->input.txidx = 0;
                                    }

                                    ttyPtr->input.level++;
                              }

                              GiveRecMutex(ttyPtr->mtx);
                        }
                  }
                  /* Fn key was hit */
                  else if ( (keyfn <= TTY4_SELECTED) && (keyfn != term->curTTY) )
                  {
                        term->curTTY = keyfn;

                        if (TTY(keyfn) == NULL)
                        {
                              TTY_Open(keyfn, TTY_PART_NONE);
                        }

                        if (TTY(keyfn))
                        {
                              RefreshTTY(keyfn, ttys);
                        }
                  }
            }
            else
            {
                  rxbfrempty = TRUE;
            }

            /* external trigger: terminal switch */
            if ((term->chTTY != -1) && ttyPtr)
            {
                  term->curTTY = term->chTTY;
                  term->chTTY  = -1;
                  RefreshTTY(term->curTTY, ttys);
            }

            /* check if task can go to short TaskDelay */
            if ((ttyPtr->newMsgCnt == 0) && rxbfrempty)
            {
                  TaskDelay(10);
            }
      }

      /* this should never happen */
      TaskTerminate();
}


//================================================================================================//
/**
 * @brief Clear all messages from terminal and free memory
 *
 * @param dev        terminal number
 */
//================================================================================================//
static void ClearTTY(u8_t dev)
{
      if (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) == OS_OK)
      {
            for (u8_t i = 0; i < TTY_MAX_LINES; i++)
            {
                  free(TTY(dev)->line[i]);
                  TTY(dev)->line[i] = NULL;
            }

            TTY(dev)->newMsgCnt = 0;
            TTY(dev)->wrIdx = 0;
            TTY(dev)->refLstLn = RESET;

            GiveRecMutex(TTY(dev)->mtx);
      }
}


//================================================================================================//
/**
 * @brief Add new message or modifi existing
 *
 * @param dev           number of terminal
 * @param *msg          message
 * @param incMsgCnt     TRUE: increase msg counter; FALSE no increase
 */
//================================================================================================//
static void AddMsg(u8_t dev, ch_t *msg, bool_t incMsgCnt)
{
      u8_t wridx = term->tty[dev]->wrIdx;
      u8_t mgcnt = 1;

      /* find all messages which are not ended by LF (this is common line) */
      for (u8_t i = 0; i < TTY_MAX_LINES; i++)
      {
            ch_t *lastmsg = TTY(dev)->line[wridx++];

            if (wridx >= TTY_MAX_LINES)
            {
                  wridx = 0;
            }

            if (lastmsg == NULL)
            {
                  break;
            }

            if (*(lastmsg + strlen(lastmsg) - 1) != '\n')
            {
                  mgcnt++;
            }
            else
            {
                  break;
            }
      }

      /* free current line and group if exist */
      wridx = TTY(dev)->wrIdx;
      while (mgcnt--)
      {
            free(TTY(dev)->line[wridx]);
            TTY(dev)->line[wridx++] = NULL;

            if (wridx >= TTY_MAX_LINES)
            {
                  wridx = 0;
            }
      }

      /* add new message */
      TTY(dev)->line[TTY(dev)->wrIdx++] = msg;

      if (TTY(dev)->wrIdx >= TTY_MAX_LINES)
      {
            TTY(dev)->wrIdx = 0;
      }

      /* increase new message counter */
      if (incMsgCnt)
      {
            if (TTY(dev)->newMsgCnt < TTY_MAX_LINES)
            {
                  TTY(dev)->newMsgCnt++;
            }
      }
}


//================================================================================================//
/**
 * @brief Get last or selected message
 *
 * @param dev           number of terminal
 * @param back          number of lines from current index
 *
 * @return index to message
 */
//================================================================================================//
static u8_t GetMsgIdx(u8_t dev, u8_t back)
{
      u8_t ridx = 0;

      /* check if index underflow when calculating with back */
      if (TTY(dev)->wrIdx < back)
      {
            ridx = TTY_MAX_LINES - (back - TTY(dev)->wrIdx);
      }
      else
      {
            ridx = TTY(dev)->wrIdx - back;
      }

      return ridx;
}


//================================================================================================//
/**
 * @brief Check if pushed button is F1-F4
 *
 * @param character     button part of code
 */
//================================================================================================//
static dfn_t decFn(ch_t character)
{
      static u8_t funcStep = 0;
      dfn_t state = TTY_SEL_NONE;

      /* try detect function keys ^[OP */
      switch (funcStep)
      {
            case 0:
            {
                  if (character == 0x1B)
                  {
                        funcStep++;
                        state = TTY_SEL_PEND;
                  }
                  else
                  {
                        funcStep = 0;
                  }
                  break;
            }

            case 1:
            {
                  if (character == 'O')
                  {
                        funcStep++;
                        state = TTY_SEL_PEND;
                  }
                  else
                  {
                        funcStep = 0;
                  }
                  break;
            }

            case 2:
            {
                  if (character == 'P')
                  {
                        state = TTY1_SELECTED;
                  }
                  else if (character == 'Q')
                  {
                        state = TTY2_SELECTED;
                  }
                  else if (character == 'R')
                  {
                        state = TTY3_SELECTED;
                  }
                  else if (character == 'S')
                  {
                        state = TTY4_SELECTED;
                  }

                  funcStep = 0;
                  break;
            }

            default:
            {
                  funcStep = 0;
            }
      }

      return state;
}


//================================================================================================//
/**
 * @brief Refresh selected TTY
 *
 * @param dev     number of terminal
 */
//================================================================================================//
static void RefreshTTY(u8_t dev, FILE_t *file)
{
      GetTermSize(file);

      /* reset attributes, clear screen, line wrap, cursor HOME */
      ch_t *clrscr = "\x1B[0m\x1B[2J\x1B[?7h\x1B[H";

      fwrite(clrscr, sizeof(ch_t), strlen(clrscr), file);

      if (TakeRecMutex(TTY(dev)->mtx, BLOCK_TIME) == OS_OK)
      {
            i8_t rows;

            if (term->row < TTY_MAX_LINES)
                  rows = term->row;
            else
                  rows = TTY_MAX_LINES;

            for (i8_t i = rows - 1; i > 0; i--)
            {
                  ch_t *msg = TTY(dev)->line[GetMsgIdx(term->curTTY, i)];

                  if (msg)
                  {
                        fwrite(msg, sizeof(ch_t), strlen(msg), file);
                  }
            }

            TTY(dev)->newMsgCnt = 0;

            GiveRecMutex(TTY(dev)->mtx);
      }
}


//================================================================================================//
/**
 * @brief Function gets terminal size
 */
//================================================================================================//
static void GetTermSize(FILE_t *ttysfile)
{
      ch_t chr = 0;

      /* set default values */
      term->col = 80;
      term->row = 24;

      /* save cursor position, cursor off, set max col and row position, query cursor position */
      ch_t *rq = "\x1B[s\x1B[?25l\x1B[250;250H\x1B[6n";
      fwrite(rq, sizeof(ch_t), strlen(rq), ttysfile);

      /* buffer for response */
      ch_t resp[10];
      resp[9] = '\0';

      /* waiting for ESC byte */
      u8_t try = 10;
      do
      {
            if (ioctl(ttysfile, UART_IORQ_GET_BYTE, &chr) != STD_RET_OK)
            {
                  TaskDelay(100);
            }

      } while (--try && chr != '\x1B');

      if (try == 0)
      {
            return;
      }

      /* get data */
      ch_t *respPtr = &resp[0];
      flag_t finded = RESET;
      for (u8_t i = 0; i < sizeof(resp) - 1; i++)
      {
            try = 10;

            while ((ioctl(ttysfile, UART_IORQ_GET_BYTE, respPtr) != STD_RET_OK) && try)
            {
                  TaskDelay(10);
                  try--;
            }

            if (*respPtr++ == 'R')
            {
                  finded = SET;
                  break;
            }
      }

      if (finded == RESET)
      {
            return;
      }

      /* restore cursor position, cursor on */
      rq = "\x1B[u\x1B[?25h";
      fwrite(rq, sizeof(ch_t), strlen(rq), ttysfile);

      /* find response begin */
      ch_t *seek = strchr(resp, '[');
      seek++;

      u8_t row = 0;
      u8_t col = 0;

      /* calculate row */
      while ((chr = *(seek++)) != ';')
      {
            row *= 10;
            row += chr - '0';
      }

      /* calculate columns */
      while ((chr = *(seek++)) != 'R')
      {
            col *= 10;
            col += chr - '0';
      }

      term->row = row;
      term->col = col;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
