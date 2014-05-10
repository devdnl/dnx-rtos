/*=========================================================================*//**
@file    lsmod.c

@author  Daniel Zorychta

@brief   Module listing

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stdlib.h>
#include <string.h>
#include "lib/mbus/mbus.h"

#include <unistd.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct payload {
        int x;
        int y;
        int z;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION_BEGIN

GLOBAL_VARIABLES_SECTION_END

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
PROGRAM_MAIN(mbus_daemon, int argc, char *argv[])
{
        (void)argc;
        (void)argv;

        if (argv[1] == NULL) {
                mbus_status_t status = mbus_daemon();

                if (status == MBUS_STATUS_DAEMON_IS_RUNNING) {
                        puts("Daemon is already started.");
                } else {
                        puts("Daemon start error.");
                }

        } else {
                if (strcmp(argv[1], "h") == 0) {

                        /* host mode */
                        mbus_t *bus = mbus_bus_new();
                        if (bus) {
                                mbus_slot_t slot1;
                                if (mbus_slot_create(bus, "/my_first_slot/1", sizeof(struct payload), &slot1) == MBUS_STATUS_SUCCESS) {
                                        puts("Slot 1 created");
                                }

                                mbus_slot_t slot2;
                                if (mbus_slot_create(bus, "/my_first_slot/2", sizeof(struct payload), &slot2) == MBUS_STATUS_SUCCESS) {
                                        puts("Slot 2 created");
                                }

                                mbus_slot_t slot3;
                                if (mbus_slot_create(bus, "/my_first_slot/3", sizeof(struct payload), &slot3) == MBUS_STATUS_SUCCESS) {
                                        puts("Slot 3 created");
                                }

                                mbus_slot_create(bus, "/nowy_slot/in", sizeof(struct payload), NULL);

                                mbus_slot_connect(bus, "/my_first_slot/1", &slot1);
                                mbus_slot_connect(bus, "/my_first_slot/2", &slot2);
                                mbus_slot_connect(bus, "/my_first_slot/3", &slot3);

                                puts("5s to send...");
                                sleep(5);
                                puts("Start");

                                struct payload msg;

                                for (int i = 0; i < 5; i++) {
                                        msg.x = i;
                                        msg.y = i + 1;
                                        msg.z = i + 2;
                                        mbus_msg_send(bus, &slot1, &msg);

                                        msg.x *= 2;
                                        msg.y *= 2;
                                        msg.z *= 2;
                                        mbus_msg_send(bus, &slot2, &msg);

                                        msg.x *= 3;
                                        msg.y *= 3;
                                        msg.z *= 3;
                                        mbus_msg_send(bus, &slot3, &msg);

                                        sleep(1);
                                        puts("Sent");
                                }

                                mbus_slot_destroy(bus, "/my_first_slot/1");
                                mbus_slot_destroy(bus, "/my_first_slot/2");
                                mbus_slot_destroy(bus, "/my_first_slot/3");

                                mbus_slot_disconnect(bus, &slot1);
                                mbus_slot_disconnect(bus, &slot2);
                                mbus_slot_disconnect(bus, &slot3);
                                mbus_bus_delete(bus);
                        }
                } else {
                        /* client mode */
                        mbus_t *bus = mbus_bus_new();
                        if (bus) {

                                char *name = calloc(1, 64);
                                if (name) {
                                      uint slots = 0;
                                      mbus_bus_get_number_of_slots(bus, &slots);

                                      for (uint slot = 0; slot < slots; slot++) {
                                            mbus_bus_get_slot_name(bus, slot, name, 64);
                                            puts(name);
                                      }

                                      free(name);
                                }




                                mbus_slot_t slot[3];

                                mbus_slot_connect(bus, "/my_first_slot/1", &slot[0]);
                                mbus_slot_connect(bus, "/my_first_slot/2", &slot[1]);
                                mbus_slot_connect(bus, "/my_first_slot/3", &slot[2]);

                                struct payload msg;
                                mbus_status_t  stat = MBUS_STATUS_SUCCESS;

                                while (stat != MBUS_STATUS_SLOT_NOT_EXIST) {
                                        for (int i = 0; i < 3; i++) {
                                                if (mbus_slot_has_msg(bus, &slot[i]) == MBUS_STATUS_SLOT_HAS_MSG) {
                                                        puts("Has message");
                                                } else {
                                                        puts("Hasn't message");
                                                }

                                                stat = mbus_msg_receive(bus, &slot[i], &msg);
                                                mbus_slot_clear(bus, &slot[i]);

                                                if (stat == MBUS_STATUS_SUCCESS) {
                                                        printf("Received slot %d: x = %d; y = %d; z = %d\n",
                                                               i, msg.x, msg.y, msg.z);
                                                } else {
                                                        printf("Slot %d is empty\n", i);
                                                }
                                        }

                                        sleep_ms(1000);
                                }

                                mbus_slot_disconnect(bus, &slot[0]);
                                mbus_slot_disconnect(bus, &slot[1]);
                                mbus_slot_disconnect(bus, &slot[2]);

                                mbus_bus_delete(bus);
                        }
                }
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
