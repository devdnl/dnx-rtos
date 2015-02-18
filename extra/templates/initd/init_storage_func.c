
//==============================================================================
/**
 * @brief  Function initialize storage device
 * @param  storage  storage device path
 * @return None
 */
//==============================================================================
static void init_storage(const char *storage)
{
        _printk("Initializing %s... ", storage);
        FILE *st = fopen(storage, "r+");
        if (st) {
                if (ioctl(st, IOCTL_STORAGE__INITIALIZE)) {
                        switch (ioctl(st, IOCTL_STORAGE__READ_MBR)) {
                                case 1 : _printk("OK\n"); break;
                                case 0 : _printk("OK (no MBR)\n"); break;
                                default: _printk(FONT_COLOR_RED"read error"RESET_ATTRIBUTES"\n");
                        }
                } else {
                        _printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
                }

                fclose(st);
        } else {
                _printk(FONT_COLOR_RED"no such file"RESET_ATTRIBUTES"\n");
        }
}
