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
#define TTY_OUT_BFR_SIZE            16


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
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
            struct
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
static void  RefreshTTY(u8_t dev, FILE_t *file);
static dfn_t decFn(ch_t character);
static void  ChangeTTY(u8_t dev);

static ch_t  *GetMsg(u8_t tty, u8_t msg);




/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
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
            u8_t msgcnt   = term->ttt[tty]->msgCnt;
            ch_t *lastmsg = ttyTerm[tty]->line[msgcnt - 1];

            if ( (msgcnt > 0) && (*(lastmsg + strlen(lastmsg) - 1) != '\n') )
            {
                  /* wait for all new messages are showed */
                  while (term->tty[dev]->newMsgCnt && (dev == term->currentTTY))
                  {
                        Sleep(10);
                  }


                  WaitForAccess(tty);

                  size_t newsize = strlen(lastmsg) + strlen(msg) + 1;

                  ch_t *modmsg = Malloc(newsize);

                  if (modmsg)
                  {
                        strcpy(modmsg, lastmsg);
                        strcat(modmsg, msg);

                        Free(lastmsg);

                        AddM

                        ttyTerm[tty]->line[msgcnt - 1] = modmsg;

                        if (tty == currentTTY)
                        {
                              ttyTerm[tty]->refLine = SET;
                        }
                  }
            }
            else
            {
                  ch_t *localMsg = Malloc(sizeof(ch_t) * strlen(msg) + 1);

                  if (localMsg)
                  {
                        strcpy(localMsg, msg);

                        WaitForAccess(tty);

                        if (ttyTerm[tty]->msgCnt >= TTY_MSGS)
                        {
                              Free(ttyTerm[tty]->line[0]);

                              for (u8_t i = 0; i < TTY_MSGS; i++)
                              {
                                    ttyTerm[tty]->line[i] = ttyTerm[tty]->line[i + 1];
                              }

                              ttyTerm[tty]->line[TTY_MSGS - 1] = localMsg;
                        }
                        else
                        {
                              ttyTerm[tty]->line[ttyTerm[tty]->msgCnt] = localMsg;
                        }

                        if (ttyTerm[tty]->msgCnt < TTY_MSGS)
                        {
                              ttyTerm[tty]->msgCnt++;
                        }

                        if (ttyTerm[tty]->newMsg < TTY_MSGS)
                        {
                              ttyTerm[tty]->newMsg++;
                        }
                  }
            }

            GiveAccess(tty);
      }

      AddTermMsg_end:
      return;
}


//================================================================================================//
/**
 * @brief Write data to TTY
 *
 * @param[in]  dev                        TTY number
 * @param[in]  *dst                       destination buffer
 * @param[in]  size                       buffer size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @retval number of read nitems
 */
