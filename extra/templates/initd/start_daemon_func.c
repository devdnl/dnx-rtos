
//==============================================================================
/**
 * @brief  Function start daemon
 * @param  name     daemon's name
 * @param  cwd      current working directory
 * @return None
 */
//==============================================================================
static void start_daemon(const char *name, const char *cwd)
{
        _printk("Starting '%s' daemon... ", name);
        if (program_new(name, cwd, NULL, NULL, NULL)) {
                _printk("OK\n");
        } else {
                _printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
        }
}
