/*=========================================================================*//**
@file    time.h

@author  Daniel Zorychta

@brief   Time standard library

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

/**
\defgroup time-h <time.h>

The library provides time functions.

@{
*/

#ifndef _TIME_H_
#define _TIME_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include <stddef.h>
#include <kernel/syscall.h>
#include <kernel/kwrapper.h>
#include <kernel/errno.h>
#include <lib/unarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief Macro convert HH:MM time zone format to seconds (difference relative to UTC).
 *
 * @b Example
 * @code
        TIMEZONE_DIFF_UTC(1,30):  UTC+01:30
        TIMEZONE_DIFF_UTC(0,0):   UTC+00:00
        TIMEZONE_DIFF_UTC(-12,0): UTC-12:00
   @endcode
 */
#define TIMEZONE_DIFF_UTC(HH,MM)        (((HH)*60 + ((HH) < 0 ? -(MM) : (MM))) * 60)

/**
 * @brief This macro expands to an expression representing the number of clock ticks
 * per second.
 *
 * Clock ticks are units of time of a constant but system-specific length,
 * as those returned by function clock.
 * Dividing a count of clock ticks by this expression yields the number of seconds.
 */
#define CLOCKS_PER_SEC                  1000

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * @brief Type representing clock tick counts.
 *
 * Alias of a fundamental arithmetic type capable of representing clock tick counts.
 * Clock ticks are units of time of a constant but system-specific length, as
 * those returned by function clock.
 * This is the type returned by clock().
 *
 * @see clock()
 */
#ifndef __CLOCK_TYPE_DEFINED__
typedef u32_t clock_t;
#endif

/**
 * @brief Type representing time.
 *
 * Alias of a fundamental arithmetic type capable of representing times, as those
 * returned by function time().
 * For historical reasons, it is generally implemented as an integral value
 * representing the number of seconds elapsed since 00:00 hours, Jan 1, 1970 UTC
 * (i.e., a unix timestamp). Although libraries may implement this type using
 * alternative time representations.
 * Portable programs should not use values of this type directly, but always rely
 * on calls to elements of the standard library to translate them to portable types.
 *
 * @see time(), mktime()
 */
#ifndef __TIME_TYPE_DEFINED__
typedef u32_t time_t;
#endif

/**
 * @brief Structure representing a calendar date and time broken down into components.
 *
 * Structure containing a calendar date and time broken down into its components.
 * The structure contains nine members of type int (in any order), which are:
 *
 * @see gmtime(), gmtime_r(), localtime(), localtime_r(), strftime()
 */
#ifndef __TM_STRUCT_DEFINED__
struct tm {
        int tm_sec;       /*!< Seconds after the minute (0-60).*/
        int tm_min;       /*!< Minutes after the hour (0-59).*/
        int tm_hour;      /*!< Hours since midnight (0-23).*/
        int tm_mday;      /*!< Day of the month (1-31).*/
        int tm_mon;       /*!< Months since January (0-11).*/
        int tm_year;      /*!< Years since (1900).*/
        int tm_wday;      /*!< Days since Sunday (0-6).*/
        int tm_yday;      /*!< Days since January 1 (0-365).*/
        int tm_isdst;     /*!< Daylight Saving Time flag.*/
};
#endif

/**
 * List of Time Zones (time converted to seconds relative to UTC)
 */
