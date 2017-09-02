/*==============================================================================
File    sd.h

Author  Daniel Zorychta

Brief   SD Card definitions.

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

/**
@defgroup SD_H_ SD_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _SD_H_
#define _SD_H_

/*==============================================================================
  Include files
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
/** card command definitions */
typedef enum {
        SD_CMD__CMD0   = (0x40+0 ),     // GO_IDLE_STATE, -
        SD_CMD__CMD1   = (0x40+1 ),     // SEND_OP_COND (MMC), -
        SD_CMD__CMD2   = (0x40+2 ),     // ALL_SEND_CID, R2
        SD_CMD__CMD3   = (0x40+3 ),     // SEND_RELATIVE_ADDR, R6
        SD_CMD__CMD4   = (0x40+3 ),     // SET_DSR, -
        SD_CMD__CMD6   = (0x40+6 ),     // SWITCH_FUNC, R1
        SD_CMD__ACMD6  = (0xC0+6 ),     // SET_BUS_WIDTH, R1
        SD_CMD__CMD7   = (0x40+7 ),     // SELECT/DESELECT_CARD, R1b/-
        SD_CMD__CMD8   = (0x40+8 ),     // SEND_IF_COND, R7
        SD_CMD__CMD9   = (0x40+9 ),     // SEND_CSD, R2
        SD_CMD__CMD10  = (0x40+10),     // SEND_CID, R2
        SD_CMD__CMD12  = (0x40+12),     // STOP_TRANSMISSION, R1b
        SD_CMD__CMD13  = (0x40+13),     // SEND_STATUS (SDC), R1
        SD_CMD__ACMD13 = (0xC0+13),     // SD_STATUS (SDC), R1
        SD_CMD__CMD16  = (0x40+16),     // SET_BLOCKLEN, R1
        SD_CMD__CMD17  = (0x40+17),     // READ_SINGLE_BLOCK, R1
        SD_CMD__CMD18  = (0x40+18),     // READ_MULTIPLE_BLOCK, R1
        SD_CMD__CMD23  = (0x40+23),     // SET_BLOCK_COUNT (MMC)
        SD_CMD__ACMD22 = (0xC0+22),     // SEND_NUM_WR_BLOCKS, R1
        SD_CMD__ACMD23 = (0xC0+23),     // SET_WR_BLK_ERASE_COUNT, R1
        SD_CMD__CMD24  = (0x40+24),     // WRITE_BLOCK, R1
        SD_CMD__CMD25  = (0x40+25),     // WRITE_MULTIPLE_BLOCK, R1
        SD_CMD__CMD27  = (0x40+27),     // PROGRAM_CSD, R1
        SD_CMD__CMD28  = (0x40+28),     // SET_WRITE_PROT, R1b
        SD_CMD__CMD29  = (0x40+29),     // CLR_WRITE_PROT, R1b
        SD_CMD__CMD30  = (0x40+30),     // SEND_WRITE_PROT, R1
        SD_CMD__CMD32  = (0x40+32),     // ERASE_WR_BLK_START, R1
        SD_CMD__CMD33  = (0x40+33),     // ERASE_WR_BLK_END, R1
        SD_CMD__CMD38  = (0x40+38),     // ERASE, R1b
        SD_CMD__ACMD41 = (0xC0+41),     // SD_SEND_OP_COND, R3
        SD_CMD__CMD42  = (0x40+42),     // LOCK_UNLOCK, R1
        SD_CMD__ACMD42 = (0xC0+42),     // SET_CLR_CARD_DETECT, R1
        SD_CMD__ACMD51 = (0xC0+51),     // SEND_SCR, R1
        SD_CMD__CMD55  = (0x40+55),     // APP_CMD, R1
        SD_CMD__CMD56  = (0x40+56),     // GEN_CMD, R1
        SD_CMD__CMD58  = (0x40+58)      // READ_OCR, R1
} SD_cmd_t;

/** card types */
typedef struct {
        enum {
                SD_TYPE__UNKNOWN,
                SD_TYPE__MMC,
                SD_TYPE__SD1,
                SD_TYPE__SD2
        } type;

        bool block;
} SD_type_t;

typedef struct {
        u32_t RESPONSE[4];
} SD_response_t;

/*==============================================================================
  Exported objects
==============================================================================*/

