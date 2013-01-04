/*=============================================================================================*//**
@file    terminal.c

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==============================================================================================*/

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "terminal.h"
#include <string.h>
#include "tty_def.h"
#include "ds1307_def.h"

/* Begin of application section declaration */
APPLICATION(terminal)
APP_SEC_BEGIN

/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
/** user prompt line size [B] */
#define PROMPT_LINE_SIZE            100

#define CD_PATH_SIZE                128

/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
typedef enum
{
      CMD_EXECUTED,
      CMD_NOT_EXIST,
      CMD_ALLOC_ERROR,
      CMD_EXIT,
} cmdStatus_t;

typedef struct {
      const ch_t *name;
      cmdStatus_t (*const cmd)(ch_t *arg);
} cmd_t;


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
ch_t *cdpath;


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Exit from terminal
 */
//================================================================================================//
cmdStatus_t cmdEXIT(ch_t *arg)
{
      (void)arg;

      return CMD_EXIT;
}


//================================================================================================//
/**
 * @brief Echo command
 */
//================================================================================================//
cmdStatus_t cmdECHO(ch_t *arg)
{
      if (arg)
            printf("%s\n", arg);
      else
            printf("\n");

      return CMD_EXECUTED;
}


//================================================================================================//
/**
 * @brief Function show stack usage
 */
//================================================================================================//
cmdStatus_t cmdSTACK(ch_t *arg)
{
      (void)arg;

      printf("Free stack: %d\n", SystemGetStackFreeSpace());

      return CMD_EXECUTED;
}


//================================================================================================//
/**
 * @brief Clear terminal
 */
//================================================================================================//
cmdStatus_t cmdCLEAR(ch_t *arg)
{
      (void)arg;

      ioctl(stdout, TTY_IORQ_CLEARSCR, NULL);

      return CMD_EXECUTED;
}


//================================================================================================//
/**
 * @brief Reboot system
 */
//================================================================================================//
cmdStatus_t cmdREBOOT(ch_t *arg)
{
      (void)arg;

      printf("Rebooting...\n");
      Sleep(500);

      SystemReboot();

      return CMD_EXECUTED;
}


//================================================================================================//
/**
 * @brief Function go to selected dir
 */
//================================================================================================//
cmdStatus_t cmdCD(ch_t *arg)
{
      ch_t  *newpath  = NULL;
      DIR_t *dir      = NULL;
      bool_t freePath = FALSE;

      if (strcmp(arg, "..") == 0) {
            ch_t *lastslash = strrchr(cdpath, '/');

            if (lastslash) {
                  if (lastslash != cdpath) {
                        *lastslash = '\0';
                  } else {
                        *(lastslash + 1) = '\0';
                  }
            }
      } else if (arg[0] != '/') {
            newpath = calloc(strlen(arg) + strlen(cdpath) + 2, sizeof(cdpath[0]));

            if (newpath) {
                  strcpy(newpath, cdpath);

                  if (newpath[strlen(newpath) - 1] != '/') {
                        newpath[strlen(newpath)] = '/';
                  }

                  strcat(newpath, arg);

                  freePath = TRUE;
            }
      } else if (arg[0] == '/') {
            newpath = arg;
      } else {
            printf("No such directory\n");
      }

      if (newpath) {
            dir = opendir(newpath);

            if (dir) {
                  closedir(dir);

                  strncpy(cdpath, newpath, CD_PATH_SIZE);
            } else {
                  printf("No such directory\n");
            }

            if (freePath)
                  free(newpath);
      }

      return CMD_EXECUTED;
}


//================================================================================================//
/**
 * @brief Function listing files in selected directory
 */
