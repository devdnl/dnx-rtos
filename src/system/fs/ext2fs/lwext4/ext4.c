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
 * @file  ext4.h
 * @brief Ext4 high level operations (file, directory, ...)
 */

#include <ext4_config.h>
#include <ext4_blockdev.h>
#include <ext4_types.h>
#include <ext4_debug.h>
#include <ext4_errno.h>
#include <ext4_fs.h>
#include <ext4_dir.h>
#include <ext4_inode.h>
#include <ext4_super.h>
#include <ext4_dir_idx.h>
#include <ext4.h>

#include <string.h>

/**@brief   File system main container (context) */
struct ext4_container {
    /**@brief   User context */
    void        *usr_ctx;

    /**@brief   Os dependent lock/unlock functions.*/
    const struct ext4_os_if *osif;

    /**@brief   Ext4 filesystem internals.*/
    struct ext4_fs fs;

    /**@brief   Block device handle.*/
    struct ext4_blockdev bdev;

    /**@brief   Block cache handle.*/
    struct ext4_bcache bcache;
};

/****************************************************************************/
static void lock(ext4_fs_t *ctx)
{
    if (ctx->osif && ctx->osif->lock) {
        ctx->osif->lock(ctx->usr_ctx);
    }
}

static void unlock(ext4_fs_t *ctx)
{
    if (ctx->osif && ctx->osif->unlock) {
        ctx->osif->unlock(ctx->usr_ctx);
    }
}

static bool ext4_is_dots(const uint8_t *name, size_t name_size)
{
    if ((name_size == 1) && (name[0] == '.'))
        return true;

    if ((name_size == 2) && (name[0] == '.') && (name[1] == '.'))
        return true;

    return false;
}

static int ext4_has_children(bool *has_children, struct ext4_inode_ref *enode)
{
    struct ext4_fs *fs = enode->fs;

    /* Check if node is directory */
    if (!ext4_inode_is_type(&fs->sb, enode->inode,
            EXT4_INODE_MODE_DIRECTORY)) {
        *has_children = false;
        return EOK;
    }

    struct ext4_directory_iterator it;
    int rc = ext4_dir_iterator_init(&it, enode, 0);
    if (rc != EOK)
        return rc;

    /* Find a non-empty directory entry */
    bool found = false;
    while (it.current != NULL) {
        if (it.current->inode != 0) {
            uint16_t name_size =
                    ext4_dir_entry_ll_get_name_length(&fs->sb,
                            it.current);
            if (!ext4_is_dots(it.current->name, name_size)) {
                found = true;
                break;
            }
        }

        rc = ext4_dir_iterator_next(&it);
        if (rc != EOK) {
            ext4_dir_iterator_fini(&it);
            return rc;
        }
    }

    rc = ext4_dir_iterator_fini(&it);
    if (rc != EOK)
        return rc;

    *has_children = found;

    return EOK;
}


static int ext4_link(ext4_fs_t *ctx, struct ext4_inode_ref *parent,
    struct ext4_inode_ref *child, const char *name, uint32_t name_len)
{
    /* Check maximum name length */
    if(name_len > EXT4_DIRECTORY_FILENAME_LEN)
        return EINVAL;

    /* Add entry to parent directory */
    int rc = ext4_dir_add_entry(parent, name, name_len,
            child);
    if (rc != EOK)
        return rc;

    /* Fill new dir -> add '.' and '..' entries */
    if (ext4_inode_is_type(&ctx->fs.sb, child->inode,
            EXT4_INODE_MODE_DIRECTORY)) {
        rc = ext4_dir_add_entry(child, ".", strlen("."),
                child);
        if (rc != EOK) {
            ext4_dir_remove_entry(parent, name, strlen(name));
            return rc;
        }

        rc = ext4_dir_add_entry(child, "..", strlen(".."),
                parent);
        if (rc != EOK) {
            ext4_dir_remove_entry(parent, name, strlen(name));
            ext4_dir_remove_entry(child, ".", strlen("."));
            return rc;
        }

        /*New empty directory. Two links (. and ..) */
        ext4_inode_set_links_count(child->inode, 2);

#if EXT4_CONFIG_DIR_INDEX_ENABLE
        /* Initialize directory index if supported */
        if (ext4_sb_has_feature_compatible(&ctx->fs.sb,
                EXT4_FEATURE_COMPAT_DIR_INDEX)) {
            rc = ext4_dir_dx_init(child);
            if (rc != EOK)
                return rc;

            ext4_inode_set_flag(child->inode,
                EXT4_INODE_FLAG_INDEX);
            child->dirty = true;
        }
#endif

        ext4_fs_inode_links_count_inc(parent);
        child->dirty = true;
        parent->dirty = true;

    }

