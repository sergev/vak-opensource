# USB floppy tools (Linux)

Utilities for USB-connected floppy drives using the UFI command set.

## usb-floppy-format

Low-level (physical) formatter for USB floppies, similar to **ufiformat**. It does **not** create filesystems; create a filesystem afterward with `mkfs.msdos` (or `mkfs.vfat`).

### Requirements

- **Linux** with the SCSI generic driver. Load the module if needed:
  ```bash
  sudo modprobe sg
  ```
- Device node: use the SCSI generic device for your floppy, e.g. `/dev/sg0`. Find it with `ls /dev/sg*` or `lsscsi`.

### Build (Linux)

```bash
make usb-floppy-format
# or
cc -Wall -Wextra -o usb-floppy-format usb-floppy-format.c
```

### Usage

```bash
usb-floppy-format -h                    # help
usb-floppy-format -i /dev/sg0           # info only (device + supported formats)
usb-floppy-format -f 1440 /dev/sg0      # format to 1440 KB (1.44 MB)
usb-floppy-format -f 1440 -F /dev/sg0   # same, skip confirmation
usb-floppy-format -f 1440 -V /dev/sg0   # format then verify
```

After low-level format, create a DOS/VFAT filesystem:

```bash
sudo mkfs.msdos /dev/fd0
# or use the block device that corresponds to your USB floppy
```

## usb-floppy-info

Queries drive and media info (vendor, product, capacity, geometry) via **libusb** and the Bulk-Only Transport. Build with `make usb-floppy-info` (requires libusb-1.0).
