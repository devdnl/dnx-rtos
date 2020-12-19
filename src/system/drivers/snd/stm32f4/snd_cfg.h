/*==============================================================================
File    snd_cfg.h

Author  Daniel Zorychta

Brief   Sound interface driver

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

#ifndef _SND_CFG_H_
#define _SND_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"

/*==============================================================================
  Exported macros
==============================================================================*/
/*
 * SAI1 Block A
 */
#define _SND_CFG_SAI1_BLKA_CR1\
        (__SND_CFG_SAI1_BLKA_NO_DIVIDER__\
        |__SND_CFG_SAI1_BLKA_CKSTR__\
        |__SND_CFG_SAI1_BLKA_LSBFRIST__\
        |((__SND_CFG_SAI1_BLKA_DS__ << SAI_xCR1_DS_Pos) & SAI_xCR1_DS_Msk)\
        |((__SND_CFG_SAI1_BLKA_PRTCFG__ << SAI_xCR1_PRTCFG_Pos) & SAI_xCR1_PRTCFG_Msk)\
        |((__SND_CFG_SAI1_BLKA_MODE__ << SAI_xCR1_PRTCFG_Pos) & SAI_xCR1_PRTCFG_Msk))

#define _SND_CFG_SAI1_BLKA_CR2\
        (((__SND_CFG_SAI1_BLKA_COMP__ << SAI_xCR2_COMP_Pos) & SAI_xCR2_COMP_Msk)\
        |__SND_CFG_SAI1_BLKA_CPL__\
        |__SND_CFG_SAI1_BLKA_TRIS__)

#define _SND_CFG_SAI1_BLKA_FRCR\
        (__SND_CFG_SAI1_BLKA_FSOFF__\
        |__SND_CFG_SAI1_BLKA_FSPOL__\
        |__SND_CFG_SAI1_BLKA_FSDEF__\
        |((((__SND_CFG_SAI1_BLKA_FSALL__) - 1) << SAI_xFRCR_FSALL_Pos) & SAI_xFRCR_FSALL_Msk)\
        |((((__SND_CFG_SAI1_BLKA_FRL__) - 1) << SAI_xFRCR_FRL_Pos) & SAI_xFRCR_FRL_Msk))