    return EOK;
}

static const char *correct_path(const char *path)
{
    if (path[0] == '/') {
        if (strlen(path) == 1) {
            path = ".";
        } else {
            path += 1;
        }
    }

    return path;
}

static int ext4_unlink(ext4_fs_t *ctx,
    struct ext4_inode_ref *parent, struct ext4_inode_ref *child_inode_ref,
    const char *name, uint32_t name_len)
{
    bool has_children;
    int rc = ext4_has_children(&has_children, child_inode_ref);
    if (rc != EOK)
        return rc;

    /* Cannot unlink non-empty node */
    if (has_children)
        return ENOTSUP;

    /* Remove entry from parent directory */
    rc = ext4_dir_remove_entry(parent, name, name_len);
    if (rc != EOK)
        return rc;

    bool is_dir = ext4_inode_is_type(&ctx->fs.sb, child_inode_ref->inode,
            EXT4_INODE_MODE_DIRECTORY);

    /* If directory - handle links from parent */
    if (is_dir) {
        //ext4_assert(ext4_inode_get_links_count(child_inode_ref->inode) == 1);
        ext4_fs_inode_links_count_dec(parent);
        parent->dirty = true;
    }

    uint32_t now = ext4_now;
    ext4_inode_set_change_inode_time(parent->inode, now);
    ext4_inode_set_modification_time(parent->inode, now);
    parent->dirty = true;

    ext4_inode_set_change_inode_time(child_inode_ref->inode, now);

    ext4_inode_set_deletion_time(child_inode_ref->inode, now);
    ext4_inode_set_links_count(child_inode_ref->inode, 0);
    child_inode_ref->dirty = true;

    return EOK;
}


/****************************************************************************/

ext4_fs_t *ext4_mount(const struct ext4_os_if *osif,
                      void                    *usr_ctx,
                      uint32_t                 block_size,
                      uint64_t                 number_of_blocks)
{
    if (!osif || !block_size || !number_of_blocks) {
        return NULL;
    }

    ext4_fs_t *ctx = ext4_malloc(sizeof(ext4_fs_t));
    if (ctx) {
        memset(ctx, 0, sizeof(ext4_fs_t));

        ctx->osif    = osif;
        ctx->usr_ctx = usr_ctx;

        if (ext4_block_init(&ctx->bdev, osif, usr_ctx, block_size, number_of_blocks) != EOK)
            goto error;

        if (ext4_fs_init(&ctx->fs, &ctx->bdev) != EOK) {
            goto error;
        }

        uint32_t bsize = ext4_sb_get_block_size(&ctx->fs.sb);
        ext4_block_set_lb_size(&ctx->bdev, bsize);

        if (ext4_bcache_init_dynamic(&ctx->bcache, EXT4_CONFIG_BLOCK_DEV_CACHE_SIZE, bsize) != EOK) {
            goto error;
        }

        if(bsize != ctx->bcache.itemsize) {
            errno = ENOTSUP;
            goto error;
        }

        /*Bind block cache to block device*/
        if (ext4_block_bind_bcache(&ctx->bdev, &ctx->bcache) != EOK) {
            goto error;
        }

        return ctx;

        error:
        ext4_fs_fini(&ctx->fs);
        ext4_block_fini(&ctx->bdev);
        ext4_bcache_fini_dynamic(&ctx->bcache);
        ext4_free(ctx);
        ctx = NULL;
    }

    return ctx;
}


int ext4_umount(ext4_fs_t *ctx)
{
    if (ctx) {
        ext4_fs_fini(&ctx->fs);
        ext4_bcache_fini_dynamic(&ctx->bcache);
        ext4_block_fini(ctx->fs.bdev);
        ext4_free(ctx);
        return EOK;
    } else {
        return EINVAL;
    }
}

