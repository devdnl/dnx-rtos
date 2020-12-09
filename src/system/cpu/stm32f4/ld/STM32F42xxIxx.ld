SEARCH_DIR(.);
SEARCH_DIR(src/system/cpu/stm32f4/ld);

MEMORY
{
   rom  (rx)  : org = 0x08040000, len = 1792k
   ram  (rwx) : org = 0x20000000, len = 112k
   ram2 (rwx) : org = 0x2001C000, len = 16k
   ram3 (rwx) : org = 0x20020000, len = 64k
   ccm  (rw)  : org = 0x10000000, len = 64k
   bkp  (rwx) : org = 0x40024000, len = 4k
}

INCLUDE common.ld
