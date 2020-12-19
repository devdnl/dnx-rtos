SEARCH_DIR(.);
SEARCH_DIR(src/system/cpu/stm32f4/ld);

MEMORY
{
   rom  (rx)  : org = 0x08000000, len = 256k
   ram  (rwx) : org = 0x20000000, len = 112k
   ram2 (rwx) : org = 0x2001C000, len = 16k
   ram3 (rwx) : org = 0x20020000, len = 0
   ccm  (rw)  : org = 0x10000000, len = 0
}

INCLUDE common.ld
