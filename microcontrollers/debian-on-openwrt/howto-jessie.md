# Create a Debian 8 filesystem image for OpenWRT

Linkit Smart 7688 board can be used as a MIPS-based Linux workstation, for
generic development tasks. To get access to a console port, you can
use any USB-to-serial adapter.

From a user point of view, an OpenWRT environment is quite
different from Linux. But it's possible to build a Debian-based
environment, located on microSD card and used via chroot.

* No modification to OpenWRT system is required
* All additional files placed on SD card
* Full traditional Linux environment is provided

To make a minimal root filesystem, you need a live MIPS Debian
system. I had used Creator CI20 board. First you need to install a
multistrap utility:

    sudo apt-get install multistrap gpgv

Create a file multistrap.conf and put the following lines to it:

    [General]
        arch=mipsel
        cleanup=true
        noauth=true
        unpack=true
        debootstrap=Debian
        aptsources=Debian

    [Debian]
        packages=apt
        source=http://http.debian.net/debian
        keyring=debian-archive-keyring
        suite=jessie

Fetch a public key for Debian 8/jessie:

    gpg --keyserver pgpkeys.mit.edu --recv-key 8B48AD6246925553
    gpg -a --export 8B48AD6246925553 | sudo apt-key add -
    gpg --keyserver pgpkeys.mit.edu --recv-key 7638D0442B90D010
    gpg -a --export 7638D0442B90D010 | sudo apt-key add -
    gpg --keyserver pgpkeys.mit.edu --recv-key CBF8D6FD518E17E1
    gpg -a --export CBF8D6FD518E17E1 | sudo apt-key add -

Run multistrap utiltity to build a directory debian/ with
a minimal set of installed packages:

    sudo multistrap -a mipsel -d debian -f multistrap.conf

Put ftp.us.debian.org public key to a destination directory:

    sudo rm debian/etc/apt/trusted.gpg.d/*
    sudo cp /etc/apt/trusted.gpg debian/etc/apt

Add a default Google DNS resolver:

    sudo bash -c "echo nameserver 8.8.8.8 > debian/etc/resolv.conf"

Fix a list of mounted filesystems:

    sudo bash -c "ln -s /proc/mounts debian/etc/mtab"

Edit file debian/root/.bashrc and add the following lines to it:

    export PATH=/sbin:/bin:/usr/sbin:/usr/bin
    export TERM=xterm
    unset HOSTNAME SHELL LOOP_MOUNTPOINT ASEC_MOUNTPOINT
    unset ANDROID_PROPERTY_WORKSPACE ANDROID_ASSETS
    unset ANDROID_BOOTLOGO LD_LIBRARY_PATH BOOTCLASSPATH
    unset ANDROID_DATA MKSH ANDROID_ROOT EXTERNAL_STORAGE

Check that a minimal filesystem works as expected:

    sudo chroot debian /bin/bash    -- start a shell in a new root filesystem
    apt-get update                  -- check network access to Debian repository
    apt-get clean
    exit                            -- exit from chroot shell

Pack the root directory into a single tarball:

    (cd debian; sudo tar cvzf ../debian.tgz .)
    sudo chown $USER debian.tgz

The size of resulting file will be about 70 Mbytes.
A resulting image is available for download here: ftp://ftp.vak.ru/unix/debian-jessie-mini.tgz

Create an empty Linux partition on SD card.
Typically, a clean FS card contains a single partition of type FAT32.
Use fdisk to change the partition type to Linux and create empty filesystem:

    $ sudo fdisk /dev/mmcblk0
    [sudo] password for vak:

    Welcome to fdisk (util-linux 2.25.2).
    Changes will remain in memory only, until you decide to write them.
    Be careful before using the write command.

    Command (m for help): t
    Selected partition 1
    Hex code (type L to list all codes): 83
    Changed type of partition 'W95 FAT32 (LBA)' to 'Linux'.

    Command (m for help): q

    $ sudo mke2fs /dev/mmcblk0p1
    mke2fs 1.42.12 (29-Aug-2014)
    /dev/mmcblk0p1 contains a vfat file system
    Proceed anyway? (y,n) y
    Creating filesystem with 3910400 4k blocks and 979200 inodes
    Filesystem UUID: 5f608995-ebf9-4d47-a760-53298b013ce7
    Superblock backups stored on blocks:
        32768, 98304, 163840, 229376, 294912, 819200, 884736, 1605632, 2654208

    Allocating group tables: done
    Writing inode tables: done
    Writing superblocks and filesystem accounting information: done

Install the debian package into the SD card:

    sudo mount /dev/mmcblk0p1 /mnt
    sudo tar xvzf debian.tgz -C /mnt
    sudo umount /mnt


# Build Linkit firmware with FPU and ext4 support

Follow instructions on page:
    https://github.com/MediaTek-Labs/linkit-smart-7688-feed

When doing menuconfig, enter Kernel Modules / Filesystems
and enable kmod-fs-ext4 module. It makes sense to enable kmod-fuse
as well, as it might come handy.

After build finished (it takes a few hours), copy the resulting image
bin/ramips/openwrt-ramips-mt7688-LinkIt7688-squashfs-sysupgrade.bin
to file lks7688.img on a microSD Flash drive. Plug the drive into the
board, and reboot the board. Hold WiFi button for 5 seconds then release.
Reflash process takes a few minutes to complete.


# Start Debian on Linkit Smart 7688 board

Connect your board to Wi-Fi network:

    uci set wireless.sta.ssid=IMG-Staff-BYOD
    uci set wireless.sta.encryption=psk2
    uci set wireless.sta.key=xxxxxxxx
    uci set wireless.sta.disabled=0
    uci commit
    wifi

To start a shell in a new Debian environment, use this script:

    mount /dev/mmcblk0p1 /mnt
    mount --bind /dev /mnt/dev
    mount --bind /proc /mnt/proc
    mount --bind /sys /mnt/sys
    HOME=/root chroot /mnt /bin/bash

You can install any needed packages, using a standard utility apt-get:

    apt-get install vim build-essential git cvs subversion openssh-client
    apt-get install dialog iputils-ping autoconf automake libtool gettext
    apt-get install flex bison byacc gdb locate procps

To unmount, use the script:

    umount /mnt/dev
    umount /mnt/proc
    umount /mnt/sys
    umount /mnt

Enjoy!
