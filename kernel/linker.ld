ENTRY(kernel_main)
OUTPUT_FORMAT(elf64-x86-64)
PHDRS
{
  text PT_LOAD FLAGS(5);
  rodata PT_LOAD FLAGS(4);
  data PT_LOAD FLAGS(6);
}
SECTIONS
{
    . = 0x00100000;
  .text : {
_TEXT_START_ = .;
    *(.text)
_TEXT_END_ = .;
  } :text

  .rodata : {
_RODATA_START_ = .;
    *(.rodata)
_RODATA_END_ = .;
  } :rodata

  .data : {
_DATA_START_ = .;
    *(.data)
_DATA_END_ = .;
  } :data

  .bss : {
_BSS_START_ = .;
    *(.bss)
    *(COMMON)
_BSS_END_ = .;
  } :data

/DISCARD/ : {
  *(.note.gnu.property);
  *(.eh_frame);
  *(.comment);
  }
}