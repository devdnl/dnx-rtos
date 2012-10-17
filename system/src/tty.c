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


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
struct ttyEntry
{
      ch_t *line[TTY_MSGS];
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static bool_t CreateTTY(u8_t tty);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static struct ttyEntry *ttyTerm[TTY_COUNT] = {NULL, NULL, NULL, NULL};
static bool_t ttyNewMsg[TTY_COUNT];

/*==================================================================================================
                                        Function definitions
==================================================================================================*/

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
 * @brief Adds message to selected terminal
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
            for (u8_t i = 0; i < TTY_MSGS; i++)
            {
                  if (ttyTerm[tty]->line[i] == NULL)
                  {
                        ttyTerm[tty]->line[i] = msg;
                        ttyNewMsg[tty] = TRUE;
                        goto AddTermMsg_end;
                  }
            }

            /* no free slots - move all lines by 1 up */
            if (ttyTerm[tty]->line[0])
            {
                  Free(ttyTerm[tty]->line[0]);
            }

            for (u8_t i = 0; i < TTY_MSGS; i++)
            {
                  ttyTerm[tty]->line[i] = ttyTerm[tty]->line[i+1];
            }

            ttyTerm[tty]->line[TTY_MSGS - 1] = msg;
            ttyNewMsg[tty] = TRUE;
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
            for (u8_t i = 0; i < ARRAY_SIZE(ttyTerm); i++)
            {
                  if (ttyTerm[tty]->line)
                     Free(ttyTerm[tty]->line);

                  ttyTerm[tty]->line[i] = NULL;
            }
      }
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
ch_t *TTY_GetMsg(u8_t tty, u8_t msg)
{
      ch_t *ptr = NULL;

      if (tty < TTY_COUNT && (msg < TTY_MSGS || msg == TTY_LAST_MSG))
      {
            if (ttyTerm[tty])
            {
                  if (msg != TTY_LAST_MSG)
                  {
                        ptr = ttyTerm[tty]->line[msg];
                  }
                  else
                  {
                        /* check if last message exist */
                        if (ttyTerm[tty]->line[TTY_MSGS - 1])
                        {
                              ptr = ttyTerm[tty]->line[TTY_MSGS - 1];
                        }

                        /* last message is not on TTY boundary */
                        for (u8_t i = 0; i < TTY_MSGS; i++)
                        {
                              if (ttyTerm[tty]->line[i] == NULL)
                              {
                                    if (i > 0)
                                    {
                                          ptr = ttyTerm[tty]->line[i - 1];
                                          break;
                                    }
                              }
                        }
                  }
            }
      }

      if (ptr)
      {
            ttyNewMsg[tty] = FALSE;
      }

      return ptr;
}


//================================================================================================//
/**
 * @brief Modify last message
 *
 * @param tty        terminal number
 * @param newmsg     new message
 */
//================================================================================================//
void TTY_ModifyLastMsg(u8_t tty, ch_t *newmsg)
{
      if (tty < TTY_COUNT && newmsg)
      {
            if (ttyTerm[tty])
            {
                  for (u8_t i = 0; i < TTY_MSGS; i++)
                  {
                        if (ttyTerm[tty]->line[i] == NULL)
                        {
                              if (i > 0)
                                    ttyTerm[tty]->line[i - 1] = newmsg;
                              else
                                    ttyTerm[tty]->line[0] = newmsg;

                              ttyNewMsg[tty] = TRUE;
                        }
                  }
            }
      }
}


//================================================================================================//
/**
 * @brief Function check if new message is received
 *
 * @param tty           terminal number
 * @retval TRUE         new message was received
 * @retval FALSE        nothing new
 */
//================================================================================================//
bool_t TTY_CheckNewMsg(u8_t tty)
{
      if (tty < TTY_COUNT)
      {
            return ttyNewMsg[tty];
      }
      else
      {
            return FALSE;
      }
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
