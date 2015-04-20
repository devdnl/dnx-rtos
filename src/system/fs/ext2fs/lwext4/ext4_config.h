/*
 * Copyright (c) 2013 Grzegorz Kostka (kostka.grzegorz@gmail.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** @addtogroup lwext4
 * @{
 */
/**
 * @file  ext4_config.h
 * @brief Configuration file.
 */

#ifndef EXT4_CONFIG_H_
#define EXT4_CONFIG_H_

#include "fs/fs.h"

/*CONFIGURATION SELECTIONS****************************************************/

#define EXT4_F_SET_EXT2    2
#define EXT4_F_SET_EXT3    3
#define EXT4_F_SET_EXT4    4
#define EXT4_LITTLE_ENDIAN 0x41424344
#define EXT4_BIG_ENDIAN    0x44434241

/*****************************************************************************/

/**@brief   File system support selection */
#ifndef EXT4_CONFIG_EXT_FEATURE_SET_LVL
#define EXT4_CONFIG_EXT_FEATURE_SET_LVL         EXT4_F_SET_EXT2
#endif

/**@brief   Memory allocation function (e.g. void *malloc(size_t size)) */
#ifndef ext4_malloc
static inline void *_ext2_malloc(size_t size) {void *mem = NULL; _sys_malloc(size, &mem); return mem;}
#define ext4_malloc                             _ext2_malloc
#endif

/**@brief   Memory free function (e.g. void free(void *mem)) */
#ifndef ext4_free
static inline void _ext2_free(void *mem) {_sys_free(&mem);}
#define ext4_free                               _ext2_free
#endif

/**@brief   Quick Sort function (e.g. void qsort(void  *base,
 *                                               size_t num,
 *                                               size_t size,
 *                                               int  (*compar)(const void*,const void*))) */
#ifndef ext4_qsort
#define ext4_qsort                              qsort
#endif

/**@brief   Current time in UNIX epoch (if not used set to 0) */
#ifndef ext4_now
#define ext4_now                                _sys_time(NULL)
#endif

/**@brief   Current UID (if not used set to 0) */
#ifndef ext4_current_uid
#define ext4_current_uid                        0
#endif

/**@brief   Current GID (if not used set to 0) */
#ifndef ext4_current_gid
#define ext4_current_gid                        0
#endif

/**@brief   Structure pack command (compiler depended) */
#ifndef EXT4_PACKED
#define EXT4_PACKED                             __attribute__((packed))
#endif

/**@brief   Enable directory indexing comb sort*/
#ifndef EXT4_CONFIG_DIR_INDEX_COMB_SORT
#define EXT4_CONFIG_DIR_INDEX_COMB_SORT         1
#endif

/**@brief   Include error codes from ext4_errno (1) or standard library (0).*/
#ifndef EXT4_CONFIG_HAVE_OWN_ERRNO
#define EXT4_CONFIG_HAVE_OWN_ERRNO              0
#endif

/**@brief   Debug printf enable (1) (stdout)*/
#ifndef EXT4_CONFIG_DEBUG_PRINTF
#define EXT4_CONFIG_DEBUG_PRINTF                0
#endif

/**@brief   Assert printf enable (1) (stdout)*/
#ifndef EXT4_CONFIG_DEBUG_ASSERT
#define EXT4_CONFIG_DEBUG_ASSERT                0
#endif

/**@brief   Number of blocks in cache (block has few sectors (1KiB - 32KiB)). (4+) */
#ifndef EXT4_CONFIG_BLOCK_DEV_CACHE_SIZE
#define EXT4_CONFIG_BLOCK_DEV_CACHE_SIZE        __EXT2FS_CACHE_SIZE__
#endif

/**@brief   CPU Byte order */
#ifndef EXT4_BYTE_ORDER
#define EXT4_BYTE_ORDER                         EXT4_LITTLE_ENDIAN
#endif


/* AUTOCONFIGURATION **********************************************************/
#if EXT4_CONFIG_EXT_FEATURE_SET_LVL == EXT4_F_SET_EXT2
    #define EXT4_CONFIG_DIR_INDEX_ENABLE        0
    #define EXT4_CONFIG_EXTENT_ENABLE           0

    /*Superblock features flag*/
    #define EXT4_CONFIG_FEATURE_COMPAT_SUPP     EXT2_FEATURE_COMPAT_SUPP

    #define EXT4_CONFIG_FEATURE_INCOMPAT_SUPP   (EXT2_FEATURE_INCOMPAT_SUPP | \
                                                EXT_FEATURE_INCOMPAT_IGNORED)

    #define EXT4_CONFIG_FEATURE_RO_COMPAT_SUPP  EXT2_FEATURE_RO_COMPAT_SUPP
#elif EXT4_CONFIG_EXT_FEATURE_SET_LVL == EXT4_F_SET_EXT3
    #define EXT4_CONFIG_DIR_INDEX_ENABLE        1
    #define EXT4_CONFIG_EXTENT_ENABLE           0

    /*Superblock features flag*/
    #define EXT4_CONFIG_FEATURE_COMPAT_SUPP     EXT3_FEATURE_COMPAT_SUPP

    #define EXT4_CONFIG_FEATURE_INCOMPAT_SUPP   (EXT3_FEATURE_INCOMPAT_SUPP | \
                                                EXT_FEATURE_INCOMPAT_IGNORED)

    #define EXT4_CONFIG_FEATURE_RO_COMPAT_SUPP  EXT3_FEATURE_RO_COMPAT_SUPP
#elif EXT4_CONFIG_EXT_FEATURE_SET_LVL == EXT4_F_SET_EXT4
    #define EXT4_CONFIG_DIR_INDEX_ENABLE        1
    #define EXT4_CONFIG_EXTENT_ENABLE           1

    /*Superblock features flag*/
    #define EXT4_CONFIG_FEATURE_COMPAT_SUPP     EXT4_FEATURE_COMPAT_SUPP

    #define EXT4_CONFIG_FEATURE_INCOMPAT_SUPP   (EXT4_FEATURE_INCOMPAT_SUPP | \
                                                EXT_FEATURE_INCOMPAT_IGNORED)

    #define EXT4_CONFIG_FEATURE_RO_COMPAT_SUPP  EXT4_FEATURE_RO_COMPAT_SUPP
#else
#define "Unsupported EXT4_CONFIG_EXT_FEATURE_SET_LVL"
#endif

#endif /* EXT4_CONFIG_H_ */

/**
 * @}
 */
