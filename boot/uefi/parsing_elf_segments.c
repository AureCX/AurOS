#include <efi.h>
#include <efilib.h>

EFI_STATUS CheckMagic(UINT8 *Buffer)
{
  if (Buffer[0] == 0x7F && Buffer[1] == 'E' && Buffer[2] == 'L' && Buffer[3] == 'F') {
    Print(L"Valid ELF!\r\n");
    return EFI_SUCCESS;
  }
  Print(L"Invalid ELF!\r\n");
  return EFI_INVALID_PARAMETER;
}