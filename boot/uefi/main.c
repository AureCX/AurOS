#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    Print(L"Hello from AurOS V2!\r\n");
    Print(L"Press any key to exit...\r\n");
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    return EFI_SUCCESS;
}