int ext4_statfs(ext4_fs_t *ctx, struct ext4_fs_stats *stats)
{
    if (ctx) {
        lock(ctx);
        stats->inodes_count      = ext4_get32(&ctx->fs.sb, inodes_count);
        stats->free_inodes_count = ext4_get32(&ctx->fs.sb, free_inodes_count);
        stats->blocks_count      = ext4_sb_get_blocks_cnt(&ctx->fs.sb);
        stats->free_blocks_count = ext4_sb_get_free_blocks_cnt(&ctx->fs.sb);
        stats->block_size        = ext4_sb_get_block_size(&ctx->fs.sb);

        stats->block_group_count = ext4_block_group_cnt(&ctx->fs.sb);
        stats->blocks_per_group  = ext4_get32(&ctx->fs.sb, blocks_per_group);
        stats->inodes_per_group  = ext4_get32(&ctx->fs.sb, inodes_per_group);

        memcpy(stats->volume_name, ctx->fs.sb.volume_name, 16);
        unlock(ctx);

        return EOK;
    } else {
        return EINVAL;
    }
}

/********************************FILE OPERATIONS*****************************/

static int ext4_path_check(const char *path, bool* is_goal)
{
    for (int i = 0; i < EXT4_DIRECTORY_FILENAME_LEN; ++i) {

        if(path[i] == '/'){
            *is_goal = false;
            return i;
        }

        if(path[i] == 0){
            *is_goal = true;
            return i;
        }
    }

    return 0;
}

/****************************************************************************/

static int ext4_generic_open(ext4_fs_t *ctx, ext4_file *f, const char *path,
    uint32_t flags, bool file_expect, uint32_t *parent_inode)
{
    struct ext4_directory_search_result result;
    struct ext4_inode_ref ref;
    bool is_goal = false;
    uint8_t inode_type = EXT4_DIRECTORY_FILETYPE_DIR;
    int r = ENOENT;
    uint32_t next_inode;

    f->flags = flags;

    /*Load root*/
    if(ext4_fs_get_inode_ref(&ctx->fs, EXT4_INODE_ROOT_INDEX, &ref) != EOK)
        return r;

    if(parent_inode)
        *parent_inode = ref.index;

    path = correct_path(path);

    int len = ext4_path_check(path, &is_goal);

    while(true) {

        len = ext4_path_check(path, &is_goal);

        if(!len){
            /*If root open was request.*/
            if(is_goal && !file_expect)
                break;

            r = ENOENT;
            break;
        }

        r = ext4_dir_find_entry(&result, &ref, path, len);
        if(r != EOK){

            if(r != ENOENT)
                break;

            if(!(f->flags & O_CREAT))
                break;

            /*O_CREAT allows create new entry*/
            struct ext4_inode_ref child_ref;
            r = ext4_fs_alloc_inode(&ctx->fs, &child_ref, is_goal ? !file_expect : true);
            if(r != EOK)
                break;

            /*Destroy last result*/
            ext4_dir_destroy_result(&ref, &result);

            /*Link with root dir.*/
            r = ext4_link(ctx, &ref, &child_ref, path, len);
            if(r != EOK){
                /*Fali. Free new inode.*/
                ext4_fs_free_inode(&child_ref);
                /*We do not want to write new inode.
                  But block has to be released.*/
                child_ref.dirty = false;
                ext4_fs_put_inode_ref(&child_ref);
                break;
            }

            ext4_fs_put_inode_ref(&child_ref);

            continue;
        }

        if(parent_inode)
            *parent_inode = ref.index;

        next_inode = result.dentry->inode;
        inode_type = ext4_dir_entry_ll_get_inode_type(&ctx->fs.sb, result.dentry);

        r = ext4_dir_destroy_result(&ref, &result);
        if(r != EOK)
            break;

        /*If expected file error*/
        if((inode_type == EXT4_DIRECTORY_FILETYPE_REG_FILE)
                && !file_expect && is_goal){
            r = ENOENT;
            break;
        }

        /*If expected directory error*/
        if((inode_type == EXT4_DIRECTORY_FILETYPE_DIR)
                && file_expect && is_goal){
            r = ENOENT;
            break;
        }

        r = ext4_fs_put_inode_ref(&ref);
        if(r != EOK)
            break;

        r = ext4_fs_get_inode_ref(&ctx->fs, next_inode, &ref);
        if(r != EOK)
            break;

        if(is_goal)
            break;

        path += len + 1;
    };

    if(r != EOK){
        ext4_fs_put_inode_ref(&ref);
        return r;
    }

    if(is_goal){
        ext4_inode_set_access_time(ref.inode, ext4_now);

        if((f->flags & O_TRUNC) && (inode_type == EXT4_DIRECTORY_FILETYPE_REG_FILE)) {

            r = ext4_fs_truncate_inode(&ref, 0);
            if(r != EOK){
                ext4_fs_put_inode_ref(&ref);
                return r;
            }
        }

        f->fsize = ext4_inode_get_size(&ctx->fs.sb, ref.inode);
        f->inode = ref.index;
        f->fpos  = 0;

        if(f->flags & O_APPEND)
            f->fpos = f->fsize;
    }

    r = ext4_fs_put_inode_ref(&ref);
    return r;
}

