/*=========================================================================*//**
@file    sntpd.c

@author  Daniel Zorychta

@brief   SNTP daemon

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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <dnx/net.h>
#include <dnx/misc.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/** SNTP server port */
#define SNTP_PORT                   123

/** SNTP send timeout - in milliseconds */
#define SNTP_SEND_TIMEOUT           1000

/** SNTP receive timeout - in milliseconds */
#define SNTP_RECV_TIMEOUT           3000

/** number of seconds between 1900 and 1970 */
#define DIFF_SEC_1900_1970          2208988800

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
// Leap Indicator
enum LI {
        LI_No_warning                      = 0,
        LI_Last_minute_has_61_sec          = 1,
        LI_Last_minute_has_59_sec          = 2,
        LI_Alarm_condition                 = 3,
};

// Version Number
enum VN {
        VN_none_0                          = 0,
        VN_none_1                          = 1,
        VN_none_2                          = 2,
        VN_IPv4_only                       = 3,
        VN_IPv4_IPv6_OSI                   = 4,
        VN_none_5                          = 5,
        VN_none_6                          = 6,
        VN_none_7                          = 7
};

// Mode
enum Mode{
        Mode_Reserved                      = 0,
        Mode_Symetric_active               = 1,
        Mode_Symetric_passive              = 2,
        Mode_Client                        = 3,
        Mode_Server                        = 4,
        Mode_Broadcast                     = 5,
        Mode_Reserved_NTP_ctrl_msg         = 6,
        Mode_Reserved_for_private_use      = 7
};

// Stratum
enum Stratum {
        Stratum_Unspecified_or_unavailable = 0,
        Stratum_Primary_reference          = 1,
        Stratum_Secondary_reference_2      = 2,
        Stratum_Secondary_reference_3      = 3,
        Stratum_Secondary_reference_4      = 4,
        Stratum_Secondary_reference_5      = 5,
        Stratum_Secondary_reference_6      = 6,
        Stratum_Secondary_reference_7      = 7,
        Stratum_Secondary_reference_8      = 8,
        Stratum_Secondary_reference_9      = 9,
        Stratum_Secondary_reference_10     = 10,
        Stratum_Secondary_reference_11     = 11,
        Stratum_Secondary_reference_12     = 12,
        Stratum_Secondary_reference_13     = 13,
        Stratum_Secondary_reference_14     = 14,
        Stratum_Secondary_reference_15     = 15,
        Startum_Reserved                   = 16,
};

