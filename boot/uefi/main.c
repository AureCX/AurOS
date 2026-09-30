#include <efi.h>
#include <efilib.h>
#include "elf.h"

EFI_FILE_HANDLE GetVolume(EFI_HANDLE image)
{
  EFI_LOADED_IMAGE *loaded_image = NULL;                  /* image interface */
  EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;      /* image interface GUID */
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *IOVolume;                        /* file system interface */
  EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID; /* file system interface GUID */
  EFI_FILE_HANDLE Volume;                                 /* the volume's interface */

  /* get the loaded image protocol interface for our "image" */
  uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (void **) &loaded_image);
  /* get the volume handle */
  uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fsGuid, (VOID*)&IOVolume);
  uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &Volume);
  return Volume;
}

UINT64 FileSize(EFI_FILE_HANDLE FileHandle)
{
  UINT64 ret;
  EFI_FILE_INFO *FileInfo;         /* file information structure */
  /* get the file's size */
  FileInfo = LibFileInfo(FileHandle);
  ret = FileInfo->FileSize;
  FreePool(FileInfo);
  return ret;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_FILE_HANDLE Volume;
    CHAR16 *FileName = L"kernel.elf";
    EFI_FILE_HANDLE FileHandle;
    UINT64 ReadSize;
    UINT8 *Buffer = NULL;
    EFI_STATUS Status;

    InitializeLib(ImageHandle, SystemTable);
    Volume = GetVolume(ImageHandle);
    Print(L"Hello from AurOS V2!\r\n");
    Status = uefi_call_wrapper(Volume->Open, 5, Volume, &FileHandle, FileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
    if (EFI_ERROR(Status)) {
      Print(L"Couldn't open the Kernel file\r\n");
      return Status;
    }
    Print(L"Kernel file opened!\r\n");
    ReadSize = FileSize(FileHandle);
    if (ReadSize < 4) {
      Print(L"Kernel file is too small\r\n");
      uefi_call_wrapper(FileHandle->Close, 1, FileHandle);
      return EFI_INVALID_PARAMETER;
    }
    Buffer = AllocatePool(ReadSize);
    if (Buffer == NULL) {
      Print(L"Couldn't allocate memory for Kernel\r\n");
      uefi_call_wrapper(FileHandle->Close, 1, FileHandle);
      return EFI_OUT_OF_RESOURCES;
    }
    Status = uefi_call_wrapper(FileHandle->Read, 3, FileHandle, &ReadSize, Buffer);
    if (EFI_ERROR(Status)) {
      Print(L"Error reading the Kernel file.\r\n");
      uefi_call_wrapper(FileHandle->Close, 1, FileHandle);
      FreePool(Buffer);
      return Status;
    }
    Print(L"Kernel ELF loaded of size %lu!\r\n", (unsigned long)ReadSize);
    Status = CheckMagic(Buffer);
    if (EFI_ERROR(Status)) {
      uefi_call_wrapper(FileHandle->Close, 1, FileHandle);
      FreePool(Buffer);
      return EFI_INVALID_PARAMETER;
    }
    uefi_call_wrapper(FileHandle->Close, 1, FileHandle);
    FreePool(Buffer);
    Print(L"Press any key to exit...\r\n");
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    return EFI_SUCCESS;
}