// http://users.ece.utexas.edu/~valvano/EE345M/SD_Physical_Layer_Spec.pdf
// http://goughlui.com/2014/01/03/project-read-collect-decode-sd-card-csd-register-data
// http://apro-tw.com/Databank/2010%20Datasheet/SLC_THEMIS/100-WPSDx-EASL_THEMIS%20Series.pdf
/*
[    0.067] RESP1: 0x400E0032
[    0.067] RESP2: 0x5B590000
[    0.067] RESP3: 0x39F77F80
[    0.067] RESP4: 0x0A4000C2
*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
static inline u32_t SD_swap_bytes(u8_t buf[4])
{
        return ((u32_t)buf[0] << 24) | ((u32_t)buf[1] << 16)
             | ((u32_t)buf[2] << 8 ) | ((u32_t)buf[3] << 0 );
}

static inline u8_t SD_CSD_get_version(SD_response_t *resp)
{
        return (resp->RESPONSE[0] >> 30) & 0x03;
}

static inline u8_t SD_CSD_get_TAAC(SD_response_t *resp)
{
        return (resp->RESPONSE[0] >> 16) & 0xFF;
}

static inline u8_t SD_CSD_get_NSAC(SD_response_t *resp)
{
        return (resp->RESPONSE[0] >> 8) & 0xFF;
}

static inline u8_t SD_CSD_get_TRAN_SPEED(SD_response_t *resp)
{
        return (resp->RESPONSE[0] >> 0) & 0xFF;
}

static inline u16_t SD_CSD_get_CCC(SD_response_t *resp)
{
        return (resp->RESPONSE[1] >> 20) & 0xFFF;
}

static inline u8_t SD_CSD_get_READ_BL_LEN(SD_response_t *resp)
{
        return (resp->RESPONSE[1] >> 16) & 0x0F;
}

static inline u8_t SD_CSD_get_READ_BL_PARTIAL(SD_response_t *resp)
{
        return (resp->RESPONSE[1] >> 15) & 0x01;
}

static inline u8_t SD_CSD_get_WRITE_BLK_MISALIGN(SD_response_t *resp)
{
        return (resp->RESPONSE[1] >> 14) & 0x01;
}

static inline u8_t SD_CSD_get_READ_BLK_MISALIGN(SD_response_t *resp)
{
        return (resp->RESPONSE[1] >> 13) & 0x01;
}

static inline u8_t SD_CSD_get_DSR_IMP(SD_response_t *resp)
{
        return (resp->RESPONSE[1] >> 12) & 0x01;
}

static inline u16_t SD_CSD_get_C_SIZE_SC(SD_response_t *resp)
{
        return ((resp->RESPONSE[1] & 0x3FF) << 2) | ((resp->RESPONSE[2] >> 30) & 0x03);
}

static inline u32_t SD_CSD_get_C_SIZE_HC(SD_response_t *resp)
{
        return ((resp->RESPONSE[1] & 0x3F) << 16) | ((resp->RESPONSE[2] >> 16) & 0xFFFF);
}

static inline u8_t SD_CSD_get_VDD_R_CURR_MIN(SD_response_t *resp)
{
        return (resp->RESPONSE[2] >> 27) & 0x07;
}

static inline u8_t SD_CSD_get_VDD_R_CURR_MAX(SD_response_t *resp)
{
        return (resp->RESPONSE[2] >> 24) & 0x07;
}

static inline u8_t SD_CSD_get_VDD_W_CURR_MIN(SD_response_t *resp)
{
        return (resp->RESPONSE[2] >> 21) & 0x07;
}

static inline u8_t SD_CSD_get_VDD_W_CURR_MAX(SD_response_t *resp)
{
        return (resp->RESPONSE[2] >> 18) & 0x07;
}

static inline u8_t SD_CSD_get_C_SIZE_MULT(SD_response_t *resp)
{
        return (resp->RESPONSE[2] >> 15) & 0x07;
}

static inline u8_t SD_CSD_get_ERASE_BLK_EN(SD_response_t *resp)
{
        return (resp->RESPONSE[2] >> 14) & 0x01;
}

static inline u8_t SD_CSD_get_SECTOR_SIZE(SD_response_t *resp)
{
        return (resp->RESPONSE[2] >> 7) & 0x7F;
}

static inline u8_t SD_CSD_get_WR_GRP_SIZE(SD_response_t *resp)
{
        return (resp->RESPONSE[2] >> 0) & 0x7F;
}

static inline u8_t SD_CSD_get_WP_GRP_ENABLE(SD_response_t *resp)
{
        return (resp->RESPONSE[3] >> 31) & 0x01;
}

static inline u8_t SD_CSD_get_R2W_FACTOR(SD_response_t *resp)
{
        return (resp->RESPONSE[3] >> 26) & 0x07;
}

static inline u8_t SD_CSD_get_WR_BL_LEN(SD_response_t *resp)
{
        return (resp->RESPONSE[3] >> 22) & 0x0F;
}

static inline u8_t SD_CSD_get_WR_BL_PARTIAL(SD_response_t *resp)
{
        return (resp->RESPONSE[3] >> 21) & 0x01;
}

static inline u8_t SD_CSD_get_FILE_FORMAT_GRP(SD_response_t *resp)
{
        return (resp->RESPONSE[3] >> 15) & 0x01;
}

static inline u8_t SD_CSD_get_COPY(SD_response_t *resp)
{
        return (resp->RESPONSE[3] >> 14) & 0x01;
}

static inline u8_t SD_CSD_get_PERM_WRITE_PROTECT(SD_response_t *resp)
{
        return (resp->RESPONSE[3] >> 13) & 0x01;
}

static inline u8_t SD_CSD_get_TMP_WRITE_PROTECT(SD_response_t *resp)
{
        return (resp->RESPONSE[3] >> 12) & 0x01;
}

static inline u8_t SD_CSD_get_FILE_FORMAT(SD_response_t *resp)
{
        return (resp->RESPONSE[3] >> 10) & 0x03;
}

static inline u8_t SD_CSD_get_CRC(SD_response_t *resp)
{
        return (resp->RESPONSE[3] >> 1) & 0x7F;
}

#ifdef __cplusplus
}
#endif

#endif /* _SD_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