/** SNTP request/response packet structure */
typedef struct {
        // Packet settings
        union {
                struct {
                        // Precision. 8 bits, signed
                        // The precision of the local clock, in seconds to the nearest
                        // power of two. The values that normally appear in this field
                        // range from -6 for mains-frequency clocks to -20 for microsecond
                        // clocks found in some workstations.
                        uint32_t Precision:8;

                        // Poll.
                        // The maximum interval between successive messages, in seconds
                        // to the nearest power of two. The values that can appear in this
                        // field presently range from 4 (16 s) to 14 (16284 s); however,
                        // most applications use only the sub-range 6 (64 s) to 10 (1024 s).
                        uint32_t Poll:8;

                        // Stratum
                        // Stratum level of the local clock.
                        uint32_t Stratum:8;

                        // Mode
                        uint32_t Mode:3;

                        // Version Number.
                        // The NTP/SNTP version number. If necessary to distinguish between
                        // IPv4, IPv6 and OSI, the encapsulating context must be inspected.
                        uint32_t VN:3;

                        // Leap Indicator.
                        // This code warns of an impending leap second
                        // to be inserted/deleted in the last minute of the current day.
                        uint32_t LI:2;

                } field;
                uint32_t word;
        } settings;

        // Root Delay. 32 bits, signed fixed point.
        // The total roundtrip delay to the primary reference source, in seconds
        // with fraction point between bits 15 and 16. Note that this variable
        // can take on both positive and negative values, depending on the relative
        // time and frequency offsets. The values that normally appear in this
        // field range from negative values of a few milliseconds to positive
        // values of several hundred milliseconds.
        uint32_t root_delay;

        // Root Dispersion. 32 bits, unsigned fixed point.
        // The nominal error relative to the primary reference source, in seconds
        // with fraction point between bits 15 and 16. The values that normally
        // appear in this field range from 0 to several hundred milliseconds.
        uint32_t root_dispersion;

        // Reference Identifier. 32 bits.
        // The reference source. In the case of NTP Version 3 or Version 4
        // stratum-0 (unspecified) or stratum-1 (primary) servers, this is
        // a four-character ASCII string, left justified and zero padded to
        // 32 bits. In NTP Version 3 secondary servers, this is the 32-bit
        // IPv4 address of the reference source. In NTP Version 4 secondary
        // servers, this is the low order 32 bits of the latest transmit
        // timestamp of the reference source. NTP primary (stratum 1) servers
        // should set this field to a code identifying the external reference
        // source according to the following list. If the external reference is
        // one of those listed, the associated code should be used. Codes for
        // sources not listed can be contrived as appropriate.
        //
        // |-----+--------------------------------------------------------------
        // |Code + Description
        // |-----+--------------------------------------------------------------
        // |ACTS + NIST dialup modem service.
        // |CHU  + Ottawa (Canada) Radio 3330, 7335, 14670 kHz.
        // |DCF  + Mainflingen (Germany) Radio 77.5 kHz.
        // |GOES + Geostationary Orbit Environment Satellite.
        // |GPS  + Global Positioning Service.
        // |LOCL + Uncalibrated local clock used as a primary reference for a subnet without external means of synchronization.
        // |LORC + LORAN-C radionavigation system.
        // |MSF  + Rugby (UK) Radio 60 kHz.
        // |OMEG + OMEGA radionavigation system.
        // |PPS  + Atomic clock or other pulse-per-second source individually calibrated to national standards.
        // |PTB  + PTB (Germany) modem service.
        // |TDF  + Allouis (France) Radio 164 kHz.
        // |USNO + USNO modem service.
        // |WWV  + Ft. Collins (US) Radio 2.5, 5, 10, 15, 20 MHz.
        // |WWVB + Boulder (US) Radio 60 kHz.
        // |WWVH + Kaui Hawaii (US) Radio 2.5, 5, 10, 15 MHz.
        // |-----+--------------------------------------------------------------
        uint32_t reference_identifier;

        // Reference Timestamp. 64 bits.
        // The time at which the local clock was last set or corrected.
        uint64_t reference_timestamp;

        // Originate Timestamp. 64 bits.
        // The time at which the request departed the client for the server.
        uint64_t originate_timestamp;

        // Receive Timestamp. 64 bits.
        // The time at which the request arrived at the server.
        uint64_t receive_timestamp;

        // Transmit Timestamp. 64 bits.
        // The time at which the reply departed the server for the client.
        uint64_t transmit_timestamp;
} sntp_msg_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        int current_host_arg;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief  Function select next host if current fail
 * @param  argc         argument count
 * @return None
 */
//==============================================================================
static void select_next_host(int argc)
{
        if (global->current_host_arg >= argc - 1) {
                global->current_host_arg = 2;
        } else {
                global->current_host_arg++;
        }
}

//==============================================================================
/**
 * @brief  Sent request to SNTP server
 * @param  conn         connection
 * @return Operation status (one of net_err_t's values)
 */
//==============================================================================
static net_err_t sntp_send_request(net_conn_t *conn)
{
        net_err_t   err          = NET_ERR_OUT_OF_MEMORY;
        net_buf_t  *tx_UDP_buf   = net_buf_new();
        sntp_msg_t *sntp_request = net_buf_alloc(tx_UDP_buf, sizeof(sntp_msg_t));

        if (tx_UDP_buf && sntp_request) {
                memset(sntp_request, 0, sizeof(sntp_msg_t));
                sntp_request->settings.field.LI        = LI_No_warning;
                sntp_request->settings.field.VN        = VN_IPv4_IPv6_OSI;
                sntp_request->settings.field.Mode      = Mode_Client;
                sntp_request->settings.field.Poll      = 0;
                sntp_request->settings.field.Precision = 0;
                sntp_request->settings.field.Stratum   = Stratum_Unspecified_or_unavailable;
                sntp_request->settings.word            = htonl(sntp_request->settings.word);

                err = net_conn_send(conn, tx_UDP_buf);
        }

        // delete both buffer and allocated memory
        net_buf_delete(tx_UDP_buf);

        return err;
}

