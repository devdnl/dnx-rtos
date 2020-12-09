/*==============================================================================
File    perf.c

Author  Daniel Zorychta

Brief   Performance measurement program

        Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation and modified by the dnx RTOS exception.

        NOTE: The modification  to the GPL is  included to allow you to
              distribute a combined work that includes dnx RTOS without
              being obliged to provide the source  code for proprietary
              components outside of the dnx RTOS.

        The dnx RTOS  is  distributed  in the hope  that  it will be useful,
        but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
        MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
        GNU General Public License for more details.

        Full license text is available on the following file: doc/license.txt.

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <dnx/os.h>
#include <dnx/thread.h>
#include <sys/ioctl.h>
#include <sha256.h>

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void perf_thread(void *arg);

/*==============================================================================
  Local objects
==============================================================================*/
GLOBAL_VARIABLES_SECTION_BEGIN
u8_t  buf[256];
u8_t  hash[SHA256_BLOCK_SIZE];
u32_t cpu_freq;
GLOBAL_VARIABLES_SECTION_END

/*==============================================================================
  Exported objects
==============================================================================*/
PROGRAM_PARAMS(perf, STACK_DEPTH_LOW);

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * Main program function.
 *
 * Note: Please adjust stack size according to programs needs.
 *
 * @param argc      argument count
 * @param argv      arguments
 */
//==============================================================================
int main(int argc, char *argv[])
{
        UNUSED_ARG2(argc, argv);

        FILE *clk = fopen("/dev/clk", "r");
        if (clk) {
                CLK_info_t ck;
                ck.iterator = 0;
                while ((ioctl(fileno(clk), IOCTL_CLK__GET_CLK_INFO, &ck) == 0)) {
                        if (ck.name == NULL) {
                                break;
                        } else if (strcmp(ck.name, "CPUCLK") == 0) {
                                global->cpu_freq = ck.freq_Hz;
                                break;
                        } else {
                                ck.iterator++;
                        }
                }

                fclose(clk);

                if (global->cpu_freq == 0) {
                        puts("CPUCLK clock not found.");
                        return EXIT_FAILURE;
                }

        } else {
                printf("To perform test /dev/clk driver is required.");
                return EXIT_FAILURE;
        }

        puts("CPU performance test in progress...");

        static const thread_attr_t THREAD_ATTR = {
                .stack_depth = STACK_DEPTH_LOW,
                .priority    = PRIORITY_HIGHEST,
                .detached    = false
        };

        thread_join(thread_create(perf_thread, &THREAD_ATTR, NULL));

        return EXIT_SUCCESS;
}

//==============================================================================
/**
 * @brief  Measurement thread.
 *
 * @param  arg          thread's argument
 */
//==============================================================================
static void perf_thread(void *arg)
{
        UNUSED_ARG1(arg);

        u32_t n = 0;
        u64_t tstart = get_time_ms();

        while (get_time_ms() - tstart < 10000) {

                for (size_t i = 0; i < sizeof(global->buf); i++) {
                        global->buf[i] = rand();
                }

                SHA256_CTX ctx;
                sha256_init(&ctx);
                sha256_update(&ctx, global->buf, sizeof(global->buf));
                sha256_final(&ctx, global->hash);

                snprintf((char*)global->buf, sizeof(global->buf), "HASH: ");
                for (size_t i = 0; i < SHA256_BLOCK_SIZE; i++) {
                        snprintf((char*)global->buf, sizeof(global->buf), "%02X", global->hash[i]);
                }

                double d1 = (double)global->hash[0];
                double d2 = (double)global->buf[3];
                snprintf((char*)global->buf, sizeof(global->buf), "%f", d1 / d2);

                float f1 = (float)global->hash[2];
                float f2 = (float)global->buf[5];
                snprintf((char*)global->buf, sizeof(global->buf), "%f", f1 / f2);

                n++;
        }

        u64_t tstop = get_time_ms();
        float dt = (tstop - tstart) / 1000.0;
        float pt = (double)n / dt;

        printf("Result: %.1f pt, %0.3f pt/MHz\n",
               pt, pt / (float)((double)global->cpu_freq / 1e6));
}

/*==============================================================================
  End of file
==============================================================================*/

