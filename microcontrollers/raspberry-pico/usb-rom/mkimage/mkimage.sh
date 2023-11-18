#!/bin/bash
#
# Create filesystem image
#
if [ $# != 2 ]; then
    echo "Usage: $0 volname dir"
    exit 0
fi

volume_name="$1"
contents="$2"
oem="MSWIN4.1"

size_kbytes=$((2048 - 64))
set -x

echo "Disk size = $size_kbytes kbytes"

# Create filesystem image filled with FF
dd if=/dev/zero of=filesys.img bs=1024 count=$size_kbytes
#python3 -c "import sys; sys.stdout.buffer.write(b'\xff' * 1024 * $size_kbytes)" > filesys.img

# Create dummy boot code
dd if=/dev/zero of=bootcode.bin bs=512 count=1

# Format entire disk as one DOS partition
fdisk -i -y -a dos -f bootcode.bin filesys.img

# Attach the image as disk
set $(hdiutil attach -nomount filesys.img)
disk=$1
echo "$disk"

# Create FAT12 filesystem on the first partition
newfs_msdos -F 12 -v "$volume_name" -O "$oem" "$disk"s1

# Mount the filesystem
set $(hdiutil mountvol "$disk"s1)
dir="$3"

# Copy contents
cp -a "$contents"/ "$dir"
sync

# Detach the image
hdiutil detach $disk

# Write image to RP2040 Flash memory.
# picotool load -n filesys.img -t bin -o 0x10010000

# Read contents of RP2040 Flash memory.
# picotool save -r 0x10010000 0x100f0000 save.bin -t bin