//================================================================================================//
cmdStatus_t cmdLS(ch_t *arg)
{
      ch_t  *newpath  = NULL;
      bool_t freePath = FALSE;

      if (arg) {
            if (strcmp(".", arg) == 0) {
                  arg++;
            } else if (strncmp("./", arg, 2) == 0) {
                  arg += 2;
            }

            if (arg[0] == '/') {
                  newpath = arg;
            } else {
                  newpath = calloc(strlen(arg) + strlen(cdpath) + 2, sizeof(cdpath[0]));

                  if (newpath) {
                        strcpy(newpath, cdpath);

                        if (newpath[strlen(newpath) - 1] != '/') {
                              newpath[strlen(newpath)] = '/';
                        }

                        strcat(newpath, arg);

                        freePath = TRUE;
                  }
            }
      } else {
            newpath = cdpath;
      }

      DIR_t *dir = opendir(newpath);

      if (dir) {
            dirent_t dirent;

            ch_t *ccolor = "\x1B[33mc";
            ch_t *rcolor = "\x1B[35m-";
            ch_t *lcolor = "\x1B[36ml";
            ch_t *dcolor = "\x1B[32md";

            printf("Total %u\n", dir->items);

            while ((dirent = readdir(dir)).name != NULL) {
                  ch_t *type = NULL;

                  switch (dirent.filetype) {
                  case FILE_TYPE_DIR:     type = dcolor; break;
                  case FILE_TYPE_DRV:     type = ccolor; break;
                  case FILE_TYPE_LINK:    type = lcolor; break;
                  case FILE_TYPE_REGULAR: type = rcolor; break;
                  default: type = "?";
                  }

                  printf("%s %u\t%s\x1B[0m\n", type, dirent.size, dirent.name);
            }

            closedir(dir);
      } else {
            printf("No such directory\n");
      }

      if (freePath)
            free(newpath);

      return CMD_EXECUTED;
}


//================================================================================================//
/**
 * @brief Function listing files in selected directory
 */
//================================================================================================//
cmdStatus_t cmdMKDIR(ch_t *arg)
{
      ch_t  *newpath  = NULL;
      bool_t freePath = FALSE;

      if (arg) {
            if (arg[0] == '/') {
                  newpath = arg;
            } else {
                  newpath = calloc(strlen(arg) + strlen(cdpath) + 2, sizeof(cdpath[0]));

                  if (newpath) {
                        strcpy(newpath, cdpath);

                        if (newpath[strlen(newpath) - 1] != '/') {
                              newpath[strlen(newpath)] = '/';
                        }

                        strcat(newpath, arg);

                        freePath = TRUE;
                  }
            }
      } else {
            newpath = cdpath;
      }

      if (mkdir(newpath) == STD_RET_ERROR) {
            printf("Cannot create directory \"%s\": no such file or directory\n", arg);
      }

      if (freePath)
            free(newpath);

      return CMD_EXECUTED;
}


//================================================================================================//
/**
 * @brief Function remove file
 */
//================================================================================================//
cmdStatus_t cmdRM(ch_t *arg)
{
      ch_t  *newpath  = NULL;
      bool_t freePath = FALSE;

      if (arg) {
            if (arg[0] == '/') {
                  newpath = arg;
            } else {
                  newpath = calloc(strlen(arg) + strlen(cdpath) + 2, sizeof(cdpath[0]));

                  if (newpath) {
                        strcpy(newpath, cdpath);

                        if (newpath[strlen(newpath) - 1] != '/') {
                              newpath[strlen(newpath)] = '/';
                        }

                        strcat(newpath, arg);

                        freePath = TRUE;
                  }
            }
      } else {
            newpath = cdpath;
      }

      if (remove(newpath) == STD_RET_ERROR) {
            printf("Cannot remove \"%s\"\n", arg);
      }

      if (freePath)
            free(newpath);

      return CMD_EXECUTED;
}


//================================================================================================//
/**
 * @brief Function touch file
 */
//================================================================================================//
cmdStatus_t cmdTOUCH(ch_t *arg)
{
      ch_t  *newpath  = NULL;
      bool_t freePath = FALSE;

      if (arg) {
            if (arg[0] == '/') {
                  newpath = arg;
            } else {
                  newpath = calloc(strlen(arg) + strlen(cdpath) + 2, sizeof(cdpath[0]));

                  if (newpath) {
                        strcpy(newpath, cdpath);

                        if (newpath[strlen(newpath) - 1] != '/') {
                              newpath[strlen(newpath)] = '/';
                        }

                        strcat(newpath, arg);

                        freePath = TRUE;
                  }
            }
      } else {
            newpath = cdpath;
      }

      FILE_t *file = fopen(newpath, "a+");

      if (file) {
            fclose(file);
      } else {
            printf("Cannot touch \"%s\"\n", arg);
      }

      if (freePath)
            free(newpath);

      return CMD_EXECUTED;
}


//================================================================================================//
/**
 * @brief Function show free memory
 */
