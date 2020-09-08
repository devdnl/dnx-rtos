/*==============================================================================
File     fmc.c

Author   Daniel Zorychta

Brief    Flexible Memory Controller

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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


==============================================================================*/

// TODO Driver is not completed!

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "stm32f4/fmc_cfg.h"
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"
#include "../fmc_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define SDRAM_MODE__NORMAL      0x0
#define SDRAM_MODE__CLK_CFG_EN  0x1
#define SDRAM_MODE__PALL        0x2
#define SDRAM_MODE__AUTOREFRESH 0x3
#define SDRAM_MODE__LOADMODEREG 0x4
#define SDRAM_MODE__SELFREFRESH 0x5
#define SDRAM_MODE__POWERDOWN   0x6

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        void  *start;
        size_t size;
} FMC_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int SDRAM_init(void);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(FMC);

#if __FMC_SDRAM_1_ENABLE__ > 0
static mem_region_t sdram1;
#endif

#if __FMC_SDRAM_2_ENABLE__ > 0
static mem_region_t sdram2;
#endif

/*==============================================================================
  Exported object
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device.
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_INIT(FMC, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        int err = EFAULT;

        if (major == 0 && minor == 0) {
                err = sys_zalloc(sizeof(FMC_t), device_handle);
                if (!err) {
                        //...
                }

                err = SDRAM_init();
        }

        return err;
}

//==============================================================================
/**
 * @brief Release device.
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_RELEASE(FMC, void *device_handle)
{
        UNUSED_ARG1(device_handle);
        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Open device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_OPEN(FMC, void *device_handle, u32_t flags)
{
        UNUSED_ARG2(device_handle, flags);
        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Close device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_CLOSE(FMC, void *device_handle, bool force)
{
        UNUSED_ARG2(device_handle, force);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Write data to device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_WRITE(FMC,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG6(device_handle, src, count, fpos, wrcnt, fattr);
        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Read data from device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_READ(FMC,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG6(device_handle, dst, count, fpos, rdcnt, fattr);
        return ENOTSUP;
}

//==============================================================================
/**
 * @brief IO control.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_IOCTL(FMC, void *device_handle, int request, void *arg)
{
        UNUSED_ARG3(device_handle, request, arg);
        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Flush device.
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_FLUSH(FMC, void *device_handle)
{
        UNUSED_ARG1(device_handle);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Device information.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_STAT(FMC, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_size = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function test RAM memory.
 *
 * @param  address      address
 * @param  size         size (bytes)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int ram_test(void *address, size_t size)
{
        static const u32_t PATTERN[] = {
                0x08080808,
                0x80008000,
                0xAAAA5555,
                0x5555AAAA,
        };

        int err = 0;

        printk("FMC: memory test @ %p of %u bytes", address, size);

        for (size_t i = 0; i < ARRAY_SIZE(PATTERN); i++) {

                u32_t *ptr = address;
                size_t len = size / sizeof(u32_t);

                while (len) {
                        *ptr++ = PATTERN[i];
                        len--;
                }

                ptr = address;
                len = size / sizeof(u32_t);
                while (len) {
                        if (*ptr != PATTERN[i]) {
                                printk("FMC: error @ %p", ptr);
                                err = EFAULT;
                                break;
                        }
                        ptr++;
                        len--;
                }
        }

        u32_t *ptr = address;
        size_t len = size / sizeof(u32_t);

        while (len) {
                *ptr++ = len;
                len--;
        }

        ptr = address;
        len = size / sizeof(u32_t);
        while (len) {
                if (*ptr != len) {
                        printk("FMC: error @ %p", ptr);
                        err = EFAULT;
                        break;
                }
                ptr++;
                len--;
        }

        printk("FMC: memory test done.");

        return err;
}

//==============================================================================
/**
 * @brief Function initialize SDRAM controller.
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int SDRAM_init(void)
{
        RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;

        /*
         * 1. Program the memory device features into the FMC_SDCRx register.
         *    The SDRAM clock frequency, RBURST and RPIPE must be programmed
         *    in the FMC_SDCR1 register.
         */
        u8_t TRAS  = __FMC_SDRAM_TRAS__ - 1;
        u8_t TRC   = __FMC_SDRAM_TRC__ - 1;
        u8_t TRP   = __FMC_SDRAM_TRP__ - 1;
        u8_t TRCD1 = __FMC_SDRAM_1_TRCD__ - 1;
        u8_t TRCD2 = __FMC_SDRAM_2_TRCD__ - 1;

        u8_t TWR1 = 0;
        TWR1 = max(TRAS - TRCD1, TWR1);
        TWR1 = max(TRC - TRCD1 - TRP, TWR1);
        TWR1 = TWR1 * __FMC_SDRAM_1_ENABLE__;

        u8_t TWR2 = 0;
        TWR2 = max(TRAS - TRCD2, TWR2);
        TWR2 = max(TRC - TRCD2 - TRP, TWR2);
        TWR2 = TWR2 * __FMC_SDRAM_2_ENABLE__;

        u8_t TWR = max(TWR1, TWR2);

        FMC_Bank5_6->SDCR[0] = ((__FMC_SDRAM_RPIPE__  & 3) << 13)
                             | ((__FMC_SDRAM_RBURST__ & 1) << 12)
                             | ((__FMC_SDRAM_SDCLK__  & 3) << 10)
                             | ((__FMC_SDRAM_CAS__    & 3) <<  7)
                             | ((__FMC_SDRAM_1_NB__   & 1) <<  6)
                             | ((__FMC_SDRAM_1_MWID__ & 3) <<  4)
                             | ((__FMC_SDRAM_1_NR__   & 3) <<  2)
                             | ((__FMC_SDRAM_1_NC__   & 3) <<  0);

        FMC_Bank5_6->SDCR[1] = ((__FMC_SDRAM_RPIPE__  & 3) << 13)
                             | ((__FMC_SDRAM_RBURST__ & 1) << 12)
                             | ((__FMC_SDRAM_SDCLK__  & 3) << 10)
                             | ((__FMC_SDRAM_CAS__    & 3) <<  7)
                             | ((__FMC_SDRAM_2_NB__   & 1) <<  6)
                             | ((__FMC_SDRAM_2_MWID__ & 3) <<  4)
                             | ((__FMC_SDRAM_2_NR__   & 3) <<  2)
                             | ((__FMC_SDRAM_2_NC__   & 3) <<  0);

        /*
         * 2. Program the memory device timing into the FMC_SDTRx register.
         *    The TRP and TRC timings must be programmed in the FMC_SDTR1
         *    register.
         *
         */
        FMC_Bank5_6->SDTR[0] = (((__FMC_SDRAM_1_TRCD__ - 1) & 0xF) << 24)
                             | (((__FMC_SDRAM_TRP__    - 1) & 0xF) << 20)
                             | (((  TWR                   ) & 0xF) << 16)
                             | (((__FMC_SDRAM_TRC__    - 1) & 0xF) << 12)
                             | (((__FMC_SDRAM_TRAS__   - 1) & 0xF) <<  8)
                             | (((__FMC_SDRAM_TXSR__   - 1) & 0xF) <<  4)
                             | (((__FMC_SDRAM_TMRD__   - 1) & 0xF) <<  0);

        FMC_Bank5_6->SDTR[1] = (((__FMC_SDRAM_2_TRCD__ - 1) & 0xF) << 24)
                             | (((__FMC_SDRAM_TRP__    - 1) & 0xF) << 20)
                             | (((  TWR                   ) & 0xF) << 16)
                             | (((__FMC_SDRAM_TRC__    - 1) & 0xF) << 12)
                             | (((__FMC_SDRAM_TRAS__   - 1) & 0xF) <<  8)
                             | (((__FMC_SDRAM_TXSR__   - 1) & 0xF) <<  4)
                             | (((__FMC_SDRAM_TMRD__   - 1) & 0xF) <<  0);

        /*
         * 3. Set MODE bits to '001' and configure the Target Bank bits
         *    (CTB1 and/or CTB2) in the FMC_SDCMR register to start delivering
         *    the clock to the memory (SDCKE is driven high).
         */
        while (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);

        FMC_Bank5_6->SDCMR = (SDRAM_MODE__CLK_CFG_EN)
                           | (__FMC_SDRAM_1_ENABLE__ * FMC_SDCMR_CTB1)
                           | (__FMC_SDRAM_2_ENABLE__ * FMC_SDCMR_CTB2)
                           | (1 << FMC_SDCMR_NRFS_Pos);

        /*
         * 4. Wait during the prescribed delay period. Typical delay is around
         *    100 μs (refer to the SDRAM datasheet for the required delay after
         *    power-up).
         */
        sys_sleep_ms(1);

        /*
         * 5. Set MODE bits to ‘010’ and configure the Target Bank bits
         *    (CTB1 and/or CTB2) in the FMC_SDCMR register to issue
         *    a “Precharge All” command.
         */
        while (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);

        FMC_Bank5_6->SDCMR = (SDRAM_MODE__PALL)
                           | (__FMC_SDRAM_1_ENABLE__ * FMC_SDCMR_CTB1)
                           | (__FMC_SDRAM_2_ENABLE__ * FMC_SDCMR_CTB2)
                           | (1 << FMC_SDCMR_NRFS_Pos);

        /*
         * 6. Set MODE bits to ‘011’, and configure the Target Bank bits
         *    (CTB1 and/or CTB2) as well as the number of consecutive Auto-refresh
         *    commands (NRFS) in the FMC_SDCMR register. Refer to the SDRAM
         *    datasheet for the number of Auto-refresh commands that should be
         *    issued. Typical number is 8.
         */
        while (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);

        FMC_Bank5_6->SDCMR = (SDRAM_MODE__AUTOREFRESH)
                           | (__FMC_SDRAM_1_ENABLE__ * FMC_SDCMR_CTB1)
                           | (__FMC_SDRAM_2_ENABLE__ * FMC_SDCMR_CTB2)
                           | (((__FMC_SDRAM_NRFS__ - 1) & 0xF) << FMC_SDCMR_NRFS_Pos);

        /*
         * 7. Configure the MRD field according to your SDRAM device, set the
         *    MODE bits to '100', and configure the Target Bank bits
         *    (CTB1 and/or CTB2) in the FMC_SDCMR register to issue a
         *    "Load Mode Register" command in order to program the SDRAM. In
         *    particular:
         *    a) The CAS latency must be selected following configured value in
         *       FMC_SDCR1/2 registers
         *    b) The Burst Length (BL) of 1 must be selected by configuring the
         *       M[2:0] bits to 000 in the mode register (refer to the SDRAM
         *       datasheet). If the Mode Register is not the same for both SDRAM
         *       banks, this step has to be repeated twice, once for each bank,
         *       and the Target Bank bits set accordingly.
         */
        while (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);

        u32_t MRD = (0x1 << 9)                  // Write Burst Mode: 1 - single location access
                  | (0x0 << 7)                  // Operation Mode  : 0 - standard operation
                  | (__FMC_SDRAM_CAS__ << 4)    // CAS Latency Mode
                  | (0x0 << 3)                  // Burst Type      : 0 - sequential
                  | (0x0 << 0);                 // Burst Length    : 0 - 1 byte

        FMC_Bank5_6->SDCMR = (SDRAM_MODE__LOADMODEREG)
                           | (__FMC_SDRAM_1_ENABLE__ * FMC_SDCMR_CTB1)
                           | (__FMC_SDRAM_2_ENABLE__ * FMC_SDCMR_CTB2)
                           | (1 << FMC_SDCMR_NRFS_Pos)
                           | (MRD << FMC_SDCMR_MRD_Pos);

        /*
         * 8. Program the refresh rate in the FMC_SDRTR register. The refresh
         *    rate corresponds to the delay between refresh cycles. Its value
         *    must be adapted to SDRAM devices.
         *
         */
        while (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);

        RCC_ClocksTypeDef freq;
        RCC_GetClocksFreq(&freq);

        i32_t NR  = max((2 << (__FMC_SDRAM_1_NR__ + 10)), (2 << (__FMC_SDRAM_2_NR__ + 10)));
        i32_t MHz = freq.HCLK_Frequency / __FMC_SDRAM_SDCLK__ / 1000000;
        i32_t SRR = (((__FMC_SDRAM_REFRESH_RATE_MS__ * 1000) / NR) * MHz) - 20;

        if (SRR < 0) {
                printk("FMC: incorrect configuration of SDRAM refresh rate.");
                SRR = 512;
        }

        FMC_Bank5_6->SDRTR |= (SRR << FMC_SDRTR_COUNT_Pos);

        while (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);

        /*
         * Register memory regions in system
         */
        int err1 = ESUCC;
        int err2 = ESUCC;

