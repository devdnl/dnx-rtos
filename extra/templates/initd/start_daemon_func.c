
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
        uint  name_len  = strlen(name);
        char *arg_start = strchr(name, ' ');
        if (arg_start) {
                name_len = arg_start - name;
        }

        printk("Starting '%.*s' daemon... ", name_len, name);
        if (program_new(name, cwd, NULL, NULL, NULL)) {
                printk("OK\n");
        } else {
                printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
        }
}