#define TIMEZONE_ACDT              TIMEZONE_DIFF_UTC( 10,30)   //!< UTC+10:30 Australian Central Daylight Savings Time
#define TIMEZONE_ACST              TIMEZONE_DIFF_UTC(  9,30)   //!< UTC+09:30 Australian Central Standard Time
#define TIMEZONE_ACT               TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    ASEAN Common Time
#define TIMEZONE_ADT               TIMEZONE_DIFF_UTC(-03,00)   //!< UTC-03    Atlantic Daylight Time
#define TIMEZONE_AEDT              TIMEZONE_DIFF_UTC( 11,00)   //!< UTC+11    Australian Eastern Daylight Savings Time
#define TIMEZONE_AEST              TIMEZONE_DIFF_UTC( 10,00)   //!< UTC+10    Australian Eastern Standard Time
#define TIMEZONE_AFT               TIMEZONE_DIFF_UTC(  4,30)   //!< UTC+04:30 Afghanistan Time
#define TIMEZONE_AKDT              TIMEZONE_DIFF_UTC( -8,00)   //!< UTC-08    Alaska Daylight Time
#define TIMEZONE_AKST              TIMEZONE_DIFF_UTC( -9,00)   //!< UTC-09    Alaska Standard Time
#define TIMEZONE_AMST_Amazon       TIMEZONE_DIFF_UTC( -3,00)   //!< UTC-03    Amazon Summer Time (Brazil)
#define TIMEZONE_AMST_Armenia      TIMEZONE_DIFF_UTC(  5,00)   //!< UTC+05    Armenia Summer Time
#define TIMEZONE_AMT_Amazon        TIMEZONE_DIFF_UTC( -4,00)   //!< UTC-04    Amazon Time (Brazil)
#define TIMEZONE_AMT_Armenia       TIMEZONE_DIFF_UTC(  4,00)   //!< UTC+04    Armenia Time
#define TIMEZONE_ART               TIMEZONE_DIFF_UTC( -3,00)   //!< UTC-03    Argentina Time
#define TIMEZONE_AST_Arabia        TIMEZONE_DIFF_UTC(  3,00)   //!< UTC+03    Arabia Standard Time
#define TIMEZONE_AST_Atlantic      TIMEZONE_DIFF_UTC( -4,00)   //!< UTC-04    Atlantic Standard Time
#define TIMEZONE_AWDT              TIMEZONE_DIFF_UTC(  9,00)   //!< UTC+09    Australian Western Daylight Time
#define TIMEZONE_AWST              TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    Australian Western Standard Time
#define TIMEZONE_AZOST             TIMEZONE_DIFF_UTC( -1,00)   //!< UTC-01    Azores Standard Time
#define TIMEZONE_AZT               TIMEZONE_DIFF_UTC(  4,00)   //!< UTC+04    Azerbaijan Time
#define TIMEZONE_BDT               TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    Brunei Time
#define TIMEZONE_BIOT              TIMEZONE_DIFF_UTC(  6,00)   //!< UTC+06    British Indian Ocean Time
#define TIMEZONE_BIT               TIMEZONE_DIFF_UTC(-12,00)   //!< UTC-12    Baker Island Time
#define TIMEZONE_BOT               TIMEZONE_DIFF_UTC( -4,00)   //!< UTC-04    Bolivia Time
#define TIMEZONE_BRT               TIMEZONE_DIFF_UTC( -3,00)   //!< UTC-03    Brasilia Time
#define TIMEZONE_BST               TIMEZONE_DIFF_UTC(  6,00)   //!< UTC+06    Bangladesh Standard Time
#define TIMEZONE_BTT               TIMEZONE_DIFF_UTC(  6,00)   //!< UTC+06    Bhutan Time
#define TIMEZONE_CAT               TIMEZONE_DIFF_UTC(  2,00)   //!< UTC+02    Central Africa Time
#define TIMEZONE_CCT               TIMEZONE_DIFF_UTC(  6,30)   //!< UTC+06:30 Cocos Islands Time
#define TIMEZONE_CDT_North_America TIMEZONE_DIFF_UTC( -5,00)   //!< UTC-05    Central Daylight Time (North America)
#define TIMEZONE_CDT_Cuba          TIMEZONE_DIFF_UTC( -4,00)   //!< UTC-04    Cuba Daylight Time
#define TIMEZONE_CEDT              TIMEZONE_DIFF_UTC(  2,00)   //!< UTC+02    Central European Daylight Time
#define TIMEZONE_CEST              TIMEZONE_DIFF_UTC(  2,00)   //!< UTC+02    Central European Summer Time (Cf. HAEC)
#define TIMEZONE_CET               TIMEZONE_DIFF_UTC(  1,00)   //!< UTC+01    Central European Time
#define TIMEZONE_CHADT             TIMEZONE_DIFF_UTC( 13,45)   //!< UTC+13:45 Chatham Daylight Time
#define TIMEZONE_CHAST             TIMEZONE_DIFF_UTC( 12,45)   //!< UTC+12:45 Chatham Standard Time
#define TIMEZONE_CHOT              TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    Choibalsan
#define TIMEZONE_ChST              TIMEZONE_DIFF_UTC( 10,00)   //!< UTC+10    Chamorro Standard Time
#define TIMEZONE_CHUT              TIMEZONE_DIFF_UTC( 10,00)   //!< UTC+10    Chuuk Time
#define TIMEZONE_CIST              TIMEZONE_DIFF_UTC( -8,00)   //!< UTC-08    Clipperton Island Standard Time
#define TIMEZONE_CIT               TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    Central Indonesia Time
#define TIMEZONE_CKT               TIMEZONE_DIFF_UTC(-10,00)   //!< UTC-10    Cook Island Time
#define TIMEZONE_CLST              TIMEZONE_DIFF_UTC( -3,00)   //!< UTC-03    Chile Summer Time
#define TIMEZONE_CLT               TIMEZONE_DIFF_UTC( -4,00)   //!< UTC-04    Chile Standard Time
#define TIMEZONE_COST              TIMEZONE_DIFF_UTC( -4,00)   //!< UTC-04    Colombia Summer Time
#define TIMEZONE_COT               TIMEZONE_DIFF_UTC( -5,00)   //!< UTC-05    Colombia Time
#define TIMEZONE_CST_North_America TIMEZONE_DIFF_UTC( -6,00)   //!< UTC-06    Central Standard Time (North America)
#define TIMEZONE_CST_China         TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    China Standard Time
#define TIMEZONE_CST_Australia_Std TIMEZONE_DIFF_UTC(  9,30)   //!< UTC+09:30 Central Standard Time (Australia)
#define TIMEZONE_CST_Australia_Sum TIMEZONE_DIFF_UTC( 10,30)   //!< UTC+10:30 Central Summer Time (Australia)
#define TIMEZONE_CST_Cuba          TIMEZONE_DIFF_UTC( -5,00)   //!< UTC-05    Cuba Standard Time
#define TIMEZONE_CT                TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    China time
#define TIMEZONE_CVT               TIMEZONE_DIFF_UTC( -1,00)   //!< UTC-01    Cape Verde Time
#define TIMEZONE_CWST              TIMEZONE_DIFF_UTC(  8,45)   //!< UTC+08:45 Central Western Standard Time (Australia)
#define TIMEZONE_CXT               TIMEZONE_DIFF_UTC(  7,00)   //!< UTC+07    Christmas Island Time
#define TIMEZONE_DAVT              TIMEZONE_DIFF_UTC(  7,00)   //!< UTC+07    Davis Time
#define TIMEZONE_DDUT              TIMEZONE_DIFF_UTC( 10,00)   //!< UTC+10    Dumont d'Urville Time
#define TIMEZONE_DFT               TIMEZONE_DIFF_UTC(  1,00)   //!< UTC+01    AIX specific equ. of Central European Time
#define TIMEZONE_EASST             TIMEZONE_DIFF_UTC( -5,00)   //!< UTC-05    Easter Island Standard Summer Time
#define TIMEZONE_EAST              TIMEZONE_DIFF_UTC( -6,00)   //!< UTC-06    Easter Island Standard Time
#define TIMEZONE_EAT               TIMEZONE_DIFF_UTC(  3,00)   //!< UTC+03    East Africa Time
#define TIMEZONE_ECT_Caribbean     TIMEZONE_DIFF_UTC( -4,00)   //!< UTC-04    Eastern Caribbean Time (does not recognise DST)
#define TIMEZONE_ECT_Ecuador       TIMEZONE_DIFF_UTC( -5,00)   //!< UTC-05    Ecuador Time
#define TIMEZONE_EDT               TIMEZONE_DIFF_UTC( -4,00)   //!< UTC-04    Eastern Daylight Time (North America)
#define TIMEZONE_EEDT              TIMEZONE_DIFF_UTC(  3,00)   //!< UTC+03    Eastern European Daylight Time
#define TIMEZONE_EEST              TIMEZONE_DIFF_UTC(  2,00)   //!< UTC+03    Eastern European Summer Time
#define TIMEZONE_EET               TIMEZONE_DIFF_UTC(  2,00)   //!< UTC+02    Eastern European Time
#define TIMEZONE_EGST              TIMEZONE_DIFF_UTC(  0,00)   //!< UTC+00    Eastern Greenland Summer Time
#define TIMEZONE_EGT               TIMEZONE_DIFF_UTC( -1,00)   //!< UTC-01    Eastern Greenland Time
#define TIMEZONE_EIT               TIMEZONE_DIFF_UTC(  9,00)   //!< UTC+09    Eastern Indonesian Time
#define TIMEZONE_EST_North_America TIMEZONE_DIFF_UTC( -5,00)   //!< UTC-05    Eastern Standard Time (North America)
#define TIMEZONE_EST_Australia     TIMEZONE_DIFF_UTC( 10,00)   //!< UTC+10    Eastern Standard Time (Australia)
#define TIMEZONE_FET               TIMEZONE_DIFF_UTC(  3,00)   //!< UTC+03    Further-eastern European Time
#define TIMEZONE_FJT               TIMEZONE_DIFF_UTC( 12,00)   //!< UTC+12    Fiji Time
#define TIMEZONE_FKST_Std          TIMEZONE_DIFF_UTC( -3,00)   //!< UTC-03    Falkland Islands Standard Time
#define TIMEZONE_FKST_Sum          TIMEZONE_DIFF_UTC( -3,00)   //!< UTC-03    Falkland Islands Summer Time
#define TIMEZONE_FKT               TIMEZONE_DIFF_UTC( -4,00)   //!< UTC-04    Falkland Islands Time
#define TIMEZONE_FNT               TIMEZONE_DIFF_UTC( -2,00)   //!< UTC-02    Fernando de Noronha Time
#define TIMEZONE_GALT              TIMEZONE_DIFF_UTC( -6,00)   //!< UTC-06    Galapagos Time
#define TIMEZONE_GAMT              TIMEZONE_DIFF_UTC( -9,00)   //!< UTC-09    Gambier Islands
#define TIMEZONE_GET               TIMEZONE_DIFF_UTC(  4,00)   //!< UTC+04    Georgia Standard Time
#define TIMEZONE_GFT               TIMEZONE_DIFF_UTC( -3,00)   //!< UTC-03    French Guiana Time
#define TIMEZONE_GILT              TIMEZONE_DIFF_UTC( 12,00)   //!< UTC+12    Gilbert Island Time
#define TIMEZONE_GIT               TIMEZONE_DIFF_UTC( -9,00)   //!< UTC-09    Gambier Island Time
#define TIMEZONE_GMT               TIMEZONE_DIFF_UTC(  0,00)   //!< UTC       Greenwich Mean Time
#define TIMEZONE_GST_South_Georgia TIMEZONE_DIFF_UTC( -2,00)   //!< UTC-02    South Georgia and the South Sandwich Islands
#define TIMEZONE_GST_Gulf          TIMEZONE_DIFF_UTC(  4,00)   //!< UTC+04    Gulf Standard Time
#define TIMEZONE_GYT               TIMEZONE_DIFF_UTC( -4,00)   //!< UTC-04    Guyana Time
#define TIMEZONE_HADT              TIMEZONE_DIFF_UTC( -9,00)   //!< UTC-09    Hawaii-Aleutian Daylight Time
#define TIMEZONE_HAEC              TIMEZONE_DIFF_UTC(  2,00)   //!< UTC+02    Heure Avancee d'Europe Centrale francised name for CEST
#define TIMEZONE_HAST              TIMEZONE_DIFF_UTC(-10,00)   //!< UTC-10    Hawaii-Aleutian Standard Time
#define TIMEZONE_HKT               TIMEZONE_DIFF_UTC( +8,00)   //!< UTC+08    Hong Kong Time
#define TIMEZONE_HMT               TIMEZONE_DIFF_UTC( +5,00)   //!< UTC+05    Heard and McDonald Islands Time
#define TIMEZONE_HOVT              TIMEZONE_DIFF_UTC( +7,00)   //!< UTC+07    Khovd Time
#define TIMEZONE_HST               TIMEZONE_DIFF_UTC(-10,00)   //!< UTC-10    Hawaii Standard Time
#define TIMEZONE_ICT               TIMEZONE_DIFF_UTC(  7,00)   //!< UTC+07    Indochina Time
#define TIMEZONE_IDT               TIMEZONE_DIFF_UTC(  3,00)   //!< UTC+03    Israel Daylight Time
#define TIMEZONE_IOT               TIMEZONE_DIFF_UTC(  3,00)   //!< UTC+03    Indian Ocean Time
#define TIMEZONE_IRDT              TIMEZONE_DIFF_UTC(  4,30)   //!< UTC+04:30 Iran Daylight Time
#define TIMEZONE_IRKT              TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    Irkutsk Time
#define TIMEZONE_IRST              TIMEZONE_DIFF_UTC(  3,30)   //!< UTC+03:30 Iran Standard Time
#define TIMEZONE_IST_Indian        TIMEZONE_DIFF_UTC(  5,30)   //!< UTC+05:30 Indian Standard Time
#define TIMEZONE_IST_Irish         TIMEZONE_DIFF_UTC(  1,00)   //!< UTC+01    Irish Standard Time[5]
#define TIMEZONE_IST_Israel        TIMEZONE_DIFF_UTC(  2,00)   //!< UTC+02    Israel Standard Time
#define TIMEZONE_JST               TIMEZONE_DIFF_UTC(  9,00)   //!< UTC+09    Japan Standard Time
#define TIMEZONE_KGT               TIMEZONE_DIFF_UTC(  6,00)   //!< UTC+06    Kyrgyzstan time
#define TIMEZONE_KOST              TIMEZONE_DIFF_UTC( 11,00)   //!< UTC+11    Kosrae Time
#define TIMEZONE_KRAT              TIMEZONE_DIFF_UTC(  7,00)   //!< UTC+07    Krasnoyarsk Time
#define TIMEZONE_KST               TIMEZONE_DIFF_UTC(  9,00)   //!< UTC+09    Korea Standard Time
#define TIMEZONE_LHST_Std          TIMEZONE_DIFF_UTC( 10,30)   //!< UTC+10:30 Lord Howe Standard Time
#define TIMEZONE_LHST_Sum          TIMEZONE_DIFF_UTC( 11,00)   //!< UTC+11    Lord Howe Summer Time
#define TIMEZONE_LINT              TIMEZONE_DIFF_UTC( 14,00)   //!< UTC+14    Line Islands Time
#define TIMEZONE_MAGT              TIMEZONE_DIFF_UTC( 12,00)   //!< UTC+12    Magadan Time
#define TIMEZONE_MART              TIMEZONE_DIFF_UTC( -9,30)   //!< UTC-09:30 Marquesas Islands Time
#define TIMEZONE_MAWT              TIMEZONE_DIFF_UTC(  5,00)   //!< UTC+05    Mawson Station Time
#define TIMEZONE_MDT               TIMEZONE_DIFF_UTC( -6,00)   //!< UTC-06    Mountain Daylight Time (North America)
#define TIMEZONE_MET               TIMEZONE_DIFF_UTC(  1,00)   //!< UTC+01    Middle European Time Same zone as CET
#define TIMEZONE_MEST              TIMEZONE_DIFF_UTC(  2,00)   //!< UTC+02    Middle European Saving Time Same zone as CEST
#define TIMEZONE_MHT               TIMEZONE_DIFF_UTC( 12,00)   //!< UTC+12    Marshall Islands
#define TIMEZONE_MIST              TIMEZONE_DIFF_UTC( 11,00)   //!< UTC+11    Macquarie Island Station Time
#define TIMEZONE_MIT               TIMEZONE_DIFF_UTC( -9,30)   //!< UTC-09:30 Marquesas Islands Time
#define TIMEZONE_MMT               TIMEZONE_DIFF_UTC(  6,30)   //!< UTC+06:30 Myanmar Time
#define TIMEZONE_MSK               TIMEZONE_DIFF_UTC(  3,00)   //!< UTC+03    Moscow Time
#define TIMEZONE_MST_Malaysia      TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    Malaysia Standard Time
#define TIMEZONE_MST_Mountain      TIMEZONE_DIFF_UTC(  7,00)   //!< UTC-07    Mountain Standard Time (North America)
#define TIMEZONE_MST               TIMEZONE_DIFF_UTC(  6,30)   //!< UTC+06:30 Myanmar Standard Time
#define TIMEZONE_MUT               TIMEZONE_DIFF_UTC(  4,00)   //!< UTC+04    Mauritius Time
#define TIMEZONE_MVT               TIMEZONE_DIFF_UTC(  5,00)   //!< UTC+05    Maldives Time
#define TIMEZONE_MYT               TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    Malaysia Time
#define TIMEZONE_NCT               TIMEZONE_DIFF_UTC( 11,00)   //!< UTC+11    New Caledonia Time
#define TIMEZONE_NDT               TIMEZONE_DIFF_UTC( -2,30)   //!< UTC-02:30 Newfoundland Daylight Time
#define TIMEZONE_NFT               TIMEZONE_DIFF_UTC( 11,30)   //!< UTC+11:30 Norfolk Time
#define TIMEZONE_NPT               TIMEZONE_DIFF_UTC(  5,45)   //!< UTC+05:45 Nepal Time
#define TIMEZONE_NST               TIMEZONE_DIFF_UTC( -3,30)   //!< UTC-03:30 Newfoundland Standard Time
#define TIMEZONE_NT                TIMEZONE_DIFF_UTC( -3,30)   //!< UTC-03:30 Newfoundland Time
#define TIMEZONE_NUT               TIMEZONE_DIFF_UTC(-11,00)   //!< UTC-11    Niue Time
#define TIMEZONE_NZDT              TIMEZONE_DIFF_UTC( 13,00)   //!< UTC+13    New Zealand Daylight Time
#define TIMEZONE_NZST              TIMEZONE_DIFF_UTC( 12,00)   //!< UTC+12    New Zealand Standard Time
#define TIMEZONE_OMST              TIMEZONE_DIFF_UTC(  6,00)   //!< UTC+06    Omsk Time
#define TIMEZONE_ORAT              TIMEZONE_DIFF_UTC(  5,00)   //!< UTC+05    Oral Time
#define TIMEZONE_PDT               TIMEZONE_DIFF_UTC( -7,00)   //!< UTC-07    Pacific Daylight Time (North America)
#define TIMEZONE_PET               TIMEZONE_DIFF_UTC( -5,00)   //!< UTC-05    Peru Time
#define TIMEZONE_PETT              TIMEZONE_DIFF_UTC( 12,00)   //!< UTC+12    Kamchatka Time
#define TIMEZONE_PGT               TIMEZONE_DIFF_UTC( 10,00)   //!< UTC+10    Papua New Guinea Time
#define TIMEZONE_PHOT              TIMEZONE_DIFF_UTC( 13,00)   //!< UTC+13    Phoenix Island Time
#define TIMEZONE_PKT               TIMEZONE_DIFF_UTC(  5,00)   //!< UTC+05    Pakistan Standard Time
#define TIMEZONE_PMDT              TIMEZONE_DIFF_UTC( -2,00)   //!< UTC-02    Saint Pierre and Miquelon Daylight time
#define TIMEZONE_PMST              TIMEZONE_DIFF_UTC( -3,00)   //!< UTC-03    Saint Pierre and Miquelon Standard Time
#define TIMEZONE_PONT              TIMEZONE_DIFF_UTC( 11,00)   //!< UTC+11    Pohnpei Standard Time
#define TIMEZONE_PST_Pacific       TIMEZONE_DIFF_UTC( -8,00)   //!< UTC-08    Pacific Standard Time (North America)
#define TIMEZONE_PST_Philippine    TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    Philippine Standard Time
#define TIMEZONE_PYST              TIMEZONE_DIFF_UTC( -3,00)   //!< UTC-03    Paraguay Summer Time (South America)
#define TIMEZONE_PYT               TIMEZONE_DIFF_UTC( -4,00)   //!< UTC-04    Paraguay Time (South America)
#define TIMEZONE_RET               TIMEZONE_DIFF_UTC(  4,00)   //!< UTC+04    Réunion Time
#define TIMEZONE_ROTT              TIMEZONE_DIFF_UTC( -3,00)   //!< UTC-03    Rothera Research Station Time
#define TIMEZONE_SAKT              TIMEZONE_DIFF_UTC( 11,00)   //!< UTC+11    Sakhalin Island time
#define TIMEZONE_SAMT              TIMEZONE_DIFF_UTC(  4,00)   //!< UTC+04    Samara Time
#define TIMEZONE_SAST              TIMEZONE_DIFF_UTC(  2,00)   //!< UTC+02    South African Standard Time
#define TIMEZONE_SBT               TIMEZONE_DIFF_UTC( 11,00)   //!< UTC+11    Solomon Islands Time
#define TIMEZONE_SCT               TIMEZONE_DIFF_UTC(  4,00)   //!< UTC+04    Seychelles Time
#define TIMEZONE_SGT               TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    Singapore Time
#define TIMEZONE_SLST              TIMEZONE_DIFF_UTC(  5,30)   //!< UTC+05:30 Sri Lanka Time
#define TIMEZONE_SRET              TIMEZONE_DIFF_UTC( 11,00)   //!< UTC+11    Srednekolymsk Time
#define TIMEZONE_SRT               TIMEZONE_DIFF_UTC( -3,00)   //!< UTC-03    Suriname Time
#define TIMEZONE_SST_Samoa         TIMEZONE_DIFF_UTC(-11,00)   //!< UTC-11    Samoa Standard Time
#define TIMEZONE_SST_Singapore     TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    Singapore Standard Time
#define TIMEZONE_SYOT              TIMEZONE_DIFF_UTC(  3,00)   //!< UTC+03    Showa Station Time
#define TIMEZONE_TAHT              TIMEZONE_DIFF_UTC(-10,00)   //!< UTC-10    Tahiti Time
#define TIMEZONE_THA               TIMEZONE_DIFF_UTC(  7,00)   //!< UTC+07    Thailand Standard Time
#define TIMEZONE_TFT               TIMEZONE_DIFF_UTC(  5,00)   //!< UTC+05    Indian/Kerguelen
#define TIMEZONE_TJT               TIMEZONE_DIFF_UTC(  5,00)   //!< UTC+05    Tajikistan Time
#define TIMEZONE_TKT               TIMEZONE_DIFF_UTC( 13,00)   //!< UTC+13    Tokelau Time
#define TIMEZONE_TLT               TIMEZONE_DIFF_UTC(  9,00)   //!< UTC+09    Timor Leste Time
#define TIMEZONE_TMT               TIMEZONE_DIFF_UTC(  5,00)   //!< UTC+05    Turkmenistan Time
#define TIMEZONE_TOT               TIMEZONE_DIFF_UTC( 13,00)   //!< UTC+13    Tonga Time
#define TIMEZONE_TVT               TIMEZONE_DIFF_UTC( 12,00)   //!< UTC+12    Tuvalu Time
#define TIMEZONE_UCT               TIMEZONE_DIFF_UTC(  0,00)   //!< UTC       Coordinated Universal Time
#define TIMEZONE_ULAT              TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    Ulaanbaatar Time
#define TIMEZONE_USZ1              TIMEZONE_DIFF_UTC(  2,00)   //!< UTC+02    Kaliningrad Time
#define TIMEZONE_UTC               TIMEZONE_DIFF_UTC(  0,00)   //!< UTC       Coordinated Universal Time
#define TIMEZONE_UYST              TIMEZONE_DIFF_UTC( -2,00)   //!< UTC-02    Uruguay Summer Time
#define TIMEZONE_UYT               TIMEZONE_DIFF_UTC( -3,00)   //!< UTC-03    Uruguay Standard Time
#define TIMEZONE_UZT               TIMEZONE_DIFF_UTC(  5,00)   //!< UTC+05    Uzbekistan Time
#define TIMEZONE_VET               TIMEZONE_DIFF_UTC( -4,30)   //!< UTC-04:30 Venezuelan Standard Time
#define TIMEZONE_VLAT              TIMEZONE_DIFF_UTC( 10,00)   //!< UTC+10    Vladivostok Time
#define TIMEZONE_VOLT              TIMEZONE_DIFF_UTC(  4,00)   //!< UTC+04    Volgograd Time
#define TIMEZONE_VOST              TIMEZONE_DIFF_UTC(  6,00)   //!< UTC+06    Vostok Station Time
#define TIMEZONE_VUT               TIMEZONE_DIFF_UTC( 11,00)   //!< UTC+11    Vanuatu Time
#define TIMEZONE_WAKT              TIMEZONE_DIFF_UTC( 12,00)   //!< UTC+12    Wake Island Time
#define TIMEZONE_WAST              TIMEZONE_DIFF_UTC(  2,00)   //!< UTC+02    West Africa Summer Time
#define TIMEZONE_WAT               TIMEZONE_DIFF_UTC(  1,00)   //!< UTC+01    West Africa Time
#define TIMEZONE_WEDT              TIMEZONE_DIFF_UTC(  1,00)   //!< UTC+01    Western European Daylight Time
#define TIMEZONE_WEST              TIMEZONE_DIFF_UTC(  1,00)   //!< UTC+01    Western European Summer Time
#define TIMEZONE_WET               TIMEZONE_DIFF_UTC(  0,00)   //!< UTC       Western European Time
#define TIMEZONE_WIT               TIMEZONE_DIFF_UTC(  7,00)   //!< UTC+07    Western Indonesian Time
#define TIMEZONE_WST               TIMEZONE_DIFF_UTC(  8,00)   //!< UTC+08    Western Standard Time
#define TIMEZONE_YAKT              TIMEZONE_DIFF_UTC(  9,00)   //!< UTC+09    Yakutsk Time
#define TIMEZONE_YEKT              TIMEZONE_DIFF_UTC(  5,00)   //!< UTC+05    Yekaterinburg Time
#define TIMEZONE_Z                 TIMEZONE_DIFF_UTC(  0,00)   //!< UTC       Zulu Time (Coordinated Universal Time)

