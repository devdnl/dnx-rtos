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


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
struct ttyEntry
{
      ch_t *line[TTY_MSGS];
      u8_t newMsg;
      u8_t msgCnt;
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
static bool_t   CreateTTY(u8_t tty);
static decode_t decodeFn(ch_t character);
static ch_t     *GetMsg(u8_t tty, u8_t msg);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static struct ttyEntry *ttyTerm[TTY_COUNT];
static u8_t currentTTY;


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

      ch_t   character  = 0;
      u8_t   msgs       = 0;
      bool_t rxbfrempty = FALSE;

      EnableKprint();

      /* something about board and system */
      kprint("Board powered by \x1B[32mFreeRTOS\n");
      kprint("By \x1B[36mDaniel Zorychta \x1B[33m<daniel.zorychta@gmail.com>\x1B[0m\n\n");

      InitDrv("uart1", "uart");

      FILE_t *uartf = fopen("/dev/uart", NULL);

      if (uartf == NULL)
      {
            TaskTerminate();
      }

      /* configure terminal VT100: clear screen, line wrap */
      ch_t *termCfg = "\x1B[2J\x1B[?7h";
      fwrite(termCfg, sizeof(ch_t), strlen(termCfg), uartf);

      /* main deamon loop */
      for (;;)
      {
            /* STDOUT support ------------------------------------------------------------------- */
            if ((msgs = TTY_CheckNewMsg(currentTTY)) > 0)
            {
                  ch_t *msg = GetMsg(currentTTY, ttyTerm[currentTTY]->msgCnt - msgs);

                  if (msg)
                  {
                        fwrite(msg, sizeof(ch_t), strlen(msg), uartf);
                  }
            }

            /* STDIN support -------------------------------------------------------------------- */
            if (ioctl(uartf, UART_IORQ_GET_BYTE, &character) == STD_RET_OK)
            {
                  decode_t keyfn = decodeFn(character);

                  if (keyfn == TTY_SEL_NONE)
                  {
                        /* send to program */
                  }
                  else if (keyfn <= TTY4_SELECTED && keyfn != currentTTY)
                  {
                        currentTTY = keyfn;
                        RefreshTTY(currentTTY, uartf);
                  }
            }
            else
            {
                  rxbfrempty = TRUE;
            }

            /* check that can go to short sleep */
            if (TTY_CheckNewMsg(currentTTY) == 0 && rxbfrempty)
                  Sleep(10);
      }

      /* this should never happen */
      TaskTerminate();
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
      ch_t *clrscr = "\x1B[2J";

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
                  if (character == ASCII_ESC)
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
static bool_t CreateTTY(u8_t tty)
{
      bool_t status = FALSE;

      if (tty < TTY_COUNT)
      {
            if (ttyTerm[tty] == NULL)
            {
                  ttyTerm[tty] = (struct ttyEntry*)Calloc(1, sizeof(struct ttyEntry));

                  if (ttyTerm[tty])
                  {
                        status = TRUE;
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
                   if (CreateTTY(tty) == FALSE)
                   {
                         goto AddTermMsg_end;
                   }
            }

            /* find free entry */
            ch_t *localMsg = (ch_t*)Malloc(sizeof(ch_t) * strlen(msg) + 1);

            if (localMsg)
            {
                  strcpy(localMsg, msg);

                  if (ttyTerm[tty]->msgCnt >= TTY_MSGS)
                  {
                        if (ttyTerm[tty]->line[0])
                        {
                              Free(ttyTerm[tty]->line[0]);
                        }

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
            }

            AddTermMsg_end:
            if (ttyTerm[tty]->msgCnt < TTY_MSGS)
                  ttyTerm[tty]->msgCnt++;

            if (ttyTerm[tty]->newMsg < TTY_MSGS)
                  ttyTerm[tty]->newMsg++;
      }

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
                  for (u8_t i = 0; i < TTY_MSGS; i++)
                  {
                        if (ttyTerm[tty]->line)
                           Free(ttyTerm[tty]->line);

                        ttyTerm[tty]->line[i] = NULL;
                  }

                  ttyTerm[tty]->msgCnt = 0;
                  ttyTerm[tty]->newMsg = 0;
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
      if (tty < TTY_COUNT && newmsg)
      {
            if (ttyTerm[tty] && ttyTerm[tty]->msgCnt)
            {
                  ch_t *localMsg = (ch_t*)Malloc(sizeof(ch_t) * strlen(newmsg));

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
                        ttyTerm[tty]->newMsg++;
            }
      }
}


//================================================================================================//
/**
 * @brief Function check if new message is received
 *
 * @param tty           terminal number
 * @return number of new messages
 */
//================================================================================================//
u8_t TTY_CheckNewMsg(u8_t tty)
{
      if (tty < TTY_COUNT && ttyTerm[tty])
      {
            return ttyTerm[tty]->newMsg;
      }
      else
      {
            return 0;
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


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
