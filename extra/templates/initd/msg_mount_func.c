
//==============================================================================
/**
 * @brief  Function initialize storage device
 * @param  storage  storage device path
 * @return None
 */
//==============================================================================
static void msg_mount(const char *filesystem, const char *src_file, const char *mount_point)
{
        printk("Mounting ");
        if (src_file != NULL && strlen(src_file) > 0) {
                printk("%s ", src_file);
        } else {
                printk("%s ", filesystem);
        }
        printk("to %s... ", mount_point);

        errno = 0;
        if (mount(filesystem, src_file, mount_point) == STD_RET_OK) {
                printk("OK\n");
        } else {
                printk(FONT_COLOR_RED" fail (%d)"RESET_ATTRIBUTES"\n", errno);
        }
}
