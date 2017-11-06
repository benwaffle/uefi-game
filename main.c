#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE img, EFI_SYSTEM_TABLE *systab) {
    InitializeLib(img, systab);
    Print(L"Hello, world!\r\nwaiting for keypress \r\n");

    EFI_STATUS res = EFI_SUCCESS;
    while (res == EFI_SUCCESS) {
        UINTN index = -1;
        res = uefi_call_wrapper(systab->BootServices->WaitForEvent, 3, 1, &systab->ConIn->WaitForKey, &index);
        if (EFI_ERROR(res)) {
            Print(L"error: %d\n", res);
        } else {
            EFI_INPUT_KEY key;
            uefi_call_wrapper(systab->ConIn->ReadKeyStroke, 2, systab->ConIn, &key);
            Print(L"%c", key.UnicodeChar);

            if (key.UnicodeChar == 'q') {
              break;
            }
        }
    }

    Print(L"\r\n");

    return EFI_SUCCESS;
}