/****************************************************************************/

int ext4_cache_write_back(ext4_fs_t *ctx, bool on)
{
    if (ctx) {
        lock(ctx);
        ext4_block_cache_write_back(ctx->fs.bdev, on);
        unlock(ctx);
        return EOK;
    } else {
        return EINVAL;
    }
}


int ext4_fremove(ext4_fs_t *ctx, const char *path)
{
    ext4_file   f;
    uint32_t parent_inode;
    int r;
    int len;
    bool is_goal;

    struct ext4_inode_ref child;
    struct ext4_inode_ref parent;

    if(!ctx)
        return ENOENT;

    path = correct_path(path);

    lock(ctx);
    r = ext4_generic_open(ctx, &f, path, O_RDONLY, true, &parent_inode);
    if(r != EOK){
        unlock(ctx);
        return r;
    }

    /*Load parent*/
    r = ext4_fs_get_inode_ref(&ctx->fs, parent_inode, &parent);
    if(r != EOK){
        unlock(ctx);
        return r;
    }

    /*We have file to delete. Load it.*/
    r = ext4_fs_get_inode_ref(&ctx->fs, f.inode, &child);
    if(r != EOK){
        ext4_fs_put_inode_ref(&parent);
        unlock(ctx);
        return r;
    }

    /*Turncate.*/
    ext4_block_cache_write_back(ctx->fs.bdev, 1);
    /*Truncate may be IO heavy. Do it writeback cache mode.*/
    r = ext4_fs_truncate_inode(&child, 0);
    ext4_block_cache_write_back(ctx->fs.bdev, 0);

    if(r != EOK)
        goto Finish;

    len = ext4_path_check(path, &is_goal);

    /*Unlink from parent.*/
    r = ext4_unlink(ctx, &parent, &child, path, len);
    if(r != EOK)
        goto Finish;

    r = ext4_fs_free_inode(&child);
    if(r != EOK)
        goto Finish;

    Finish:
    ext4_fs_put_inode_ref(&child);
    ext4_fs_put_inode_ref(&parent);
    unlock(ctx);
    return r;
}

int ext4_fopen(ext4_fs_t *ctx, ext4_file *f, const char *path, uint32_t flags)
{
    if (!ctx || !path)
        return EINVAL;

    lock(ctx);
    ext4_block_cache_write_back(ctx->fs.bdev, true);
    int r = ext4_generic_open(ctx, f, path, flags, true, NULL);
    ext4_block_cache_write_back(ctx->fs.bdev, false);
    unlock(ctx);
    return r;
}

int ext4_fclose(ext4_fs_t *ctx, ext4_file *f)
{
    if (!ctx || !f)
        return EINVAL;

    f->flags = 0;
    f->inode = 0;
    f->fpos  = f->fsize = 0;

    return EOK;
}