/*==============================================================================
  Exported objects
==============================================================================*/
#ifndef DOXYGEN
extern int _ltimeoff;
#endif

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function returns the processor time consumed by the program.
 *
 * The value returned is expressed in clock ticks, which are units of time of
 * a constant but system-specific length (with a relation of @ref CLOCKS_PER_SEC
 * clock ticks per second).
 * The epoch used as reference by clock varies between systems, but it is related
 * to the program execution (generally its launch). To calculate the actual
 * processing time of a program, the value returned by clock shall be compared
 * to a value returned by a previous call to the same function.
 *
 * @return The number of clock ticks elapsed since an epoch related to the
 *         particular program execution. On failure, the function returns a value
 *         of -1.
 *
 * @see clock_t
 */
//==============================================================================
static inline clock_t clock(void)
{
        return _builtinfunc(kernel_get_time_ms);
}

//==============================================================================
/**
 * @brief  Calculates the difference in seconds between beginning and end.
 *
 * @param  end          Higher bound of the time interval whose length is calculated.
 * @param  beginning    Lower bound of the time interval whose length is calculated.
 *                      If this describes a time point later than end, the result
 *                      is negative.
 *
 * @return The result of (end-beginning) in seconds as a floating-point value of
 *         type double.
 *
 * @see time_t
 */
