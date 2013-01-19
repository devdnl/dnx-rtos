/*=============================================================================================*//**
@file    mpl115a2.c

@author  Daniel Zorychta

@brief   This file support temperature and pressure sensor - MPL115A2

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
#include "mpl115a2.h"
#include "i2c.h"
#include "vfs.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define MPL115A2_ADDRESS            0x60

#define I2CFILE                     "/dev/i2c"

/** multiplier used to remove fraction */
#define MULTIPLIER                  100

/** ADC temperature value @ 25C multiplied by 100 (605.75 * 100) */
#define ADC25C                      60575

/** degree per ADC count multiplied by 100 (-5.35 * 100) */
#define TEMP_A_FACTOR               (i16_t)-535

/** waiting for release resources */
#define BLOCK_TIME                  0

/** wait for release resources when driver is released */
#define RELEASE_BLOCK_TIME          1000


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
/** enum with registers names */
enum registers_enum
{
      REG_PADC_MSB = 0x00,
      REG_PADC_LSB = 0x01,
      REG_TADC_MSB = 0x02,
      REG_TADC_LSB = 0x03,
      REG_A0_MSB   = 0x04,
      REG_A0_LSB   = 0x05,
      REG_B1_MSB   = 0x06,
      REG_B1_LSB   = 0x07,
      REG_B2_MSB   = 0x08,
      REG_B2_LSB   = 0x09,
      REG_C12_MSB  = 0x0A,
      REG_C12_LSB  = 0x0B,
      REG_C11_MSB  = 0x0C,
      REG_C11_LSB  = 0x0D,
      REG_C22_MSB  = 0x0E,
      REG_C22_LSB  = 0x0F,
      REG_CONVERT  = 0x12,
};