int ext4_fread(ext4_fs_t *ctx, ext4_file *f, void *buf, uint32_t size, uint32_t *rcnt)
{
    int r = EOK;
    uint32_t u;
    uint32_t fblock;
    uint32_t fblock_start;
    uint32_t fblock_cnt;
    struct ext4_block b;
    uint8_t *u8_buf = buf;
    struct ext4_inode_ref ref;
    uint32_t sblock;
    uint32_t sblock_end;
    uint32_t block_size;

    if (!ctx || !buf)
        return EINVAL;

    if(f->flags & O_WRONLY)
        return EPERM;

    if(!size)
        return EOK;

    lock(ctx);

    if(rcnt)
        *rcnt = 0;

    if(ext4_fs_get_inode_ref(&ctx->fs, f->inode, &ref) != EOK){
        unlock(ctx);
        return r;
    }

    /*Sync file size*/
    f->fsize = ext4_inode_get_size(&ctx->fs.sb, ref.inode);


    block_size = ext4_sb_get_block_size(&ctx->fs.sb);
    size = size > (f->fsize - f->fpos) ? (f->fsize - f->fpos) : size;
    sblock = (f->fpos) / block_size;
    sblock_end = (f->fpos + size) / block_size;
    u = (f->fpos) % block_size;


    if(u){

        uint32_t ll = size > (block_size - u) ? (block_size - u) : size;

        r = ext4_fs_get_inode_data_block_index(&ref, sblock, &fblock);
        if(r != EOK)
            goto Finish;

        r = ext4_block_get(ctx->fs.bdev, &b, fblock);
        if(r != EOK)
            goto Finish;

        memcpy(u8_buf, b.data + u, ll);

        r = ext4_block_set(ctx->fs.bdev, &b);
        if(r != EOK)
            goto Finish;

        u8_buf  += ll;
        size    -= ll;
        f->fpos += ll;

        if(rcnt)
            *rcnt += ll;

        sblock++;
    }

    fblock_start = 0;
    fblock_cnt = 0;
    while(size >= block_size){
        while(sblock < sblock_end){
            r = ext4_fs_get_inode_data_block_index(&ref, sblock, &fblock);
            if(r != EOK)
                goto Finish;

            sblock++;

            if(!fblock_start){
                fblock_start = fblock;
            }

            if((fblock_start + fblock_cnt) != fblock)
                break;

            fblock_cnt++;
        }

        r = ext4_blocks_get_direct(ctx->fs.bdev, u8_buf, fblock_start, fblock_cnt);
        if(r != EOK)
            goto Finish;

        size    -= block_size * fblock_cnt;
        u8_buf  += block_size * fblock_cnt;
        f->fpos += block_size * fblock_cnt;

        if(rcnt)
            *rcnt += block_size * fblock_cnt;

        fblock_start = fblock;
        fblock_cnt = 1;
    }

    if(size){
        r = ext4_fs_get_inode_data_block_index(&ref, sblock, &fblock);
        if(r != EOK)
            goto Finish;

        r = ext4_block_get(ctx->fs.bdev, &b, fblock);
        if(r != EOK)
            goto Finish;

        memcpy(u8_buf, b.data , size);

        r = ext4_block_set(ctx->fs.bdev, &b);
        if(r != EOK)
            goto Finish;

        f->fpos += size;

        if(rcnt)
            *rcnt += size;
    }

    Finish:
    ext4_fs_put_inode_ref(&ref);
    unlock(ctx);
    return r;
}