//==============================================================================
static inline double difftime(time_t end, time_t beginning)
{
        return (double)(end - beginning);
}

//==============================================================================
/**
 * @brief  Convert tm structure to time_t
 *
 * This function performs the reverse translation that localtime does.
 * The values of the members tm_wday and tm_yday of timeptr are ignored, and
 * the values of the other members are interpreted even if out of their valid
 * ranges (see struct tm). For example, tm_mday may contain values above 31,
 * which are interpreted accordingly as the days that follow the last day of
 * the selected month.
 * A call to this function automatically adjusts the values of the members of
 * timeptr if they are off-range or -in the case of tm_wday and tm_yday- if they
 * have values that do not match the date described by the other members.
 *
 * @param  timeptr      Pointer to a tm structure that contains a calendar time
 *                      broken down into its components (see struct tm)
 *
 * @return A time_t value corresponding to the calendar time passed as argument.
 *         If the calendar time cannot be represented, a value of -1 is returned.
 *
 * @see time_t, struct tm, time()
 */
//==============================================================================
#if __OS_ENABLE_TIMEMAN__ == _YES_
extern time_t mktime(struct tm *timeptr);
#else
static inline time_t mktime(struct tm *timeptr)
{
        UNUSED_ARG1(timeptr);
        return 0;
}
#endif

