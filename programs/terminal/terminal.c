/*=========================================================================*//**
@file    terminal.c

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "terminal.h"
#include "tty_def.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define PROMPT_LINE_LEN                 100
#define CWD_PATH_LEN                    128

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
enum cmd_status {
        CMD_STATUS_EXECUTED,
        CMD_STATUS_NOT_EXIST,
        CMD_STATUS_NOT_ENOUGH_FREE_MEMORY
};

struct cmd_entry {
        const char *name;
        enum cmd_status (*const cmd)(char *arg);
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void print_prompt(void);
static enum cmd_status find_internal_command(char *cmd, char *arg);
static enum cmd_status find_external_command(char *cmd, char *arg);
static enum cmd_status cmd_cd(char *arg);
static enum cmd_status cmd_ls(char *arg);
static enum cmd_status cmd_mkdir(char *arg);
static enum cmd_status cmd_touch(char *arg);
static enum cmd_status cmd_rm(char *arg);
static enum cmd_status cmd_free(char *arg);
static enum cmd_status cmd_uptime(char *arg);
static enum cmd_status cmd_clear(char *arg);
static enum cmd_status cmd_reboot(char *arg);
static enum cmd_status cmd_df(char *arg);
static enum cmd_status cmd_mount(char *arg);
static enum cmd_status cmd_umount(char *arg);
static enum cmd_status cmd_help(char *arg);

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES {
        char line[PROMPT_LINE_LEN];
        char cwd[CWD_PATH_LEN];
};

static const struct cmd_entry commands[] = {
        {"cd"    , cmd_cd    },
        {"ls"    , cmd_ls    },
        {"mkdir" , cmd_mkdir },
        {"touch" , cmd_touch },
        {"rm"    , cmd_rm    },
        {"free"  , cmd_free  },
        {"uptime", cmd_uptime},
        {"clear" , cmd_clear },
        {"reboot", cmd_reboot},
        {"df"    , cmd_df    },
        {"mount" , cmd_mount },
        {"umount", cmd_umount},
        {"help"  , cmd_help  },
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(terminal, STACK_DEPTH_LOW);

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Terminal main function
 */
//==============================================================================
int PROGRAM_MAIN(terminal, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        char *cmd;
        char *arg;
        u8_t  tty = 0;
        enum cmd_status cmd_status;

        strcpy(global->cwd, "/");

        ioctl(stdin, TTY_IORQ_GET_CURRENT_TTY, &tty);

        printf("Welcome to %s/%s (tty%u)\n", get_OS_name(), get_kernel_name(), tty);

        for (;;) {
                /* clear input line and print prompt */
                memset(global->line, '\0', PROMPT_LINE_LEN);
                print_prompt();

                /* waiting for command */
                scanf("%100s", global->line);

                /* finds all spaces before command */
                cmd  = global->line;
                cmd += strspn(global->line, " ");

                /* finds first space after command */
                if ((arg = strchr(cmd, ' ')) != NULL) {
                        *(arg++) = '\0';
                        arg += strspn(arg, " ");
                } else {
                        arg = strchr(cmd, '\0');
                }

                /* terminal exit */
                if (strcmp("exit", cmd) == 0) {
                        break;
                }

                if (strcmp("", cmd) == 0) {
                        continue;
                }

                /* identify program localization */
                cmd_status = find_internal_command(cmd, arg);

                if (cmd_status == CMD_STATUS_NOT_EXIST) {
                        cmd_status = find_external_command(cmd, arg);
                }

                switch (cmd_status) {
                case CMD_STATUS_EXECUTED:
                        continue;

                case CMD_STATUS_NOT_EXIST:
                        printf("\"%s\" is unknown command.\n", cmd);
                        break;

                case CMD_STATUS_NOT_ENOUGH_FREE_MEMORY:
                        printf("Not enough free memory.\n");
                        break;
                }
        }

        return 0;
}

//==============================================================================
/**
 * @brief Function print line prompt
 */
//==============================================================================
static void print_prompt(void)
{
        printf(FONT_COLOR_GREEN"root@%s:%s"RESET_ATTRIBUTES"\n", get_host_name(), global->cwd);
        printf(FONT_COLOR_GREEN"$ "RESET_ATTRIBUTES);
}