//================================================================================================//
cmdStatus_t cmdFREE(ch_t *arg)
{
      (void)arg;

      u32_t free = SystemGetFreeMemSize();
      u32_t used = SystemGetUsedMemSize();

      printf("Total: %d\n", SystemGetMemSize());
      printf("Free : %d\n", free);
      printf("Used : %d\n", used);
      printf("Memory usage: %d%%\n", (used * 100)/SystemGetMemSize());

      return CMD_EXECUTED;
}


//================================================================================================//
/**
 * @brief Function show uptime
 */
//================================================================================================//
cmdStatus_t cmdUPTIME(ch_t *arg)
{
      (void) arg;

      bcdTime_t time = {0x00, 0x00, 0x00};

      FILE_t *frtc = fopen("/dev/rtc", "r");

      if (frtc) {
            ioctl(frtc, RTC_IORQ_GETTIME, &time);
            fclose(frtc);
      } else {
            printf("Unable to open \"/dev/rtc\" file!\n");
      }

      u32_t uptime = SystemGetUptime();
      u32_t udays  = (uptime / (3600 * 24));
      u32_t uhrs   = (uptime / 3600) % 24;
      u32_t umins  = (uptime / 60) % 60;

      printf("%x2:%x2:%x2, up %ud %u2:%u2\n",
             time.hours, time.minutes, time.seconds,
             udays, uhrs, umins);

      return CMD_EXECUTED;
}


//================================================================================================//
/**
 * @brief Function show mounted filesystems
 */
//================================================================================================//
cmdStatus_t cmdDF(ch_t *arg)
{
      (void)arg;

      struct vfs_mntent mnt;
      mnt.mnt_dir    = calloc(64, ARRAY_ITEM_SIZE(mnt.mnt_dir));
      mnt.mnt_fsname = calloc(64, ARRAY_ITEM_SIZE(mnt.mnt_fsname));
      mnt.free       = 0;
      mnt.total      = 0;

      if (mnt.mnt_dir && mnt.mnt_fsname) {
            printf("File system\tTotal\tFree\t%%Used\tMount point\n");

            for (u32_t i = 0; ;i++) {
                  if (getmntentry(i, &mnt) == STD_RET_OK) {
                        printf("%s\t\t%u\t%u\t%u%%\t%s\n",
                               mnt.mnt_fsname,
                               mnt.total,
                               mnt.free,
                               ((mnt.total - mnt.free) * 100)/mnt.total,
                               mnt.mnt_dir);
                  } else {
                        break;
                  }
            }
      }

      if (mnt.mnt_dir)
            free(mnt.mnt_dir);

      if (mnt.mnt_fsname)
            free(mnt.mnt_fsname);

      return CMD_EXECUTED;
}


//================================================================================================//
/**
 * @brief Function find internal terminal commands
 *
 * @param *cmd          command
 * @param *arg          argument list
 *
 * @return operation status
 */
//================================================================================================//
cmdStatus_t FindInternalCmd(ch_t *cmd, ch_t *arg)
{
      const cmd_t intCmd[] = {
            {"exit"  , cmdEXIT  },
            {"echo"  , cmdECHO  },
            {"stack" , cmdSTACK },
            {"cd"    , cmdCD    },
            {"ls"    , cmdLS    },
            {"mkdir" , cmdMKDIR },
            {"touch" , cmdTOUCH },
            {"rm"    , cmdRM    },
            {"free"  , cmdFREE  },
            {"uptime", cmdUPTIME},
            {"clear" , cmdCLEAR },
            {"reboot", cmdREBOOT},
            {"df"    , cmdDF    },
      };

      u8_t i;

      for (i = 0; i < ARRAY_SIZE(intCmd); i++) {
            if (strcmp(cmd, intCmd[i].name) == 0) {
                  return intCmd[i].cmd(arg);
            }
      }

      return CMD_NOT_EXIST;
}


//================================================================================================//
/**
 * @brief Function find external commands (registered applications)
 *
 * @param *cmd          command
 * @param *arg          argument list
 *
 * @return operation status
 */