//==============================================================================
/**
 * @brief  Get current time
 *
 * The function returns this value, and if the argument is not a null pointer,
 * it also sets this value to the object pointed by timer.
 * The value returned generally represents the number of seconds since 00:00
 * hours, Jan 1, 1970 UTC (i.e., the current unix timestamp). Although libraries
 * may use a different representation of time: Portable programs should not use
 * the value returned by this function directly, but always rely on calls to
 * other elements of the standard library to translate them to portable types
 * (such as localtime, gmtime or difftime).
 *
 * @param  timer        Pointer to an object of type time_t, where the time
 *                      value is stored.
 *                      Alternatively, this parameter can be a null pointer,
 *                      in which case the parameter is not used (the function
 *                      still returns a value of type time_t with the result).
 *
 * @return The current calendar time as a time_t object.
 *         If the argument is not a null pointer, the return value is the same
 *         as the one stored in the location pointed by argument timer.
 *         If the function could not retrieve the calendar time, it returns
 *         a value of -1.
 *
 * @b Example
 * @code
        #include <time.h>

        //...

        time_t t = time(NULL);

        struct tm tm;
        if (UTC) {
                gmtime_r(&t, &tm);
        } else {
                localtime_r(&t, &tm);
        }

        printf(asctime(&tm));

        //...
   @endcode
 *
 * @see time_t, mktime()
 */
