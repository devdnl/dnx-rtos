/*==============================================================================
File    rand.c

Author  Daniel Zorychta

Brief   Pseudo-random generator functions.

        Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <config.h>
#include <stdio.h>

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
static u32_t seed = 123456789;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function returns a pseudo-random integer.
 *
 * The rand() function returns a pseudo-random integer in the range 0 to
 * @ref RAND_MAX inclusive (i.e., the mathematical range [0, RAND_MAX]).
 *
 * @return The rand() function return a value between @b 0 and @ref RAND_MAX (inclusive).
 *
 * @b Example
 * @code
        // ...
        int foo = rand();
        // ...
   @endcode
 *
 * @see RAND_MAX
 */
//==============================================================================
int rand(void)
{
        const u64_t a = 1103515245;
        const u32_t c = 12345;
        const u64_t m = (1ULL << 32);

        seed = (a * seed + c) % m;

        return seed;
}

//==============================================================================
/**
 * @brief Function sets the seed for rand() function.
 *
 * The srand() function sets its argument as the seed for a new sequence
 * of pseudo-random integers to be returned by rand().  These sequences
 * are repeatable by calling srand() with the same seed value.
 *
 * @param seed      new seed value
 *
 * @b Example
 * @code
        // ...
        #include <stdlib.h>
        #include <dnx/os.h>

        srand(get_tick_counter());
        int foo = rand();
        // ...
   @endcode
 */
//==============================================================================
void srand(unsigned int s)
{
        seed = s;
}

/*==============================================================================
  End of file
==============================================================================*/
