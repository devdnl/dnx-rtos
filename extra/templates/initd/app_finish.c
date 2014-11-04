
        /* waits until all applications are closed */
        while (true) {
                size_t closed_programs = 0;
                for (size_t i = 0; i < ARRAY_SIZE(p); i++) {
                        if (p[i]) {
                                if (program_is_closed(p[i])) {
                                        program_delete(p[i]);
                                        p[i] = NULL;
                                        closed_programs++;
                                }
                        } else {
                                closed_programs++;
                        }
                }

                if (closed_programs >= ARRAY_SIZE(p)) {
                        for (size_t i = 0; i < ARRAY_SIZE(f); i++) {
                                fclose(f[i]);
                        }

                        break;
                }

                sleep(1);
        }
