#!/usr/bin/env bash

set -e

cd $(dirname $0)

PROGNAME=$(echo "$1" | sed 's/[-+/*,."!\@#$%^&*()+-]/_/g')
BRIEF=$2
AUTHOR=$3
EMAIL=$4

if [[ "$*" < "2" ]]; then
    echo "Usage: $(basename $0) <program-name> [brief] [author] [email]"
    exit 1
else
    cd ../src/application/programs
    mkdir $PROGNAME
    cd $PROGNAME

cat << EOF > $PROGNAME.c
/*==============================================================================
File    ${PROGNAME,,}.c

Author  $AUTHOR

Brief   $BRIEF

        Copyright (C) $(date "+%Y") $AUTHOR <$EMAIL>

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
GLOBAL_VARIABLES_SECTION {
};

/*==============================================================================
  Exported objects
==============================================================================*/
PROGRAM_PARAMS(${PROGNAME,,}, STACK_DEPTH_VERY_LOW);

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
        puts("Hello world!");
        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/

EOF

cat << EOF > Makefile
# Makefile for GNU make

CSRC_PROGRAMS   += ${PROGNAME,,}/${PROGNAME,,}.c
CXXSRC_PROGRAMS +=
HDRLOC_PROGRAMS +=
EOF

    echo "Done"
fi
