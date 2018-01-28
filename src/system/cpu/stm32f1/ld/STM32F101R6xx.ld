SEARCH_DIR(.);
SEARCH_DIR(src/system/cpu/stm32f1/ld);

MEMORY
{
   rom (rx)  : org = 0x08000000, len = 32k
   ram (rwx) : org = 0x20000000, len = 6k
}

INCLUDE common.ld