int ext4_fwrite(ext4_fs_t *ctx, ext4_file *f, const void *buf, uint32_t size, uint32_t *wcnt)
{
    int r = EOK;
    uint32_t u;
    uint32_t fblock;
    struct ext4_block b;
    const uint8_t *u8_buf = buf;
    struct ext4_inode_ref ref;
    uint32_t sblock;
    uint32_t sblock_end;
    uint32_t file_blocks;
    uint32_t block_size;
    uint32_t fblock_start;
    uint32_t fblock_cnt;

    if (!ctx || !buf)
        return EINVAL;

    if(f->flags == O_RDONLY)
        return EPERM;

    if(!size)
        return EOK;

    lock(ctx);

    if(wcnt)
        *wcnt = 0;

    r = ext4_fs_get_inode_ref(&ctx->fs, f->inode, &ref);
    if(r != EOK){
        unlock(ctx);
        return r;
    }

    /*Sync file size*/
    f->fsize = ext4_inode_get_size(&ctx->fs.sb, ref.inode);

    block_size = ext4_sb_get_block_size(&ctx->fs.sb);

    sblock_end = (f->fpos + size) > f->fsize ? (f->fpos + size) : f->fsize;
    sblock_end /= block_size;
    file_blocks = (f->fsize / block_size);

    if(f->fsize % block_size)
        file_blocks++;

    sblock = (f->fpos) / block_size;

    u = (f->fpos) % block_size;


    if(u){
        uint32_t ll = size > (block_size - u) ? (block_size - u) : size;

        r = ext4_fs_get_inode_data_block_index(&ref, sblock, &fblock);
        if(r != EOK)
            goto Finish;

        r = ext4_block_get(ctx->fs.bdev, &b, fblock);
        if(r != EOK)
            goto Finish;

        memcpy(b.data + u, u8_buf, ll);
        b.dirty = true;

        r = ext4_block_set(ctx->fs.bdev, &b);
        if(r != EOK)
            goto Finish;

        u8_buf  += ll;
        size    -= ll;
        f->fpos += ll;

        if(wcnt)
            *wcnt += ll;

        sblock++;
    }


    /*Start write back cache mode.*/
    r = ext4_block_cache_write_back(ctx->fs.bdev, true);
    if(r != EOK)
        goto Finish;

    fblock_start = 0;
    fblock_cnt = 0;
    while(size >= block_size){

        while(sblock < sblock_end){
            if(sblock < file_blocks){
                r = ext4_fs_get_inode_data_block_index(&ref, sblock, &fblock);
                if(r != EOK)
                    break;
            }
            else {
                r = ext4_fs_append_inode_block(&ref, &fblock, &sblock);
                if(r != EOK)
                    break;
            }

            sblock++;

            if(!fblock_start){
                fblock_start = fblock;
            }

            if((fblock_start + fblock_cnt) != fblock)
                break;

            fblock_cnt++;
        }

        r = ext4_blocks_set_direct(ctx->fs.bdev, u8_buf, fblock_start, fblock_cnt);
        if(r != EOK)
            break;

        size    -= block_size * fblock_cnt;
        u8_buf  += block_size * fblock_cnt;
        f->fpos += block_size * fblock_cnt;

        if(wcnt)
            *wcnt += block_size * fblock_cnt;

        fblock_start = fblock;
        fblock_cnt = 1;
    }

    /*Stop write back cache mode*/
    ext4_block_cache_write_back(ctx->fs.bdev, false);

    if(r != EOK)
        goto Finish;

    if(size){
        ext4_inode_set_modification_time(ref.inode, ext4_now);

        if(sblock < file_blocks){
            r = ext4_fs_get_inode_data_block_index(&ref, sblock, &fblock);
            if(r != EOK)
                goto Finish;
        }
        else {
            r = ext4_fs_append_inode_block(&ref, &fblock, &sblock);
            if(r != EOK)
                goto Finish;
        }

        r = ext4_block_get(ctx->fs.bdev, &b, fblock);
        if(r != EOK)
            goto Finish;

        memcpy(b.data, u8_buf , size);
        b.dirty = true;

        r = ext4_block_set(ctx->fs.bdev, &b);
        if(r != EOK)
            goto Finish;

        f->fpos += size;

        if(wcnt)
            *wcnt += size;
    }

    if(f->fpos > f->fsize){
        f->fsize = f->fpos;
        ext4_inode_set_size(ref.inode, f->fsize);
        ref.dirty = true;
    }

    Finish:
    ext4_fs_put_inode_ref(&ref);
    unlock(ctx);
    return r;

}

int ext4_fseek(ext4_fs_t *ctx, ext4_file *f, uint64_t offset, uint32_t origin)
{
    if (!ctx || !f)
        return EINVAL;

    switch(origin){
    case SEEK_SET:
        if(offset > f->fsize)
            return EINVAL;

        f->fpos = offset;
        return EOK;

    case SEEK_CUR:
        if((offset + f->fpos) > f->fsize)
            return EINVAL;

        f->fpos += offset;
        return EOK;

    case SEEK_END:
        if(offset > f->fsize)
            return EINVAL;

        f->fpos = f->fsize - offset;
        return EOK;
    }

    return EINVAL;
}

uint64_t ext4_ftell (ext4_fs_t *ctx, ext4_file *f)
{
    if (!ctx || !f)
        return 0;
    else
        return f->fpos;
}

int ext4_fstat(ext4_fs_t *ctx, ext4_file *f, struct ext4_filestat *stat)
{
    if (!ctx || !f || !stat)
        return EINVAL;

    lock(ctx);

    struct ext4_inode_ref iref;
    int r = ext4_fs_get_inode_ref(&ctx->fs, f->inode, &iref);
    if (r == EOK) {

        struct ext4_inode *inode = iref.inode;

        stat->st_atime = ext4_inode_get_access_time(inode);
        stat->st_dev   = 0;
        stat->st_gid   = ext4_inode_get_gid(inode);
        stat->st_ino   = f->inode;
        stat->st_mode  = ext4_inode_get_mode(&iref.fs->sb, inode) & 01777;
        stat->st_mtime = ext4_inode_get_modification_time(inode);
        stat->st_size  = f->fsize;
        stat->st_type  = EXT4_DIRENTRY_REG_FILE;
        stat->st_uid   = ext4_inode_get_uid(inode);

        ext4_fs_put_inode_ref(&iref);
    }

    unlock(ctx);
    return r;
}

