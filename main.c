#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE img, EFI_SYSTEM_TABLE *systab) {
    InitializeLib(img, systab);
    Print(L"Hello, world\n");

    return EFI_SUCCESS;
}
