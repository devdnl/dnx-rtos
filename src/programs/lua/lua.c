/*=========================================================================*//**
@file    lua.c

@author  Daniel Zorychta

@brief   Lua Virtual Machine

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


#include "stdlib.h"

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