int ext4_stat(ext4_fs_t *ctx, const char *path, struct ext4_filestat *stat)
{
    if (!ctx || !path || !stat)
        return EINVAL;

    lock(ctx);

    struct ext4_file f;
    int r = ext4_generic_open(ctx, &f, path, O_RDONLY, true, NULL);
    if (r == EOK){
        r = ext4_fstat(ctx, &f, stat);
    }

    unlock(ctx);
    return r;
}

int ext4_chmod(ext4_fs_t *ctx, const char *path, uint32_t mode)
{
    if (!ctx || !path)
        return EINVAL;

    lock(ctx);

    struct ext4_file f;
    int r = ext4_generic_open(ctx, &f, path, O_RDONLY, true, NULL);
    if (r == EOK){
        struct ext4_inode_ref iref;
        int r = ext4_fs_get_inode_ref(&ctx->fs, f.inode, &iref);
        if (r == EOK) {
            ext4_inode_set_mode(&ctx->fs.sb, iref.inode, mode);
            ext4_fs_put_inode_ref(&iref);
        }
    }

    unlock(ctx);
    return r;
}

int ext4_chown(ext4_fs_t *ctx, const char *path, uint32_t uid, uint32_t gid)
{
    if (!ctx || !path)
        return EINVAL;

    lock(ctx);

    struct ext4_file f;
    int r = ext4_generic_open(ctx, &f, path, O_RDONLY, true, NULL);
    if (r == EOK){
        struct ext4_inode_ref iref;
        int r = ext4_fs_get_inode_ref(&ctx->fs, f.inode, &iref);
        if (r == EOK) {
            ext4_inode_set_uid(iref.inode, uid);
            ext4_inode_set_gid(iref.inode, gid);
            ext4_fs_put_inode_ref(&iref);
        }
    }

    unlock(ctx);
    return r;
}

/*********************************DIRECTORY OPERATION************************/

int ext4_dir_rm(ext4_fs_t *ctx, const char *path)
{
    int r;
    int len;
    ext4_file f;

    struct ext4_inode_ref current;
    struct ext4_inode_ref child;
    struct ext4_directory_iterator it;

    uint32_t inode_up;
    uint32_t inode_current;
    uint32_t depth = 1;

    bool has_children;
    bool is_goal;
    bool dir_end;

    if (!ctx || !path)
        return EINVAL;

    path = correct_path(path);

    lock(ctx);

    /*Check if exist.*/
    r = ext4_generic_open(ctx, &f, path, O_RDONLY, false, &inode_up);
    if(r != EOK){
        unlock(ctx);
        return r;
    }

    len = ext4_path_check(path, &is_goal);

    inode_current = f.inode;
    dir_end = false;

    ext4_block_cache_write_back(ctx->fs.bdev, 1);

    do {
        /*Load directory node.*/
        r = ext4_fs_get_inode_ref(&ctx->fs, inode_current, &current);
        if(r != EOK){
            break;
        }

        /*Initialize iterator.*/
        r = ext4_dir_iterator_init(&it, &current, 0);
        if(r != EOK){
            ext4_fs_put_inode_ref(&current);
            break;
        }

        while(r == EOK){

            if(!it.current){
                dir_end = true;
                break;
            }

            /*Get up directory inode when ".." entry*/
            if((it.current->name_length == 2) &&
                    ext4_is_dots(it.current->name, it.current->name_length)){
                inode_up = it.current->inode;
            }

            /*If directory or file entry,  but not "." ".." entry*/
            if(!ext4_is_dots(it.current->name, it.current->name_length)){

                /*Get child inode reference do unlink directory/file.*/
                r = ext4_fs_get_inode_ref(&ctx->fs, it.current->inode, &child);
                if(r != EOK)
                    break;

                /*If directory with no leaf children*/
                r = ext4_has_children(&has_children, &child);
                if(r != EOK){
                    ext4_fs_put_inode_ref(&child);
                    break;
                }

                if(has_children){
                    /*Has directory children. Go into this tirectory.*/
                    inode_up = inode_current;
                    inode_current = it.current->inode;
                    depth++;
                    ext4_fs_put_inode_ref(&child);
                    break;
                }

                /*Directory is empty. Truncate it.*/
                r = ext4_fs_truncate_inode(&child, 0);
                if(r != EOK){
                    ext4_fs_put_inode_ref(&child);
                    break;
                }

                /*No children in child directory or file. Just unlink.*/
                r = ext4_unlink(ctx, &current, &child,
                        (char *)it.current->name, it.current->name_length);
                if(r != EOK){
                    ext4_fs_put_inode_ref(&child);
                    break;
                }

                r = ext4_fs_free_inode(&child);
                if(r != EOK){
                    ext4_fs_put_inode_ref(&child);
                    break;
                }

                r = ext4_fs_put_inode_ref(&child);
                if(r != EOK)
                    break;
            }

            r = ext4_dir_iterator_next(&it);
        }

        if(dir_end){
            /*Directory iterator reached last entry*/
            ext4_has_children(&has_children, &current);
            if(!has_children){
                inode_current = inode_up;
                if(depth)
                    depth--;
            }
            /*Last unlink*/
            if(!depth){
                /*Load parent.*/
                struct ext4_inode_ref parent;
                r = ext4_fs_get_inode_ref(&ctx->fs, inode_up, &parent);
                if(r != EOK)
                    goto End;

                r = ext4_fs_truncate_inode(&current, 0);
                if(r != EOK){
                    ext4_fs_put_inode_ref(&parent);
                    goto End;
                }

                /* In this place all directories should be unlinked.
                 * Last unlink from root of current directory*/
                r = ext4_unlink(ctx, &parent, &current, path, len);
                if(r != EOK){
                    ext4_fs_put_inode_ref(&parent);
                    goto End;
                }

                r = ext4_fs_free_inode(&current);
                if(r != EOK){
                    ext4_fs_put_inode_ref(&parent);
                    goto End;
                }

                r = ext4_fs_put_inode_ref(&parent);
                if(r != EOK)
                    goto End;
            }
        }

        End:
        ext4_dir_iterator_fini(&it);
        ext4_fs_put_inode_ref(&current);
        dir_end = false;

        /*When something goes wrong. End loop.*/
        if(r != EOK)
            break;

    }while(depth);

    ext4_block_cache_write_back(ctx->fs.bdev, 0);
    unlock(ctx);
    return r;
}

