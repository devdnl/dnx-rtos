SEARCH_DIR(.);
SEARCH_DIR(src/system/portable/efr32/ld);

MEMORY
{
   rom  (rx)  : org = 0x00000000, len = 256k
   ram  (rwx) : org = 0x20000000, len = 32k
}

INCLUDE common.ld