//==============================================================================
/**
 * @brief Function find external commands (registered applications)
 *
 * @param *cmd          command
 * @param *arg          argument list
 *
 * @return operation status
 */
//==============================================================================
static enum cmd_status find_external_command(char *cmd, char *arg)
{
        enum prog_state state  = PROGRAM_UNKNOWN_STATE;
        enum cmd_status status = CMD_STATUS_NOT_EXIST;

        new_program(cmd, arg, global->cwd, stdin, stdout, &state, NULL);

        while (state == PROGRAM_RUNNING) {
                suspend_this_task();
        }

        switch (state) {
        case PROGRAM_UNKNOWN_STATE:
        case PROGRAM_RUNNING:
                break;

        case PROGRAM_ENDED:
                status = CMD_STATUS_EXECUTED;
                break;

        case PROGRAM_HANDLE_ERROR:
        case PROGRAM_ARGUMENTS_PARSE_ERROR:
        case PROGRAM_NOT_ENOUGH_FREE_MEMORY:
                status = CMD_STATUS_NOT_ENOUGH_FREE_MEMORY;
                break;

        case PROGRAM_DOES_NOT_EXIST:
                status = CMD_STATUS_NOT_EXIST;
                break;
        }

        return status;
}

//==============================================================================
/**
 * @brief Function find internal terminal commands
 *
 * @param *cmd          command
 * @param *arg          argument list
 *
 * @return operation status
 */
//==============================================================================
static enum cmd_status find_internal_command(char *cmd, char *arg)
{
        for (uint i = 0; i < ARRAY_SIZE(commands); i++) {
                if (strcmp(cmd, commands[i].name) == 0) {
                        return commands[i].cmd(arg);
                }
        }

        return CMD_STATUS_NOT_EXIST;
}

//==============================================================================
/**
 * @brief Function change current working path
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_cd(char *arg)
{
        char  *newpath  = NULL;
        dir_t *dir      = NULL;
        bool_t freePath = FALSE;

        if (strcmp(arg, "..") == 0) {
                char *lastslash = strrchr(global->cwd, '/');

                if (lastslash) {
                        if (lastslash != global->cwd) {
                                *lastslash = '\0';
                         } else {
                                 *(lastslash + 1) = '\0';
                         }
                }
        } else if (arg[0] != '/') {
                newpath = calloc(strlen(arg) + strlen(global->cwd) + 2, sizeof(global->cwd[0]));

                if (newpath) {
                        strcpy(newpath, global->cwd);

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

                        strncpy(global->cwd, newpath, CWD_PATH_LEN);
                } else {
                        printf("No such directory\n");
                }

                if (freePath)
                        free(newpath);
        }

        return CMD_STATUS_EXECUTED;
}

//==============================================================================
/**
 * @brief Function listing all files in the selected directory
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_ls(char *arg)
{
        char  *newpath  = NULL;
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
                        newpath = calloc(strlen(arg) + strlen(global->cwd) + 2,
                                         sizeof(global->cwd[0]));

                        if (newpath) {
                                strcpy(newpath, global->cwd);

                                if (newpath[strlen(newpath) - 1] != '/') {
                                        newpath[strlen(newpath)] = '/';
                                }

                                strcat(newpath, arg);

                                freePath = TRUE;
                        }
                }
        } else {
                newpath = global->cwd;
        }

        dir_t *dir = opendir(newpath);

        if (dir) {
                dirent_t dirent;

                char *ccolor = FONT_COLOR_YELLOW"c";
                char *rcolor = FONT_COLOR_MAGENTA"-";
                char *lcolor = FONT_COLOR_CYAN"l";
                char *dcolor = FONT_COLOR_GREEN"d";

                printf("Total %u\n", dir->items);

                while ((dirent = readdir(dir)).name != NULL) {
                        char *type = NULL;

                        switch (dirent.filetype) {
                        case FILE_TYPE_DIR:     type = dcolor; break;
                        case FILE_TYPE_DRV:     type = ccolor; break;
                        case FILE_TYPE_LINK:    type = lcolor; break;
                        case FILE_TYPE_REGULAR: type = rcolor; break;
                        default: type = "?";
                        }

                        printf("%s %u\t%s"RESET_ATTRIBUTES"\n",
                               type,
                               dirent.size,
                               dirent.name);
                }

                closedir(dir);
        } else {
                printf("No such directory\n");
        }

        if (freePath)
                free(newpath);

        return CMD_STATUS_EXECUTED;
}

//==============================================================================
/**
 * @brief Function create new directory
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_mkdir(char *arg)
{
        char  *newpath  = NULL;
        bool_t freePath = FALSE;

        if (arg) {
                if (arg[0] == '/') {
                        newpath = arg;
                } else {
                        newpath = calloc(strlen(arg) + strlen(global->cwd) + 2,
                                         sizeof(global->cwd[0]));

                        if (newpath) {
                                strcpy(newpath, global->cwd);

                                if (newpath[strlen(newpath) - 1] != '/') {
                                        newpath[strlen(newpath)] = '/';
                                }

                                strcat(newpath, arg);

                                freePath = TRUE;
                        }
                }
        } else {
                newpath = global->cwd;
        }

        if (mkdir(newpath) == STD_RET_ERROR) {
                printf("Cannot create directory \"%s\": no such file or directory\n", arg);
        }

        if (freePath)
                free(newpath);

        return CMD_STATUS_EXECUTED;
}

//==============================================================================
/**
 * @brief Function create new file or modify modification date
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_touch(char *arg)
{
        char  *newpath  = NULL;
        bool_t freePath = FALSE;

        if (arg) {
                if (arg[0] == '/') {
                        newpath = arg;
                } else {
                        newpath = calloc(strlen(arg) + strlen(global->cwd) + 2,
                                         sizeof(global->cwd[0]));

                        if (newpath) {
                                strcpy(newpath, global->cwd);

                                if (newpath[strlen(newpath) - 1] != '/') {
                                        newpath[strlen(newpath)] = '/';
                                }

                                strcat(newpath, arg);

                                freePath = TRUE;
                        }
                }
        } else {
                newpath = global->cwd;
        }

        file_t *file = fopen(newpath, "a+");

        if (file) {
                fclose(file);
        } else {
                printf("Cannot touch \"%s\"\n", arg);
        }

        if (freePath)
                free(newpath);

        return CMD_STATUS_EXECUTED;
}

//==============================================================================
/**
 * @brief Function remove slected file
 *
 * @param *arg          argument
 */
