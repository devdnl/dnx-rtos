SEARCH_DIR(.);
SEARCH_DIR(src/system/cpu/stm32h7/ld);

MEMORY
{
   flash   (rx)  : org = 0x08000000, len = 1024k
   axisram (rwx) : org = 0x24000000, len = 384k
   sram1   (rwx) : org = 0x30000000, len = 32k
   sram2   (rwx) : org = 0x30020000, len = 16k
   sram4   (rwx) : org = 0x38000000, len = 64k
   itcm    (rwx) : org = 0x00000000, len = 64k
   dtcm    (rw)  : org = 0x20000000, len = 128k
   bkp     (rw)  : org = 0x38800000, len = 4k
}

INCLUDE common.ld
