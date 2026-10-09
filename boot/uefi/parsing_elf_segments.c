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

SegmentRange FindSegmentsRange(uint16_t NbProgramHeaders, Elf64_Phdr *ProgramHeaders)
{
  UINT64 LowestAddr = 0;
  UINT64 HighestAddr = 0;
  int FoundSegment = 0;

  for (uint64_t i = 0; i < NbProgramHeaders; ++i) {
    if (ProgramHeaders[i].p_type == PT_LOAD) {
      UINT64 SegmentStart = ProgramHeaders[i].p_vaddr;
      UINT64 SegmentEnd = ProgramHeaders[i].p_vaddr + ProgramHeaders[i].p_memsz;

      if (!FoundSegment) {
        LowestAddr = SegmentStart;
        HighestAddr = SegmentEnd;
        FoundSegment = 1;
      }
      if (SegmentStart < LowestAddr)
          LowestAddr = SegmentStart;
      if (SegmentEnd > HighestAddr)
          HighestAddr = SegmentEnd;
    }
  }
  if (!FoundSegment)
    return (SegmentRange){0, 0, 0};
  if (LowestAddr != 0 && LowestAddr % 4096 != 0)
    for (; LowestAddr % 4096 != 0; --LowestAddr);
  return (SegmentRange){LowestAddr, HighestAddr, HighestAddr - LowestAddr};
}

EFI_STATUS LoadSegment(UINT8 *Buffer, UINT64 BufferSize, Elf64_Phdr *ProgramHeader)
{
  if (ProgramHeader->p_offset + ProgramHeader->p_filesz > BufferSize) {
    Print(L"Buffer size is too small to contain the segment data\r\n");
    return EFI_INVALID_PARAMETER;
  }
  if (ProgramHeader->p_filesz > ProgramHeader->p_memsz)
  {
    Print(L"Segment file size is larger than its memory size\r\n");
    return EFI_INVALID_PARAMETER;
  }
  Print(L"Segment is valid!\r\n");
  UINT8 *src = Buffer + ProgramHeader->p_offset;
  UINT8 *dest = (UINT8 *)(UINTN)ProgramHeader->p_vaddr;
  UINT64 size = ProgramHeader->p_filesz;
  Print(L"Src: 0x%1x, Dest: 0x%1x, Size: %d\r\n", src, dest, size);
  Print(L"Memsize %d\r\n", ProgramHeader->p_memsz);
  CopyMem(dest, src, size);
  for (UINT64 i = 0; i < size; ++i)
    if (dest[i] != src[i]) {
      Print(L"CopyMem failed at byte %d\r\n", i);
      return EFI_OUT_OF_RESOURCES;
    }
  Print(L"Every Segment byte matches!\r\n");
  UINT64 BssSize = ProgramHeader->p_memsz - ProgramHeader->p_filesz;
  if (BssSize > 0) {
    SetMem(dest + size, BssSize, 0);
    for (UINT64 i = 0; i < BssSize; ++i) {
      if (dest[size + i] != 0) {
        Print(L".bss section isn't zeroed out.\r\n");
        return EFI_ABORTED;
      }
    }
    Print(L".bss section is set to 0.\r\n");
  }
  return EFI_SUCCESS;
}

Elf64_Phdr *ParseProgramHeaders(UINT8 *Buffer, UINT64 BufferSize, Elf64_Ehdr *Header)
{
  Elf64_Phdr *ProgramHeaders = NULL;

  if (Header->e_phoff + (Header->e_phnum * Header->e_phentsize) > BufferSize) {
    Print(L"Buffer size is too small to contain the program headers\r\n");
    return NULL;
  }
  if (Header->e_phentsize != sizeof(Elf64_Phdr)) {
    Print(L"Unexpected program header entry size.\r\n");
    return NULL;
  }
  ProgramHeaders = (Elf64_Phdr *)(Buffer + Header->e_phoff);
  SegmentRange SegmentsRange = FindSegmentsRange(Header->e_phnum, ProgramHeaders);
  if (SegmentsRange.Range == 0) {
    Print(L"Couldn't find any loadable segment.\r\n");
    return NULL;
  }
  Print(L"Segments range: 0x%lx\r\n", SegmentsRange.Range);
  UINT64 NbPages = 0;
  for (; SegmentsRange.Range > NbPages * 4096; ++NbPages);
  Print(L"Number of Pages to allocate: %lu\r\n", (unsigned long)NbPages);
  EFI_STATUS Status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress, EfiLoaderData, NbPages, &SegmentsRange.LowestAddr);
  if (EFI_ERROR(Status)) {
    Print(L"Couldn't allocate pages for the kernel segments, aborting.\r\n");
    return NULL;
  }
  for (int i = 0; i < Header->e_phnum; ++i) {
    Elf64_Phdr *ProgramHeader = &ProgramHeaders[i];
    if (ProgramHeader->p_type == PT_LOAD) {
      Print(L"PT LOAD found!\r\n");
      Print(L"Program Header %d - Virtual Address: 0x%lx\r\n", i, ProgramHeader->p_vaddr);
      Print(L"Program Header %d - File Size: 0x%lx\r\n", i, ProgramHeader->p_filesz);
      Print(L"Program Header %d - Memory Size: 0x%lx\r\n", i, ProgramHeader->p_memsz);
      EFI_STATUS SegmentStatus = LoadSegment(Buffer, BufferSize, ProgramHeader);
      if (EFI_ERROR(SegmentStatus)) {
        EFI_STATUS FreeStatus = uefi_call_wrapper(BS->FreePages, 2, SegmentsRange.LowestAddr, NbPages);
        if (EFI_ERROR(FreeStatus))
          Print(L"Warning: couldn't free kernel pages.\r\n");
        return NULL;
      }
    }
  }
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