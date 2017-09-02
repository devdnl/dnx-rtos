SEARCH_DIR(.);
SEARCH_DIR(src/system/portable/stm32f4/ld);

MEMORY
{
   rom  (rx)  : org = 0x08000000, len = 256k
   ram  (rwx) : org = 0x20000000, len = 64k
   ram2 (rwx) : org = 0x20040000, len = 0
   ram3 (rwx) : org = 0x20020000, len = 0
}

INCLUDE common.ld
