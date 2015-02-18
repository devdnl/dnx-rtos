
//==============================================================================
/**
 * @brief  Function initialize storage device
 * @param  storage  storage device path
 * @return None
 */
//==============================================================================
static void init_storage(const char *storage)
{
        printk("Initializing %s... ", storage);
        FILE *st = fopen(storage, "r+");
        if (st) {
                if (ioctl(st, IOCTL_STORAGE__INITIALIZE)) {
                        switch (ioctl(st, IOCTL_STORAGE__READ_MBR)) {
                                case 1 : printk("OK\n"); break;
                                case 0 : printk("OK (no MBR)\n"); break;
                                default: printk(FONT_COLOR_RED"read error"RESET_ATTRIBUTES"\n");
                        }
                } else {
                        printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
                }

                fclose(st);
        } else {
                printk(FONT_COLOR_RED"no such file"RESET_ATTRIBUTES"\n");
        }
}
