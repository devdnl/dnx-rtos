/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * httpd.c
 *
 * Author : Adam Dunkels <adam@sics.se>
 *
 * CHANGELOG: this file has been modified by Sergio Perez Alca�iz <serpeal@upvnet.upv.es>
 *            Departamento de Inform�tica de Sistemas y Computadores
 *            Universidad Polit�cnica de Valencia
 *            Valencia (Spain)
 *            Date: March 2003
 *
 */

#include "httpde.h"
#include "lwip/tcp.h"
#include "fsdata.c"
#include <string.h>

struct http_state
{
  char *file;
  u32_t left;
};


ch_t htmlBuffer[2048];


/*-----------------------------------------------------------------------------------*/
static void conn_err(void *arg, err_t err)
{
      (void) arg;
      (void) err;

      struct http_state *hs;

      hs = arg;
      mem_free(hs);
}
/*-----------------------------------------------------------------------------------*/
static void close_conn(struct tcp_pcb *pcb, struct http_state *hs)
{

      tcp_arg(pcb, NULL);
      tcp_sent(pcb, NULL);
      tcp_recv(pcb, NULL);
      mem_free(hs);
      tcp_close(pcb);
}
/*-----------------------------------------------------------------------------------*/
static void send_data(struct tcp_pcb *pcb, struct http_state *hs)
{
      err_t err;
      u16_t len;

      /* We cannot send more data than space available in the send buffer. */
      if (tcp_sndbuf(pcb) < hs->left)
      {
            len = tcp_sndbuf(pcb);
      }
      else
      {
            len = hs->left;
      }

      err = tcp_write(pcb, hs->file, len, 0);

      if (err == ERR_OK)
      {
            hs->file += len;
            hs->left -= len;
      }
}

