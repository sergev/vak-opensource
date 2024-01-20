#!/bin/sh

busybox mount --bind /dev /mnt/sda1/dev
busybox mount --bind /dev/pts /mnt/sda1/dev/pts
busybox mount --bind /proc /mnt/sda1/proc
busybox mount --bind /sys /mnt/sda1/sys

HOME=/root chroot /mnt/sda1 /bin/sh

busybox umount /mnt/sda1/dev/pts
busybox umount /mnt/sda1/dev
busybox umount /mnt/sda1/proc
busybox umount /mnt/sda1/sys
