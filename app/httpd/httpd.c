/*=============================================================================================*//**
@file    httpd.c

@author  Daniel Zorychta

@brief

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
/*==================================================================================================
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
* http.h
*
* Author : Adam Dunkels <adam@sics.se>
*
* CHANGELOG: this file has been modified by Sergio Perez Alca�iz <serpeal@upvnet.upv.es>
*            Departamento de Inform�tica de Sistemas y Computadores
*            Universidad Polit�cnica de Valencia
*            Valencia (Spain)
*            Date: March 2003
*
*=================================================================================================*/

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "httpd.h"
#include "fsdata.h"
#include "lwip/def.h"
#include "lwip/tcp.h"
#include "fsdata.c"
#include <string.h>
#include "mpl115a2_def.h"
#include "ds1307_def.h"

/* Begin of application section declaration */
APPLICATION(httpd)
APP_SEC_BEGIN

/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define BFR_SIZE                    4096

#define FILE_SENSOR                 "/dev/sensor"
#define FILE_GRAPH                  "/srv/www/graph.svg"

/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
struct http_state
{
      char *file;
      u32_t left;
};

struct fs_file {
      char *data;
      int len;
};

/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
ch_t *htmlBuffer;

/*==================================================================================================
                                        Function definitions
==================================================================================================*/
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
void conn_err(void *arg, err_t err)
{
      (void) arg;
      (void) err;

      struct http_state *hs;

      hs = arg;
      mem_free(hs);
}
/*-----------------------------------------------------------------------------------*/
void close_conn(struct tcp_pcb *pcb, struct http_state *hs)
{

      tcp_arg(pcb, NULL);
      tcp_sent(pcb, NULL);
      tcp_recv(pcb, NULL);
      mem_free(hs);
      tcp_close(pcb);
}
/*-----------------------------------------------------------------------------------*/
void send_data(struct tcp_pcb *pcb, struct http_state *hs)
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
err_t http_poll(void *arg, struct tcp_pcb *pcb)
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
err_t http_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
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
err_t http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
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

                  if (strncmp(data, "GET ", 4) == 0)
                  {
                        i32_t i = 0;
                        u32_t n = 0;

                        for (i = 0; i < 40; i++)
                        {
                              if (  ((char *) data + 4)[i] == ' '
                                 || ((char *) data + 4)[i] == '\r'
                                 || ((char *) data + 4)[i] == '\n')
                              {
                                    ((char *) data + 4)[i] = 0;
                              }
                        }

                        strcpy(fname, (ch_t *)data + 4);

                        pbuf_free(p);

                        if (!fs_open(fname, &file))
                        {
                              if (strncmp(fname, "/", 5) == 0)
                              {
                                    fs_open("/index.html", &file);
                                    snprintf(fname, sizeof(fname), "/index.html");
                              }
                              else
                              {
                                    fs_open("/404.html", &file);
                                    snprintf(fname, sizeof(fname), "/404.html");
                              }
                        }

                        hs->file = file.data;
                        hs->left = file.len;

                        /* check that file is a HTML file */
                        if (strncmp(strchr(fname, '.'), ".html", 5) == 0)
                        {
                              /* allocate new buffer for page */
                              ch_t  *pagePtr = htmlBuffer;
                              u32_t pageSize = 0;

                              for (i = 0; i < file.len; i++)
                              {
                                    if (strncmp(&file.data[i], "<?", 2) == 0)
                                    {
                                          i += 2;
                                          n  = 0;

                                          if (strncmp(&file.data[i], "temp/?>", 7) == 0)
                                          {
                                                i += 6;
                                                i8_t temp = 0;

                                                FILE_t *sensor = fopen(FILE_SENSOR, "r");

                                                if (sensor)
                                                {
                                                      ioctl(sensor, MPL115A2_IORQ_GETTEMP, &temp);

                                                      if (fclose(sensor) != STD_RET_OK)
                                                            kprint("httpd: error while closing 'sensor' file\n");
                                                }

                                                n = snprintf(pagePtr, 50, "%d", (i32_t)temp);
                                          }
                                          else if (strncmp(&file.data[i], "pres/?>", 7) == 0)
                                          {
                                                i += 6;
                                                u16_t pressure = 0;

                                                FILE_t *sensor = fopen(FILE_SENSOR, "r");

                                                if (sensor)
                                                {
                                                      ioctl(sensor, MPL115A2_IORQ_GETPRES, &pressure);

                                                      if (fclose(sensor) != STD_RET_OK)
                                                            kprint("httpd: error while closing 'sensor' file\n");
                                                }

                                                n = snprintf(pagePtr, 50, "%d", (u32_t)pressure);
                                          }
                                          else if (strncmp(&file.data[i], "date/?>", 7) == 0)
                                          {
                                                i += 6;

                                                bcdTime_t time = {0x00, 0x00, 0x00};
                                                bcdDate_t date = {0, 0, 0, 0};
                                                FILE_t *rtc = fopen("/dev/rtc", "r");

                                                if (rtc)
                                                {
                                                      ioctl(rtc, RTC_IORQ_GETTIME, &time);
                                                      ioctl(rtc, RTC_IORQ_GETDATE, &date);
                                                      if (fclose(rtc) != STD_RET_OK)
                                                            kprint("httpd: error while closing 'rtc' file\n");
                                                }
//                                                else
//                                                {
//                                                      memset(&time, 0x00, sizeof(time));
//                                                      memset(&date, 0x00, sizeof(date));
//                                                }

                                                n = snprintf(pagePtr, 50, "%x2-%x2-20%x2, %x2:%x2\n",
                                                            date.day,
                                                            date.month,
                                                            date.year,
                                                            time.hours,
                                                            time.minutes);
                                          }
                                          else if (strncmp(&file.data[i], "graph/?>", 8) == 0)
                                          {
                                                i += 7;

                                                FILE_t *svg = fopen(FILE_GRAPH, "r");

                                                if (svg)
                                                {
                                                      n = fread(pagePtr, sizeof(ch_t), BFR_SIZE, svg);

                                                      fclose(svg);
                                                }
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
err_t http_accept(void *arg, struct tcp_pcb *pcb, err_t err)
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


//================================================================================================//
/**
 * @brief clear main function
 */
//================================================================================================//
stdRet_t appmain(ch_t *argv)
{
      (void)argv;

      /* check if application is started as daemon */
      if (stdout != NULL) {
            printf("%s can be started only as daemon!\n", HTTPD_NAME);
            goto httpd_exit;
      }

      struct tcp_pcb *pcb;

      htmlBuffer = malloc(BFR_SIZE);

      if (!htmlBuffer)
      {
            kprint("httpd: not enough free memory\n");
            return STD_RET_ERROR;
      }

      pcb = tcp_new();
      tcp_bind(pcb, IP_ADDR_ANY, 80);
      pcb = tcp_listen(pcb);
      tcp_accept(pcb, http_accept);

      while (TRUE)
      {
            Sleep(2000);
      }

      httpd_exit:
      return STD_RET_OK;
}

/* End of application section declaration */
APP_SEC_END

/*==================================================================================================
                                            End of file
==================================================================================================*/
