/*=========================================================================*//**
@file    lua.c

@author  Daniel Zorychta

@brief   Lua Virtual Machine

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
int_main(lua, STACK_DEPTH_HUGE, int argc, char *argv[])
{
        if (argc == 1) {
                printf("%s [file]\n", argv[0]);
                return 0;
        }

        lua_State *L = luaL_newstate();
        if (L) {
                luaL_openlibs(L);
                if (luaL_dofile(L, argv[1]) != 0) {
                        perror(lua_tostring(L, -1));
                        lua_pop(L, 1);
                }

                lua_close(L);
        } else {
                perror("Lua VM not created!");
        }

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
