/*=========================================================================*//**
@file    ftpd.c

@author  Daniel Zorychta

@brief   The FTP server.

@note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <dnx/net.h>
#include <dnx/misc.h>
#include <dnx/os.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define USER_NAME               "dnx"
#define PASSWORD                "dnx"
#define FTP_PORT                21
#define FTP_PASV_PORT_START     2121
#define FTP_PASV_PORT_END       2131

#define TIMEOUT_SEND            1000
#define TIMEOUT_RECV            1000

#define VERBOSE(...)            if (global->verbose) {printf(__VA_ARGS__);}

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef enum {
    FTP_CMD__ABOR,       //!< abort a file transfer
    FTP_CMD__CWD ,       //!< change working directory
    FTP_CMD__DELE,       //!< delete a remote file
    FTP_CMD__LIST,       //!< list remote files
    FTP_CMD__MDTM,       //!< return the modification time of a file
    FTP_CMD__MKD ,       //!< make a remote directory
    FTP_CMD__NLST,       //!< name list of remote directory
    FTP_CMD__PASS,       //!< send password
    FTP_CMD__PASV,       //!< enter passive mode
    FTP_CMD__PORT,       //!< open a data port
    FTP_CMD__PWD ,       //!< print working directory
    FTP_CMD__QUIT,       //!< terminate the connection
    FTP_CMD__RETR,       //!< retrieve a remote file
    FTP_CMD__RMD ,       //!< remove a remote directory
    FTP_CMD__RNFR,       //!< rename from
    FTP_CMD__RNTO,       //!< rename to
    FTP_CMD__SITE,       //!< site-specific commands
    FTP_CMD__SIZE,       //!< return the size of a file
    FTP_CMD__STOR,       //!< store a file on the remote host
    FTP_CMD__TYPE,       //!< set transfer type
    FTP_CMD__USER,       //!< send username
    // Less common commands
    FTP_CMD__ACCT,       //!< *send account information
    FTP_CMD__APPE,       //!< append to a remote file
    FTP_CMD__CDUP,       //!< CWD to the parent of the current directory
    FTP_CMD__HELP,       //!< return help on using the server
    FTP_CMD__MODE,       //!< set transfer mode
    FTP_CMD__NOOP,       //!< do nothing
    FTP_CMD__REIN,       //!< *reinitialize the connection
    FTP_CMD__STAT,       //!< return server status
    FTP_CMD__STOU,       //!< store a file uniquely
    FTP_CMD__STRU,       //!< set file transfer structure
    FTP_CMD__SYST,       //!< return system type
    FTP_CMD__COUNT
} FTP_cmd_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        bool                    passive_mode;   // true for PASSIVE mode, false for ACTIVE
        NET_INET_sockaddr_t     client_addr;    // client address in ACTIVE mode
        SOCKET                 *datasock;       // data channel
        uint16_t                dataport;
        char                   *RNFR;
        char                    PWD[128];
        char                    buf[512];
        int                     bufsz;
        char                   *params;
        bool                    quit;
        bool                    verbose;
        const char             *CWD_arg;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
static FTP_cmd_t get_cmd(void);
static void handle_cmd_ABOR(SOCKET *socket);

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief  Select next connection port in PASV mode.
 */
//==============================================================================
static void select_next_port(void)
{
        if (global->dataport < FTP_PASV_PORT_END) {
                global->dataport++;
        } else {
                global->dataport = FTP_PASV_PORT_START;
        }
}

//==============================================================================
/**
 * @brief  Send file list.
 * @param  socket       data socket where list to be sent.
 * @return Number of sent bytes.
 */
