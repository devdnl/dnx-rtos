/*=============================================================================================*//**
@file    net.c

@author  Daniel Zorychta

@brief   This file support upper layer of Ethernet interface and LwIP stack

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
#include "net.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
typedef struct
{
      struct tcp_pcb *pcb;
      struct pbuf    *p;
      err_t          err;
      netStatus_t    status;
} socketEntry_t;


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static u32_t FindFreePCB(void);
static err_t TCPAccept(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t TCPReceived(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
static void  TCPConnError(void *arg, err_t err);
static err_t TCPPoll(void *arg, struct tcp_pcb *pcb);
static err_t TCPSent(void *arg, struct tcp_pcb *pcb, u16_t len);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
/** TCP PCB list */
static socketEntry_t socketList[MEMP_NUM_TCP_PCB];


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
netSoc_t NET_NewTCPSocket(ip_addr_t *ipaddr, u16_t port)
{
      netSoc_t socket = 0;

      u32_t pcbIdx = FindFreePCB();

      socketEntry_t *netSoc = &socketList[pcbIdx];

      /* allocate memory for TCP pcb variable */
      if (pcbIdx < MEMP_NUM_TCP_PCB)
      {
            netSoc->pcb = (struct tcp_pcb *)Malloc(sizeof(struct tcp_pcb));

            if (netSoc->pcb == NULL)
                  goto NET_NewTCPSocket_end;
      }

      /* create new TCP protocol and bind with IP and port */
      netSoc->pcb = tcp_new();

      if (tcp_bind(netSoc->pcb, ipaddr, port) != ERR_OK)
      {
            Free(netSoc->pcb);
            goto NET_NewTCPSocket_end;
      }

      netSoc->status.integer = 0;

      netSoc->pcb = tcp_listen(netSoc->pcb);

      tcp_arg(netSoc->pcb, netSoc);

      tcp_accept(netSoc->pcb, TCPAccept);

      /* set socket number; if socket = 0 then is invalid socket number */
      socket = pcbIdx + 1;

      NET_NewTCPSocket_end:
            return socket;
}


stdStatus_t NET_GetTCPStatus(netSoc_t socket, netStatus_t *status)
{
      if (!socket)
            return STD_STATUS_ERROR;

      netSoc_t socketNumber = socket - 1;

      *status = socketList[socketNumber].status;

      return STD_STATUS_OK;
}


stdStatus_t NET_TCPAcceptReceived(netSoc_t socket)
{
      if (!socket)
            goto NET_TCPAcceptReceived_error;

      socketEntry_t *netSoc = &socketList[socket - 1];

      netSoc->status.flag.DataReceived = FALSE;

      if (netSoc->err == ERR_OK && netSoc->p != NULL)
      {
            /* Inform TCP that we have taken the data. */
            tcp_recved(netSoc->pcb, netSoc->p->tot_len);
      }
      else if (netSoc->err == ERR_OK && netSoc->p == NULL)
      {
            NET_TCPClose(socket);
            goto NET_TCPAcceptReceived_error;
      }

      return STD_STATUS_OK;

      NET_TCPAcceptReceived_error:
            return STD_STATUS_ERROR;
}


void *NET_GetReceivedDataBuffer(netSoc_t socket)
{
      if (!socket)
            return NULL;

      socketEntry_t *netSoc = &socketList[socket - 1];

      return netSoc->p->payload;
}


void NET_FreeReceivedBuffer(netSoc_t socket)
{
      if (!socket)
            return;

      socketEntry_t *netSoc = &socketList[socket - 1];

      pbuf_free(netSoc->p);
}


stdStatus_t NET_TCPClose(netSoc_t socket)
{
      if (!socket)
            return STD_STATUS_ERROR;

      socketEntry_t *netSoc = &socketList[socket - 1];

      TaskSuspendAll();
      tcp_sent(netSoc->pcb, NULL);
      tcp_recv(netSoc->pcb, NULL);
      tcp_close(netSoc->pcb);
      TaskResumeAll();

      netSoc->status.integer = 0;

      return STD_STATUS_OK;
}