//==============================================================================
static inline time_t time(time_t *timer)
{
#if __OS_ENABLE_TIMEMAN__ == _YES_
        struct timeval timeval;
        int err = -1;
        syscall(SYSCALL_GETTIME, &err, &timeval);

        if (timer) {
                *timer = timeval.tv_sec;
        }

        return err ? (time_t)-1 : timeval.tv_sec;
#else
        UNUSED_ARG1(timer);
        return -1;
#endif
}

//==============================================================================
/**
 * @brief  Set system's time
 *
 * stime() sets the system's idea of the time and date. The time, pointed to by
 * timer, is measured in seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
 *
 * @param  timer        pointer to an object of type time_t, where the time
 *                      value is stored.
 *
 * @return On success 0 is returned.
 *         On error  -1 is returned.
 *
 * @see time_t, time()
 */
//==============================================================================
static inline int stime(time_t *timer)
{
#if __OS_ENABLE_TIMEMAN__ == _YES_
        int r = -1;
        syscall(SYSCALL_SETTIME, &r, timer);
        return r;
#else
        UNUSED_ARG1(timer);
        return -1;
#endif
}

//==============================================================================
/**
 * @brief  Setup time zone by setting difference between UTC and local time
 *
 * @param  tdiff        time difference in seconds (can be negative)
 *
 * @see timezone, stime(), time()
 */
//==============================================================================
static inline void tzset(int tdiff)
{
#if __OS_ENABLE_TIMEMAN__ == _YES_
        _ltimeoff = tdiff;
#else
        UNUSED_ARG1(tdiff);
#endif
}

//==============================================================================
/**
 * @brief  Return difference in seconds between UTC and local time
 *
 * @return Difference between UTC and local time in seconds.
 */