//==============================================================================
static int send_file_list(SOCKET *socket)
{
        socket_set_send_timeout(socket, TIMEOUT_SEND);

        int n = 0;

        struct stat fsta;
        if (stat(global->params, &fsta) == 0) {

                char *name = malloc(strsize(global->params));
                if (name) {
                        strcpy(name, global->params);

                        struct tm tm;
                        localtime_r(&fsta.st_mtime, &tm);

                        int sz = snprintf(global->buf, sizeof(global->buf),
                                          "%crw-r--r-- 1 %s  %s %u ",
                                          (fsta.st_type == FILE_TYPE_DIR ? 'd' : '-'),
                                          get_user_name(),
                                          get_user_name(),
                                          cast(uint, fsta.st_size));

                        sz += strftime(global->buf + sz, sizeof(global->buf) - sz,
                                       "%b %d %H:%M ", &tm);

                        sz += snprintf(global->buf + sz, sizeof(global->buf) - sz,
                                       "%s\r\n", name);

                        if (socket_send(socket, global->buf, sz, NET_FLAGS__NOCOPY) == sz) {
                                VERBOSE(global->buf);
                                n += sz;
                        }

                        free(name);
                }

        } else {
                errno    = 0;
                DIR *dir = opendir(global->PWD);
                if (dir) {
                        dirent_t *dirent = NULL;
                        while ((dirent = readdir(dir))) {

                                memset(&fsta, 0, sizeof(struct stat));
                                stat(dirent->name, &fsta);

                                struct tm tm;
                                localtime_r(&fsta.st_mtime, &tm);

                                int sz = snprintf(global->buf, sizeof(global->buf),
                                                  "%crw-r--r-- 1 %s %s %u ",
                                                  (dirent->filetype == FILE_TYPE_DIR ? 'd' : '-'),
                                                  get_user_name(),
                                                  get_user_name(),
                                                  cast(uint, dirent->size));

                                sz += strftime(global->buf + sz, sizeof(global->buf) - sz,
                                               "%b %d %Y ", &tm);

                                sz += snprintf(global->buf + sz, sizeof(global->buf) - sz,
                                               "%s\r\n", dirent->name);

                                if (socket_send(socket, global->buf, sz, NET_FLAGS__NOCOPY) == sz) {
                                        VERBOSE(global->buf);
                                        n += sz;
                                } else {
                                        break;
                                }
                        }

                        closedir(dir);

                } else {
                        perror(global->PWD);
                }
        }

        return n;
}

//==============================================================================
/**
 * @brief  Receive file data.
 * @param  socket       Socket from data to be received.
 * @param  mode         File mode (fopen mode).
 * @return On success 0 is returned, otherwise other value.
 */
