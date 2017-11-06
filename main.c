#include <efi.h>
#include <efilib.h>

#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)
#define WFILE WIDE1(__FILE__)

#define CHECK(rc, crit) { \
  if (EFI_ERROR(rc)) { \
    CHAR16 Buffer[64]; \
    StatusToString(Buffer, rc); \
    Print(L"[%s:%d] %s (code %d)\r\n", WFILE, __LINE__, Buffer, rc); \
    if (crit) Exit(rc, 0, NULL); \
  } \
}

static int memcmp(const void *s1, const void *s2, UINTN n)
{
	const unsigned char *c1 = s1, *c2 = s2;
	int d = 0;

	if (!s1 && !s2)
		return 0;
	if (s1 && !s2)
		return 1;
	if (!s1 && s2)
		return -1;

	while (n--) {
		d = (int)*c1++ - (int)*c2++;
		if (d)
			break;
	}
	return d;
}


EFI_STATUS EFIAPI efi_main(EFI_HANDLE img, EFI_SYSTEM_TABLE *systab) {
  InitializeLib(img, systab);
  Print(L"Hello, world!\r\n");

  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

  EFI_STATUS status = LibLocateProtocol(&GraphicsOutputProtocol, (void**)&gop);
  CHECK(status, TRUE);

  for (int i = 0; i < gop->Mode->MaxMode; ++i) {
    UINTN sizeOfInfo;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    status = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &sizeOfInfo, &info);
    CHECK(status, TRUE);

    Print(L"%c%dx%d ", memcmp(gop->Mode->Info, info, sizeof(*info)) == 0 ? '*' : ' ', info->HorizontalResolution, info->VerticalResolution);
		switch(info->PixelFormat) {
      case PixelRedGreenBlueReserved8BitPerColor:
        Print(L"RGBR");
        break;
      case PixelBlueGreenRedReserved8BitPerColor:
        Print(L"BGRR");
        break;
      case PixelBitMask:
        Print(L"R:%08x G:%08x B:%08x X:%08x",
              info->PixelInformation.RedMask,
              info->PixelInformation.GreenMask,
              info->PixelInformation.BlueMask,
              info->PixelInformation.ReservedMask);
        break;
      case PixelBltOnly:
        Print(L"(blt only)");
        break;
      default:
        Print(L"(Invalid pixel format)");
        break;
		}
		Print(L" pitch %d\n", info->PixelsPerScanLine);
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    EFI_INPUT_KEY key;
    status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
    CHECK(status, FALSE);
  }

  return EFI_SUCCESS;
}
