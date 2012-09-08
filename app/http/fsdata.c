/*************************************************************************
fsdata.c generator for UIP0.9
Works with cc65 C compiler
Based on the work of Adam Dunkels <adam@dunkels.com>
The syntax has been left as-is, with respect to the perl version
Supports for a maximum of 2 levels directory and 999 files in each directory
For more information, please go to http://www.design4fpga.com
      ALL RIGHTS RESERVED, COPYRIGHT APRIL 2006, DESIGN4FPGA
*************************************************************************/

/*************************************************************************
REDISTRIBUTION OF THIS PIECE OF SOFTWARE IN SOURCE OR AS A BINARY FILE IS PERMITTED IF AND ONLY IF
THE FOLLOWING RULES ARE OBSERVED:

1) THE REDISTRIBUTION FILE OR ARCHIVE SHOULD INCLUDE THIS README FILE AS WELL AS THE COPYRIGHT NOTICE.

BY USING THIS SOFTWARE, YOU ARE ACKNOWLEDGING THAT YOU HAVE READ AND AGREED WITH THE FOLLOWING
DISCLAIMER AND THE COPYRIGHT NOTICE ABOVE.

DISCLAIMER:

THIS PIECE OF SOFTWARE IS SUPPLIED ''AS IS'', WE (DESIGN4FPGA) SHALL NOT BE HELD RESPONSIBLE OR LIABLE
FOR ANY RESULTING EVENTS THAT MIGHT HAPPEN DURING THE USE OF THIS PROGRAM IN PARTICULAR:

1)	THE LOSS OF INFORMATION DUE TO THE USE OR MISUSE OF FS_GENERATOR.EXE
2)	THE LOSS OF BUSINESS, STOCKS, MONEY, PEOPLE AND LIVES OR ANY UNFORTUNATE CIRCUMSTANCE THAT MAY PREVAIL
AS A RESULT OF A SYSTEM FAILURE, DIRECTLY OR INDIRECTLY DUE TO THE USE OF FS_GENERATOR.EXE
*************************************************************************/

#include "fsdata.h"

/* files */
#include "header.gif.c"
#include "index.html.c"
#include "error404.html.c"

/* file tree */
static const struct fsdata_file file_header_gif[]    = {{NULL, "/header.gif", header_gif, sizeof(header_gif)}};
static const struct fsdata_file file_index_html[]    = {{file_header_gif, "/index.html", index_html, sizeof(index_html)}};
static const struct fsdata_file file_error404_html[] = {{file_index_html, "/404.html", error404_html, sizeof(error404_html)}};
#define FS_ROOT file_error404_html
