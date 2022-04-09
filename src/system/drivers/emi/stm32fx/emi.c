/*==============================================================================
File     emi.c

Author   Daniel Zorychta

Brief    External Memory Interface

         Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "../emi_ioctl.h"

#if defined(ARCH_stm32f1)
#error No FMC implementation!
#elif defined(ARCH_stm32f4)
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"
#elif defined(ARCH_stm32f7)
#include "stm32f7/stm32f7xx.h"
#include "stm32f7/lib/stm32f7xx_rcc.h"
#elif defined(ARCH_stm32h7)
#include "stm32h7/stm32h7xx.h"
#include "stm32h7/lib/stm32h7xx_ll_rcc.h"
#endif

/*==============================================================================
  Local macros
==============================================================================*/
#define SDRAM_CMD__NORMAL                       0x0
#define SDRAM_CMD__CLK_CFG_EN                   0x1
#define SDRAM_CMD__PRECHARGE_ALL                0x2
#define SDRAM_CMD__AUTOREFRESH                  0x3
#define SDRAM_CMD__LOADMODEREG                  0x4
#define SDRAM_CMD__SELFREFRESH                  0x5
#define SDRAM_CMD__POWERDOWN                    0x6

#define SDRAM_MODE_REG_BURST_LENGTH_Pos         0
#define SDRAM_MODE_REG_BURST_LENGTH_1           (0 << SDRAM_MODE_REG_BURST_LENGTH_Pos)
#define SDRAM_MODE_REG_BURST_LENGTH_2           (1 << SDRAM_MODE_REG_BURST_LENGTH_Pos)
#define SDRAM_MODE_REG_BURST_LENGTH_4           (2 << SDRAM_MODE_REG_BURST_LENGTH_Pos)
#define SDRAM_MODE_REG_BURST_LENGTH_8           (3 << SDRAM_MODE_REG_BURST_LENGTH_Pos)
#define SDRAM_MODE_REG_BURST_LENGTH_FULL_PAGE   (7 << SDRAM_MODE_REG_BURST_LENGTH_Pos)

#define SDRAM_MODE_REG_BURST_TYPE_Pos           3
#define SDRAM_MODE_REG_BURST_TYPE_SEQUENTIAL    (0 << SDRAM_MODE_REG_BURST_TYPE_Pos)
#define SDRAM_MODE_REG_BURST_TYPE_INTERLEAVED   (1 << SDRAM_MODE_REG_BURST_TYPE_Pos)

#define SDRAM_MODE_REG_CAS_LATENCY_Pos          4
#define SDRAM_MODE_REG_CAS_LATENCY_1            (1 << SDRAM_MODE_REG_CAS_LATENCY_Pos)
#define SDRAM_MODE_REG_CAS_LATENCY_2            (2 << SDRAM_MODE_REG_CAS_LATENCY_Pos)
#define SDRAM_MODE_REG_CAS_LATENCY_3            (3 << SDRAM_MODE_REG_CAS_LATENCY_Pos)

#define SDRAM_MODE_REG_OPERATING_MODE_Pos       7
#define SDRAM_MODE_REG_OPERATING_MODE_STANDARD  (0 << SDRAM_MODE_REG_OPERATING_MODE_Pos)

#define SDRAM_MODE_REG_WRITE_BURST_MODE_Pos     9
#define SDRAM_MODE_REG_WRITE_BURST_MODE_PROG_LENGTH (0 << SDRAM_MODE_REG_WRITE_BURST_MODE_Pos)
#define SDRAM_MODE_REG_WRITE_BURST_MODE_SINGLE_LOCATION_ACCESS (1 << SDRAM_MODE_REG_WRITE_BURST_MODE_Pos)

#if defined(ARCH_stm32f1)
#error No FMC implementation!
#elif defined(ARCH_stm32f4)
#define FMC_BANK_5_6            FMC_Bank5_6
#define FMC_ENABLE()
#elif defined(ARCH_stm32f7)
#define FMC_BANK_5_6            FMC_Bank5_6_R
#define FMC_ENABLE()
#elif defined(ARCH_stm32h7)
#define FMC_BANK_5_6            FMC_Bank5_6_R
#define FMC_ENABLE()            SET_BIT(FMC_Bank1_R->BTCR[0], FMC_BCR1_FMCEN)
#endif

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
static int register_heap_regions(void *addr, size_t mem_size, mem_region_t region[],
                                 size_t region_num, size_t region_size);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(FMC);

#if __EMI_SDRAM_1_ENABLE__ > 0
static mem_region_t sdram1_heap[__EMI_SDRAM_1_HEAP_REGIONS__ + 1];
#endif

