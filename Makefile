NAME	=	BOOTX64.EFI

BOOT_SRC	=	boot/uefi/main.c

KERNEL_SRC	=	kernel/main.c

CC	=	gcc

LD	=	ld

OBJCOPY	=	objcopy

BOOT_MAIN = boot/uefi/main.o

KERNEL_MAIN	=	kernel/main.o

BOOTLOADER = bootloader.so

BOOTFILE = esp/EFI/BOOT/$(NAME)

$(BOOT_MAIN): $(BOOT_SRC)
	$(CC) $(BOOT_SRC) -c -o $(BOOT_MAIN) \
	-I/usr/include/efi \
	-I/usr/include/efi/x86_64 \
	-fpic \
	-ffreestanding \
	-fno-stack-protector \
	-fno-stack-check \
	-fshort-wchar \
	-mno-red-zone

$(KERNEL_MAIN): $(KERNEL_SRC)
	$(CC) $(KERNEL_SRC) -c -o $(KERNEL_MAIN)	\
	-ffreestanding

$(BOOTLOADER): $(BOOT_MAIN) $(KERNEL_MAIN)
	$(LD) \
    -nostdlib \
    -znocombreloc \
    -T /usr/lib/elf_x86_64_efi.lds \
    -shared \
    -Bsymbolic \
    /usr/lib/crt0-efi-x86_64.o \
    $(BOOT_MAIN) \
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
	rm -f $(BOOTLOADER) $(NAME) $(BOOTFILE) $(BOOT_MAIN) $(KERNEL_MAIN)

re:	clean all

.PHONY: all clean run re