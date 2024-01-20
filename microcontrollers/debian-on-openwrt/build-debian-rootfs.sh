#!/bin/sh
#
# Create contents of Debian root filesystem.
# Prerequisites:
#   sudo apt install multistrap
#
#if [ $# != 2 ]; then
#    echo "Usage: $0 rootfs bookworm"
#    exit 0
#fi
rootfs="rootfs"
release="bookworm"

set -x

# Remove previous build:

    sudo rm -rf $rootfs

# Run multistrap to build a directory rootfs/ with the minimal set of installed packages:

    sudo multistrap --arch mipsel --dir $rootfs --file multistrap-$release.conf
    if [ $? != 0 ]; then exit 1; fi

# Install the list of 'apt' sources:

    sudo bash -c "echo deb http://cdn-fastly.deb.debian.org/debian bookworm main > $rootfs/etc/apt/sources.list"

# Configure DNS resolver (dns.google):

    sudo bash -c "echo nameserver 8.8.8.8 > $rootfs/etc/resolv.conf"

# Fix a list of mounted filesystems:

    sudo ln -s /proc/mounts $rootfs/etc/mtab

# Generate default locale:

    echo "locales locales/default_environment_locale select en_US.UTF-8" | sudo chroot $rootfs /usr/bin/debconf-set-selections
    echo "locales locales/locales_to_be_generated multiselect en_US.UTF-8 UTF-8" | sudo chroot $rootfs /usr/bin/debconf-set-selections
    sudo rm rootfs/etc/locale.gen
    sudo chroot $rootfs /usr/sbin/dpkg-reconfigure --frontend noninteractive locales
    #if [ $? != 0 ]; then exit 1; fi

# Fix apt issues.

    sudo chroot $rootfs /usr/bin/apt-get update
    sudo chroot $rootfs /usr/bin/apt-get upgrade -y
    sudo chroot $rootfs /usr/bin/apt-get remove -y exim4-config exim4-base exim4-daemon-light bsd-mailx
    sudo chroot $rootfs /usr/bin/apt-get upgrade -y
