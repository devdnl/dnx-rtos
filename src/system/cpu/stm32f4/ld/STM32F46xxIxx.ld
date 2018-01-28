SEARCH_DIR(.);
SEARCH_DIR(src/system/cpu/stm32f4/ld);

MEMORY
{
   rom  (rx)  : org = 0x08000000, len = 2048k
   ram  (rwx) : org = 0x20000000, len = 160k
   ram2 (rwx) : org = 0x20028000, len = 32k
   ram3 (rwx) : org = 0x20030000, len = 128k
   ccm  (rw)  : org = 0x10000000, len = 64k
   bkp  (rwx) : org = 0x40024000, len = 4k
}

INCLUDE common.ld
