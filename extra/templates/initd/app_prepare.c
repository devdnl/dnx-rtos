
        /* initialize handles for applications and streams */
        prog_t *p[<!number_of_applications!>];
        memset(p, 0, sizeof(p));

        FILE *f[<!number_of_streams!>];
        memset(f, 0, sizeof(f));

        /* open streams and start applications */