//==============================================================================
static inline int timezone()
{
#if __OS_ENABLE_TIMEMAN__ == _YES_
        return _ltimeoff;
#else
        return 0;
#endif
}

//==============================================================================
/**
 * @brief  Convert tm structure to string
 *
 * Interprets the contents of the tm structure pointed by timeptr as a calendar
 * time and converts it to a C-string containing a human-readable version of the
 * corresponding date and time.
 * The returned string has the following format:
 *
 *      Www Mmm dd hh:mm:ss yyyy
 *
 * Where Www is the weekday, Mmm the month (in letters), dd the day of the month,
 * hh:mm:ss the time, and yyyy the year.
 *
 * The string is followed by a new-line character ('\\n') and terminated with
 * a null-character.
 *
 * @param  timeptr      Pointer to a tm structure that contains a calendar time
 *                      broken down into its components.
 *
 * @return A C-string containing the date and time information in a human-readable
 *         format.
 *         The returned value points to an internal array whose validity or value
 *         may be altered by any subsequent call to asctime or ctime.
 *
 * @b Example
 * @code
        #include <time.h>

        //...

        time_t t = time(NULL);

        struct tm tm;
        if (UTC) {
                gmtime_r(&t, &tm);
        } else {
                localtime_r(&t, &tm);
        }

        printf(asctime(&tm));

        //...
   @endcode
 *
 * @see tm, asctime_r()
 */
//==============================================================================
#if __OS_ENABLE_TIMEMAN__ == _YES_
extern char *asctime(const struct tm *timeptr);
#else
static inline char *asctime(const struct tm *timeptr)
{
        UNUSED_ARG1(timeptr);
        return "";
}
#endif

//==============================================================================
/**
 * @brief  Convert tm structure to string
 *
 * Interprets the contents of the tm structure pointed by timeptr as a calendar
 * time and converts it to a C-string containing a human-readable version of the
 * corresponding date and time.
 * The returned string has the following format:
 *
 *      Www Mmm dd hh:mm:ss yyyy
 *
 * Where Www is the weekday, Mmm the month (in letters), dd the day of the month,
 * hh:mm:ss the time, and yyyy the year.
 *
 * The string is followed by a new-line character ('\\n') and terminated with
 * a null-character.
 *
 * @param  timeptr      Pointer to a tm structure that contains a calendar time
 *                      broken down into its components.
 *
 * @param  buf          Pointer to the buffer where generated string is stored.
 *                      <b>The size of the buffer must be at least 32 bytes long</b>.
 *
 * @return A C-string containing the date and time information in a human-readable
 *         format.
 *         The returned value points to an internal array whose validity or value
 *         may be altered by any subsequent call to asctime or ctime.
 *
 * @see tm, asctime()
 */
//==============================================================================
#if __OS_ENABLE_TIMEMAN__ == _YES_
extern char *asctime_r(const struct tm *timeptr, char *buf);
#else
static inline char *asctime_r(const struct tm *timeptr, char *buf)
{
        UNUSED_ARG2(timeptr, buf);
        return "";
}
#endif

//==============================================================================
/**
 * @brief  Convert time_t value to string
 *
 * Interprets the value pointed by timer as a calendar time and converts it to
 * a C-string containing a human-readable version of the corresponding time and
 * date, in terms of local time.
 *
 * The returned string has the following format:
 *
 *      Www Mmm dd hh:mm:ss zzzzz yyyy
 *
 * Where Www is the weekday, Mmm the month (in letters), dd the day of the month,
 * hh:mm:ss the time, and yyyy the year.
 *
 * The string is followed by a new-line character ('\\n') and terminated with
 * a null-character.
 *
 * This function is equivalent to:
 *
 *      asctime(localtime(timer))
 *
 * @param  timer        Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @return A C-string containing the date and time information in a human-readable format.
 *         The returned value points to an internal array whose validity or
 *         value may be altered by any subsequent call to asctime() or ctime().
 *
 * @see time_t, asctime(), ctime_r()
 */
//==============================================================================
#if __OS_ENABLE_TIMEMAN__ == _YES_
extern char *ctime(const time_t *timer);
#else
static inline char *ctime(const time_t *timer)
{
        UNUSED_ARG1(timer);
        return "";
}
#endif

//==============================================================================
/**
 * @brief  Convert time_t value to string
 *
 * Interprets the value pointed by timer as a calendar time and converts it to
 * a C-string containing a human-readable version of the corresponding time and
 * date, in terms of local time.
 *
 * The returned string has the following format:
 *
 *      Www Mmm dd hh:mm:ss zzzzz yyyy
 *
 * Where Www is the weekday, Mmm the month (in letters), dd the day of the month,
 * hh:mm:ss the time, and yyyy the year.
 *
 * The string is followed by a new-line character ('\\n') and terminated with
 * a null-character.
 *
 * This function is equivalent to:
 *
 *      asctime(localtime(timer))
 *
 * @param  timer        Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @param  buf          Pointer to the buffer where generated string is stored.
 *                      <b>The size of the buffer must be at least 32 bytes long</b>.
 *
 * @return A C-string containing the date and time information in a human-readable format.
 *         The returned value points to an internal array whose validity or
 *         value may be altered by any subsequent call to asctime() or ctime().
 *
 * @see time_t, asctime(), ctime()
 */
//==============================================================================
#if __OS_ENABLE_TIMEMAN__ == _YES_
extern char *ctime_r(const time_t *timer, char *buf);
#else
static inline char *ctime_r(const time_t *timer, char *buf)
{
        UNUSED_ARG2(timer, buf);
        return "";
}
#endif

//==============================================================================
/**
 * @brief  Convert time_t to tm as UTC time
 *
 * Uses the value pointed by timer to fill a tm structure with the values that
 * represent the corresponding time, expressed as a UTC time (i.e., the time
 * at the GMT timezone).
 *
 * @param  timer        Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the UTC time representation of timer.
 *
 * @see time_t, tm, gmtime_r(), localtime(), localtime_r()
 */