#if __EMI_SDRAM_2_ENABLE__ > 0
static mem_region_t sdram2_heap[__EMI_SDRAM_2_HEAP_REGIONS__ + 1];
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
API_MOD_INIT(EMI, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        int err = EFAULT;

        if (major == 0 && minor == 0) {
                err = sys_zalloc(sizeof(FMC_t), device_handle);
                if (!err) {
                        //...
                }

                SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);
                volatile u32_t tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);
                UNUSED(tmpreg);

                SET_BIT(RCC->AHB3RSTR, RCC_AHB3RSTR_FMCRST);
                CLEAR_BIT(RCC->AHB3RSTR, RCC_AHB3RSTR_FMCRST);

                err = SDRAM_init();

                FMC_ENABLE();
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
API_MOD_RELEASE(EMI, void *device_handle)
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
API_MOD_OPEN(EMI, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(EMI, void *device_handle, bool force)
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
API_MOD_WRITE(EMI,
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
API_MOD_READ(EMI,
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
API_MOD_IOCTL(EMI, void *device_handle, int request, void *arg)
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
API_MOD_FLUSH(EMI, void *device_handle)
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
API_MOD_STAT(EMI, void *device_handle, struct vfs_dev_stat *device_stat)
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

        printk("FMC: memory test success.");

        return err;
}

//==============================================================================
/**
 * @brief  Wait while the SDRAM is busy.
 */
//==============================================================================
static void wait_while_sdram_busy(void)
{
#if defined(ARCH_stm32f1)
#error No FMC implementation!
#elif defined(ARCH_stm32f4)
        while (FMC_BANK_5_6->SDSR & FMC_SDSR_BUSY);
#elif defined(ARCH_stm32f7)
        sys_sleep_ms(2);
#elif defined(ARCH_stm32h7)
        sys_sleep_ms(2);
#endif
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
        /*
         * 1. Program the memory device features into the FMC_SDCRx register.
         *    The SDRAM clock frequency, RBURST and RPIPE must be programmed
         *    in the FMC_SDCR1 register.
         */
        FMC_BANK_5_6->SDCR[0] = ((__EMI_SDRAM_RPIPE__  & 3) << FMC_SDCRx_RPIPE_Pos)
                              | ((__EMI_SDRAM_RBURST__ & 1) << FMC_SDCRx_RBURST_Pos)
                              | ((__EMI_SDRAM_SDCLK__  & 3) << FMC_SDCRx_SDCLK_Pos)
                              | ((__EMI_SDRAM_CAS__    & 3) << FMC_SDCRx_CAS_Pos)
                              | ((__EMI_SDRAM_1_NB__   & 1) << FMC_SDCRx_NB_Pos)
                              | ((__EMI_SDRAM_1_MWID__ & 3) << FMC_SDCRx_MWID_Pos)
                              | ((__EMI_SDRAM_1_NR__   & 3) << FMC_SDCRx_NR_Pos)
                              | ((__EMI_SDRAM_1_NC__   & 3) << FMC_SDCRx_NC_Pos);

        FMC_BANK_5_6->SDCR[1] = ((__EMI_SDRAM_RPIPE__  & 3) << FMC_SDCRx_RPIPE_Pos)
                              | ((__EMI_SDRAM_RBURST__ & 1) << FMC_SDCRx_RBURST_Pos)
                              | ((__EMI_SDRAM_SDCLK__  & 3) << FMC_SDCRx_SDCLK_Pos)
                              | ((__EMI_SDRAM_CAS__    & 3) << FMC_SDCRx_CAS_Pos)
                              | ((__EMI_SDRAM_2_NB__   & 1) << FMC_SDCRx_NB_Pos)
                              | ((__EMI_SDRAM_2_MWID__ & 3) << FMC_SDCRx_MWID_Pos)
                              | ((__EMI_SDRAM_2_NR__   & 3) << FMC_SDCRx_NR_Pos)
                              | ((__EMI_SDRAM_2_NC__   & 3) << FMC_SDCRx_NC_Pos);

        /*
         * 2. Program the memory device timing into the FMC_SDTRx register.
         *    The TRP and TRC timings must be programmed in the FMC_SDTR1
         *    register.
         *
         */
        FMC_BANK_5_6->SDTR[0] = (((__EMI_SDRAM_1_TRCD__ - 1) & 0xF) << FMC_SDTRx_TRCD_Pos)
                              | (((__EMI_SDRAM_TRP__    - 1) & 0xF) << FMC_SDTRx_TRP_Pos)
                              | (((__EMI_SDRAM_TWR__    - 1) & 0xF) << FMC_SDTRx_TWR_Pos)
                              | (((__EMI_SDRAM_TRC__    - 1) & 0xF) << FMC_SDTRx_TRC_Pos)
                              | (((__EMI_SDRAM_TRAS__   - 1) & 0xF) << FMC_SDTRx_TRAS_Pos)
                              | (((__EMI_SDRAM_TXSR__   - 1) & 0xF) << FMC_SDTRx_TXSR_Pos)
                              | (((__EMI_SDRAM_TMRD__   - 1) & 0xF) << FMC_SDTRx_TMRD_Pos);

        FMC_BANK_5_6->SDTR[1] = (((__EMI_SDRAM_2_TRCD__ - 1) & 0xF) << FMC_SDTRx_TRCD_Pos)
                              | (((__EMI_SDRAM_TRP__    - 1) & 0xF) << FMC_SDTRx_TRP_Pos)
                              | (((__EMI_SDRAM_TWR__    - 1) & 0xF) << FMC_SDTRx_TWR_Pos)
                              | (((__EMI_SDRAM_TRC__    - 1) & 0xF) << FMC_SDTRx_TRC_Pos)
                              | (((__EMI_SDRAM_TRAS__   - 1) & 0xF) << FMC_SDTRx_TRAS_Pos)
                              | (((__EMI_SDRAM_TXSR__   - 1) & 0xF) << FMC_SDTRx_TXSR_Pos)
                              | (((__EMI_SDRAM_TMRD__   - 1) & 0xF) << FMC_SDTRx_TMRD_Pos);

        FMC_ENABLE();

        /*
         * 3. Set MODE bits to '001' and configure the Target Bank bits
         *    (CTB1 and/or CTB2) in the FMC_SDCMR register to start delivering
         *    the clock to the memory (SDCKE is driven high).
         */
        wait_while_sdram_busy();

        FMC_BANK_5_6->SDCMR = (SDRAM_CMD__CLK_CFG_EN)
                            | (__EMI_SDRAM_1_ENABLE__ * FMC_SDCMR_CTB1)
                            | (__EMI_SDRAM_2_ENABLE__ * FMC_SDCMR_CTB2)
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
        wait_while_sdram_busy();

        FMC_BANK_5_6->SDCMR = (SDRAM_CMD__PRECHARGE_ALL)
                            | (__EMI_SDRAM_1_ENABLE__ * FMC_SDCMR_CTB1)
                            | (__EMI_SDRAM_2_ENABLE__ * FMC_SDCMR_CTB2)
                            | (1 << FMC_SDCMR_NRFS_Pos);

        /*
         * 6. Set MODE bits to ‘011’, and configure the Target Bank bits
         *    (CTB1 and/or CTB2) as well as the number of consecutive Auto-refresh
         *    commands (NRFS) in the FMC_SDCMR register. Refer to the SDRAM
         *    datasheet for the number of Auto-refresh commands that should be
         *    issued. Typical number is 8.
         */
        wait_while_sdram_busy();

        FMC_BANK_5_6->SDCMR = (SDRAM_CMD__AUTOREFRESH)
                            | (__EMI_SDRAM_1_ENABLE__ * FMC_SDCMR_CTB1)
                            | (__EMI_SDRAM_2_ENABLE__ * FMC_SDCMR_CTB2)
                            | (((__EMI_SDRAM_NRFS__ - 1) & 0xF) << FMC_SDCMR_NRFS_Pos);

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
        wait_while_sdram_busy();

        u32_t MRD = SDRAM_MODE_REG_WRITE_BURST_MODE_SINGLE_LOCATION_ACCESS
                  | SDRAM_MODE_REG_OPERATING_MODE_STANDARD
                  | (__EMI_SDRAM_CAS__ << SDRAM_MODE_REG_CAS_LATENCY_Pos)
                  | SDRAM_MODE_REG_BURST_TYPE_SEQUENTIAL
                  | SDRAM_MODE_REG_BURST_LENGTH_1;

        FMC_BANK_5_6->SDCMR = (SDRAM_CMD__LOADMODEREG)
                            | (__EMI_SDRAM_1_ENABLE__ * FMC_SDCMR_CTB1)
                            | (__EMI_SDRAM_2_ENABLE__ * FMC_SDCMR_CTB2)
                            | (1 << FMC_SDCMR_NRFS_Pos)
                            | (MRD << FMC_SDCMR_MRD_Pos);

        /*
         * 8. Program the refresh rate in the FMC_SDRTR register. The refresh
         *    rate corresponds to the delay between refresh cycles. Its value
         *    must be adapted to SDRAM devices.
         */
        wait_while_sdram_busy();

        u32_t fmc_freq = 0;
#if defined(ARCH_stm32f1)
#elif defined(ARCH_stm32f4)
        LL_RCC_ClocksTypeDef freq;
        LL_RCC_GetSystemClocksFreq(&freq);
        fmc_freq = HCLK_Frequency;
#elif defined(ARCH_stm32f7)
        LL_RCC_ClocksTypeDef freq;
        LL_RCC_GetSystemClocksFreq(&freq);
        fmc_freq = HCLK_Frequency;
#elif defined(ARCH_stm32h7)
        fmc_freq = LL_RCC_GetFMCClockFreq(LL_RCC_FMC_CLKSOURCE);
#endif

        i32_t NR  = max((2 << (__EMI_SDRAM_1_NR__ + 10)), (2 << (__EMI_SDRAM_2_NR__ + 10)));
        i32_t MHz = fmc_freq / __EMI_SDRAM_SDCLK__ / 1000000;
        i32_t SRR = (((__EMI_SDRAM_REFRESH_RATE_MS__ * 1000) / NR) * MHz) - 20;

        if (SRR < 41) {
                printk("FMC: incorrect SDRAM refresh rate.");
                SRR = 41;
        }

        FMC_BANK_5_6->SDRTR |= (SRR << FMC_SDRTR_COUNT_Pos);

        wait_while_sdram_busy();

        /*
         * Register memory regions in system
         */
        int err1 = ESUCC;
        int err2 = ESUCC;

#if __EMI_SDRAM_1_ENABLE__ > 0
        u32_t mem_addr1 = 0xC0000000;

        size_t mem_size1 = (2 << (__EMI_SDRAM_1_NR__ + 10))      // Row bits (0 - 11 bits: A10)
                         * (2 << (__EMI_SDRAM_1_NC__ +  7))      // Col bits (0 - 8 bits:  A7)
                         * (2 << (__EMI_SDRAM_1_NB__     ))      // Banks (2 or 4)
                         * (8 << (__EMI_SDRAM_1_MWID__   ))      // Bus width
                         / (8);                                  // Bits per byte

        err1 = ram_test((void*)mem_addr1, mem_size1);
        if (!err1) {
                err1 = register_heap_regions((void*)mem_addr1, mem_size1,
                                             sdram1_heap, ARRAY_SIZE(sdram1_heap),
                                             (__EMI_SDRAM_1_HEAP_REGION_SIZE__ * 1024) & 0xFFFFFFFC);
        }
#endif

#if __EMI_SDRAM_2_ENABLE__ > 0
        u32_t mem_addr2 = 0xD0000000;

        size_t mem_size2 = (2 << (__EMI_SDRAM_2_NR__ + 10))      // Row bits (0 - 11 bits: A10)
                         * (2 << (__EMI_SDRAM_2_NC__ +  7))      // Col bits (0 - 8 bits:  A7)
                         * (2 << (__EMI_SDRAM_2_NB__     ))      // Banks (2 or 4)
                         * (8 << (__EMI_SDRAM_2_MWID__   ))      // Bus width
                         / (8);                                  // Bits per byte

        err2 = ram_test((void*)mem_addr2, mem_size2);
        if (!err2) {
                err2 = register_heap_regions((void*)mem_addr2, mem_size2,
                                             sdram2_heap, ARRAY_SIZE(sdram2_heap),
                                             (__EMI_SDRAM_2_HEAP_REGION_SIZE__ * 1024) & 0xFFFFFFFC);
        }
#endif

        return err1 ? err1 : (err2 ? err2 : ESUCC);
}

//==============================================================================
/**
 * @brief  Function register selected number of memory regions in SDRAM.
 *
 * @param  add          memory start address
 * @param  mem_size     memory size
 * @param  region       region array
 * @param  region_num   number of regions in array
 * @param  region_size  size of single region
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int register_heap_regions(void *addr, size_t mem_size, mem_region_t region[],
                                 size_t region_num, size_t region_size)
{
        int err = EINVAL;

        for (size_t i = 0; (i < (region_num - 1)) && (mem_size > 0); i++) {
                size_t heap_sz = min(mem_size, region_size);
                err = sys_register_region(&region[i], addr, heap_sz,
                                          _MM_FLAG__DMA_CAPABLE, "SDRAM");
                addr     += heap_sz;
                mem_size -= heap_sz;
        }

        if (mem_size > 0) {
                err = sys_register_region(&region[region_num - 1], addr, mem_size,
                                          _MM_FLAG__DMA_CAPABLE, "SDRAM");
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
