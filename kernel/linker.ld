ENTRY(kernel_main)
OUTPUT_FORMAT(elf64-x86-64)
SECTIONS
{
    . = 0x00100000;
  .text : {
_TEXT_START_ = .;
    *(.text)
_TEXT_END_ = .;
  }

  .rodata : {
_RODATA_START_ = .;
    *(.rodata)
_RODATA_END_ = .;
  }

  .data : {
_DATA_START_ = .;
    *(.data)
_DATA_END_ = .;
  }

  .bss : {
_BSS_START_ = .;
    *(.bss)
    *(COMMON)
_BSS_END_ = .;
  }
}