int ext4_dir_mk(ext4_fs_t *ctx, const char *path)
{
    int r;
    ext4_file f;

    if (!ctx || !path)
        return EINVAL;

    lock(ctx);

    /*Check if exist.*/
    r = ext4_generic_open(ctx, &f, path, O_RDONLY, false, 0);
    if(r == EOK){
        /*Directory already created*/
        unlock(ctx);
        return r;
    }

    /*Create new dir*/
    r = ext4_generic_open(ctx, &f, path, O_WRONLY|O_CREAT|O_TRUNC, false, 0);
    if(r != EOK){
        unlock(ctx);
        return r;
    }

    unlock(ctx);
    return r;
}

int ext4_dir_open(ext4_fs_t *ctx, ext4_dir *d, const char *path)
{
    if (!ctx || !path)
        return EINVAL;

    lock(ctx);
    int r = ext4_generic_open(ctx, &d->f, path, O_RDONLY, false, NULL);
    unlock(ctx);
    return r;
}

int ext4_dir_close(ext4_fs_t *ctx, ext4_dir *d)
{
    return ext4_fclose(ctx, &d->f);
}

ext4_direntry* ext4_dir_entry_get(ext4_fs_t *ctx, ext4_dir *d, uint32_t id)
{
    int r;
    uint32_t i;
    ext4_direntry *de = 0;
    struct ext4_inode_ref dir;
    struct ext4_directory_iterator it;

    if (!ctx || !d)
        return NULL;

    lock(ctx);

    r = ext4_fs_get_inode_ref(&ctx->fs, d->f.inode, &dir);
    if(r != EOK){
        goto Finish;
    }

    r = ext4_dir_iterator_init(&it, &dir, 0);
    if(r != EOK){
        ext4_fs_put_inode_ref(&dir);
        goto Finish;
    }

    i = 0;
    while(r == EOK){

        if(!it.current)
            break;

        if(i == id){
            memcpy(&d->de, it.current, sizeof(ext4_direntry));
            de = &d->de;
            break;
        }

        i++;
        r = ext4_dir_iterator_next(&it);
    }

    ext4_dir_iterator_fini(&it);
    ext4_fs_put_inode_ref(&dir);

    Finish:
    unlock(ctx);
    return de;
}

/**
 * @}
 */
