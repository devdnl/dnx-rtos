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
#include "memman.h"
#include "regdrv.h"
#include "vfs.h"
#include <string.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define TTY_OUT_BFR_SIZE            16


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
struct ttyEntry
{
      ch_t   *line[TTY_MSGS];
      u8_t   newMsg;
      u8_t   msgCnt;
      flag_t refLine;
      flag_t lock;

      struct
      {
            u16_t level;
            ch_t  buffer[TTY_OUT_BFR_SIZE];
            u16_t txidx;
            u16_t rxidx;
      } fifo;
};


typedef enum
{
      TTY1_SELECTED,
      TTY2_SELECTED,
      TTY3_SELECTED,
      TTY4_SELECTED,
      TTY_SEL_PEND,
      TTY_SEL_NONE
} decode_t;


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static void     RefreshTTY(u8_t tty, FILE_t *file);
static stdRet_t CreateTTY(u8_t tty);
static decode_t decodeFn(ch_t character);
static ch_t     *GetMsg(u8_t tty, u8_t msg);
static void     WaitForAccess(u8_t tty);
static void     GiveAccess(u8_t tty);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static struct ttyEntry *ttyTerm[TTY_COUNT];
static u8_t currentTTY;
static i8_t sugTTY;


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief TTY daemon
 */
//================================================================================================//
void ttyd(void *arg)
{
      (void) arg;

      sugTTY = -1;

      ch_t     chr        = 0;
      bool_t   rxbfrempty = FALSE;
      ch_t     *msg;
      FILE_t   *uartf;
      decode_t keyfn;
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
                  keyfn = decodeFn(chr);

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
 * @brief Function wait to safety access to data
 *
 * @param tty     number of terminal
 */
//================================================================================================//
static void WaitForAccess(u8_t tty)
{
      while (TRUE)
      {
            TaskSuspendAll();
            if (ttyTerm[tty]->lock == SET)
            {
                  TaskResumeAll();
                  Sleep(1);
            }
            else
            {
                  ttyTerm[tty]->lock = SET;
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
static void GiveAccess(u8_t tty)
{
      TaskSuspendAll();
      ttyTerm[tty]->lock = RESET;
      TaskResumeAll();
}



//================================================================================================//
/**
 * @brief Refresh selected TTY
 *
 * @param tty     number of terminal
 */
//================================================================================================//
static void RefreshTTY(u8_t tty, FILE_t *file)
{
      ch_t *clrscr = "\x1B[2J\x1B[?7h\x1B[H\x1B[0m";

      fwrite(clrscr, sizeof(ch_t), strlen(clrscr), file);

      for (u8_t i = 0; i < TTY_MSGS; i++)
      {
            ch_t *msg = GetMsg(tty, i);

            if (msg)
            {
                  fwrite(msg, sizeof(ch_t), strlen(msg), file);
            }
      }
}


//================================================================================================//
/**
 * @brief Check if pushed button is F1-F4
 *
 * @param character     button
 */
//================================================================================================//
static decode_t decodeFn(ch_t character)
{
      static u8_t funcStep = 0;
      decode_t state = TTY_SEL_NONE;

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
 * @brief Create TTY
 *
 * @param tty     number of terminal
 * @retval TRUE   terminal created
 * @retval FALSE  terminal not created
 */
//================================================================================================//
static stdRet_t CreateTTY(u8_t tty)
{
      bool_t status = STD_RET_ERROR;

      if (tty < TTY_COUNT)
      {
            if (ttyTerm[tty] == NULL)
            {
                  ttyTerm[tty] = Calloc(1, sizeof(struct ttyEntry));

                  if (ttyTerm[tty])
                  {
                        status = STD_RET_OK;
                  }
            }
      }

      return status;
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


//================================================================================================//
/**
 * @brief Adds message to selected terminal
 * Function allocate own buffer for message to separate itself from application message buffer
 *
 * @param tty        terminal number
 * @param *msg       pointer to the data with msg
 */
//================================================================================================//
void TTY_AddMsg(u8_t tty, ch_t *msg)
{
      if (tty < TTY_COUNT && msg)
      {
            if (ttyTerm[tty] == NULL)
            {
                   if (CreateTTY(tty) == STD_RET_ERROR)
                   {
                         goto AddTermMsg_end;
                   }
            }

            /* wait to refresh last modified/joined line */
            while ((ttyTerm[tty]->refLine == SET) && (tty == currentTTY))
            {
                  Sleep(10);
            }

            /* check if previous message has not line end (\n) */
            u8_t msgcnt   = ttyTerm[tty]->msgCnt;
            ch_t *lastmsg = ttyTerm[tty]->line[msgcnt - 1];

            if ( (msgcnt > 0) && (*(lastmsg + strlen(lastmsg) - 1) != '\n') )
            {
                  /* wait for all new messages are showed */
                  while (ttyTerm[tty]->newMsg && (tty == currentTTY))
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
 * @brief Clear all messages from terminal and free memory
 *
 * @param tty        terminal number
 */
//================================================================================================//
void TTY_Clear(u8_t tty)
{
      if (tty < TTY_COUNT)
      {
            if (ttyTerm[tty])
            {
                  WaitForAccess(tty);

                  for (u8_t i = 0; i < TTY_MSGS; i++)
                  {
                        if (ttyTerm[tty]->line)
                        {
                           Free(ttyTerm[tty]->line[i]);
                        }

                        ttyTerm[tty]->line[i] = NULL;
                  }

                  ttyTerm[tty]->msgCnt = 0;
                  ttyTerm[tty]->newMsg = 0;

                  GiveAccess(tty);
            }
      }
}


//================================================================================================//
/**
 * @brief Modify last message
 * Function allocate own buffer for message to separate itself from application message buffer
 *
 * @param tty        terminal number
 * @param newmsg     new message
 */
//================================================================================================//
void TTY_ModifyLastMsg(u8_t tty, ch_t *newmsg)
{
      if (tty < TTY_COUNT && newmsg && ttyTerm[tty])
      {
            WaitForAccess(tty);

            if (ttyTerm[tty]->msgCnt)
            {
                  ch_t *localMsg = Calloc(strlen(newmsg), sizeof(ch_t));

                  if (localMsg)
                  {
                        strcpy(localMsg, newmsg);

                        if (ttyTerm[tty]->line[ttyTerm[tty]->msgCnt - 1])
                        {
                              Free(ttyTerm[tty]->line[ttyTerm[tty]->msgCnt - 1]);
                              ttyTerm[tty]->line[ttyTerm[tty]->msgCnt - 1] = localMsg;
                        }
                  }

                  if (ttyTerm[tty]->newMsg < TTY_MSGS)
                  {
                        ttyTerm[tty]->newMsg++;
                  }
            }

            GiveAccess(tty);
      }
}


//================================================================================================//
/**
 * @brief Function returns current TTY
 *
 * @return current TTY
 */
//================================================================================================//
u8_t TTY_GetCurrTTY(void)
{
      return currentTTY;
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
void TTY_ChangeTTY(u8_t tty)
{
      if (tty < TTY_COUNT)
      {
            WaitForAccess(tty);
            sugTTY = tty;
            GiveAccess(tty);
      }
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
