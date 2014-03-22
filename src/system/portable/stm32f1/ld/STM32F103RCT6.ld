SEARCH_DIR(.);
SEARCH_DIR(src/system/portable/stm32f1/ld);

MEMORY
{
rom (rx)  : org = 0x08000000, len = 256k
ram (rwx) : org = 0x20000000, len = 48k
}

INCLUDE stm32_common.ld
