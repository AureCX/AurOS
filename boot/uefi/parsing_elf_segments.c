#include <efi.h>
#include <efilib.h>
#include "elf.h"

EFI_STATUS CheckMagic(UINT8 *Buffer)
{
  if (Buffer[0] == 0x7F && Buffer[1] == 'E' && Buffer[2] == 'L' && Buffer[3] == 'F') {
    Print(L"Valid ELF!\r\n");
    return EFI_SUCCESS;
  }
  Print(L"Invalid ELF!\r\n");
  return EFI_INVALID_PARAMETER;
}

Elf64_Ehdr *ParseElfHeader(UINT8 *Buffer, UINT64 HeaderSize)
{
  if (HeaderSize < sizeof(Elf64_Ehdr)) {
    Print(L"Buffer size is too small to contain an ELF header\r\n");
    return NULL;
  }
  Elf64_Ehdr *Header = (Elf64_Ehdr *)Buffer;
  Print(L"etype: %d\r\n", Header->e_type);
  Print(L"entry: 0x%lx\r\n", Header->e_entry);
  Print(L"ELF: %d\r\n", Header->e_ident[4]);
  Print(L"Little endian: %d\r\n", Header->e_ident[5]);
  Print(L"ELF V1: %d\r\n", Header->e_ident[6]);
  Print(L"Machine type: %d\r\n", Header->e_machine);
  Print(L"ELF version: %d\r\n", Header->e_version);
  Print(L"Header size %d\r\n", Header->e_ehsize);
  Print(L"Program header count: %d\r\n\n", Header->e_phnum);
  return Header;
}

Elf64_Phdr *ParseProgramHeaders(UINT8 *Buffer, UINT64 BufferSize, Elf64_Ehdr *Header)
{
  Elf64_Phdr *ProgramHeaders = NULL;

  if (Header->e_phoff + (Header->e_phnum * Header->e_phentsize) > BufferSize) {
    Print(L"Buffer size is too small to contain the program headers\r\n");
    return NULL;
  }
  ProgramHeaders = (Elf64_Phdr *)(Buffer + Header->e_phoff);
  Print(L"Program Header 0 - Virtual Address: 0x%lx\r\n", ProgramHeaders[0].p_vaddr);
  Print(L"Program Header 1 - Virtual Address: 0x%lx\r\n", ProgramHeaders[1].p_vaddr);
  Print(L"Program Header 2 - Virtual Address: 0x%lx\r\n", ProgramHeaders[2].p_vaddr);
  return ProgramHeaders;
  // FOR FUTURE TESTS 
  // for (int i = 0; i < Header->e_phnum; ++i) {
  //   ProgramHeader = (Elf64_Phdr *)(Buffer + Header->e_phoff + (i * Header->e_phentsize));
  //   Print(L"Program Header %d\r\n", i);
  //   Print(L"Type: %d\r\n", ProgramHeader->p_type);
  //   Print(L"Flags: %d\r\n", ProgramHeader->p_flags);
  //   Print(L"Offset: 0x%lx\r\n", ProgramHeader->p_offset);
  //   Print(L"Virtual Address: 0x%lx\r\n", ProgramHeader->p_vaddr);
  //   Print(L"File Size: 0x%lx\r\n", ProgramHeader->p_filesz);
  //   Print(L"Memory Size: 0x%lx\r\n", ProgramHeader->p_memsz);
  //   Print(L"Alignment: 0x%lx\r\n", ProgramHeader->p_align);
  // }
}