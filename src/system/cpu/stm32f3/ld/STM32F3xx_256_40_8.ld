SEARCH_DIR(.);
SEARCH_DIR(src/system/cpu/stm32f3/ld);

MEMORY
{
   rom  (rx)  : org = 0x08000000, len = 256k
   ram  (rwx) : org = 0x20000000, len = 40k
   ccm  (rwx) : org = 0x10000000, len = 8k
}

INCLUDE common.ld