#if __FMC_SDRAM_1_ENABLE__ > 0
        size_t mem_size1 = (2 << (__FMC_SDRAM_1_NR__ + 10))      // Row bits (0 - 11 bits: A10)
                         * (2 << (__FMC_SDRAM_1_NC__ +  7))      // Col bits (0 - 8 bits:  A7)
                         * (2 << (__FMC_SDRAM_1_NB__     ))      // Banks (2 or 4)
                         * (8 << (__FMC_SDRAM_1_MWID__   ))      // Bus width
                         / (8);                                  // Bits per byte

        err1 = ram_test((void*)0xC0000000, mem_size1);
        if (!err1) {
                err1 = sys_memory_register(&sdram1, (void*)0xC0000000, mem_size1);
        }
#endif

#if __FMC_SDRAM_2_ENABLE__ > 0
        size_t mem_size2 = (2 << (__FMC_SDRAM_2_NR__ + 10))     // Row bits (0 - 11 bits: A10)
                         * (2 << (__FMC_SDRAM_2_NC__ +  7))     // Col bits (0 - 8 bits:  A7)
                         * (2 << (__FMC_SDRAM_2_NB__     ))     // Banks (2 or 4)
                         * (8 << (__FMC_SDRAM_2_MWID__   ))     // Bus width
                         / (8));

        err2 = ram_test((void*)0xD0000000, mem_size2);
        if (!err2) {
                err2 = sys_memory_register(&sdram2, (void*)0xD0000000, mem_size2);
        }
#endif

        return err1 ? err1 : (err2 ? err2 : ESUCC);
}

/*==============================================================================
  End of file
==============================================================================*/
