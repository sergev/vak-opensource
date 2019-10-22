CROSS_COMPILE: mips-mti-linux-gnu verion: 2016.05-03

Поменял img на mti.
Проверил с версией codescape-linux/2019.09-01 - u-boot работает.
Для этого надо слегка поправить u-boot:
    cd u-boot-pic32/include/linux
    cp compiler-gcc5.h compiler-gcc6.h
    cp compiler-gcc5.h compiler-gcc7.h

u-boot:
	git clone https://github.com/MicrochipTech/u-boot-pic32.git

	cd u-boot-pic32
	patch -p1 < ../u-boot.patch
	make CROSS_COMPILE=mips-mti-linux-gnu- pic32mzdask_defconfig
	make CROSS_COMPILE=mips-mti-linux-gnu- USE_PRIVATE_LIBGCC=arch/mips/lib/libgcc.a CONFIG_USE_PRIVATE_LIBGCC=y

	cd ..
	make

	mdb.sh mdb.cfg
К сожалению, утилита MDB не срабатывает:
program obj/u-boot-32m.hex
Programming target...
Program failed.

Прошить получается только в диалоговом режиме через MPLABX.

	obj/u-boot-128m.hex - PIC32MZ2064DAA288
	obj/u-boot-32m.hex  - PIC32MZ2064DAG169

На самом деле на плате стоит PIC32MZ2064DA_R_169, но MPLABX отказывается конфигурить
для него программатор "PIC32MZ DA Starter Kit".


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
