/*=============================================================================================*//**
@file    measd.c

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
                                            Include files
==================================================================================================*/
#include "clear.h"
#include <string.h>
#include "mpl115a2_def.h"

/* Begin of application section declaration */
APPLICATION(measd)
APP_SEC_BEGIN

/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define DATA_COUNT                  51
#define FILE_BUFFER_SIZE            4096
#define GRAPH_X_POINT               (300/(DATA_COUNT - 1))
#define SLEEP_TIME                  5000UL /* = 15 min */
#define SENSOR_FILE                 "/dev/sensor"
#define OUTPUT_FILE                 "/srv/www/graph.svg"


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
struct meas
{
      u16_t pres;
      i8_t  temp;
};


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
u8_t wridx = 0;
struct meas dataList[DATA_COUNT];


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Function write data to buffer
 */
//================================================================================================//
void writeData(struct meas *measurement)
{
      dataList[wridx++] = *measurement;

      if (wridx >= DATA_COUNT)
            wridx = 0;
}


//================================================================================================//
/**
 * @brief Function read data from buffer
 */
//================================================================================================//
struct meas readData(u8_t idx)
{
      struct meas data;
      data.pres = 0;
      data.temp = 0;

      if (idx > DATA_COUNT)
            idx = DATA_COUNT;

      i8_t rdidx = wridx - idx;

      if (rdidx < 0)
            rdidx += DATA_COUNT;

      data = dataList[rdidx];

      return data;
}


//================================================================================================//
/**
 * @brief clear main function
 */
//================================================================================================//
stdRet_t appmain(ch_t *argv)
{
      (void)argv;

      /* clear measurement */
      for (u32_t i = 0; i < ARRAY_SIZE(dataList); i++)
      {
            struct meas def = {.pres = 1013, .temp = 22};
            dataList[i] = def;
      }

      /* prepare reference timer */
      size_t LastWakeTime = SystemGetOSTickCnt();

      for (;;)
      {
            FILE_t *sensor = fopen(SENSOR_FILE, "r");

            if (sensor)
            {
                  struct meas data;
                  data.pres = 0;
                  data.temp = 0;

                  ioctl(sensor, MPL115A2_IORQ_GETPRES, &data.pres);
                  ioctl(sensor, MPL115A2_IORQ_GETTEMP, &data.temp);

                  writeData(&data);

                  fclose(sensor);
            }

            FILE_t *file = fopen(OUTPUT_FILE, "a+");

            if (file)
            {
                  fseek(file, 0, SEEK_SET);

                  ch_t *buffer = calloc(FILE_BUFFER_SIZE, sizeof(ch_t));

                  if (buffer)
                  {
                        u32_t seek = 0;

                        seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "<svg xmlns=\"http://www.w3.org/2000/svg\"version=\"1.1\"width=\"440px\"height=\"220px\"viewBox=\"0 0 440 220\">");
                        seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "<rect width=\"440\"height=\"220\"style=\"fill:rgb(255,255,255);stroke-width:2;stroke:rgb(0,0,0)\"/>");

                        for (u8_t i = 20; i <= 200; i += 20)
                        {
                              seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "<line x1=\"50\"y1=\"%u\"x2=\"350\"y2=\"%u\"style=\"stroke:rgb(180,180,180);stroke-width:1\"/>", i, i);
                        }

                        seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "<text x=\"5\"y=\"15\"fill=\"black\">&#176C</text>");

                        i32_t deg = 50;
                        for (u8_t i = 25; i <= 205; i += 20)
                        {
                              seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "<text x=\"25\"y=\"%u\"fill=\"red\">%d</text>", i, deg);
                              deg -= 10;
                        }

                        seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "<text x=\"412\"y=\"15\"fill=\"black\">hPa</text>");

                        i32_t pre = 1060;
                        for (u8_t i = 25; i <= 205; i += 20)
                        {
                              seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "<text x=\"355\"y=\"%u\"fill=\"blue\">%d</text>", i, pre);
                              pre -= 20;
                        }

                        seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "<polyline points=\"");

                        u32_t x = 50;
                        for (i8_t i = DATA_COUNT; i > 0; --i)
                        {
                              struct meas data = readData(i);
                              seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "%u,%u ", x, 120 - (data.temp * 2));
                              x += GRAPH_X_POINT;
                        }

                        seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "\"style=\"fill:none;stroke:red;stroke-width:2\"/><polyline points=\"");

                        x = 50;
                        for (i8_t i = DATA_COUNT; i > 0; --i)
                        {
                              struct meas data = readData(i);
                              seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "%u,%u ", x, 220 - (data.pres - 860));
                              x += GRAPH_X_POINT;
                        }

                        seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "\"style=\"fill:none;stroke:blue;stroke-width:2\"/>");


                        seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "<line x1=\"50\"y1=\"0\"x2=\"50\"y2=\"220\"style=\"stroke:rgb(0,0,0);stroke-width:2\"/>");
                        seek += snprintf(buffer + seek, FILE_BUFFER_SIZE - seek, "<line x1=\"350\"y1=\"0\"x2=\"350\"y2=\"220\"style=\"stroke:rgb(0,0,0);stroke-width:2\"/></svg>");

                        u32_t size = strlen(buffer) < 3092 ? 3092 : strlen(buffer);

                        if (fwrite(buffer, sizeof(ch_t), size, file) == 0)
                        {
                              kprint("Write %s error\n", OUTPUT_FILE);
                        }

                        free(buffer);
                  }

                  if (fclose(file) != STD_RET_OK)
                        kprint("measd: error while closing 'file' file\n");
            }

            /* sleep in equal periods */
            SleepUntil(&LastWakeTime, SLEEP_TIME);
      }

      return STD_RET_OK;
}

/* End of application section declaration */
APP_SEC_END

/*==================================================================================================
                                            End of file
==================================================================================================*/
