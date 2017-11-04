INC = /usr/include/efi
CFLAGS = -I$(INC) -I$(INC)/x86_64 -I$(INC)/protocol \
		 -Wall -std=c11 \
		 -fpic -fshort-wchar -fno-stack-protector -mno-red-zone
LDFLAGS = -nostdlib -znocombreloc -shared -Bsymbolic \
		  -L/usr/lib -lgnuefi -lefi \
		  -T/usr/lib/elf_x86_64_efi.lds

main.efi: main.so
	objcopy -j .text \
		    -j .sdata \
			-j .data \
			-j .dynamic \
			-j .dynsym \
			-j .rel \
			-j .rela \
			-j .reloc \
			--target=efi-app-x86_64 \
			$< \
			$@
	$(RM) $<

main.so: main.o
	$(LD) -o $@ $< /usr/lib/crt0-efi-x86_64.o $(LDFLAGS)
	$(RM) $<

main.o: main.c

clean:
	$(RM) main.efi
	$(RM) main.so
	$(RM) main.o

qemu:
	qemu-system-x86_64 -cpu qemu64 \
		-drive if=pflash,format=raw,file=/usr/share/ovmf/ovmf_code_x64.bin,readonly=on \
		-drive if=pflash,format=raw,file=./ovmf_vars_x64.bin \
		-drive file=uefi.img,if=ide

.PHONY: clean qemu
