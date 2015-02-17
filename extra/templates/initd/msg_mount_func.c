
//==============================================================================
/**
 * @brief  Function initialize storage device
 * @param  storage  storage device path
 * @return None
 */
//==============================================================================
static void msg_mount(const char *filesystem, const char *src_file, const char *mount_point)
{
        _printk("Mounting ");
        if (src_file != NULL && strlen(src_file) > 0) {
                _printk("%s ", src_file);
        } else {
                _printk("%s ", filesystem);
        }
        _printk("to %s... ", mount_point);

        errno = 0;
        if (mount(filesystem, src_file, mount_point) == STD_RET_OK) {
                _printk("OK\n");
        } else {
                _printk(FONT_COLOR_RED" fail (%d)"RESET_ATTRIBUTES"\n", errno);
        }
}
