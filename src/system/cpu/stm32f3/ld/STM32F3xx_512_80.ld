SEARCH_DIR(.);
SEARCH_DIR(src/system/cpu/stm32f3/ld);

MEMORY
{
   rom  (rx)  : org = 0x08000000, len = 512k
   ram  (rwx) : org = 0x20000000, len = 80k
}

INCLUDE common.ld
