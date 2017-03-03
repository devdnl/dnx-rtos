#!/usr/bin/env bash

cd $(dirname $0)

PROGNAME=$(echo "$1" | sed 's/[-+/*,."!\@#$%^&*()+-]/_/g')
BRIEF=$2
AUTHOR=$3
EMAIL=$4

if [[ "$*" < "2" ]]; then
    echo "Usage: $(basename $0) <program-name> [brief] [author] [email]"
    exit 1
else
    cd ../src/programs
    mkdir $PROGNAME
    cd $PROGNAME

cat << EOF > $PROGNAME.c
/*==============================================================================
File     ${PROGNAME,,}.c

Author   $AUTHOR

Brief    $BRIEF

         Copyright (C) $(date "+%Y") $AUTHOR <$EMAIL>

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
int_main(${PROGNAME,,}, STACK_DEPTH_VERY_LOW, int argc, char *argv[])
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