stdStatus_t NET_CloseTCPSocket(netSoc_t socket)
{
      if (!socket)
            return STD_STATUS_ERROR;

      socketEntry_t *netSoc = &socketList[socket - 1];

      TaskSuspendAll();
      tcp_arg(netSoc->pcb, NULL);
      tcp_sent(netSoc->pcb, NULL);
      tcp_recv(netSoc->pcb, NULL);
      tcp_accept(netSoc->pcb, NULL);
      tcp_close(netSoc->pcb);
      Free(netSoc->pcb);
      netSoc->pcb = NULL;
      netSoc->status.integer = 0;
      TaskResumeAll();

      return STD_STATUS_OK;
}


stdStatus_t NET_TCPWrite(netSoc_t socket, void *src, u32_t *len)
{
      u32_t size;

      if (!socket || !src || !len || !*len)
            return STD_STATUS_ERROR;

      socketEntry_t *netSoc = &socketList[socket - 1];

      /* We cannot send more data than space avaliable in the send buffer. */
      if (tcp_sndbuf(netSoc->pcb) < *len)
      {
            size = tcp_sndbuf(netSoc->pcb);
      }
      else
      {
            size = *len;
      }

      if (tcp_write(netSoc->pcb, src, size, 0) == ERR_OK)
      {
            src  += size;
            *len -= size;

            netSoc->status.flag.DataPosted = FALSE;

            tcp_sent(netSoc->pcb, TCPSent);
      }

      return STD_STATUS_OK;
}


stdStatus_t NET_TCPAcceptError(netSoc_t socket)
{
      if (!socket)
            return STD_STATUS_ERROR;

      socketEntry_t *netSoc = &socketList[socket - 1];

      netSoc->status.flag.ConnectionError = FALSE;

      return STD_STATUS_OK;
}


stdStatus_t NET_TCPAcceptPoll(netSoc_t socket)
{
      if (!socket)
            return STD_STATUS_ERROR;

      socketEntry_t *netSoc = &socketList[socket - 1];

      netSoc->status.flag.Poll = FALSE;

      return STD_STATUS_OK;
}


static u32_t FindFreePCB(void)
{
      u32_t i;

      for (i = 0; i < MEMP_NUM_TCP_PCB; i++)
      {
            if (socketList[i].pcb == NULL)
                  return i;
      }

      return MEMP_NUM_TCP_PCB;
}


static err_t TCPAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{
      (void) err;
      (void) pcb;

      socketEntry_t *netSoc = (socketEntry_t *)arg;

      netSoc->status.flag.BindAccepted = TRUE;

      /* Tell TCP that we wish to be informed of incoming data by a call to the TCPReceived() function */
      tcp_recv(netSoc->pcb, TCPReceived);

      tcp_err(netSoc->pcb, TCPConnError);

      tcp_poll(netSoc->pcb, TCPPoll, 10);

      return ERR_OK;
}


static err_t TCPReceived(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
      (void) pcb;

      socketEntry_t *netSoc = (socketEntry_t *)arg;

      netSoc->status.flag.DataReceived = TRUE;
      netSoc->p   = p;
      netSoc->err = err;

      return ERR_OK;
}


static void TCPConnError(void *arg, err_t err)
{
      (void) err;

      socketEntry_t *netSoc = (socketEntry_t *)arg;

      netSoc->status.flag.ConnectionError = TRUE;
}


static err_t TCPPoll(void *arg, struct tcp_pcb *pcb)
{
      (void) pcb;

      socketEntry_t *netSoc = (socketEntry_t *)arg;

      netSoc->status.flag.Poll = TRUE;

      return ERR_OK;
}

static err_t TCPSent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
      (void) pcb;
      (void) len;

      socketEntry_t *netSoc = (socketEntry_t *)arg;

      netSoc->status.flag.DataPosted = TRUE;

      return ERR_OK;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
