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
#include <dnx/thread.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define USER_NAME               "dnx"
#define PASSWORD                "dnx"
#define FTP_PORT                21
#define FTP_PASV_PORT_START     2121

#define TIMEOUT_SEND            5000
#define TIMEOUT_RECV            5000

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
        bool verbose;

        struct thread {
                tid_t       id;
                SOCKET     *socket;         // cmd channel
                SOCKET     *sock_pasv;      // data channel
                char       *RNFR;
                char       *cmd_args;
                uint16_t    data_port;
                char        CWD[256];
                char        buf[4096];
                int         bufsz;
                bool        quit;
        } thread[8];
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(ftpd, STACK_DEPTH_LOW);
static FTP_cmd_t get_cmd(struct thread *self);
static void handle_cmd_ABOR(struct thread *self);

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Send file list.
 * @param  socket       data socket where list to be sent.
 * @return Number of sent bytes.
 */
//==============================================================================
static int send_file_list(struct thread *self, SOCKET *socket)
{
        socket_set_send_timeout(socket, TIMEOUT_SEND);

        int n = 0;

        struct stat fsta;
        if (stat(self->cmd_args, &fsta) == 0) {

                char *name = malloc(strsize(self->cmd_args));
                if (name) {
                        strcpy(name, self->cmd_args);

                        struct tm tm;
                        localtime_r(&fsta.st_mtime, &tm);

                        int sz = snprintf(self->buf, sizeof(self->buf),
                                          "%crw-r--r-- 1 %s  %s %u ",
                                          (S_ISDIR(fsta.st_mode) ? 'd' : '-'),
                                          "root",
                                          "root",
                                          cast(uint, fsta.st_size));

                        sz += strftime(self->buf + sz, sizeof(self->buf) - sz,
                                       "%b %d %H:%M ", &tm);

                        sz += snprintf(self->buf + sz, sizeof(self->buf) - sz,
                                       "%s\n", name);

                        if (socket_send(socket, self->buf, sz, NET_FLAGS__COPY) == sz) {
                                VERBOSE(self->buf);
                                n += sz;
                        }

                        free(name);
                }

        } else {
                errno    = 0;
                DIR *dir = opendir(self->CWD);
                if (dir) {
                        dirent_t *dirent = NULL;
                        while ((dirent = readdir(dir))) {

                                memset(&fsta, 0, sizeof(struct stat));
                                stat(dirent->d_name, &fsta);

                                struct tm tm;
                                localtime_r(&fsta.st_mtime, &tm);

                                int sz = snprintf(self->buf, sizeof(self->buf),
                                                  "%crw-r--r-- 1 %s %s %u ",
                                                  (S_ISDIR(dirent->mode) ? 'd' : '-'),
                                                  "root",
                                                  "root",
                                                  cast(uint, dirent->size));

                                sz += strftime(self->buf + sz, sizeof(self->buf) - sz,
                                               "%b %d %Y ", &tm);

                                sz += snprintf(self->buf + sz, sizeof(self->buf) - sz,
                                               "%s\n", dirent->d_name);

                                if (socket_send(socket, self->buf, sz, NET_FLAGS__COPY) == sz) {
                                        VERBOSE(self->buf);
                                        n += sz;
                                } else {
                                        break;
                                }
                        }

                        closedir(dir);

                } else {
                        perror(self->CWD);
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
static int receive_file(struct thread *self, const char *mode, SOCKET *socket)
{
        socket_set_recv_timeout(socket, TIMEOUT_RECV);

        int   err  = 1;
        FILE *file = fopen(self->cmd_args, mode);
        if (file) {

                int sz;
                do {
                        sz = socket_read(socket, self->buf, sizeof(self->buf));
                        if (sz > 0) {
                                if (fwrite(self->buf, 1, sz, file) != (size_t)sz) {
                                        VERBOSE("File write error %d", errno);
                                        break;
                                }
                        } else {
                                if (errno == ECONNABORTED) {
                                        VERBOSE("Connection aborted - OK\n");
                                        err = 0;

                                } else {
                                        VERBOSE("Connection error %d\n", errno);
                                }

                                break;
                        }

                } while (sz > 0);

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
static int transmit_file(struct thread *self, SOCKET *socket)
{
        socket_set_send_timeout(socket, TIMEOUT_SEND);

        int   err  = 1;
        FILE *file = fopen(self->cmd_args, "r");
        if (file) {
                struct stat stat;
                err = fstat(fileno(file), &stat);

                while (!err && (stat.st_size > 0)) {

                        int len = fread(self->buf, 1, sizeof(self->buf), file);
                        if (len < 0) {
                                VERBOSE("File read error %d\n", errno);
                                break;

                        } else if (len == 0) {
                                VERBOSE("File end reached\n");
                                break;
                        }

                        stat.st_size -= len;

                        NET_flags_t flags = NET_FLAGS__COPY;

                        if (stat.st_size) {
                                flags |= NET_FLAGS__MORE;
                        }

                        int n = socket_send(socket, self->buf, len, flags);

                        if (n < 0) {
                                VERBOSE("Socket write error %d\n", errno);
                        }

                        err = (n != len);
                }

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
static void handle_cmd_USER(struct thread *self)
{
        if (isstreq(self->cmd_args, USER_NAME)) {
                static const char *PASS_NEEDED  = "331 Enter user password\r\n";
                socket_send(self->socket, PASS_NEEDED, strlen(PASS_NEEDED), NET_FLAGS__COPY);
                VERBOSE(PASS_NEEDED);
        } else {
                static const char *UNKNOWN_USER = "530 Unknown user\r\n";
                socket_send(self->socket, UNKNOWN_USER, strlen(UNKNOWN_USER), NET_FLAGS__COPY);
                VERBOSE(UNKNOWN_USER);
        }
}

//==============================================================================
/**
 * @brief  Handle PASS command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_PASS(struct thread *self)
{
        if (isstreq(self->cmd_args, PASSWORD)) {
                static const char *PASS_OK  = "230 Hello!\r\n";
                socket_send(self->socket, PASS_OK, strlen(PASS_OK), NET_FLAGS__COPY);
                VERBOSE(PASS_OK);
        } else {
                static const char *PASS_NOK = "530 Incorrect password!\r\n";
                socket_send(self->socket, PASS_NOK, strlen(PASS_NOK), NET_FLAGS__COPY);
                VERBOSE(PASS_NOK);
        }
}

//==============================================================================
/**
 * @brief  Handle QUIT command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_QUIT(struct thread *self)
{
        static const char *BYE = "221 Bye\r\n";
        socket_send(self->socket, BYE, strlen(BYE), NET_FLAGS__COPY);
        VERBOSE(BYE);

        self->quit = true;
}


//==============================================================================
/**
 * @brief  Handle SYST command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_SYST(struct thread *self)
{
        static const char *TYPE = "215 UNIX Type: L8\r\n";
        socket_send(self->socket, TYPE, strlen(TYPE), NET_FLAGS__COPY);
        VERBOSE(TYPE);
}

//==============================================================================
/**
 * @brief  Handle PWD command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_PWD(struct thread *self)
{
        size_t PATH_LEN = 256;
        char *path = malloc(PATH_LEN);
        if (path) {
                int n = snprintf(path, PATH_LEN, "257 \"%s\"\r\n", self->CWD);
                socket_send(self->socket, path, n, NET_FLAGS__COPY);
                VERBOSE(path);
                free(path);
        } else {
                static const char *PWD_EMPTY = "\"/\"\r\n";
                socket_send(self->socket, PWD_EMPTY, strlen(PWD_EMPTY), NET_FLAGS__COPY);
                VERBOSE(PWD_EMPTY);
        }
}

//==============================================================================
/**
 * @brief  Handle TYPE command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_TYPE(struct thread *self)
{
        if (isstreq(self->cmd_args, "I") || isstreq(self->cmd_args, "L 8")) {
                static const char *ACCEPT = "200 Binary mode ON\r\n";
                socket_send(self->socket, ACCEPT, strlen(ACCEPT), NET_FLAGS__COPY);
                VERBOSE(ACCEPT);
        } else {
                static const char *REJECT = "504 Use binary mode\r\n";
                socket_send(self->socket, REJECT, strlen(REJECT), NET_FLAGS__COPY);
                VERBOSE(REJECT);
        }
}

//==============================================================================
/**
 * @brief  Handle PASV command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_PASV(struct thread *self)
{
        NET_INET_status_t status;
        memset(&status, 0, sizeof(status));
        NET_family_t family;

        if (ifstatus("inet", &family, &status) == 0) {

                int n = snprintf(self->buf, sizeof(self->buf),
                                 "227 =%d,%d,%d,%d,%d,%d\r\n",
                                 NET_INET_IPv4_a(status.address),
                                 NET_INET_IPv4_b(status.address),
                                 NET_INET_IPv4_c(status.address),
                                 NET_INET_IPv4_d(status.address),
                                 (self->data_port >> 8) & 0xFF,
                                 (self->data_port & 0xFF) );

                socket_send(self->socket, self->buf, n, NET_FLAGS__COPY);
                VERBOSE(self->buf);

        } else {
                static const char *REJECT = "504 Not supported\r\n";
                socket_send(self->socket, REJECT, strlen(REJECT), NET_FLAGS__COPY);
                VERBOSE(REJECT);
        }
}

//==============================================================================
/**
 * @brief  Handle PORT command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_PORT(struct thread *self)
{
        static const char *ERR = "504 Not supported\r\n";
        socket_send(self->socket, ERR, strlen(ERR), NET_FLAGS__COPY);
        VERBOSE(ERR);
}

//==============================================================================
/**
 * @brief  Handle LIST command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_LIST(struct thread *self)
{
        static const char *MARK = "150 Accepted data connection...\r\n";
        socket_send(self->socket, MARK, strlen(MARK), NET_FLAGS__COPY);
        VERBOSE(MARK);

        const char *STATUS = "226 Done\r\n";

        if (self->sock_pasv) {

                int err = 0;

                SOCKET *dsock = NULL;
                err = socket_accept(self->sock_pasv, &dsock);
                if (!err) {
                        send_file_list(self, dsock);
                        socket_close(dsock);
                }

                if (err) {
                        STATUS = "426 Connection timeout\r\n";
                }

        } else {
                STATUS = "425 No connection\r\n";
        }

        socket_send(self->socket, STATUS, strlen(STATUS), NET_FLAGS__COPY);
        VERBOSE(STATUS);
}

//==============================================================================
/**
 * @brief  Handle CWD command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_CWD(struct thread *self)
{
        char *STATUS = "250 OK\n";

        DIR *dir = opendir(self->cmd_args);
        if (dir) {
                strlcpy(self->CWD, self->cmd_args, sizeof(self->CWD));
                closedir(dir);
        } else {
                STATUS = "550 No such directory\r\n";
        }

        socket_send(self->socket, STATUS, strlen(STATUS), NET_FLAGS__COPY);
        VERBOSE(STATUS);
}

//==============================================================================
/**
 * @brief  Handle MKD command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_MKD(struct thread *self)
{
        if (mkdir(self->cmd_args, 0777) == 0) {
                static const char *OK = "250 Directory created\r\n";
                socket_send(self->socket, OK, strlen(OK), NET_FLAGS__COPY);
                VERBOSE(OK);
        } else {
                static const char *ERR = "550 Failed\r\n";
                socket_send(self->socket, ERR, strlen(ERR), NET_FLAGS__COPY);
                VERBOSE(ERR);
        }
}

//==============================================================================
/**
 * @brief  Handle DELE and RMD commands.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_DELE_RMD(struct thread *self)
{
        if (remove(self->cmd_args) == 0) {
                static const char *OK = "250 File removed\r\n";
                socket_send(self->socket, OK, strlen(OK), NET_FLAGS__COPY);
                VERBOSE(OK);
        } else {
                static const char *ERR = "550 Failed\r\n";
                socket_send(self->socket, ERR, strlen(ERR), NET_FLAGS__COPY);
                VERBOSE(ERR);
        }
}

//==============================================================================
/**
 * @brief  Handle SIZE command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_SIZE(struct thread *self)
{
        struct stat st;
        if (stat(self->cmd_args, &st) == 0) {
                snprintf(self->buf, sizeof(self->buf),
                         "250 %u\r\n", cast(uint, st.st_size));
        } else {
                strcpy(self->buf, "550 No such file.\r\n");
        }

        socket_send(self->socket, self->buf, strlen(self->buf), NET_FLAGS__COPY);
        VERBOSE(self->buf);
}

//==============================================================================
/**
 * @brief  Handle STOR and APPE commands.
 * @param  socket       Communication socket.
 * @param  stor         true: STOR; false: APPE
 */
//==============================================================================
static void handle_cmd_STOR_APPE(struct thread *self, bool stor)
{
        static const char *MARK = "150 Receiving data...\r\n";
        socket_send(self->socket, MARK, strlen(MARK), NET_FLAGS__COPY);
        VERBOSE(MARK);

        const char *STATUS = "226 Done\r\n";

        if (self->sock_pasv) {

                int err = 0;

                SOCKET *dsock = NULL;
                err = socket_accept(self->sock_pasv, &dsock);
                if (!err) {
                        if (receive_file(self, stor ? "w" : "a", dsock) != 0) {
                                STATUS = "451 IO error\r\n";
                        }
                        socket_close(dsock);
                } else {
                        STATUS = "426 Connection timeout\r\n";
                }

        } else {
                STATUS = "425 No connection\r\n";
        }

        socket_send(self->socket, STATUS, strlen(STATUS), NET_FLAGS__COPY);
        VERBOSE(STATUS);
}

//==============================================================================
/**
 * @brief  Handle RNFR command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_RNFR(struct thread *self)
{
        const char *STATUS = "350 Selected file.\r\n";

        FILE *f = fopen(self->cmd_args, "r");
        if (f) {
                if (self->RNFR) {
                        free(self->RNFR);
                }

                self->RNFR = malloc(strsize(self->cmd_args));
                if (self->RNFR) {
                        strcpy(self->RNFR, self->cmd_args);
                }

                fclose(f);
        } else {
                STATUS = "450 No such file\r\n";
        }

        socket_send(self->socket, STATUS, strlen(STATUS), NET_FLAGS__COPY);
        VERBOSE(STATUS);
}

//==============================================================================
/**
 * @brief  Handle RNTO command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_RNTO(struct thread *self)
{
        const char *STATUS = "250 File renamed.\r\n";

        if (self->RNFR) {
                if (rename(self->RNFR, self->cmd_args) != 0) {
                        STATUS = "550 Rename error.\r\n";
                }

                free(self->RNFR);
                self->RNFR = NULL;
        } else {
                STATUS = "503 Select file.\r\n";
        }

        socket_send(self->socket, STATUS, strlen(STATUS), NET_FLAGS__COPY);
        VERBOSE(STATUS);
}

//==============================================================================
/**
 * @brief  Handle RETR command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_RETR(struct thread *self)
{
        static const char *MARK = "150 Transmitting data...\r\n";
        socket_send(self->socket, MARK, strlen(MARK), NET_FLAGS__COPY);
        VERBOSE(MARK);

        const char *STATUS = "226 Done\r\n";

        if (self->sock_pasv) {

                int err = 0;

                SOCKET *dsock = NULL;
                err = socket_accept(self->sock_pasv, &dsock);
                if (!err) {
                        if (transmit_file(self, dsock) != 0) {
                                STATUS = "426 connection error\r\n";
                        }
                        socket_close(dsock);
                } else {
                        STATUS = "426 Connection timeout\r\n";
                }

        } else {
                STATUS = "425 No connection\r\n";
        }

        socket_send(self->socket, STATUS, strlen(STATUS), NET_FLAGS__COPY);
        VERBOSE(STATUS);
}

//==============================================================================
/**
 * @brief  Handle ABOR command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_ABOR(struct thread *self)
{
        static const char *ABORT = "226 ABOR command successful\r\n";
        socket_send(self->socket, ABORT, strlen(ABORT), NET_FLAGS__COPY);
        VERBOSE(ABORT);
}

//==============================================================================
/**
 * @brief  Handle MODE command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_MODE(struct thread *self)
{
        if (strcasecmp(self->cmd_args, "s") == 0) {
                static const char *OK = "200 OK\r\n";
                socket_send(self->socket, OK, strlen(OK), NET_FLAGS__COPY);
                VERBOSE(OK);
        } else {
                static const char *ERR = "504 Rejected\r\n";
                socket_send(self->socket, ERR, strlen(ERR), NET_FLAGS__COPY);
                VERBOSE(ERR);
        }
}

//==============================================================================
/**
 * @brief  Handle NOOP command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_NOOP(struct thread *self)
{
        static const char *OK = "200 OK\r\n";
        socket_send(self->socket, OK, strlen(OK), NET_FLAGS__COPY);
        VERBOSE(OK);
}

//==============================================================================
/**
 * @brief  Handle REIN command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_REIN(struct thread *self)
{
//        strlcpy(self->CWD, self->CWD_arg, sizeof(global->PWD));
//        chdir_(global->PWD);

        static const char *OK = "200 OK\r\n";
        socket_send(self->socket, OK, strlen(OK), NET_FLAGS__COPY);
        VERBOSE(OK);
}

//==============================================================================
/**
 * @brief  Handle STAT command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_STAT(struct thread *self)
{
        static const char *REJECT = "450 Rejected\r\n";
        socket_send(self->socket, REJECT, strlen(REJECT), NET_FLAGS__COPY);
        VERBOSE(REJECT);
}

//==============================================================================
/**
 * @brief  Handle STRU command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_STRU(struct thread *self)
{
        if (strcasecmp(self->cmd_args, "f") == 0) {
                static const char *OK = "200 OK\r\n";
                socket_send(self->socket, OK, strlen(OK), NET_FLAGS__COPY);
                VERBOSE(OK);
        } else {
                static const char *ERR = "504 Rejected\r\n";
                socket_send(self->socket, ERR, strlen(ERR), NET_FLAGS__COPY);
                VERBOSE(ERR);
        }
}

//==============================================================================
/**
 * @brief  Handle unknown command.
 * @param  socket       Communication socket.
 */
//==============================================================================
static void handle_cmd_unknown(struct thread *self)
{
        static const char *UNKNOWN_CMD = "500 Unknown command\r\n";
        socket_send(self->socket, UNKNOWN_CMD, strlen(UNKNOWN_CMD), NET_FLAGS__COPY);
        VERBOSE(UNKNOWN_CMD);
}

//==============================================================================
/**
 * @brief  Return number of command.
 * @return Command ID.
 */
//==============================================================================
static FTP_cmd_t get_cmd(struct thread *self)
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

        VERBOSE("> %.*s", self->bufsz, self->buf);

        self->cmd_args = self->buf;
        FTP_cmd_t cmd;
        for (cmd = FTP_CMD__ABOR; cmd < FTP_CMD__COUNT; cmd++) {
                size_t cmdlen = strlen(CMD_STR[cmd]);
                if (strncasecmp(CMD_STR[cmd], self->buf, cmdlen) == 0) {

                        for (int i = 0; i < self->bufsz; i++) {
                                if (  self->buf[i] == '\r'
                                   || self->buf[i] == '\n') {

                                        self->buf[i] = '\0';
                                        break;
                                }
                        }

                        self->cmd_args += cmdlen;

                        if (*self->cmd_args == ' ') {
                                self->cmd_args++;
                        }

                        break;
                }
        }

        return cmd;
}


//==============================================================================
/**
 * @brief  FTP server.
 * @param  arg          thread self pointer
 */
//==============================================================================
static int serve(void *arg)
{
        msleep(100);

        struct thread *self = arg;

        VERBOSE("Thread-%u\n", self->id);

        strlcpy(self->CWD, "/", sizeof(self->CWD));
        memset(self->buf, 0, sizeof(self->buf));
        self->data_port = FTP_PASV_PORT_START + self->id - 1;
        self->cmd_args = NULL;
        self->bufsz = 0;
        self->quit  = false;

        if (!self->sock_pasv) {
                self->sock_pasv = socket_open("inet", NET_PROTOCOL__TCP);
                if (self->sock_pasv) {
                        NET_INET_sockaddr_t ADDR_ANY = {
                                .addr = NET_INET_IPv4_ANY,
                                .port = self->data_port
                        };

                        int err = socket_bind(self->sock_pasv, &ADDR_ANY);
                        if (!err) {
                                err = socket_listen(self->sock_pasv);
                        }

                        if (err) {
                                VERBOSE("PASV socket error: %s\n", strerror(errno));
                                socket_close(self->sock_pasv);
                                self->sock_pasv = NULL;
                                goto finish;
                        }
                }
        }


        NET_INET_sockaddr_t addr;
        socket_get_address(self->socket, &addr);

        VERBOSE("New connection from: %d.%d.%d.%d\n",
                NET_INET_IPv4_a(addr.addr),
                NET_INET_IPv4_b(addr.addr),
                NET_INET_IPv4_c(addr.addr),
                NET_INET_IPv4_d(addr.addr));

        // send server features
        static const char *WELCOME = "220 Service ready\r\n";
        socket_send(self->socket, WELCOME, strlen(WELCOME), NET_FLAGS__COPY);

        // receive command
        while (not self->quit) {
                errno = 0;
                self->bufsz = socket_recv(self->socket, self->buf, sizeof(self->buf),
                                          NET_FLAGS__FREEBUF);

                if (self->bufsz == -1) {
                        VERBOSE("CMD stream: %s\n", strerror(errno));
                        break;
                } else if (self->bufsz == 0) {
                        continue;
                }

                switch (get_cmd(self)) {
                case FTP_CMD__ABOR: handle_cmd_ABOR(self); break;
                case FTP_CMD__CWD : handle_cmd_CWD(self); break;
                case FTP_CMD__DELE: handle_cmd_DELE_RMD(self); break;
                case FTP_CMD__RMD : handle_cmd_DELE_RMD(self); break;
                case FTP_CMD__LIST: handle_cmd_LIST(self); break;
                case FTP_CMD__MDTM: handle_cmd_unknown(self); break;
                case FTP_CMD__MKD : handle_cmd_MKD(self); break;
                case FTP_CMD__NLST: handle_cmd_unknown(self); break;
                case FTP_CMD__PASS: handle_cmd_PASS(self); break;
                case FTP_CMD__PASV: handle_cmd_PASV(self); break;
                case FTP_CMD__PORT: handle_cmd_PORT(self); break;
                case FTP_CMD__PWD : handle_cmd_PWD(self); break;
                case FTP_CMD__QUIT: handle_cmd_QUIT(self); break;
                case FTP_CMD__RETR: handle_cmd_RETR(self); break;
                case FTP_CMD__RNFR: handle_cmd_RNFR(self); break;
                case FTP_CMD__RNTO: handle_cmd_RNTO(self); break;
                case FTP_CMD__SITE: handle_cmd_unknown(self); break;
                case FTP_CMD__SIZE: handle_cmd_SIZE(self); break;
                case FTP_CMD__STOR: handle_cmd_STOR_APPE(self, true); break;
                case FTP_CMD__APPE: handle_cmd_STOR_APPE(self, false); break;
                case FTP_CMD__STOU: handle_cmd_unknown(self); break;
                case FTP_CMD__TYPE: handle_cmd_TYPE(self); break;
                case FTP_CMD__USER: handle_cmd_USER(self); break;
                case FTP_CMD__ACCT: handle_cmd_unknown(self); break;
                case FTP_CMD__CDUP: handle_cmd_unknown(self); break;
                case FTP_CMD__HELP: handle_cmd_unknown(self); break;
                case FTP_CMD__MODE: handle_cmd_MODE(self); break;
                case FTP_CMD__NOOP: handle_cmd_NOOP(self); break;
                case FTP_CMD__REIN: handle_cmd_REIN(self); break;
                case FTP_CMD__STAT: handle_cmd_STAT(self); break;
                case FTP_CMD__STRU: handle_cmd_STRU(self); break;
                case FTP_CMD__SYST: handle_cmd_SYST(self); break;
                default: handle_cmd_unknown(self); break;
                }

                self->bufsz = 0;
        }

        if (self->RNFR) {
                free(self->RNFR);
                self->RNFR = NULL;
        }

        finish:
        VERBOSE("Connection closed.\n");
        socket_close(self->socket);
        self->id = 0;

        return 0;
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
int main(int argc, char *argv[])
{
        for (int i = 1; i < argc; i++) {
                if (isstreq(argv[i], "-v")) {
                        global->verbose = true;
                } else if (isstreq(argv[i], "-c")) {
//                        chdir(argv[i+1]);
                }
        }

        SOCKET *socket = socket_open("inet", NET_PROTOCOL__TCP);
        if (socket) {
                static const NET_INET_sockaddr_t ADDR_ANY = {
                        .addr = NET_INET_IPv4_ANY,
                        .port = FTP_PORT
                };

                if (socket_bind(socket, &ADDR_ANY) == 0) {

                        if (socket_listen(socket) == 0) {
                                int err;
                                do {
                                        SOCKET *new_socket;
                                        err = socket_accept(socket, &new_socket);
                                        if (!err) {

                                                bool started = false;

                                                for (size_t i = 0; i < ARRAY_SIZE(global->thread); i++) {

                                                        if (global->thread[i].id == 0) {

                                                                thread_attr_t attr;
                                                                attr.detached = false;
                                                                attr.priority = 0;
                                                                attr.stack_depth = STACK_DEPTH_MEDIUM;

                                                                global->thread[i].id = thread_create(serve, &attr, &global->thread[i]);
                                                                if (global->thread[i].id) {
                                                                        global->thread[i].socket = new_socket;
                                                                        started = true;
                                                                }

                                                                break;
                                                        }
                                                }

                                                if (!started) {
                                                        VERBOSE("No empty connection slots\n");
                                                        socket_close(new_socket);
                                                }
                                        }
                                } while (!err);
                        }
                }

                socket_close(socket);
        }

        for (size_t i = 0; i < ARRAY_SIZE(global->thread); i++) {
                thread_cancel(global->thread[i].id);
        }

        if (errno != 0) {
                perror("Socket error");
        }

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
