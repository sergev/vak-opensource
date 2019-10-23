toolchain:
        Install MIPS Codescape MTI GNU/Linux Toolchain, version 2019.09-01:
        https://codescape.mips.com/components/toolchain/2019.09-01/downloads.html

u-boot:
	git clone https://github.com/MicrochipTech/u-boot-pic32.git

	cd u-boot-pic32
	patch -p1 < ../u-boot.patch
	make CROSS_COMPILE=mips-mti-linux-gnu- pic32mzdask_defconfig
	make CROSS_COMPILE=mips-mti-linux-gnu- USE_PRIVATE_LIBGCC=arch/mips/lib/libgcc.a CONFIG_USE_PRIVATE_LIBGCC=y

	cd ..
	make

        ./upload.sh

linux:
	git clone https://github.com/MicrochipTech/linux-pic32.git

	cd linux-pic32
	make ARCH=mips CROSS_COMPILE=mips-mti-linux-gnu- pic32mzda_defconfig
	patch -p1 < ../linux.patch
	make ARCH=mips CROSS_COMPILE=mips-mti-linux-gnu-
	gzip -9 < arch/mips/boot/vmlinux.bin > arch/mips/boot/vmlinux.bin.gz
	../u-boot-pic32/tools/mkimage -A MIPS -a 0x88000000 -e 0x88000400 -d arch/mips/boot/vmlinux.bin.gz arch/mips/boot/uImage

	cd ..
	cp linux-pic32/arch/mips/boot/uImage mmc0:/boot/uImage
	cp linux-pic32/arch/mips/boot/dts/pic32/pic32mzda_sk.dtb mmc0:/boot/pic32mzda.dtb

Наверное, нужно и загружаемые модули куда-то в mmc0:/lib/modules скопировать?

in u-boot:
	setenv bootargs "console=ttyS1,115200n8 root=/dev/mmcblk0p1"
	setenv bootcmd "ext4load mmc 0:1 0x88500000 /boot/uImage ; ext4load mmc 0:1 0x88C00000 /boot/pic32mzda.dtb ; bootm 0x88500000 - 0x88C00000"
	saveenv
	boot
