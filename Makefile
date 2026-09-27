NAME	=	BOOTX64.EFI

SRC	=	boot/uefi/main.c

CC	=	gcc

LD	=	ld

OBJCOPY	=	objcopy

MAIN = main.o

BOOTLOADER = bootloader.so

BOOTFILE = esp/EFI/BOOT/$(NAME)

$(MAIN): $(SRC)
	$(CC) $(SRC) -c -o $(MAIN) \
	-I/usr/include/efi \
	-I/usr/include/efi/x86_64 \
	-fpic \
	-ffreestanding \
	-fno-stack-protector \
	-fno-stack-check \
	-fshort-wchar \
	-mno-red-zone

$(BOOTLOADER): $(MAIN)
	$(LD) \
    -nostdlib \
    -znocombreloc \
    -T /usr/lib/elf_x86_64_efi.lds \
    -shared \
    -Bsymbolic \
    /usr/lib/crt0-efi-x86_64.o \
    $(MAIN) \
    -L/usr/lib \
    -lefi \
    -lgnuefi \
    -o $(BOOTLOADER)

$(NAME): $(BOOTLOADER)
	$(OBJCOPY) \
    -j .text \
	-j .sdata \
	-j .data \
	-j .rodata \
	-j .dynamic \
	-j .dynsym \
	-j .rel \
	-j .rela \
	-j .reloc \
	-O efi-app-x86_64 \
	--subsystem=10 \
	$(BOOTLOADER) \
	$(NAME)
	
$(BOOTFILE): $(NAME)
	mkdir -p esp/EFI/BOOT
	cp $(NAME) $(BOOTFILE)

all: $(NAME) $(BOOTFILE)

run: all
	qemu-system-x86_64 \
    -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd \
    -drive if=pflash,format=raw,file=OVMF_VARS.fd \
    -drive format=raw,file=fat:rw:esp

clean:
	rm -f *.o $(BOOTLOADER) $(NAME) $(BOOTFILE)

re:	clean all

.PHONY: all clean run re