//==============================================================================
static int receive_file(SOCKET *socket, const char *mode)
{
        socket_set_recv_timeout(socket, TIMEOUT_RECV);

        int   err  = 0;
        FILE *file = fopen(global->params, mode);
        if (file) {

                int sz;
                do {
                        sz = socket_read(socket, global->buf, sizeof(global->buf));
                        if (sz > 0) {
                                if (fwrite(global->buf, 1, sz, file) != (size_t)sz) {
                                        err = 1;
                                }
                        }

                } while (sz > 0 && err == 0);

                fclose(file);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Transmit file to via selected socket.
 * @param  socket       Socket by which data to be transferred.
 * @return On success 0 is returned, otherwise other value.
 */
//==============================================================================
static int transmit_file(SOCKET *socket)
{
        socket_set_send_timeout(socket, TIMEOUT_SEND);

        int   err  = 0;
        FILE *file = fopen(global->params, "r");
        if (file) {

                int sz;
                do {
                        sz = fread(global->buf, 1, sizeof(global->buf), file);
                        if (sz > 0) {
                                if (socket_write(socket, global->buf, sz) < 0) {
                                        err = 1;
                                }
                        }

                } while (sz > 0 && err == 0);

                fclose(file);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Handle USER command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_USER(SOCKET *socket)
{
        if (isstreq(global->params, USER_NAME)) {
                static const char *PASS_NEEDED  = "331 Enter user password\r\n";
                socket_send(socket, PASS_NEEDED, strlen(PASS_NEEDED), NET_FLAGS__NOCOPY);
                VERBOSE(PASS_NEEDED);
        } else {
                static const char *UNKNOWN_USER = "530 Unknown user\r\n";
                socket_send(socket, UNKNOWN_USER, strlen(UNKNOWN_USER), NET_FLAGS__NOCOPY);
                VERBOSE(UNKNOWN_USER);
        }
}

//==============================================================================
/**
 * @brief  Handle PASS command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_PASS(SOCKET *socket)
{
        if (isstreq(global->params, PASSWORD)) {
                static const char *PASS_OK  = "230 Hello!\r\n";
                socket_send(socket, PASS_OK, strlen(PASS_OK), NET_FLAGS__NOCOPY);
                VERBOSE(PASS_OK);
        } else {
                static const char *PASS_NOK = "530 Incorrect password!\r\n";
                socket_send(socket, PASS_NOK, strlen(PASS_NOK), NET_FLAGS__NOCOPY);
                VERBOSE(PASS_NOK);
        }
}

//==============================================================================
/**
 * @brief  Handle QUIT command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_QUIT(SOCKET *socket)
{
        static const char *BYE = "221 Bye\r\n";
        socket_send(socket, BYE, strlen(BYE), NET_FLAGS__NOCOPY);
        VERBOSE(BYE);

        global->quit = true;
}


//==============================================================================
/**
 * @brief  Handle SYST command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_SYST(SOCKET *socket)
{
        static const char *TYPE = "215 UNIX Type: L8\r\n";
        socket_send(socket, TYPE, strlen(TYPE), NET_FLAGS__NOCOPY);
        VERBOSE(TYPE);
}

//==============================================================================
/**
 * @brief  Handle PWD command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_PWD(SOCKET *socket)
{
        static const char *PWD_b = "257 \"";
        socket_send(socket, PWD_b, strlen(PWD_b), NET_FLAGS__NOCOPY);
        VERBOSE(PWD_b);

        socket_send(socket, global->CWD_arg, strlen(global->CWD_arg), NET_FLAGS__NOCOPY);
        VERBOSE(global->CWD_arg);

        static const char *PWD_e = "\"\r\n";
        socket_send(socket, PWD_e, strlen(PWD_e), NET_FLAGS__NOCOPY);
        VERBOSE(PWD_e);
}

//==============================================================================
/**
 * @brief  Handle TYPE command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_TYPE(SOCKET *socket)
{
        if (isstreq(global->params, "I") || isstreq(global->params, "L 8")) {
                static const char *ACCEPT = "200 Binary mode ON\r\n";
                socket_send(socket, ACCEPT, strlen(ACCEPT), NET_FLAGS__NOCOPY);
                VERBOSE(ACCEPT);
        } else {
                static const char *REJECT = "504 Use binary mode\r\n";
                socket_send(socket, REJECT, strlen(REJECT), NET_FLAGS__NOCOPY);
                VERBOSE(REJECT);
        }
}

//==============================================================================
/**
 * @brief  Handle PASV command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_PASV(SOCKET *socket)
{
        NET_INET_status_t status;
        memset(&status, 0, sizeof(status));

        if (ifstatus(NET_FAMILY__INET, &status) == 0) {

                global->passive_mode = true;

                global->datasock = socket_new(NET_FAMILY__INET, NET_PROTOCOL__TCP);
                if (global->datasock) {
                        NET_INET_sockaddr_t ADDR_ANY = {
                            .addr = NET_INET_IPv4_ANY,
                            .port = global->dataport
                        };

                        int err = socket_bind(global->datasock, &ADDR_ANY);
                        if (!err) {

                                err = socket_listen(global->datasock);
                                if (!err) {

                                        int n = snprintf(global->buf, sizeof(global->buf),
                                                         "227 =%d,%d,%d,%d,%d,%d\r\n",
                                                         NET_INET_IPv4_a(status.address),
                                                         NET_INET_IPv4_b(status.address),
                                                         NET_INET_IPv4_c(status.address),
                                                         NET_INET_IPv4_d(status.address),
                                                         (global->dataport >> 8) & 0xFF,
                                                         (global->dataport & 0xFF) );

                                        socket_send(socket, global->buf, n, NET_FLAGS__COPY);
                                        VERBOSE(global->buf);
                                }
                        }

                        if (err) {
                                socket_delete(global->datasock);
                                global->datasock = NULL;
                        }
                }

        } else {
                static const char *REJECT = "504 Not supported\r\n";
                socket_send(socket, REJECT, strlen(REJECT), NET_FLAGS__NOCOPY);
                VERBOSE(REJECT);
        }
}

//==============================================================================
/**
 * @brief  Handle PORT command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_PORT(SOCKET *socket)
{
        int a1,a2,a3,a4,p1,p2;

        sscanf(global->params, "%d,%d,%d,%d,%d,%d", &a1, &a2, &a3, &a4, &p1, &p2);

        global->client_addr.addr = NET_INET_IPv4(a1, a2, a3, a4);
        global->client_addr.port = (p1 << 8) | p2;

        global->passive_mode = false;

        global->datasock = socket_new(NET_FAMILY__INET, NET_PROTOCOL__TCP);
        if (global->datasock) {
                static const char *OK = "200 OK\r\n";
                socket_send(socket, OK, strlen(OK), NET_FLAGS__NOCOPY);
                VERBOSE(OK);
        } else {
                static const char *ERR = "425 No connection\r\n";
                socket_send(socket, ERR, strlen(ERR), NET_FLAGS__NOCOPY);
                VERBOSE(ERR);
        }
}

//==============================================================================
/**
 * @brief  Handle LIST command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_LIST(SOCKET *socket)
{
        static const char *MARK = "150 Accepted data connection...\r\n";
        socket_send(socket, MARK, strlen(MARK), NET_FLAGS__NOCOPY);
        VERBOSE(MARK);

        const char *STATUS = "226 Done\r\n";

        if (global->datasock) {

                int err = 0;

                if (global->passive_mode) {

                        SOCKET *dsock = NULL;
                        err = socket_accept(global->datasock, &dsock);
                        if (!err) {
                                send_file_list(dsock);
                                socket_delete(dsock);
                        }

                } else {
                        err = socket_connect(global->datasock, &global->client_addr);
                        if (!err) {
                                send_file_list(global->datasock);
                                socket_disconnect(global->datasock);
                        }
                }

                if (err) {
                        STATUS = "426 Connection timeout\r\n";
                }

                socket_delete(global->datasock);
                global->datasock = NULL;

                select_next_port();

        } else {
                STATUS = "425 No connection\r\n";
        }

        socket_send(socket, STATUS, strlen(STATUS), NET_FLAGS__NOCOPY);
        VERBOSE(STATUS);
}

//==============================================================================
/**
 * @brief  Handle CWD command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_CWD(SOCKET *socket)
{
        char *STATUS = "250 OK\r\n";

        DIR *dir = opendir(global->params);
        if (dir) {
                strlcpy(global->PWD, global->params, sizeof(global->PWD));
                chdir(global->PWD);
                closedir(dir);
        } else {
                STATUS = "550 No such directory\r\n";
        }

        socket_send(socket, STATUS, strlen(STATUS), NET_FLAGS__NOCOPY);
        VERBOSE(STATUS);
}

//==============================================================================
/**
 * @brief  Handle MKD command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_MKD(SOCKET *socket)
{
        if (mkdir(global->params, 0777) == 0) {
                static const char *OK = "250 Directory created\r\n";
                socket_send(socket, OK, strlen(OK), NET_FLAGS__NOCOPY);
                VERBOSE(OK);
        } else {
                static const char *ERR = "550 Failed\r\n";
                socket_send(socket, ERR, strlen(ERR), NET_FLAGS__NOCOPY);
                VERBOSE(ERR);
        }
}

//==============================================================================
/**
 * @brief  Handle DELE and RMD commands.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_DELE_RMD(SOCKET *socket)
{
        if (remove(global->params) == 0) {
                static const char *OK = "250 File removed\r\n";
                socket_send(socket, OK, strlen(OK), NET_FLAGS__NOCOPY);
                VERBOSE(OK);
        } else {
                static const char *ERR = "550 Failed\r\n";
                socket_send(socket, ERR, strlen(ERR), NET_FLAGS__NOCOPY);
                VERBOSE(ERR);
        }
}

//==============================================================================
/**
 * @brief  Handle SIZE command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_SIZE(SOCKET *socket)
{
        struct stat st;
        if (stat(global->params, &st) == 0) {
                snprintf(global->buf, sizeof(global->buf),
                         "250 %u\r\n", cast(uint, st.st_size));
        } else {
                strcpy(global->buf, "550 No such file.\r\n");
        }

        socket_send(socket, global->buf, strlen(global->buf), NET_FLAGS__NOCOPY);
        VERBOSE(global->buf);
}

//==============================================================================
/**
 * @brief  Handle STOR and APPE commands.
 * @param  socket       Communication socket.
 * @param  stor         true: STOR; false: APPE
 */
//==============================================================================
static void handle_cmd_STOR_APPE(SOCKET *socket, bool stor)
{
        static const char *MARK = "150 Receiving data...\r\n";
        socket_send(socket, MARK, strlen(MARK), NET_FLAGS__NOCOPY);
        VERBOSE(MARK);

        const char *STATUS = "226 Done\r\n";

        if (global->datasock) {

                int err = 0;

                if (global->passive_mode) {

                        SOCKET *dsock = NULL;
                        err = socket_accept(global->datasock, &dsock);
                        if (!err) {
                                if (receive_file(dsock, stor ? "w" : "a") != 0) {
                                        STATUS = "451 IO error\r\n";
                                }
                                socket_delete(dsock);
                        } else {
                                STATUS = "426 Connection timeout\r\n";
                        }

                } else {
                        err = socket_connect(global->datasock, &global->client_addr);
                        if (!err) {
                                if (receive_file(global->datasock, stor ? "w" : "a") != 0) {
                                        STATUS = "451 IO error\r\n";
                                }
                                socket_disconnect(global->datasock);
                        } else {
                                STATUS = "426 Connection timeout\r\n";
                        }
                }

                socket_delete(global->datasock);
                global->datasock = NULL;

                select_next_port();

        } else {
                STATUS = "425 No connection\r\n";
        }

        socket_send(socket, STATUS, strlen(STATUS), NET_FLAGS__NOCOPY);
        VERBOSE(STATUS);
}

//==============================================================================
/**
 * @brief  Handle RNFR command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_RNFR(SOCKET *socket)
{
        const char *STATUS = "350 Selected file.\r\n";

        FILE *f = fopen(global->params, "r");
        if (f) {
                if (global->RNFR) {
                        free(global->RNFR);
                }

                global->RNFR = malloc(strsize(global->params));
                if (global->RNFR) {
                        strcpy(global->RNFR, global->params);
                }

                fclose(f);
        } else {
                STATUS = "450 No such file\r\n";
        }

        socket_send(socket, STATUS, strlen(STATUS), NET_FLAGS__NOCOPY);
        VERBOSE(STATUS);
}

//==============================================================================
/**
 * @brief  Handle RNTO command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_RNTO(SOCKET *socket)
{
        const char *STATUS = "250 File renamed.\r\n";

        if (global->RNFR) {
                if (rename(global->RNFR, global->params) != 0) {
                        STATUS = "550 Rename error.\r\n";
                }

                free(global->RNFR);
                global->RNFR = NULL;
        } else {
                STATUS = "503 Select file.\r\n";
        }

        socket_send(socket, STATUS, strlen(STATUS), NET_FLAGS__NOCOPY);
        VERBOSE(STATUS);
}

//==============================================================================
/**
 * @brief  Handle RETR command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_RETR(SOCKET *socket)
{
        static const char *MARK = "150 Transmitting data...\r\n";
        socket_send(socket, MARK, strlen(MARK), NET_FLAGS__NOCOPY);
        VERBOSE(MARK);

        const char *STATUS = "226 Done\r\n";

        if (global->datasock) {

                int err = 0;

                if (global->passive_mode) {

                        SOCKET *dsock = NULL;
                        err = socket_accept(global->datasock, &dsock);
                        if (!err) {
                                if (transmit_file(dsock) != 0) {
                                        STATUS = "426 connection error\r\n";
                                }
                                socket_delete(dsock);
                        } else {
                                STATUS = "426 Connection timeout\r\n";
                        }

                } else {
                        err = socket_connect(global->datasock, &global->client_addr);
                        if (!err) {
                                if (transmit_file(global->datasock) != 0) {
                                        STATUS = "426 connection error\r\n";
                                }
                                socket_disconnect(global->datasock);
                        } else {
                                STATUS = "426 Connection timeout\r\n";
                        }
                }

                socket_delete(global->datasock);
                global->datasock = NULL;

                select_next_port();

        } else {
                STATUS = "425 No connection\r\n";
        }

        socket_send(socket, STATUS, strlen(STATUS), NET_FLAGS__NOCOPY);
        VERBOSE(STATUS);
}

//==============================================================================
/**
 * @brief  Handle ABOR command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_ABOR(SOCKET *socket)
{
        static const char *ABORT = "226 ABOR command successful\r\n";
        socket_send(socket, ABORT, strlen(ABORT), NET_FLAGS__NOCOPY);
        VERBOSE(ABORT);
}

//==============================================================================
/**
 * @brief  Handle MODE command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_MODE(SOCKET *socket)
{
        if (strcasecmp(global->params, "s") == 0) {
                static const char *OK = "200 OK\r\n";
                socket_send(socket, OK, strlen(OK), NET_FLAGS__NOCOPY);
                VERBOSE(OK);
        } else {
                static const char *ERR = "504 Rejected\r\n";
                socket_send(socket, ERR, strlen(ERR), NET_FLAGS__NOCOPY);
                VERBOSE(ERR);
        }
}

//==============================================================================
/**
 * @brief  Handle NOOP command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_NOOP(SOCKET *socket)
{
        static const char *OK = "200 OK\r\n";
        socket_send(socket, OK, strlen(OK), NET_FLAGS__NOCOPY);
        VERBOSE(OK);
}

//==============================================================================
/**
 * @brief  Handle REIN command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_REIN(SOCKET *socket)
{
        strlcpy(global->PWD, global->CWD_arg, sizeof(global->PWD));
        chdir(global->PWD);

        static const char *OK = "200 OK\r\n";
        socket_send(socket, OK, strlen(OK), NET_FLAGS__NOCOPY);
        VERBOSE(OK);
}

//==============================================================================
/**
 * @brief  Handle STAT command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_STAT(SOCKET *socket)
{
        static const char *REJECT = "450 Rejected\r\n";
        socket_send(socket, REJECT, strlen(REJECT), NET_FLAGS__NOCOPY);
        VERBOSE(REJECT);
}

//==============================================================================
/**
 * @brief  Handle STRU command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_STRU(SOCKET *socket)
{
        if (strcasecmp(global->params, "f") == 0) {
                static const char *OK = "200 OK\r\n";
                socket_send(socket, OK, strlen(OK), NET_FLAGS__NOCOPY);
                VERBOSE(OK);
        } else {
                static const char *ERR = "504 Rejected\r\n";
                socket_send(socket, ERR, strlen(ERR), NET_FLAGS__NOCOPY);
                VERBOSE(ERR);
        }
}

//==============================================================================
/**
 * @brief  Handle unknown command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_unknown(SOCKET *socket)
{
        static const char *UNKNOWN_CMD = "500 Unknown command\r\n";
        socket_send(socket, UNKNOWN_CMD, strlen(UNKNOWN_CMD), NET_FLAGS__NOCOPY);
        VERBOSE(UNKNOWN_CMD);
}

//==============================================================================
/**
 * @brief  Return number of command.
 * @return Command ID.
 */
//==============================================================================
static FTP_cmd_t get_cmd(void)
{
        static const char *CMD_STR[] = {
                [FTP_CMD__ABOR] = "ABOR",
                [FTP_CMD__CWD ] = "CWD",
                [FTP_CMD__DELE] = "DELE",
                [FTP_CMD__LIST] = "LIST",
                [FTP_CMD__MDTM] = "MDTM",
                [FTP_CMD__MKD ] = "MKD",
                [FTP_CMD__NLST] = "NLST",
                [FTP_CMD__PASS] = "PASS",
                [FTP_CMD__PASV] = "PASV",
                [FTP_CMD__PORT] = "PORT",
                [FTP_CMD__PWD ] = "PWD",
                [FTP_CMD__QUIT] = "QUIT",
                [FTP_CMD__RETR] = "RETR",
                [FTP_CMD__RMD ] = "RMD",
                [FTP_CMD__RNFR] = "RNFR",
                [FTP_CMD__RNTO] = "RNTO",
                [FTP_CMD__SITE] = "SITE",
                [FTP_CMD__SIZE] = "SIZE",
                [FTP_CMD__STOR] = "STOR",
                [FTP_CMD__TYPE] = "TYPE",
                [FTP_CMD__USER] = "USER",
                [FTP_CMD__ACCT] = "ACCT",
                [FTP_CMD__APPE] = "APPE",
                [FTP_CMD__CDUP] = "CDUP",
                [FTP_CMD__HELP] = "HELP",
                [FTP_CMD__MODE] = "MODE",
                [FTP_CMD__NOOP] = "NOOP",
                [FTP_CMD__REIN] = "REIN",
                [FTP_CMD__STAT] = "STAT",
                [FTP_CMD__STOU] = "STOU",
                [FTP_CMD__STRU] = "STRU",
                [FTP_CMD__SYST] = "SYST",
        };

        VERBOSE("> %.*s", global->bufsz, global->buf);

        global->params = global->buf;
        FTP_cmd_t cmd;
        for (cmd = FTP_CMD__ABOR; cmd < FTP_CMD__COUNT; cmd++) {
                size_t cmdlen = strlen(CMD_STR[cmd]);
                if (strncasecmp(CMD_STR[cmd], global->buf, cmdlen) == 0) {

                        for (int i = 0; i < global->bufsz; i++) {
                                if (  global->buf[i] == '\r'
                                   || global->buf[i] == '\n') {

                                        global->buf[i] = '\0';
                                        break;
                                }
                        }

                        global->params += cmdlen;

                        if (*global->params == ' ') {
                                global->params++;
                        }

                        break;
                }
        }

        return cmd;
}


//==============================================================================
/**
 * @brief  FTP server.
 * @param  socket       Connection socket.
 */
//==============================================================================
static void serve(SOCKET *socket)
{
        NET_INET_sockaddr_t addr;
        socket_get_address(socket, &addr);

        VERBOSE("New connection from: %d.%d.%d.%d\n",
                NET_INET_IPv4_a(addr.addr),
                NET_INET_IPv4_b(addr.addr),
                NET_INET_IPv4_c(addr.addr),
                NET_INET_IPv4_d(addr.addr));

        // send server features
        static const char *WELCOME = "220 Features: p.\r\n";
        socket_send(socket, WELCOME, strlen(WELCOME), NET_FLAGS__NOCOPY);

        global->quit = false;

        // receive command
        while (not global->quit) {
                errno = 0;
                global->bufsz = socket_recv(socket, global->buf, sizeof(global->buf),
                                            NET_FLAGS__FREEBUF);

                if (global->bufsz == -1) {
                        perror("FTP");
                        break;
                } else if (global->bufsz == 0) {
                        continue;
                }

                switch (get_cmd()) {
                case FTP_CMD__ABOR: handle_cmd_ABOR(socket); break;
                case FTP_CMD__CWD : handle_cmd_CWD(socket); break;
                case FTP_CMD__DELE: handle_cmd_DELE_RMD(socket); break;
                case FTP_CMD__RMD : handle_cmd_DELE_RMD(socket); break;
                case FTP_CMD__LIST: handle_cmd_LIST(socket); break;
                case FTP_CMD__MDTM: handle_cmd_unknown(socket); break;
                case FTP_CMD__MKD : handle_cmd_MKD(socket); break;
                case FTP_CMD__NLST: handle_cmd_unknown(socket); break;
                case FTP_CMD__PASS: handle_cmd_PASS(socket); break;
                case FTP_CMD__PASV: handle_cmd_PASV(socket); break;
                case FTP_CMD__PORT: handle_cmd_PORT(socket); break;
                case FTP_CMD__PWD : handle_cmd_PWD(socket); break;
                case FTP_CMD__QUIT: handle_cmd_QUIT(socket); break;
                case FTP_CMD__RETR: handle_cmd_RETR(socket); break;
                case FTP_CMD__RNFR: handle_cmd_RNFR(socket); break;
                case FTP_CMD__RNTO: handle_cmd_RNTO(socket); break;
                case FTP_CMD__SITE: handle_cmd_unknown(socket); break;
                case FTP_CMD__SIZE: handle_cmd_SIZE(socket); break;
                case FTP_CMD__STOR: handle_cmd_STOR_APPE(socket, true); break;
                case FTP_CMD__APPE: handle_cmd_STOR_APPE(socket, false); break;
                case FTP_CMD__STOU: handle_cmd_unknown(socket); break;
                case FTP_CMD__TYPE: handle_cmd_TYPE(socket); break;
                case FTP_CMD__USER: handle_cmd_USER(socket); break;
                case FTP_CMD__ACCT: handle_cmd_unknown(socket); break;
                case FTP_CMD__CDUP: handle_cmd_unknown(socket); break;
                case FTP_CMD__HELP: handle_cmd_unknown(socket); break;
                case FTP_CMD__MODE: handle_cmd_MODE(socket); break;
                case FTP_CMD__NOOP: handle_cmd_NOOP(socket); break;
                case FTP_CMD__REIN: handle_cmd_REIN(socket); break;
                case FTP_CMD__STAT: handle_cmd_STAT(socket); break;
                case FTP_CMD__STRU: handle_cmd_STRU(socket); break;
                case FTP_CMD__SYST: handle_cmd_SYST(socket); break;
                default: handle_cmd_unknown(socket); break;
                }

                global->bufsz = 0;
        }

        VERBOSE("Connection closed.\n");
        socket_delete(socket);
}

//==============================================================================
/**
 * @brief Program main function
 *
 * @param  argc         count of arguments
 * @param *argv[]       argument table
 *
 * @return program status
 */
//==============================================================================
int_main(ftpd, STACK_DEPTH_LOW, int argc, char *argv[])
{
        for (int i = 1; i < argc; i++) {
                if (isstreq(argv[i], "-v")) {
                        global->verbose = true;
                } else if (isstreq(argv[i], "-c")) {
                        global->CWD_arg = argv[i+1];
                }
        }

        if (!((global->CWD_arg != NULL) && (global->CWD_arg[0] == '/'))) {
                global->CWD_arg = "/";
        }

        global->dataport = FTP_PASV_PORT_START;

        static const NET_INET_sockaddr_t ADDR_ANY = {
            .addr = NET_INET_IPv4_ANY,
            .port = FTP_PORT
        };

        SOCKET *socket = socket_new(NET_FAMILY__INET, NET_PROTOCOL__TCP);
        if (socket) {
                if (socket_bind(socket, &ADDR_ANY) == 0) {
                        if (socket_listen(socket) == 0) {
                                int err;
                                do {
                                        SOCKET *new_socket;
                                        err = socket_accept(socket, &new_socket);
                                        if (!err) {
                                                strlcpy(global->PWD,
                                                        global->CWD_arg,
                                                        sizeof(global->PWD));

                                                chdir(global->PWD);

                                                serve(new_socket);
                                        }
                                } while (!err);
                        }
                }

                socket_delete(socket);
        }

        if (errno != 0) {
                perror("Socket error");
        }

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