#define _SND_CFG_SAI1_BLKA_SLOTR\
        (((((1 << __SND_CFG_SAI1_BLKA_NBSLOT__) - 1) << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk)\
        |((((__SND_CFG_SAI1_BLKA_NBSLOT__) - 1) << SAI_xSLOTR_NBSLOT_Pos) & SAI_xSLOTR_NBSLOT_Msk)\
        |((__SND_CFG_SAI1_BLKA_SLOTSZ__ << SAI_xSLOTR_SLOTSZ_Pos) & SAI_xSLOTR_SLOTSZ_Msk)\
        |((__SND_CFG_SAI1_BLKA_FBOFF__ << SAI_xSLOTR_FBOFF_Pos) & SAI_xSLOTR_FBOFF_Msk))

/*
 * SAI1 Block B
 */
#define _SND_CFG_SAI1_BLKB_CR1\
        (__SND_CFG_SAI1_BLKB_NO_DIVIDER__\
        |__SND_CFG_SAI1_BLKB_CKSTR__\
        |__SND_CFG_SAI1_BLKB_LSBFRIST__\
        |((__SND_CFG_SAI1_BLKB_DS__ << SAI_xCR1_DS_Pos) & SAI_xCR1_DS_Msk)\
        |((__SND_CFG_SAI1_BLKB_PRTCFG__ << SAI_xCR1_PRTCFG_Pos) & SAI_xCR1_PRTCFG_Msk)\
        |((__SND_CFG_SAI1_BLKB_MODE__ << SAI_xCR1_PRTCFG_Pos) & SAI_xCR1_PRTCFG_Msk))

#define _SND_CFG_SAI1_BLKB_CR2\
        (((__SND_CFG_SAI1_BLKB_COMP__ << SAI_xCR2_COMP_Pos) & SAI_xCR2_COMP_Msk)\
        |__SND_CFG_SAI1_BLKB_CPL__\
        |__SND_CFG_SAI1_BLKB_TRIS__)

#define _SND_CFG_SAI1_BLKB_FRCR\
        (__SND_CFG_SAI1_BLKB_FSOFF__\
        |__SND_CFG_SAI1_BLKB_FSPOL__\
        |__SND_CFG_SAI1_BLKB_FSDEF__\
        |((((__SND_CFG_SAI1_BLKB_FSALL__) - 1) << SAI_xFRCR_FSALL_Pos) & SAI_xFRCR_FSALL_Msk)\
        |((((__SND_CFG_SAI1_BLKB_FRL__) - 1) << SAI_xFRCR_FRL_Pos) & SAI_xFRCR_FRL_Msk))

#define _SND_CFG_SAI1_BLKB_SLOTR\
        (((((1 << __SND_CFG_SAI1_BLKB_NBSLOT__) - 1) << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk)\
        |((((__SND_CFG_SAI1_BLKB_NBSLOT__) - 1) << SAI_xSLOTR_NBSLOT_Pos) & SAI_xSLOTR_NBSLOT_Msk)\
        |((__SND_CFG_SAI1_BLKB_SLOTSZ__ << SAI_xSLOTR_SLOTSZ_Pos) & SAI_xSLOTR_SLOTSZ_Msk)\
        |((__SND_CFG_SAI1_BLKB_FBOFF__ << SAI_xSLOTR_FBOFF_Pos) & SAI_xSLOTR_FBOFF_Msk))


/*
 * SAI2 Block A
 */
#define _SND_CFG_SAI2_BLKA_CR1\
        (__SND_CFG_SAI2_BLKA_NO_DIVIDER__\
        |__SND_CFG_SAI2_BLKA_CKSTR__\
        |__SND_CFG_SAI2_BLKA_LSBFRIST__\
        |((__SND_CFG_SAI2_BLKA_DS__ << SAI_xCR1_DS_Pos) & SAI_xCR1_DS_Msk)\
        |((__SND_CFG_SAI2_BLKA_PRTCFG__ << SAI_xCR1_PRTCFG_Pos) & SAI_xCR1_PRTCFG_Msk)\
        |((__SND_CFG_SAI2_BLKA_MODE__ << SAI_xCR1_PRTCFG_Pos) & SAI_xCR1_PRTCFG_Msk))

#define _SND_CFG_SAI2_BLKA_CR2\
        (((__SND_CFG_SAI2_BLKA_COMP__ << SAI_xCR2_COMP_Pos) & SAI_xCR2_COMP_Msk)\
        |__SND_CFG_SAI2_BLKA_CPL__\
        |__SND_CFG_SAI2_BLKA_TRIS__)

#define _SND_CFG_SAI2_BLKA_FRCR\
        (__SND_CFG_SAI2_BLKA_FSOFF__\
        |__SND_CFG_SAI2_BLKA_FSPOL__\
        |__SND_CFG_SAI2_BLKA_FSDEF__\
        |((((__SND_CFG_SAI2_BLKA_FSALL__) - 1) << SAI_xFRCR_FSALL_Pos) & SAI_xFRCR_FSALL_Msk)\
        |((((__SND_CFG_SAI2_BLKA_FRL__) - 1) << SAI_xFRCR_FRL_Pos) & SAI_xFRCR_FRL_Msk))

#define _SND_CFG_SAI2_BLKA_SLOTR\
        (((((1 << __SND_CFG_SAI2_BLKA_NBSLOT__) - 1) << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk)\
        |((((__SND_CFG_SAI2_BLKA_NBSLOT__) - 1) << SAI_xSLOTR_NBSLOT_Pos) & SAI_xSLOTR_NBSLOT_Msk)\
        |((__SND_CFG_SAI2_BLKA_SLOTSZ__ << SAI_xSLOTR_SLOTSZ_Pos) & SAI_xSLOTR_SLOTSZ_Msk)\
        |((__SND_CFG_SAI2_BLKA_FBOFF__ << SAI_xSLOTR_FBOFF_Pos) & SAI_xSLOTR_FBOFF_Msk))

/*
 * SAI2 Block B
 */
#define _SND_CFG_SAI2_BLKB_CR1\
        (__SND_CFG_SAI2_BLKB_NO_DIVIDER__\
        |__SND_CFG_SAI2_BLKB_CKSTR__\
        |__SND_CFG_SAI2_BLKB_LSBFRIST__\
        |((__SND_CFG_SAI2_BLKB_DS__ << SAI_xCR1_DS_Pos) & SAI_xCR1_DS_Msk)\
        |((__SND_CFG_SAI2_BLKB_PRTCFG__ << SAI_xCR1_PRTCFG_Pos) & SAI_xCR1_PRTCFG_Msk)\
        |((__SND_CFG_SAI2_BLKB_MODE__ << SAI_xCR1_PRTCFG_Pos) & SAI_xCR1_PRTCFG_Msk))

#define _SND_CFG_SAI2_BLKB_CR2\
        (((__SND_CFG_SAI2_BLKB_COMP__ << SAI_xCR2_COMP_Pos) & SAI_xCR2_COMP_Msk)\
        |__SND_CFG_SAI2_BLKB_CPL__\
        |__SND_CFG_SAI2_BLKB_TRIS__)

#define _SND_CFG_SAI2_BLKB_FRCR\
        (__SND_CFG_SAI2_BLKB_FSOFF__\
        |__SND_CFG_SAI2_BLKB_FSPOL__\
        |__SND_CFG_SAI2_BLKB_FSDEF__\
        |((((__SND_CFG_SAI2_BLKB_FSALL__) - 1) << SAI_xFRCR_FSALL_Pos) & SAI_xFRCR_FSALL_Msk)\
        |((((__SND_CFG_SAI2_BLKB_FRL__) - 1) << SAI_xFRCR_FRL_Pos) & SAI_xFRCR_FRL_Msk))

#define _SND_CFG_SAI2_BLKB_SLOTR\
        (((((1 << __SND_CFG_SAI2_BLKB_NBSLOT__) - 1) << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk)\
        |((((__SND_CFG_SAI2_BLKB_NBSLOT__) - 1) << SAI_xSLOTR_NBSLOT_Pos) & SAI_xSLOTR_NBSLOT_Msk)\
        |((__SND_CFG_SAI2_BLKB_SLOTSZ__ << SAI_xSLOTR_SLOTSZ_Pos) & SAI_xSLOTR_SLOTSZ_Msk)\
        |((__SND_CFG_SAI2_BLKB_FBOFF__ << SAI_xSLOTR_FBOFF_Pos) & SAI_xSLOTR_FBOFF_Msk))

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _SND_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
