/* Basic linker script for .text at TEXT_START_ADDR and .data; .bss at DATA_START_ADDR */
OUTPUT_ARCH("riscv")

SECTIONS
{
  /* text: Code segment */
  . = TEXT_START_ADDR;
  .text :
  {
    *startup.o (.text);
    *(.text*);
  }
  text_end = .;
  /* data: Initialized data segment */
  . = DATA_START_ADDR;
  .data :
  {
    *startup.o (.data);
    *(.rodata*);
    *(.sdata*);
    *(.data*);
  }
  bss_start = .;
  /* bss: Uninitialized data segment */
  .bss :
  {
    *(.bss*);
    ASSERT ((bss_end < 0x20001bfc), "Error: No room left for the thread 0's stack, cause data&bss is too large");
    ASSERT ((text_end < 0x00003fff), "Error: No room left for the text");
  }
  bss_end = .;
  bss_size = bss_end - bss_start;

}
