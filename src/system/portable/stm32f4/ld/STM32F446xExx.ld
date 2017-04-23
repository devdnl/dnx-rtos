SEARCH_DIR(.);
SEARCH_DIR(src/system/portable/stm32f4/ld);

MEMORY
{
   rom  (rx)  : org = 0x08000000, len = 512k
   ram  (rwx) : org = 0x20000000, len = 112k
   ram2 (rwx) : org = 0x2001C000, len = 16k
}

INCLUDE common.ld
