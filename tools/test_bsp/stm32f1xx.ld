SEARCH_DIR(.);
SEARCH_DIR(src/system/cpu/stm32f1/ld);

MEMORY
{
   rom  (rx)  : org = 0x08000000, len = 2048k
   ram  (rwx) : org = 0x20000000, len = 112k
   bkp  (rwx) : org = 0x40024000, len = 4k
}

INCLUDE common.ld