//==============================================================================
#if __OS_ENABLE_TIMEMAN__ == _YES_
extern struct tm *gmtime(const time_t *timer);
#else
static inline struct tm *gmtime(const time_t *timer)
{
        UNUSED_ARG1(timer);
        return NULL;
}
#endif

//==============================================================================
/**
 * @brief  Convert time_t to tm as UTC time
 *
 * Uses the value pointed by timer to fill a tm structure with the values that
 * represent the corresponding time, expressed as a UTC time (i.e., the time
 * at the GMT timezone).
 *
 * @param timer    Pointer to an object of type time_t that contains a time value.
 *                 time_t is an alias of a fundamental arithmetic type
 *                 capable of representing times as returned by function time.
 *
 * @param tm       Pointer to an object of type struct tm that will contains
 *                 converted timer value to time structure.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the UTC time representation of timer.
 *
 * @b Example
 * @code
        #include <time.h>

        //...

        time_t t = time(NULL);

        struct tm tm;
        if (UTC) {
                gmtime_r(&t, &tm);
        } else {
                localtime_r(&t, &tm);
        }

        printf(asctime(&tm));

        //...
   @endcode
 *
 * @see time_t, tm, gmtime(), localtime(), localtime_r()
 */
//==============================================================================
#if __OS_ENABLE_TIMEMAN__ == _YES_
extern struct tm *gmtime_r(const time_t *timer, struct tm *tm);
#else
static inline struct tm *gmtime_r(const time_t *timer, struct tm *tm)
{
        UNUSED_ARG2(timer, tm);
        return NULL;
}
#endif

//==============================================================================
/**
 * @brief  Convert time_t to tm as local time
 *
 * Uses the value pointed by timer to fill a tm structure with the values that
 * represent the corresponding time, expressed for the local timezone.
 *
 * @param  timer        Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the local time representation of timer.
 *
 * @see time_t, tm, gmtime(), gmtime_r(), localtime_r()
 */
//==============================================================================
#if __OS_ENABLE_TIMEMAN__ == _YES_
extern struct tm *localtime(const time_t *timer);
#else
static inline struct tm *localtime(const time_t *timer)
{
        UNUSED_ARG1(timer);
        return NULL;
}
#endif

//==============================================================================
/**
 * @brief  Convert time_t to tm as local time
 *
 * Uses the value pointed by <i>timer</i> to fill a tm structure with the values that
 * represent the corresponding time, expressed for the local timezone.
 *
 * @param timer    Pointer to an object of type time_t that contains a time value.
 *                 time_t is an alias of a fundamental arithmetic type
 *                 capable of representing times as returned by function time.
 *
 * @param tm       Pointer to an object of type struct tm that will contains
 *                 converted timer value to time structure.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the local time representation of timer.
 *
 * @b Example
 * @code
        #include <time.h>

        //...

        time_t t = time(NULL);

        struct tm tm;
        if (UTC) {
                gmtime_r(&t, &tm);
        } else {
                localtime_r(&t, &tm);
        }

        printf(asctime(&tm));

        //...
   @endcode
 *
 * @see time_t, tm, gmtime(), gmtime_r(), localtime()
 */
//==============================================================================
#if __OS_ENABLE_TIMEMAN__ == _YES_
extern struct tm *localtime_r(const time_t *timer, struct tm *tm);
#else
static inline struct tm *localtime_r(const time_t *timer, struct tm *tm)
{
        UNUSED_ARG2(timer, tm);
        return NULL;
}
#endif

//==============================================================================
/**
 * @brief  Format time as string
 *
 * Copies into <i>ptr</i> the content of format, expanding its format specifiers into
 * the corresponding values that represent the time described in <i>timeptr</i>, with
 * a limit of <i>maxsize</i> characters.
 *
 * @param  ptr          Pointer to the destination array where the resulting
 *                      C string is copied.
 * @param  maxsize      Maximum number of characters to be copied to ptr,
 *                      including the terminating null-character.
 * @param  format       C string containing any combination of regular characters
 *                      and special format specifiers. These format specifiers
 *                      are replaced by the function to the corresponding values
 *                      to represent the time specified in timeptr.
 * @param timeptr       Pointer to a tm structure that contains a calendar time
 *                      broken down into its components.
 *
 * @return If the length of the resulting C string, including the terminating
 *         null-character, doesn't exceed maxsize, the function returns the
 *         total number of characters copied to ptr (not including the terminating
 *         null-character).
 *         Otherwise, it returns zero, and the contents of the array pointed by
 *         ptr are indeterminate.
 *
 * @b Supported @b flags @n
 * @arg % - % character
 * @arg n - new line
 * @arg H - Hour in 24h format (00-23)
 * @arg I - Hour in 12h format (01-12)
 * @arg M - Minute (00-59)
 * @arg S - Second (00-61)
 * @arg A - Full weekday name
 * @arg a - Abbreviated weekday name
 * @arg B - Full month name
 * @arg b - Abbreviated month name
 * @arg h - Abbreviated month name
 * @arg C - Year divided by 100 and truncated to integer (00-99) (century)
 * @arg y - Year, last two digits (00-99)
 * @arg Y - Year
 * @arg d - Day of the month, zero-padded (01-31)
 * @arg p - AM or PM designation
 * @arg j - Day of the year (001-366)
 * @arg m - Month as a decimal number (01-12)
 * @arg X - Time representation                                14:55:02
 * @arg F - Short YYYY-MM-DD date, equivalent to %Y-%m-%d      2001-08-23
 * @arg D - Short MM/DD/YY date, equivalent to %m/%d/%y        08/23/01
 * @arg x - Short MM/DD/YY date, equivalent to %m/%d/%y        08/23/01
 * @arg z - ISO 8601 offset from UTC in timezone (1 minute=1, 1 hour=100) +0100, -1230
 */
//==============================================================================
#if __OS_ENABLE_TIMEMAN__ == _YES_
extern size_t strftime(char *ptr, size_t maxsize, const char *format, const struct tm *timeptr);
#else
static inline size_t strftime(char *ptr, size_t maxsize, const char *format, const struct tm *timeptr)
{
        UNUSED_ARG4(ptr, maxsize, format, timeptr);
        return 0;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* _TIME_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
