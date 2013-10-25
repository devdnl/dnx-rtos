/*******************************************************************************
* author: Daniel Zorychta, danz@jabster.pl
*
* File is based on Freddie Chopin's linker script
*
* chip: STM32F107xBxx
* compiler: arm-none-eabi-gcc
*
*******************************************************************************/

SEARCH_DIR(.);
SEARCH_DIR(src/system/portable/stm32f1/);

/*==============================================================================
stacks sizes
==============================================================================*/
/*
 * Handler mode (core exceptions / interrupts) can use only main stack.
 * Thread mode can use main stack (default) or process stack - selected in
 * CONTROL special register
 */
__main_stack_size    = 256;
__process_stack_size = 0;

/*==============================================================================
available memories definitions
==============================================================================*/
MEMORY
{
   rom (rx)  : org = 0x08000000, len = 128k
   ram (rwx) : org = 0x20000000, len = 48k
}

INCLUDE stm32_common.ld

/*******************************************************************************
END OF FILE
*******************************************************************************/