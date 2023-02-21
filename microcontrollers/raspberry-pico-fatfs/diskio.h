/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2019          /
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// Bits of SD card status.
//
typedef int media_status_t;
enum {
    MEDIA_NOINIT = 0x01,
    // Indicates that the device has not been initialized and not
    // ready to work. This flag is set on system reset, media removal
    // or failure of disk_initialize function. It is cleared on
    // disk_initialize function succeeded. Any media change that
    // occurs asynchronously must be captured and reflect it to the
    // status flags, or auto-mount function will not work correctly.
    // If the system does not support media change detection,
    // application program needs to explicitly re-mount the volume
    // with f_mount() function after each media change.

    MEDIA_NODISK = 0x02,
    // Indicates that no medium in the drive. This is always
    // cleared when the drive is non-removable class.
    // Note that FatFs does not refer this flag.

    MEDIA_PROTECT = 0x04,
    // Indicates that the medium is write protected. This is always
    // cleared when the drive has no write protect function.
    // Not valid if MEDIA_NODISK is set.
};

//
// Results of Disk Functions.
//
typedef enum {
    DISK_OK = 0,     // Successful
    DISK_ERROR = 1,  // R/W Error
    DISK_WRPRT = 2,  // Write Protected
    DISK_NOTRDY = 3, // Not Ready
    DISK_PARERR = 4, // Invalid Parameter
} disk_result_t;

//
// Disk control functions.
//
media_status_t disk_initialize(uint8_t pdrv);
media_status_t disk_status(uint8_t pdrv);
disk_result_t disk_read(uint8_t pdrv, uint8_t *buff, unsigned sector, unsigned count);
disk_result_t disk_write(uint8_t pdrv, const uint8_t *buff, unsigned sector, unsigned count);
disk_result_t disk_ioctl(uint8_t pdrv, uint8_t cmd, void *buff);

//
// Command code for disk_ioctl() fucntion.
//
enum {
    // Generic command (Used by FatFs)
    CTRL_SYNC = 0,        // Complete pending write process (needed at FF_FS_READONLY == 0)
    GET_SECTOR_COUNT = 1, // Get media size (needed at FF_USE_MKFS == 1)
    GET_SECTOR_SIZE = 2,  // Get sector size (needed at FF_MAX_SS != FF_MIN_SS)
    GET_BLOCK_SIZE = 3,   // Get erase block size (needed at FF_USE_MKFS == 1)
    CTRL_TRIM = 4,        // Inform device that the data on the block of sectors is
                          // no longer used (needed at FF_USE_TRIM == 1)

    // Generic command (Not used by FatFs)
    CTRL_POWER = 5,  // Get/Set power status
    CTRL_LOCK = 6,   // Lock/Unlock media removal
    CTRL_EJECT = 7,  // Eject media
    CTRL_FORMAT = 8, // Create physical format on the media

    // MMC/SDC specific ioctl command
    MMC_GET_TYPE = 10,   // Get card type
    MMC_GET_CSD = 11,    // Get CSD
    MMC_GET_CID = 12,    // Get CID
    MMC_GET_OCR = 13,    // Get OCR
    MMC_GET_SDSTAT = 14, // Get SD status
    ISDIO_READ = 55,     // Read data form SD iSDIO register
    ISDIO_WRITE = 56,    // Write data to SD iSDIO register
    ISDIO_MRITE = 57,    // Masked write data to SD iSDIO register

    // ATA/CF specific ioctl command
    ATA_GET_REV = 20,   // Get F/W revision
    ATA_GET_MODEL = 21, // Get model name
    ATA_GET_SN = 22,    // Get serial number
};

#ifdef __cplusplus
}
#endif

#endif