/** data store structure */
struct mplst
{
      mutex_t mtx;
      i16_t   a0;
      i16_t   b1;
      i16_t   b2;
      i16_t   c12;
      i8_t    temp;
      u16_t   pres;
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static stdRet_t GetTemperature(i8_t *temperature);
static stdRet_t GetPressure(u16_t *pressure);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static struct mplst *mplmem;


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize MPL115A2 device
 *
 * @param dev     device number
 * @param part    device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t MPL115A2_Init(devx_t dev, fd_t part)
{
      (void)part;
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (mplmem == NULL) {
            mplmem = calloc(1, sizeof(struct mplst));

            if (mplmem) {
                  mplmem->mtx = CreateMutex();

                  if (mplmem->mtx) {
                        FILE_t *fi2c;
                        u8_t    tmp[8];

                        if ((fi2c = fopen(I2CFILE, "r")) != NULL) {
                              u32_t freq = MPL115A2_SCL_FREQUENCY;

                              if (ioctl(fi2c, I2C_IORQ_SETSCLFREQ, &freq) != STD_RET_OK) {
                                    goto MPL115A2_Init_File_Error;
                              }

                              tmp[0] = MPL115A2_ADDRESS;

                              if (ioctl(fi2c, I2C_IORQ_SETSLAVEADDR, &tmp[0]) == STD_RET_OK) {
                                    fseek(fi2c, REG_A0_MSB, SEEK_SET);

                                    if (fread(&tmp, sizeof(u8_t), ARRAY_SIZE(tmp), fi2c) == ARRAY_SIZE(tmp)) {
                                          mplmem->a0  = (tmp[0] << 8) | tmp[1];
                                          mplmem->b1  = (tmp[2] << 8) | tmp[3];
                                          mplmem->b2  = (tmp[4] << 8) | tmp[5];
                                          mplmem->c12 = (tmp[6] << 8) | tmp[7];

                                          status = STD_RET_OK;
                                    }
                              }

                              MPL115A2_Init_File_Error:
                              fclose(fi2c);
                        }
                  }

                  if (status == STD_RET_ERROR) {
                        if (mplmem->mtx) {
                              DeleteMutex(mplmem->mtx);
                        }

                        free(mplmem);
                        mplmem = NULL;
                  }
            }
      } else {
            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Release MPL115A2 device
 *
 * @param dev     device number
 * @param part    device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t MPL115A2_Release(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(mplmem->mtx, RELEASE_BLOCK_TIME) == OS_OK) {
            GiveMutex(mplmem->mtx);
            DeleteMutex(mplmem->mtx);
            free(mplmem);
            mplmem = NULL;

            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Opens specified port and initialize default settings
 *
 * @param[in]  dev                        MPL115A2 number
 * @param[in]  part                       device part
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  operation error
 */
//================================================================================================//
stdRet_t MPL115A2_Open(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Function close opened port
 *
 * @param[in]  dev                        MPL115A2 number
 * @param[in]  part                       device part
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  operation error
 */
//================================================================================================//
stdRet_t MPL115A2_Close(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Write data to MPL115A2
 *
 * @param[in]  dev                        MPL115A2 number
 * @param[in]  part                       device part
 * @param[in]  *src                       source buffer
 * @param[in]  size                       buffer size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @retval number of written nitems
 */
//================================================================================================//
size_t MPL115A2_Write(devx_t dev, fd_t part, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)part;
      (void)src;
      (void)size;
      (void)nitems;
      (void)seek;

      size_t n = 0;

      return n;
}


//================================================================================================//
/**
 * @brief Write data to MPL115A2
 *
 * @param[in]  dev                        MPL115A2 number
 * @param[in]  part                       device part
 * @param[in]  *dst                       destination buffer
 * @param[in]  size                       item size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @retval number of read nitems
 */
//================================================================================================//
size_t MPL115A2_Read(devx_t dev, fd_t part, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)part;
      (void)dst;
      (void)size;
      (void)nitems;
      (void)seek;

      size_t n = 0;

      return n;
}


//================================================================================================//
/**
 * @brief Specific settings of MPL115A2
 *
 * @param[in    ] dev           MPL115A2 device
 * @param[in    ] part          device part
 * @param[in    ] ioRQ          input/output request
 * @param[in,out] *data         input/output data
 *
 * @retval STD_RET_OK           operation success
 * @retval STD_RET_ERROR        operation error
 */
//================================================================================================//
stdRet_t MPL115A2_IOCtl(devx_t dev, fd_t part, IORq_t ioRQ, void *data)
{
      (void)dev;
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      switch (ioRQ) {
      case MPL115A2_IORQ_GETTEMP:
            if (data) {
                  status = GetTemperature(data);
            }
            break;

      case MPL115A2_IORQ_GETPRES:
            if (data) {
                  status = GetPressure(data);
            }
            break;

      default:
            break;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Get temperature
 *
 * @param[out] *temperature   range: -128..+127^C; resolution: 1^C
 *
 * @retval STD_RET_OK         read success
 * @retval STD_RET_ERROR      error occur
 */
//================================================================================================//
static stdRet_t GetTemperature(i8_t *temperature)
{
      stdRet_t  status = STD_RET_ERROR;
      u8_t      tmp[2];
      u16_t     temp;
      FILE_t   *fi2c;

      if (TakeMutex(mplmem->mtx, BLOCK_TIME) == OS_OK) {
            if ((fi2c = fopen(I2CFILE, "r+")) != NULL) {
                  u32_t freq = MPL115A2_SCL_FREQUENCY;

                  if (ioctl(fi2c, I2C_IORQ_SETSCLFREQ, &freq) != STD_RET_OK) {
                        goto MPL115A2_GetTemperature_ClosePort;
                  }

                  tmp[0] = MPL115A2_ADDRESS;

                  if (ioctl(fi2c, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK) {
                        goto MPL115A2_GetTemperature_ClosePort;
                  }

                  /* start new conversion */
                  tmp[0] = 0x01;
                  fseek(fi2c, REG_CONVERT, SEEK_SET);

                  if (fwrite(&tmp, sizeof(u8_t), 1, fi2c) != 1) {
                        goto MPL115A2_GetTemperature_ClosePort;
                  }

                  TaskDelay(5);

                  /* load temperature */
                  fseek(fi2c, REG_TADC_MSB, SEEK_SET);

                  if (fread(&tmp, sizeof(u8_t), ARRAY_SIZE(tmp), fi2c) == ARRAY_SIZE(tmp)) {
                        /* binds temperature */
                        temp = (((tmp[0] << 8) | tmp[1]) >> 6);
                        mplmem->temp = (i8_t)(((i16_t)(temp * MULTIPLIER - ADC25C)) / TEMP_A_FACTOR);

                        status = STD_RET_OK;
                  }

                  MPL115A2_GetTemperature_ClosePort:
                  fclose(fi2c);
            }

            GiveMutex(mplmem->mtx);
      }

      *temperature = mplmem->temp;

      return status;
}


//================================================================================================//
/**
 * @brief Get Pressure
 *
 * @param[out] *pressure      range: 500..1150hPa
 *
 * @retval STD_RET_OK         read success
 * @retval STD_RET_ERROR      error occur
 */
//================================================================================================//
static stdRet_t GetPressure(u16_t *pressure)
{
      stdRet_t  status = STD_RET_ERROR;
      u8_t      tmp[4];
      FILE_t   *fi2c;

      if (TakeMutex(mplmem->mtx, BLOCK_TIME) == OS_OK) {
            if ((fi2c = fopen(I2CFILE, "r+")) != NULL) {
                  u32_t freq = MPL115A2_SCL_FREQUENCY;

                  if (ioctl(fi2c, I2C_IORQ_SETSCLFREQ, &freq) != STD_RET_OK) {
                        goto MPL115A2_GetPressure_ClosePort;
                  }

                  tmp[0] = MPL115A2_ADDRESS;

                  if (ioctl(fi2c, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
                        goto MPL115A2_GetPressure_ClosePort;

                  /* start new conversion */
                  tmp[0] = 0x01;

                  fseek(fi2c, REG_CONVERT, SEEK_SET);

                  if (fwrite(&tmp, sizeof(u8_t), 1, fi2c) != 1)
                        goto MPL115A2_GetPressure_ClosePort;

                  TaskDelay(5);

                  /* load temperature */
                  fseek(fi2c, REG_PADC_MSB, 0);

                  if (fread(&tmp, sizeof(u8_t), ARRAY_SIZE(tmp), fi2c) == ARRAY_SIZE(tmp)) {
                        /* binds pressure */
                        u16_t Padc = ((tmp[0] << 8) | tmp[1]) >> 6;

                        /* binds temperature */
                        u16_t Tadc = ((tmp[2] << 8) | tmp[3]) >> 6;

                        /* compute pressure */
                        i32_t si_c12x2 = mplmem->c12 * Tadc;
                        i32_t si_a1    = (((mplmem->b1 << 11) + si_c12x2) >> 11);
                        i32_t si_a2    = mplmem->b2 >> 1;
                        i32_t si_a1x1  = si_a1 * Padc;
                        i32_t si_y1    = ((mplmem->a0 << 10) + si_a1x1) >> 10;
                        i32_t si_a2x2  = si_a2 * Tadc;
                        i32_t siPcomp  = ((si_y1 << 10) + si_a2x2) >> 13;

                        mplmem->pres = (u16_t)((650 * siPcomp) / 1023) + 500;

                        status = STD_RET_OK;
                  }

                  MPL115A2_GetPressure_ClosePort:
                  fclose(fi2c);
            }

            GiveMutex(mplmem->mtx);
      }

      *pressure = mplmem->pres;

      return status;
}

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