//==============================================================================
/**
 * @brief  Receive response from SNTP server
 * @param  conn         connection
 * @param  timestamp    timestamp value
 * @return Operation status (one of net_err_t's values)
 */
//==============================================================================
static net_err_t sntp_receive_response(net_conn_t *conn, time_t *timestamp)
{
        net_buf_t *rx_UDP_buf = NULL;
        net_err_t  err        = net_conn_receive(conn, &rx_UDP_buf);

        if (err == NET_ERR_OK) {
                sntp_msg_t *sntp_response;
                u16_t       sntp_response_len;

                err = net_buf_data(rx_UDP_buf, (void**)&sntp_response, &sntp_response_len);
                if (err == NET_ERR_OK) {

                        if (sntp_response_len == sizeof(sntp_msg_t)) {
                                sntp_response->settings.word = ntohl(sntp_response->settings.word);

                                if (  sntp_response->settings.field.Mode == Mode_Server
                                   || sntp_response->settings.field.Mode == Mode_Broadcast) {

                                        time_t ts = 0;
                                        memcpy(&ts, &sntp_response->receive_timestamp, sizeof(time_t));
                                        ts         = ntohl(ts) - DIFF_SEC_1900_1970;
                                        *timestamp = ts;
                                } else {
                                        err = NET_ERR_ILLEGAL_VALUE;
                                }
                        }

                        net_buf_delete(rx_UDP_buf);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Get IP address from given host names (find in DNS)
 * @param[in]  once         only one try (program started not as daemon)
 * @param[in]  argc         number of arguments (used to obtain host names)
 * @param[in]  argv         program arguments
 * @param[out] ip           found IP address
 * @return Operation status (one of net_err_t's values)
 */
//==============================================================================
static net_err_t get_SNTP_host_IP(bool once, int argc, char *argv[], net_ip_t *ip)
{
        int appends = argc - 2;

        net_err_t err;
        while ((err = net_conn_get_host_by_name(argv[global->current_host_arg], ip)) != NET_ERR_OK) {

                fprintf(stderr, "[%d] %s: no host\n",
                        clock() / CLOCKS_PER_SEC,
                        argv[global->current_host_arg]);

                select_next_host(argc);

                appends--;
                if (once && appends == 0) {
                        break;
                } else {
                        sleep(1);
                        continue;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Program main function
 * @param argc          number of arguments
 * @param argv          argument list
 */
//==============================================================================
int_main(sntpd, STACK_DEPTH_LOW, int argc, char *argv[])
{
        if (argc < 3) {
                fprintf(stderr, "%s <interval_sec> <host_1> [host_2] [host_3]\n", argv[0]);
                return EXIT_FAILURE;
        }

        int interval = -1;
        sscanf(argv[1], "%d", &interval);

        if (interval < 0) {
                fprintf(stderr, "Invalid interval value\n");
                return EXIT_FAILURE;
        }

        global->current_host_arg = 2;

        int tref = prepare_sleep_until();

        do {
                net_ip_t  ntp_ip;
                net_err_t err = get_SNTP_host_IP(interval == 0, argc, argv, &ntp_ip);

                if (err == NET_ERR_OK) {

                        net_conn_t *conn = net_conn_new(NET_CONN_TYPE_UDP);
                        net_err_t   err  = NET_ERR_OUT_OF_MEMORY;

                        if (conn) {
                                net_conn_set_send_timeout(conn, SNTP_SEND_TIMEOUT);
                                net_conn_set_receive_timeout(conn, SNTP_RECV_TIMEOUT);

                                if ((err = net_conn_connect(conn, &ntp_ip, SNTP_PORT)) == NET_ERR_OK) {

                                        if (sntp_send_request(conn) == NET_ERR_OK) {

                                                time_t timestamp = 0;
                                                if (sntp_receive_response(conn, &timestamp) == NET_ERR_OK) {
                                                        stime(&timestamp);
                                                } else {
                                                        select_next_host(argc);
                                                }
                                        }

                                        net_conn_close(conn);
                                }
                        }

                        if (conn)
                                net_conn_delete(conn);

                        sleep_until(interval, &tref);
                }

                if (err != NET_ERR_OK) {
                        fprintf(stderr, "[%d] error %d\n", clock() / CLOCKS_PER_SEC, err);
                }

        } while (interval);

        return interval ? EXIT_FAILURE : EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
