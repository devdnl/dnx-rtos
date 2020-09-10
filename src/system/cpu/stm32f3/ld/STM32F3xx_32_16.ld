SEARCH_DIR(.);
SEARCH_DIR(src/system/cpu/stm32f3/ld);

MEMORY
{
   rom  (rx)  : org = 0x08000000, len = 32k
   ram  (rwx) : org = 0x20000000, len = 16k
}

INCLUDE common.ld
