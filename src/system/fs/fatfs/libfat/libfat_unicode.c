#include "libfat.h"

#if _LIBFAT_USE_LFN != 0

#if   _LIBFAT_CODE_PAGE == 932
#include "codepage/cc932.c"
#elif _LIBFAT_CODE_PAGE == 936
#include "codepage/cc936.c"
#elif _LIBFAT_CODE_PAGE == 949
#include "codepage/cc949.c"
#elif _LIBFAT_CODE_PAGE == 950
#include "codepage/cc950.c"
#else
#include "codepage/ccsbcs.c"
#endif

#endif
