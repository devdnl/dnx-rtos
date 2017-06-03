#define DATA_CHAIN_SIZE                 __RAMFS_FILE_CHAIN_SIZE__
/** regular file data chain */
typedef struct data_chain {
        struct data_chain *next;
        u8_t buf[DATA_CHAIN_SIZE];
} data_chain_t;


        union {
                pipe_t       *pipe_t;
                llist_t      *llist_t;
                data_chain_t *data_chain_t;
                dev_t         dev_t;
        } data;
static int  add_node_to_open_files_list (struct RAMFS *hdl, node_t *parent, node_t *child);
static void clear_regular_file          (node_t *node);
static int  write_regular_file          (node_t *node, const u8_t *src, size_t count, fpos_t fpos, size_t *wrcnt);
static int  read_regular_file           (node_t *node, u8_t *dst, size_t count, fpos_t fpos, size_t *rdcnt);
        if (!err) {
                if (err)
                err = sys_llist_create(NULL, NULL, cast(llist_t**, &hdl->root_dir.data.llist_t));
                if (err)
                if (err)
                if (err) {
                        if (hdl->root_dir.data.llist_t)
                                sys_llist_destroy(hdl->root_dir.data.llist_t);
        if (!err) {
                if (!err) {
                        if (!err) {
                                if (!err) {
                                        child->data.dev_t = dev;
        if (!err) {
                if (!err) {
                        if (!err) {
                                if (!err) {
        if (!err) {
                if (!err) {
                        if (!err) {
                                if (!err) {
        if (!err) {
                if (!err) {
                                dir->d_items    = sys_llist_size(parent->data.llist_t);
        if (!err) {
                node_t *child  = sys_llist_at(parent->data.llist_t, dir->d_seek++);
                                err = sys_driver_stat(child->data.dev_t, &dev_stat);
                                if (!err) {
                                dir->dirent.dev = child->data.dev_t;
        if (!err) {
                if (err){
                if (err) {
        if (!err) {
                if (!err) {
                        if (!err) {
        if (!err) {
                if (!err) {
        if (!err) {
                if (!err) {
        if (!err) {
                if (!err) {
                                        stat->st_dev = target->data.dev_t;
                                        err = sys_driver_stat(target->data.dev_t,
                                                              &dev_stat);
                                        if (!err) {
        if (!err) {
                                stat->st_dev = opened_file->child->data.dev_t;
                                err = sys_driver_stat(opened_file->child->data.dev_t,
                                                      &dev_stat);
                                if (!err) {
        if (!err) {
                if (err) {
                        if (err) {
                        if (err) {
                if (err) {
                                clear_regular_file(child);
                        err = sys_driver_open(child->data.dev_t, flags);
                        if (!err) {
        if (!err) {
                                err = sys_driver_close(target->data.dev_t, force);
                                err = sys_pipe_close(target->data.pipe_t);
                                if (opened_file->remove_at_close) {
                                                        break;
                                                                  opened_file->parent,
                                                                  opened_file->child,
                                                                  pos);
                        if (!err) {
        if (!err) {
                        node_t *node = opened_file->child;
                        sys_get_time(&node->mtime);
                        if (node->type == FILE_TYPE_DRV) {
                                return sys_driver_write(node->data.dev_t, src,
                                                        count, fpos, wrcnt, fattr);
                        } else if (node->type == FILE_TYPE_PIPE) {
                               err = sys_pipe_write(node->data.pipe_t, src,
                                                    count, wrcnt, fattr.non_blocking_wr);
                               if (!err) {
                                               err = sys_pipe_get_length(node->data.pipe_t,
                                                                         &size);
                                               node->size = size;
                       } else if (node->type == FILE_TYPE_REGULAR) {
                               err = write_regular_file(node, src, count, *fpos, wrcnt);
        if (!err) {
                        node_t *node = opened_file->child;
                        if (node->type == FILE_TYPE_DRV) {
                                return sys_driver_read(node->data.dev_t, dst,
                                                       count, fpos, rdcnt, fattr);
                        } else if (node->type == FILE_TYPE_PIPE) {
                                err = sys_pipe_read(node->data.pipe_t, dst,
                                                    count, rdcnt, fattr.non_blocking_rd);
                                if (!err) {
                                                err = sys_pipe_get_length(node->data.pipe_t,
                                                                          &size);
                                                node->size = size;
                        } else if (node->type == FILE_TYPE_REGULAR) {
                                err = read_regular_file(node, dst, count, *fpos, rdcnt);
        if (!err) {
                                return sys_driver_ioctl(opened_file->child->data.dev_t,
                                                        request, arg);
                                        return sys_pipe_close(opened_file->child->data.pipe_t);
                                        return sys_pipe_clear(opened_file->child->data.pipe_t);
        if (!err) {
                                return sys_driver_flush(opened_file->child->data.dev_t);
                if (sys_llist_size(target->data.llist_t) > 0) {
                        sys_llist_destroy(target->data.llist_t);
                        target->data.llist_t = NULL;
                if (target->data.pipe_t) {
                        sys_pipe_destroy(target->data.pipe_t);
                        target->data.pipe_t = NULL;

        } else if (target->type == FILE_TYPE_REGULAR) {
                clear_regular_file(target);
        sys_llist_erase(base->data.llist_t, position);
                int list_size = sys_llist_size(current_node->data.llist_t);
                        node_t *next_node = sys_llist_at(current_node->data.llist_t, i++);
        sys_llist_foreach(node_t*, node, parent->data.llist_t) {
        if (!err) {
                node->name         = filename;
                node->data.llist_t = NULL;
                node->gid          = 0;
                node->uid          = 0;
                node->mode         = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
                node->mtime        = tm;
                node->ctime        = tm;
                node->size         = 0;
                node->type         = type;
                if (!err) {
                        if (sys_llist_push_back(parent->data.llist_t, node)) {
                                        *item = sys_llist_size(parent->data.llist_t) - 1;
                if (err) {
static int add_node_to_open_files_list(struct RAMFS *hdl,
                                       node_t       *parent,
                                       node_t       *child)
        if (!err) {
                sys_llist_foreach(struct opened_file_info*, opened_file, hdl->opended_files) {
                if (!sys_llist_push_back(hdl->opended_files, opened_file_info)) {
//==============================================================================
/**
 * @brief Function clear data of regular file.
 *
 * @param node                  node to write
 */
//==============================================================================
static void clear_regular_file(node_t *node)
{
        data_chain_t *chain = node->data.data_chain_t;

        while (chain) {
                data_chain_t *next = chain->next;
                sys_free(cast(void**, &chain));
                chain = next;
        }

        node->size = 0;
        node->data.data_chain_t = NULL;
}

//==============================================================================
/**
 * @brief Function write data to regular file.
 *
 * @param node                  node to write
 * @param src                   source buffer
 * @param count                 number of bytes to write
 * @param fpos                  file seek
 * @param wrcnt                 write counter
 *
 * @retval One of errno value (errno.h)
 */
//==============================================================================
static int write_regular_file(node_t *node, const u8_t *src,
                              size_t count, fpos_t fpos, size_t *wrcnt)
{
        int           err   = ESUCC;
        data_chain_t *chain = node->data.data_chain_t;
        size_t        depth = fpos / DATA_CHAIN_SIZE;
        size_t        seek  = fpos - (depth * DATA_CHAIN_SIZE);

        if (chain == NULL) {
                err = sys_zalloc(sizeof(data_chain_t), cast(void**, &chain));
                if (!err) {
                        node->data.data_chain_t = chain;
                }
        }

        // travel to selected chain
        while (!err && depth) {
                if (chain->next == NULL) {
                        err = sys_zalloc(sizeof(data_chain_t),
                                         cast(void**, &chain->next));
                }

                chain = chain->next;
                depth--;
        }

        // at position
        while (!err && count) {
                size_t tocpy = min(DATA_CHAIN_SIZE - seek, count);
                memcpy(&chain->buf[seek], src, tocpy);
                src    += tocpy;
                fpos   += tocpy;
                *wrcnt += tocpy;
                count  -= tocpy;
                seek    = 0;

                if (count) {
                        if (chain->next == NULL) {
                                err = sys_zalloc(sizeof(data_chain_t),
                                                 cast(void**, &chain->next));
                        }

                        chain = chain->next;
                }
        }

        // calculate file size
        if (depth == 0) {
                node->size = max(node->size, fpos);
        }

        return err;
}

//==============================================================================
/**
 * @brief Function read data from regular file.
 *
 * @param node                  node to read
 * @param dst                   destination buffer
 * @param count                 number of bytes to read
 * @param fpos                  file seek
 * @param rdcnt                 read counter
 *
 * @retval One of errno value (errno.h)
 */
//==============================================================================
static int read_regular_file(node_t *node, u8_t *dst,
                             size_t count, fpos_t fpos, size_t *rdcnt)
{
        int err = ESUCC;

        data_chain_t *chain = node->data.data_chain_t;
        size_t        depth = fpos / DATA_CHAIN_SIZE;
        size_t        seek  = fpos - (depth * DATA_CHAIN_SIZE);

        for (; chain && depth; chain = chain->next, depth--);

        while (chain && count && fpos < node->size) {
                size_t tocpy = min(DATA_CHAIN_SIZE - seek, count);
                       tocpy = min(tocpy, node->size - fpos);

                memcpy(dst, &chain->buf[seek], tocpy);
                dst    += tocpy;
                fpos   += tocpy;
                *rdcnt += tocpy;
                count  -= tocpy;
                seek    = 0;

                chain = chain->next;
        }

        return err;
}
