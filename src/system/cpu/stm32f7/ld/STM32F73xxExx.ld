SEARCH_DIR(.);
SEARCH_DIR(src/system/cpu/stm32f7/ld);

MEMORY
{
   rom  (rx)  : org = 0x08000000, len = 512k
   dtcm (rwx) : org = 0x20000000, len = 64k
   itcm (rwx) : org = 0x00000000, len = 16k
   ram1 (rwx) : org = 0x20010000, len = 176k
   ram2 (rwx) : org = 0x2003C000, len = 16k
   bkp  (rwx) : org = 0x40024000, len = 4k
}

INCLUDE common.ld