//==============================================================================
static enum cmd_status cmd_rm(char *arg)
{
        char  *newpath  = NULL;
        bool_t freePath = FALSE;

        if (arg) {
                if (arg[0] == '/') {
                        newpath = arg;
                } else {
                        newpath = calloc(strlen(arg) + strlen(global->cwd) + 2,
                                         sizeof(global->cwd[0]));

                        if (newpath) {
                                strcpy(newpath, global->cwd);

                                if (newpath[strlen(newpath) - 1] != '/') {
                                        newpath[strlen(newpath)] = '/';
                                }

                                strcat(newpath, arg);

                                freePath = TRUE;
                        }
                }
        } else {
                newpath = global->cwd;
        }

        if (remove(newpath) == STD_RET_ERROR) {
                printf("Cannot remove \"%s\"\n", arg);
        }

        if (freePath)
                free(newpath);

        return CMD_STATUS_EXECUTED;
}

//==============================================================================
/**
 * @brief Function shows the free memory
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_free(char *arg)
{
        (void) arg;

        u32_t free = get_free_memory();
        u32_t used = get_used_memory();

        printf("Total: %d\n", get_memory_size());
        printf("Free : %d\n", free);
        printf("Used : %d (kernel: %d, system: %d, drivers: %d, programs: %d)\n", used,
               get_used_memory_by_kernel(),
               get_used_memory_by_system(),
               get_used_memory_by_drivers(),
               get_used_memory_by_programs());
        printf("Memory usage: %d%%\n",
               (used * 100)/get_memory_size());

        return CMD_STATUS_EXECUTED;
}

//==============================================================================
/**
 * @brief Function show uptime
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_uptime(char *arg)
{
        (void) arg;

        u32_t uptime = get_uptime();
        u32_t udays  = (uptime / (3600 * 24));
        u32_t uhrs   = (uptime / 3600) % 24;
        u32_t umins  = (uptime / 60) % 60;

        printf("up %ud %u2:%u2\n", udays, uhrs, umins);

        return CMD_STATUS_EXECUTED;
}

//==============================================================================
/**
 * @brief Function clears terminal
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_clear(char *arg)
{
        (void)arg;

        ioctl(stdout, TTY_IORQ_CLEAR_SCR, NULL);

        return CMD_STATUS_EXECUTED;
}

//==============================================================================
/**
 * @brief Function reboot system
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_reboot(char *arg)
{
        (void) arg;

        printf("Rebooting...\n");
        milisleep(500);
        reboot();

        return CMD_STATUS_EXECUTED;
}

//==============================================================================
/**
 * @brief Function listing all mounted file systems
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_df(char *arg)
{
        (void) arg;

        struct vfs_mntent mnt;
        mnt.mnt_dir = calloc(64, ARRAY_ITEM_SIZE(mnt.mnt_dir));
        mnt.mnt_fsname = calloc(64, ARRAY_ITEM_SIZE(mnt.mnt_fsname));
        mnt.free = 0;
        mnt.total = 0;

        if (mnt.mnt_dir && mnt.mnt_fsname) {
                printf("File system\tTotal\tFree\t%%Used\tMount point\n");

                for (u32_t i = 0;; i++) {
                        if (getmntentry(i, &mnt) == STD_RET_OK) {
                                printf("%s\t\t%u\t%u\t%u%%\t%s\n",
                                       mnt.mnt_fsname,
                                       mnt.total,
                                       mnt.free,
                                       ((mnt.total - mnt.free) * 100)/mnt.total,
                                       mnt.mnt_dir);

                                memset(mnt.mnt_dir, 0, 64);
                                memset(mnt.mnt_fsname, 0, 64);
                        } else {
                                break;
                        }
                }
        }

        if (mnt.mnt_dir)
                free(mnt.mnt_dir);

        if (mnt.mnt_fsname)
                free(mnt.mnt_fsname);

        return CMD_STATUS_EXECUTED;
}

//==============================================================================
/**
 * @brief Function mount file system
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_mount(char *arg)
{
        char *strb   = NULL;
        char *stre   = NULL;
        char *fstype = NULL;
        char *fssrc  = NULL;
        char *fsmntp = NULL;
        i8_t  len    = 0;

        strb = arg;
        stre = strchr(strb, ' ');
        len  = stre - strb;

        if (arg[0] == '\0') {
                printf("Usage: mount [file system name] [source path|-] [mount point]\n");
                return CMD_STATUS_EXECUTED;
        }

        if ((fstype = calloc(len + 1, sizeof(char))) != NULL) {
                strncpy(fstype, strb, stre - strb);
        } else {
                goto exit;
        }

        strb = stre + 1;
        stre = strchr(strb, ' ');
        len  = stre - strb;

        if ((fssrc = calloc(len + 1, sizeof(char))) != NULL) {
                strncpy(fssrc, strb, stre - strb);
        } else {
                goto exit;
        }

        strb = stre + 1;
        len  = strlen(strb);

        if ((fsmntp = calloc(len + 1, sizeof(char))) != NULL) {
                strcpy(fsmntp, strb);
        } else {
                goto exit;
        }

        exit:
        if (!fstype || !fssrc || !fsmntp) {
                printf("Bad arguments!\n");
        } else {
                if (fssrc[0] == '/' || fssrc[0] == '-') {
                        if (mount(fstype, fssrc, fsmntp) != STD_RET_OK) {
                                printf("Error while mounting file system!\n");
                        }
                } else {
                        printf("Typed path is not correct!\n");
                }
        }

        if (fstype) {
                free(fstype);
        }

        if (fssrc) {
                free(fssrc);
        }

        if (fsmntp) {
                free(fsmntp);
        }

        return CMD_STATUS_EXECUTED;
}

#ifdef __cplusplus
}
#endif

//==============================================================================
/**
 * @brief Function umount mounted file system
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_umount(char *arg)
{
        if (arg[0] == '\0') {
                printf("Usage: umount [mount point]\n");
        } else {
                if (umount(arg) != STD_RET_OK) {
                        printf("Cannot unmount file system!\n");
                }
        }

        return CMD_STATUS_EXECUTED;
}

//==============================================================================
/**
 * @brief Function listing all internal supported commands
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_help(char *arg)
{
        (void) arg;

        for (uint cmd = 0; cmd < ARRAY_SIZE(commands); cmd++) {
                printf("%s\n", commands[cmd].name);
        }

        return CMD_STATUS_EXECUTED;
}

/*==============================================================================
  End of file
==============================================================================*/
