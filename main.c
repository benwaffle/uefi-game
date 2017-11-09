// vim:et ts=2 sw=2
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

void square(uint32_t *buf, uint32_t color, unsigned x, unsigned y, unsigned width, unsigned height, unsigned stride) {
  for (unsigned yy = y; yy < y + height; ++yy)
    for (unsigned xx = x; xx < x + width; ++xx)
      buf[yy * stride + xx] = color;
}

uint32_t color(uint8_t red, uint8_t green, uint8_t blue) {
  return (blue << 24) | (green << 16) | (red << 8) | 0xff;
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE img, EFI_SYSTEM_TABLE *systab) {
  InitializeLib(img, systab);
  //Print(L"This is a game\r\n");

  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

  EFI_STATUS status = LibLocateProtocol(&GraphicsOutputProtocol, (void**)&gop);
  CHECK(status, TRUE);

  int width = gop->Mode->Info->HorizontalResolution;
  int height = gop->Mode->Info->VerticalResolution;

  if (gop->Mode->Info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
    Print(L"Error: don't know about pixel format %d\r\n", gop->Mode->Info->PixelFormat);
    Exit(EFI_SUCCESS, 0, NULL);
  }

  Print(L"%dx%d\r\n", width, height);
  //WaitForSingleEvent(systab->ConIn->WaitForKey, 0);

  uint64_t size = width * height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  uint32_t *buffer = AllocateZeroPool(size);

  int x = 300;
  int y = 400;
  int delta = 10;

  while (TRUE) {
    square(buffer, color(0xf4, 0x71, 0x42), 0, 0, width, height, width);
    square(buffer, color(0x59, 0xf4, 0x42), x, y, 100, 100, width);

    status = uefi_call_wrapper(gop->Blt, 10, gop, buffer, EfiBltBufferToVideo, 0, 0, 0, 0, width, height, 0);
    CHECK(status, FALSE);

    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    EFI_INPUT_KEY key;
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
    if (key.ScanCode == SCAN_UP) y -= delta;
    if (key.ScanCode == SCAN_DOWN) y += delta;
    if (key.ScanCode == SCAN_LEFT) x -= delta;
    if (key.ScanCode == SCAN_RIGHT) x += delta;
  }

  Print(L"press any key to exit...\r\n");
  WaitForSingleEvent(systab->ConIn->WaitForKey, 0);

  return EFI_SUCCESS;
}