/*-----------------------------------------------------------------------------------*/
static err_t http_poll(void *arg, struct tcp_pcb *pcb)
{
      if (arg == NULL)
      {
            tcp_close(pcb);
      }
      else
      {
            send_data(pcb, (struct http_state *) arg);
      }

      return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t http_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
      (void) len;

      struct http_state *hs;

      hs = arg;

      if (hs->left > 0)
      {
            send_data(pcb, hs);
      }
      else
      {
            close_conn(pcb, hs);
      }

      return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
      int i, j;
      char *data;
      char fname[40];
      struct fs_file file = { 0, 0 };
      struct http_state *hs;

      hs = arg;

      if (err == ERR_OK && p != NULL)
      {
            /* Inform TCP that we have taken the data. */
            tcp_recved(pcb, p->tot_len);

            if (hs->file == NULL)
            {
                  data = p->payload;

//                  if (strncmp(data, "GET /STM32F107ADC", 17) == 0)
//                  {
//                        pbuf_free(p);
//
//                        fs_open("/STM32F107ADC.html", &file);
//
//                        hs->file = file.data;
//                        hs->left = file.len;
//
//                        send_data(pcb, hs);
//
//                        /* Tell TCP that we wish be to informed of data that has been
//                         successfully sent by a call to the http_sent() function. */
//                        tcp_sent(pcb, http_sent);
//                  }
//                  else if (strncmp(data, "GET /method=get", 15) == 0)
//                  {
//                        i = 15;
//
//                        while (data[i] != 0x20/* */)
//                        {
//                              i++;
//                              if (data[i] == 0x6C /* l */)
//                              {
//                                    i++;
//                                    if (data[i] == 0x65 /* e */)
//                                    {
//                                          i++;
//                                          if (data[i] == 0x64 /* d*/)
//                                          {
//                                                i += 2;
//                                                if (data[i] == 0x31 /* 1 */)
//                                                {
//                                                }
//
//                                                if (data[i] == 0x32 /* 2 */)
//                                                {
//                                                }
//
//                                                if (data[i] == 0x33 /* 3 */)
//                                                {
//                                                }
//
//                                                if (data[i] == 0x34 /* 4 */)
//                                                {
//                                                }
//                                          }
//                                    }
//                              }
//                        }
//
//                        pbuf_free(p);
//
//                        fs_open("/STM32F107LED.html", &file);
//
//                        hs->file = file.data;
//                        hs->left = file.len;
//
//                        send_data(pcb, hs);
//
//                        /* Tell TCP that we wish be to informed of data that has been
//                         successfully sent by a call to the http_sent() function. */
//                        tcp_sent(pcb, http_sent);
//                  }
                  /*else*/ if (strncmp(data, "GET ", 4) == 0)
                  {
                        for (i = 0; i < 40; i++)
                        {
                              if (  ((char *) data + 4)[i] == ' '
                                 || ((char *) data + 4)[i] == '\r'
                                 || ((char *) data + 4)[i] == '\n')
                              {
                                    ((char *) data + 4)[i] = 0;
                              }
                        }

                        i = 0;
                        j = 0;

                        do
                        {
                              fname[i] = ((char *) data + 4)[j];
                              j++;
                              i++;
                        }
                        while (fname[i - 1] != 0 && i < 40);

                        pbuf_free(p);

                        if (!fs_open(fname, &file))
                        {
                              fs_open("/index.html", &file);
                        }
                        hs->file = file.data;
                        hs->left = file.len;

                        /* check that file is a HTML file */
                        if (strncmp(strchr(fname, '.'), ".html", 5) == 0)
                        {
                              /* allocate new buffer for page */
                              i32_t i = 0;
                              u32_t n = 0;
                              ch_t  *pagePtr = htmlBuffer;
                              u32_t pageSize = 0;

                              for (i = 0; i < file.len; i++)
                              {
                                    if (strncmp(&file.data[i], "<?", 2) == 0)
                                    {
                                          i += 2;
                                          n = 0;

                                          if (strncmp(&file.data[i], "temp/?>", 7) == 0)
                                          {
                                                i += 6;
                                                n = snprint(pagePtr, 50, "temperatura");
                                          }
                                          else if (strncmp(&file.data[i], "pres/?>", 7) == 0)
                                          {
                                                i += 6;
                                                n = snprint(pagePtr, 50, "cisnienie");
                                          }
                                          else if (strncmp(&file.data[i], "date/?>", 7) == 0)
                                          {
                                                i += 6;
                                                n = snprint(pagePtr, 50, "date_app");
                                          }

                                          pagePtr  += n;
                                          pageSize += n;
                                    }
                                    else
                                    {
                                          *pagePtr++ = file.data[i];
                                          pageSize++;
                                    }
                              }

                              hs->file = htmlBuffer;
                              hs->left = pageSize;
                        }

                        send_data(pcb, hs);

                        /* Tell TCP that we wish be to informed of data that has been
                         successfully sent by a call to the http_sent() function. */
                        tcp_sent(pcb, http_sent);
                  }
                  else
                  {
                        close_conn(pcb, hs);
                  }
            }
            else
            {
                  pbuf_free(p);
            }
      }

      if (err == ERR_OK && p == NULL)
      {

            close_conn(pcb, hs);
      }

      return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static err_t http_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
      (void) arg;
      (void) err;

      struct http_state *hs;

      /* Allocate memory for the structure that holds the state of the connection */
      hs = mem_malloc(sizeof(struct http_state));

      if (hs == NULL)
      {
            return ERR_MEM;
      }

      /* Initialize the structure. */
      hs->file = NULL;
      hs->left = 0;

      /* Tell TCP that this is the structure we wish to be passed for our callbacks */
      tcp_arg(pcb, hs);

      /* Tell TCP that we wish to be informed of incoming data by a call to the http_recv() function */
      tcp_recv(pcb, http_recv);

      tcp_err(pcb, conn_err);

      tcp_poll(pcb, http_poll, 10);
      return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
void httpd_init(void *argv)
{
      (void) argv;

      struct tcp_pcb *pcb;

      pcb = tcp_new();
      tcp_bind(pcb, IP_ADDR_ANY, 80);
      pcb = tcp_listen(pcb);
      tcp_accept(pcb, http_accept);

      TaskTerminate();
}
/*-----------------------------------------------------------------------------------*/
int fs_open(char *name, struct fs_file *file)
{
      struct fsdata_file_noconst *f;

      for (f = (struct fsdata_file_noconst *) FS_ROOT;
           f != NULL;
           f = (struct fsdata_file_noconst *) f->next)
      {
            if (!strcmp(name, f->name))
            {
                  file->data = f->data;
                  file->len = f->len;
                  return 1;
            }
      }
      return 0;
}
/*-----------------------------------------------------------------------------------*/