//================================================================================================//
cmdStatus_t FindExternalCmd(ch_t *cmd, ch_t *arg)
{
      cmdStatus_t status = CMD_NOT_EXIST;

      TaskSuspendAll();

      app_t *appHdl = Exec(cmd, arg);

      if (appHdl)
      {
            appHdl->stdin  = stdin;
            appHdl->stdout = stdout;
            appHdl->cwd    = cdpath;
            TaskResumeAll();

            /* DNLTODO terminal must suspend and will be resumed when application is killed */

            while (appHdl->exitCode == STD_RET_UNKNOWN)
            {
                  Sleep(250);
            }

            KillApp(appHdl);

            status = CMD_EXECUTED;
      } else {
            TaskResumeAll();
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function print prompt
 */
//================================================================================================//
void PrintPrompt(void)
{
      printf("\x1B[32mroot@%s:%s\x1B[0m\n", SystemGetHostname(), cdpath);
      printf("\x1B[32m$\x1B[0m ");
}


//================================================================================================//
/**
 * @brief terminal main function
 */
//================================================================================================//
stdRet_t appmain(ch_t *argv) /* DNLTODO terminal with -e mode: script execution mode */
{
      stdRet_t    termStatus = STD_RET_OK;
      cmdStatus_t cmdStatus;
      ch_t *line;
      ch_t *history;
      ch_t *cmd;
      ch_t *arg;

      /* allocate memory for input line */
      line    = calloc(PROMPT_LINE_SIZE, sizeof(ch_t));
      history = calloc(PROMPT_LINE_SIZE, sizeof(ch_t));
      cdpath  = calloc(CD_PATH_SIZE, sizeof(ch_t));

      if (!line || !history || !cdpath)
      {
            if (line)
                  free(line);

            if (history)
                  free(history);

            if (cdpath)
                  free(cdpath);

            printf("No enough free memory\n");
            termStatus = STD_RET_ERROR;
            goto Terminal_Exit;
      }

      strcpy(cdpath, "/");

      u32_t tty = 0;
      ioctl(stdin, TTY_IORQ_GETCURRENTTTY, &tty);

      printf("Welcome to %s/%s (tty%u)\n", SystemGetOSName(), SystemGetKernelName(), tty);

      /* main loop ------------------------------------------------------------------------------ */
      for (;;)
      {
            /* clear input line and print prompt */
            memset(line, ASCII_NULL, PROMPT_LINE_SIZE);
            PrintPrompt();

            /* waiting for command */
            scanf("%100s", line);

            /* check that history was called */
            if (strcmp(line, "\x1B[A") == 0)
            {
                  ch_t character;

                  strcpy(line, history);
                  printf("%s", history);

                  do
                  {
                        character = getChar();
                  }
                  while (!(character == ASCII_LF || character == ASCII_CR));

                  printf("\n");
            }
            else
            {
                  memcpy(history, line, PROMPT_LINE_SIZE);
            }

            /* finds all spaces before command */
            cmd = line;
            cmd += strspn(line, " ");

            /* finds first space after command */
            if ((arg = strchr(cmd, ' ')) != NULL)
            {
                  *(arg++) = ASCII_NULL;
                  arg += strspn(arg, " ");
            }
            else
            {
                  arg = strchr(cmd, ASCII_NULL);
            }

            /* check internal commands */
            if ((cmdStatus = FindInternalCmd(cmd, arg)) == CMD_EXECUTED)
            {
                  continue;
            }
            else if (cmdStatus == CMD_EXIT)
            {
                  break;
            }

            /* check external commands */
            if ((cmdStatus = FindExternalCmd(cmd, arg)) == CMD_EXECUTED)
            {
                  continue;
            }

            /* check status */
            if (cmdStatus == CMD_ALLOC_ERROR)
            {
                  printf("Not enough free memory to run application.\n");
            }
            else if (cmdStatus == CMD_NOT_EXIST)
            {
                  if (strlen(cmd) != 0)
                  {
                        printf("\"%s\" is unknown command.\n", cmd);
                  }
            }
      }

      /* free used memory */
      free(line);
      free(history);
      free(cdpath);

      /* if stack size is debugging */
      if (ParseArg(argv, "stack", PARSE_AS_EXIST, NULL) == STD_RET_OK)
      {
            printf("Free stack: %d levels\n", SystemGetStackFreeSpace());
      }

      Terminal_Exit:
      return termStatus;
}

/* End of application section declaration */
APP_SEC_END


/*==================================================================================================
                                            End of file
==================================================================================================*/
