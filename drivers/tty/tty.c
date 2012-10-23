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


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
/* memory structure */
struct termHdl
{
      struct ttyEntry
      {
            ch_t   *line[TTY_MAX_LINES];        /* line buffer */
            u8_t   wrIdx;                       /* write index */
            u8_t   newMsgCnt;                   /* new message counter */

            flag_t refLstLn;                    /* request to refresh last line */
            flag_t lock;                        /* access lock */

            /* FIFO used in keyboard read */
            struct inputfifo
            {
                  u16_t level;
                  ch_t  buffer[TTY_OUT_BFR_SIZE];
                  u16_t txidx;
                  u16_t rxidx;
            } input;
      } *tty[TTY_LAST];

      u8_t currentTTY;
      i8_t changeTTY;
      taskHdl_t taskHdl;
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
static void  WaitForAccess(u8_t dev);
static void  GiveAccess(u8_t dev);
static void  AddMsg(u8_t dev, ch_t *msg, u8_t back);
static dfn_t decFn(ch_t character);
static u8_t  GetMsgIdx(u8_t dev, u8_t back);
static void  RefreshTTY(u8_t dev, FILE_t *file);


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
 *
 * @retval STD_RET_OK
 */
//================================================================================================//
stdRet_t TTY_Init(nod_t dev)
{
      stdRet_t status = STD_RET_ERROR;

      if (dev < TTY_LAST)
      {

            if (term == NULL)
            {
                  term = Calloc(1, sizeof(struct termHdl));

                  if (term)
                  {
                        if (TaskCreate(ttyd, TTYD_NAME,    TTYD_STACK_SIZE,
                                       NULL, Priority(-1), &term->taskHdl) )
                        {
                               status = STD_RET_OK;
                        }
                        else
                        {
                              Free(term);
                        }
                  }
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Opens specified port and initialize default settings
 *
 * @param[in]  dev                        TTY number
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  operation error
 */
//================================================================================================//
stdRet_t TTY_Open(nod_t dev)
{
      stdRet_t status = STD_RET_ERROR;

      if (dev < TTY_LAST)
      {
            if (term)
            {
                  if (term->tty[dev] == NULL)
                  {
                        term->tty[dev] = Calloc(1, sizeof(struct ttyEntry));
                  }

                  if (term->tty[dev])
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
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  operation error
 */
//================================================================================================//
stdRet_t TTY_Close(nod_t dev)
{
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
 * @param[in]  *src                       source buffer
 * @param[in]  size                       buffer size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @retval number of written nitems
 */
//================================================================================================//
size_t TTY_Write(nod_t dev, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)seek;

      size_t n = 0;

      if (dev < TTY_LAST && src && size && nitems)
      {
            /* wait for refresh last modified/joined line */
            while ((term->tty[dev]->refLstLn == SET) && (dev == term->currentTTY))
            {
                  Sleep(10);
            }

            /* check if screen is cleared */
            if (strncmp("\x1B[2J", src, 4) == 0)
            {
                  ClearTTY(dev);
            }

            /* check if previous message had not line end (\n) */
            u8_t lstidx   = term->tty[dev]->wrIdx;
            ch_t *lastmsg = term->tty[dev]->line[lstidx - 1];

            if ( (*(lastmsg + strlen(lastmsg) - 1) != '\n') )
            {
                  /* wait for all new messages are showed */
                  while (term->tty[dev]->newMsgCnt && (dev == term->currentTTY))
                  {
                        Sleep(10);
                  }


                  WaitForAccess(dev);

                  size_t newsize = strlen(lastmsg) + size + 1;

                  ch_t *modmsg = Malloc(newsize);

                  if (modmsg)
                  {
                        strcpy(modmsg, lastmsg);
                        strncat(modmsg, src, size);
                        *(modmsg + newsize - 1) = '\0';

                        Free(lastmsg);

                        AddMsg(dev, modmsg, lstidx - term->tty[dev]->wrIdx);

                        if (dev == term->currentTTY)
                        {
                              term->tty[dev]->refLstLn = SET;
                        }
                  }
            }
            else
            {
                  ch_t *localMsg = Malloc(sizeof(ch_t) * size + 1);

                  if (localMsg)
                  {
                        strncpy(localMsg, src, size + 1);

                        WaitForAccess(dev);

                        AddMsg(dev, localMsg, 0);
                  }
            }

            GiveAccess(dev);

            n = size;
      }

      return n;
}


//================================================================================================//
/**
 * @brief Write data to TTY
 *
 * @param[in]  dev                        TTY number
 * @param[in]  *dst                       destination buffer
 * @param[in]  size                       item size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @retval number of read nitems
 */
//================================================================================================//
size_t TTY_Read(nod_t dev, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)seek;

      size_t n = 0;

      if (dev < TTY_LAST && dst && size && nitems)
      {
            WaitForAccess(dev);

            struct inputfifo *input = &term->tty[dev]->input;

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

            GiveAccess(dev);
      }

      return n;
}


//================================================================================================//
/**
 * @brief Specific settings of TTY
 *
 * @param[in    ] dev           TTY device
 * @param[in    ] ioRQ          input/output request
 * @param[in,out] *data         input/output data
 *
 * @retval STD_RET_OK           operation success
 * @retval STD_RET_ERROR        operation error
 */
//================================================================================================//
stdRet_t TTY_IOCtl(nod_t dev, IORq_t ioRQ, void *data)
{
      stdRet_t status = STD_RET_ERROR;

      if (dev < TTY_LAST)
      {
            switch (ioRQ)
            {
                  /* return current TTY */
                  case TTY_IORQ_GETCURRENTTTY:
                        *((u8_t*)data) = term->currentTTY;
                        status = STD_RET_OK;
                        break;

                  /* set active terminal */
                  case TTY_IORQ_SETACTIVETTY:
                        term->changeTTY = *((u8_t*)data);
                        break;

                  default:
                        break;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Release TTY device
 *
 * @param dev     device number
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t TTY_Release(nod_t dev)
{
      stdRet_t status = STD_RET_ERROR;

      if (dev < TTY_LAST)
      {
            /* free unused terminal */
            if (term->tty[dev])
            {
                  for (u8_t i = 0; i < TTY_MAX_LINES; i++)
                  {
                        Free(term->tty[dev]->line);
                  }
            }

            Free(term->tty[dev]);

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
            Free(term);
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

      ch_t   chr        = 0;
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

            Sleep(250);
      }

      /* configure terminal VT100: clear screen, line wrap, cursor HOME */
      ch_t *termCfg = "\x1B[2J\x1B[?7h\x1B[H";
      fwrite(termCfg, sizeof(ch_t), strlen(termCfg), ttys);

      /* main daemon loop */
      for (;;)
      {
            /* pointer to current terminal */
            struct ttyEntry *ttyPtr = term->tty[term->currentTTY];

            /* STDOUT support ------------------------------------------------------------------- */
            if ((ttyPtr->refLstLn == SET) && ttyPtr)
            {
                  /* cursor off, carriage return, line erase from cursor, font attributes reset */
                  msg = "\x1B[?25l\r\x1B[K\x1B[0m";
                  fwrite(msg, sizeof(ch_t), strlen(msg), ttys);

                  /* refresh line */
                  msg = ttyPtr->line[GetMsgIdx(term->currentTTY, 0)];
                  fwrite(msg, sizeof(ch_t), strlen(msg), ttys);

                  /* cursor on */
                  msg = "\x1B[?25h";
                  fwrite(msg, sizeof(ch_t), strlen(msg), ttys);

                  ttyPtr->refLstLn = RESET;
            }
            else if ((ttyPtr->newMsgCnt > 0) && ttyPtr)
            {
                  msg = ttyPtr->line[GetMsgIdx(term->currentTTY, ttyPtr->newMsgCnt)];

                  if (ttyPtr->newMsgCnt)
                  {
                        ttyPtr->newMsgCnt--;
                  }

                  if (msg)
                  {
                        fwrite(msg, sizeof(ch_t), strlen(msg), ttys);
                  }
            }

            /* STDIN support -------------------------------------------------------------------- */
            if (ioctl(ttys, UART_IORQ_GET_BYTE, &chr) == STD_RET_OK)
            {
                  keyfn = decFn(chr);

                  /* no Fn key was detected */
                  if (keyfn == TTY_SEL_NONE && ttyPtr)
                  {
                        WaitForAccess(term->currentTTY);

                        if (ttyPtr->input.level < TTY_OUT_BFR_SIZE)
                        {
                              ttyPtr->input.buffer[ttyPtr->input.txidx++] = chr;

                              if (ttyPtr->input.txidx >= TTY_OUT_BFR_SIZE)
                              {
                                    ttyPtr->input.txidx = 0;
                              }

                              ttyPtr->input.level++;
                        }

                        GiveAccess(term->currentTTY);
                  }
                  /* Fn key was hit */
                  else if ( (keyfn <= TTY4_SELECTED) && (keyfn != term->currentTTY) )
                  {
                        term->currentTTY = keyfn;

                        if (term->tty[term->currentTTY] == NULL)
                        {
                              TTY_Open(keyfn);
                        }

                        if (term->tty[term->currentTTY])
                        {
                              RefreshTTY(term->currentTTY, ttys);
                        }
                  }
            }
            else
            {
                  rxbfrempty = TRUE;
            }

            /* external trigger: terminal switch */
            if ((term->changeTTY != -1) && ttyPtr)
            {
                  term->currentTTY = term->changeTTY;
                  term->changeTTY  = -1;
                  RefreshTTY(term->currentTTY, ttys);
            }

            /* check if task can go to short sleep */
            if ((ttyPtr->newMsgCnt == 0) && rxbfrempty)
            {
                  Sleep(20);
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
      WaitForAccess(dev);

      for (u8_t i = 0; i < TTY_LAST; i++)
      {
            Free(term->tty[dev]->line[i]);
            term->tty[dev]->line[i] = NULL;
      }

      term->tty[dev]->newMsgCnt = 0;

      GiveAccess(dev);
}


//================================================================================================//
/**
 * @brief Function wait to safety access to data
 *
 * @param dev     number of terminal
 */
//================================================================================================//
static void WaitForAccess(u8_t dev)
{
      while (TRUE)
      {
            TaskSuspendAll();
            if (term->tty[dev]->lock == SET)
            {
                  TaskResumeAll();
                  Sleep(2);
            }
            else
            {
                  term->tty[dev]->lock = SET;
                  TaskResumeAll();
                  break;
            }
      }
}


//================================================================================================//
/**
 * @brief Function give safety access to data
 *
 * @param tty     number of terminal
 */
//================================================================================================//
static void GiveAccess(u8_t dev)
{
      TaskSuspendAll();
      term->tty[dev]->lock = RESET;
      TaskResumeAll();
}



//================================================================================================//
/**
 * @brief Add new message or modifi existing
 *
 * @param dev           number of terminal
 * @param *msg          message
 * @param back          number of lines from current index
 */
//================================================================================================//
static void AddMsg(u8_t dev, ch_t *msg, u8_t back)
{
      if (back > TTY_MAX_LINES)
      {
            u8_t widx;

            /* check if index underflow when calculating with back */
            if ((i8_t)(term->tty[dev]->wrIdx - back) < 0)
            {
                  widx = TTY_MAX_LINES - (back - term->tty[dev]->wrIdx);
            }
            else
            {
                  widx = term->tty[dev]->wrIdx - back;
            }

            /* free last message */
            Free(term->tty[dev]->line[widx]);

            /* add new message */
            term->tty[dev]->line[widx] = msg;

            /* if back message wasn't changed */
            if (back == 0)
            {
                  /* increase write index */
                  term->tty[dev]->wrIdx++;

                  if (term->tty[dev]->wrIdx >= TTY_MAX_LINES)
                  {
                        term->tty[dev]->wrIdx = 0;
                  }

                  /* increase new message counter */
                  if (term->tty[dev]->newMsgCnt < TTY_MAX_LINES)
                  {
                        term->tty[dev]->newMsgCnt++;
                  }
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
      u8_t ridx;

      /* check if index underflow when calculating with back */
      if ((i8_t)(term->tty[dev]->wrIdx - back) <= 0)
      {
            ridx = TTY_MAX_LINES - (back - term->tty[dev]->wrIdx) - 1;
      }
      else
      {
            ridx = term->tty[dev]->wrIdx - back - 1;
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
      ch_t *clrscr = "\x1B[2J\x1B[?7h\x1B[H\x1B[0m";

      fwrite(clrscr, sizeof(ch_t), strlen(clrscr), file);

      for (i8_t i = TTY_MAX_LINES - 1; i >= 0; i--)
      {
            ch_t *msg = term->tty[dev]->line[GetMsgIdx(term->currentTTY, i)];

            if (msg)
            {
                  fwrite(msg, sizeof(ch_t), strlen(msg), file);
            }

            if (term->tty[dev]->newMsgCnt > 0)
            {
                  term->tty[dev]->newMsgCnt--;
            }
      }
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
