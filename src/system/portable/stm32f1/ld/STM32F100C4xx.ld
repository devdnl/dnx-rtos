SEARCH_DIR(.);
SEARCH_DIR(src/system/portable/stm32f1/ld);

MEMORY
{
   rom (rx)  : org = 0x08000000, len = 16k
   ram (rwx) : org = 0x20000000, len = 4k
}

INCLUDE stm32_common.ld
