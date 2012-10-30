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

/* Begin of application section declaration */
APPLICATION(measd)
APP_SEC_BEGIN

/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define DATA_COUNT                  20
#define FILE_BUFFER_SIZE            1024


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
      memset(dataList, 0, sizeof(dataList));

      /* prepare reference timer */
      size_t LastWakeTime = SystemGetOSTickCnt();

      for (;;)
      {
            FILE_t *sensor = fopen("/dev/sensor", "r");

            if (sensor)
            {
                  struct meas data;
                  data.pres = 0;
                  data.temp = 0;

                  while (ioctl(sensor, MPL115A2_IORQ_GETPRES, &data.pres) != STD_RET_OK)
                        Sleep(5);

                  while (ioctl(sensor, MPL115A2_IORQ_GETTEMP, &data.temp) != STD_RET_OK)
                        Sleep(5);

                  writeData(&data);

                  fclose(sensor);
            }

            FILE_t *file = fopen("/proc/graph.svg", "w");

            if (file)
            {
                  ch_t *buffer = Calloc(FILE_BUFFER_SIZE, sizeof(ch_t));

                  if (buffer)
                  {
                        u32_t seek = 0;

                        seek += snprint(buffer + seek, FILE_BUFFER_SIZE, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"500px\" height=\"300px\" viewBox=\"0 0 500 300\">");
                        seek += snprint(buffer + seek, FILE_BUFFER_SIZE, "<rect width=\"500\" height=\"300\" style=\"fill:rgb(255,255,255);stroke-width:1;stroke:rgb(0,0,0)\"/>");
                        seek += snprint(buffer + seek, FILE_BUFFER_SIZE, "<line x1=\"0\" y1=\"150\" x2=\"500\" y2=\"150\" style=\"stroke:rgb(180,180,180);stroke-width:2\"/>");
                        seek += snprint(buffer + seek, FILE_BUFFER_SIZE, "<polyline points=\"");

                        for (i8_t i = DATA_COUNT; i >= 0; --i)
                        {
                              struct meas data = readData(i);
                              seek += snprint(buffer + seek, FILE_BUFFER_SIZE, "%u,%u ", (DATA_COUNT * 25) - (i * 25), 150 - data.temp);
                        }

                        seek += snprint(buffer + seek, FILE_BUFFER_SIZE, "\" style=\"fill:none;stroke:red;stroke-width:2\" />");
                        seek += snprint(buffer + seek, FILE_BUFFER_SIZE, "</svg>");

                        fwrite(buffer, sizeof(ch_t), strlen(buffer), file);

                        Free(buffer);
                  }

                  fclose(file);
            }

            /* sleep in equal periods */
            SleepUntil(&LastWakeTime, 2000);
      }

      return STD_RET_OK;
}

/* End of application section declaration */
APP_SEC_END

/*==================================================================================================
                                            End of file
==================================================================================================*/