//================================================================================================//
size_t TTY_Read(nod_t dev, void *dst, size_t size, size_t nitems, size_t seek)
{

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
      (void)data;

      stdRet_t status = STD_RET_ERROR;

      if (dev < TTY_LAST)
      {
            switch (ioRQ)
            {
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

      sugTTY = -1;

      ch_t     chr        = 0;
      bool_t   rxbfrempty = FALSE;
      ch_t     *msg;
      FILE_t   *uartf;
      dfn_t keyfn;
      struct ttyEntry *tty = NULL;

      EnableKprint();

      /* something about board and system */
      kprint("Board powered by \x1B[32mFreeRTOS\x1B[0m\n");
      kprint("By \x1B[36mDaniel Zorychta \x1B[33m<daniel.zorychta@gmail.com>\x1B[0m\n\n");

      InitDrv("uart1", "uart");

      uartf = fopen("/dev/uart", NULL);

      if (uartf == NULL)
      {
            TaskTerminate();
      }

      /* configure terminal VT100: clear screen, line wrap, cursor HOME */
      ch_t *termCfg = "\x1B[2J\x1B[?7h\x1B[H";
      fwrite(termCfg, sizeof(ch_t), strlen(termCfg), uartf);

      /* main daemon loop */
      for (;;)
      {
            if (ttyTerm[currentTTY] == NULL)
            {
                  if (CreateTTY(currentTTY) != STD_RET_OK)
                  {
                        Sleep(100);
                        continue;
                  }
            }
            else
            {
                  tty = ttyTerm[currentTTY];
            }

            /* STDOUT support ------------------------------------------------------------------- */
            if (tty->refLine == SET)
            {
                  /* cursor off, carriage return, line erase from cursor, font attributes reset */
                  msg = "\x1B[?25l\r\x1B[K\x1B[0m";
                  fwrite(msg, sizeof(ch_t), strlen(msg), uartf);

                  /* refresh line */
                  msg = GetMsg(currentTTY, TTY_LAST_MSG);
                  fwrite(msg, sizeof(ch_t), strlen(msg), uartf);

                  /* cursor on */
                  msg = "\x1B[?25h";
                  fwrite(msg, sizeof(ch_t), strlen(msg), uartf);

                  tty->refLine = RESET;
            }
            else if (tty->newMsg > 0)
            {
                  msg = GetMsg(currentTTY, tty->msgCnt - tty->newMsg);

                  if (msg)
                  {
                        fwrite(msg, sizeof(ch_t), strlen(msg), uartf);
                  }
            }

            /* STDIN support -------------------------------------------------------------------- */
            if (ioctl(uartf, UART_IORQ_GET_BYTE, &chr) == STD_RET_OK)
            {
                  keyfn = decFn(chr);

                  if (keyfn == TTY_SEL_NONE)
                  {
                        WaitForAccess(currentTTY);

                        if (tty->fifo.level <= TTY_OUT_BFR_SIZE)
                        {
                              tty->fifo.buffer[tty->fifo.txidx++] = chr;

                              if (tty->fifo.txidx >= TTY_OUT_BFR_SIZE)
                              {
                                    tty->fifo.txidx = 0;
                              }

                              tty->fifo.level++;
                        }

                        GiveAccess(currentTTY);
                  }
                  else if (keyfn <= TTY4_SELECTED && keyfn != currentTTY)
                  {
                        currentTTY = keyfn;

                        if (ttyTerm[currentTTY] == NULL)
                        {
                              CreateTTY(currentTTY);
                        }

                        if (ttyTerm[currentTTY])
                        {
                              RefreshTTY(currentTTY, uartf);
                        }
                  }
            }
            else
            {
                  rxbfrempty = TRUE;
            }

            /* external trigger: terminal switch */
            if (sugTTY != -1)
            {
                  currentTTY = sugTTY;
                  sugTTY = -1;
                  RefreshTTY(currentTTY, uartf);
            }

            /* check that task can go to short sleep */
            if (tty->newMsg == 0 && rxbfrempty)
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
 * @brief Refresh selected TTY
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

            if ((i8_t)(term->tty[dev]->wrIdx - back) < 0)
            {
                  widx = TTY_MAX_LINES - (back - term->tty[dev]->wrIdx);
            }
            else
            {
                  widx = term->tty[dev]->wrIdx - back;
            }

            term->tty[dev]->line[widx - 1] = msg;

            if (back == 0)
            {
                  term->tty[dev]->wrIdx++;
            }
      }
}


//================================================================================================//
/**
 * @brief Refresh selected TTY
 *
 * @param tty     number of terminal
 */
//================================================================================================//
static void RefreshTTY(u8_t dev, FILE_t *file)
{
      /* DNLTODO implement terminal refresh */
//      ch_t *clrscr = "\x1B[2J\x1B[?7h\x1B[H\x1B[0m";
//
//      fwrite(clrscr, sizeof(ch_t), strlen(clrscr), file);
//
//      for (u8_t i = 0; i < TTY_LAST; i++)
//      {
//            ch_t *msg = GetMsg(tty, i);
//
//            if (msg)
//            {
//                  fwrite(msg, sizeof(ch_t), strlen(msg), file);
//            }
//      }
}


//================================================================================================//
/**
 * @brief Check if pushed button is F1-F4
 *
 * @param character     button
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
 * @brief Gets message from terminal
 *
 * @param tty        terminal number
 * @param msg        message number
 *
 * @return pointer to the message
 */
//================================================================================================//
static ch_t *GetMsg(u8_t tty, u8_t msg)
{
      ch_t *ptr = NULL;

      WaitForAccess(tty);

      if (tty < TTY_COUNT && (msg < TTY_MSGS || msg == TTY_LAST_MSG) && ttyTerm[tty])
      {
            if (msg != TTY_LAST_MSG)
            {
                  ptr = ttyTerm[tty]->line[msg];
            }
            else
            {
                  ptr = ttyTerm[tty]->line[ttyTerm[tty]->msgCnt - 1];
            }
      }

      if (ptr && ttyTerm[tty]->newMsg > 0)
      {
            ttyTerm[tty]->newMsg--;
      }

      GiveAccess(tty);

      return ptr;
}


////================================================================================================//
///**
// * @brief Adds message to selected terminal
// * Function allocate own buffer for message to separate itself from application message buffer
// *
// * @param tty        terminal number
// * @param *msg       pointer to the data with msg
// */
////================================================================================================//
//void TTY_AddMsg(u8_t tty, ch_t *msg)
//{
//      if (tty < TTY_COUNT && msg)
//      {
//            if (ttyTerm[tty] == NULL)
//            {
//                   if (CreateTTY(tty) == STD_RET_ERROR)
//                   {
//                         goto AddTermMsg_end;
//                   }
//            }
//
//            /* wait to refresh last modified/joined line */
//            while ((ttyTerm[tty]->refLine == SET) && (tty == currentTTY))
//            {
//                  Sleep(10);
//            }
//
//            /* check if screen is cleared */
//            if (strncmp("\x1B[2J", msg, 4) == 0)
//            {
//                  TTY_Clear(tty);
//            }
//
//            /* check if previous message has not line end (\n) */
//            u8_t msgcnt   = ttyTerm[tty]->msgCnt;
//            ch_t *lastmsg = ttyTerm[tty]->line[msgcnt - 1];
//
//            if ( (msgcnt > 0) && (*(lastmsg + strlen(lastmsg) - 1) != '\n') )
//            {
//                  /* wait for all new messages are showed */
//                  while (ttyTerm[tty]->newMsg && (tty == currentTTY))
//                  {
//                        Sleep(10);
//                  }
//
//                  WaitForAccess(tty);
//
//                  size_t newsize = strlen(lastmsg) + strlen(msg) + 1;
//
//                  ch_t *modmsg = Malloc(newsize);
//
//                  if (modmsg)
//                  {
//                        strcpy(modmsg, lastmsg);
//                        strcat(modmsg, msg);
//
//                        Free(lastmsg);
//
//                        ttyTerm[tty]->line[msgcnt - 1] = modmsg;
//
//                        if (tty == currentTTY)
//                        {
//                              ttyTerm[tty]->refLine = SET;
//                        }
//                  }
//            }
//            else
//            {
//                  ch_t *localMsg = Malloc(sizeof(ch_t) * strlen(msg) + 1);
//
//                  if (localMsg)
//                  {
//                        strcpy(localMsg, msg);
//
//                        WaitForAccess(tty);
//
//                        if (ttyTerm[tty]->msgCnt >= TTY_MSGS)
//                        {
//                              Free(ttyTerm[tty]->line[0]);
//
//                              for (u8_t i = 0; i < TTY_MSGS; i++)
//                              {
//                                    ttyTerm[tty]->line[i] = ttyTerm[tty]->line[i + 1];
//                              }
//
//                              ttyTerm[tty]->line[TTY_MSGS - 1] = localMsg;
//                        }
//                        else
//                        {
//                              ttyTerm[tty]->line[ttyTerm[tty]->msgCnt] = localMsg;
//                        }
//
//                        if (ttyTerm[tty]->msgCnt < TTY_MSGS)
//                        {
//                              ttyTerm[tty]->msgCnt++;
//                        }
//
//                        if (ttyTerm[tty]->newMsg < TTY_MSGS)
//                        {
//                              ttyTerm[tty]->newMsg++;
//                        }
//                  }
//            }
//
//            GiveAccess(tty);
//      }
//
//      AddTermMsg_end:
//      return;
//}


////================================================================================================//
///**
// * @brief Modify last message
// * Function allocate own buffer for message to separate itself from application message buffer
// *
// * @param tty        terminal number
// * @param newmsg     new message
// */
////================================================================================================//
//void TTY_ModifyLastMsg(u8_t tty, ch_t *newmsg)
//{
//      if (tty < TTY_COUNT && newmsg && ttyTerm[tty])
//      {
//            WaitForAccess(tty);
//
//            if (ttyTerm[tty]->msgCnt)
//            {
//                  ch_t *localMsg = Calloc(strlen(newmsg), sizeof(ch_t));
//
//                  if (localMsg)
//                  {
//                        strcpy(localMsg, newmsg);
//
//                        if (ttyTerm[tty]->line[ttyTerm[tty]->msgCnt - 1])
//                        {
//                              Free(ttyTerm[tty]->line[ttyTerm[tty]->msgCnt - 1]);
//                              ttyTerm[tty]->line[ttyTerm[tty]->msgCnt - 1] = localMsg;
//                        }
//                  }
//
//                  if (ttyTerm[tty]->newMsg < TTY_MSGS)
//                  {
//                        ttyTerm[tty]->newMsg++;
//                  }
//            }
//
//            GiveAccess(tty);
//      }
//}


//================================================================================================//
/**
 * @brief Function returns current TTY
 *
 * @return current TTY
 */
//================================================================================================//
static u8_t GetCurrTTY(void)
{
      return term->currentTTY;
}


//================================================================================================//
/**
 * @brief Function returns incoming characters
 *
 * @return current TTY
 */
//================================================================================================//
ch_t TTY_GetChr(u8_t tty)
{
      ch_t chr = 0;

      if (tty < TTY_COUNT && ttyTerm[tty])
      {
            WaitForAccess(tty);

            struct ttyEntry *ctty = ttyTerm[tty];

            if (ctty->fifo.level > 0)
            {
                  chr = ctty->fifo.buffer[ctty->fifo.rxidx++];

                  if (ctty->fifo.rxidx >= TTY_OUT_BFR_SIZE)
                  {
                        ctty->fifo.rxidx = 0;
                  }

                  ctty->fifo.level--;
            }

            GiveAccess(tty);
      }

      return chr;
}


//================================================================================================//
/**
 * @brief Function try to change terminal
 *
 */
//================================================================================================//
static void ChangeTTY(u8_t dev)
{
      WaitForAccess(dev);
      term->changeTTY = dev;
      GiveAccess(dev);
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
