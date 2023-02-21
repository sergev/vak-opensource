/* glue.c
Copyright 2021 Carl John Kugler III

Licensed under the Apache License, Version 2.0 (the License); you may not use
this file except in compliance with the License. You may obtain a copy of the
License at

   http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
*/

/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include <stdio.h>

#include "diskio.h" /* Declarations of disk functions */
#include "hw_config.h"
#include "my_debug.h"
#include "sd_card.h"

#define TRACE_PRINTF(fmt, args...)
// #define TRACE_PRINTF printf  // task_printf

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

media_status_t disk_status(uint8_t pdrv) /* Physical drive nmuber to identify the drive */
{
    TRACE_PRINTF(">>> %s\n", __FUNCTION__);

    sd_card_t *p_sd = sd_get_by_num(pdrv);
    if (!p_sd)
        return MEDIA_NOINIT;

    sd_card_detect(p_sd); // Fast: just a GPIO read
    return p_sd->m_Status;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

media_status_t disk_initialize(uint8_t pdrv) /* Physical drive number to identify the drive */
{
    TRACE_PRINTF(">>> %s\n", __FUNCTION__);

    sd_card_t *p_sd = sd_get_by_num(pdrv);
    if (!p_sd)
        return MEDIA_NOINIT;

    return sd_init(p_sd);
}

static int sdrc2dresult(int sd_rc)
{
    switch (sd_rc) {
    case SD_BLOCK_DEVICE_ERROR_NONE:
        return DISK_OK;

    case SD_BLOCK_DEVICE_ERROR_UNUSABLE:
    case SD_BLOCK_DEVICE_ERROR_NO_RESPONSE:
    case SD_BLOCK_DEVICE_ERROR_NO_INIT:
    case SD_BLOCK_DEVICE_ERROR_NO_DEVICE:
        return DISK_NOTRDY;

    case SD_BLOCK_DEVICE_ERROR_PARAMETER:
    case SD_BLOCK_DEVICE_ERROR_UNSUPPORTED:
        return DISK_PARERR;

    case SD_BLOCK_DEVICE_ERROR_WRITE_PROTECTED:
        return DISK_WRPRT;

    case SD_BLOCK_DEVICE_ERROR_CRC:
    case SD_BLOCK_DEVICE_ERROR_WOULD_BLOCK:
    case SD_BLOCK_DEVICE_ERROR_ERASE:
    case SD_BLOCK_DEVICE_ERROR_WRITE:
    default:
        return DISK_ERROR;
    }
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

disk_result_t disk_read(uint8_t pdrv,    /* Physical drive nmuber to identify the drive */
                  uint8_t *buff,   /* Data buffer to store read data */
                  unsigned sector, /* Start sector in LBA */
                  unsigned count)  /* Number of sectors to read */
{
    TRACE_PRINTF(">>> %s\n", __FUNCTION__);
    sd_card_t *p_sd = sd_get_by_num(pdrv);
    if (!p_sd)
        return DISK_PARERR;
    int rc = sd_read_blocks(p_sd, buff, sector, count);
    return sdrc2dresult(rc);
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

disk_result_t disk_write(uint8_t pdrv,        /* Physical drive nmuber to identify the drive */
                   const uint8_t *buff, /* Data to be written */
                   unsigned sector,     /* Start sector in LBA */
                   unsigned count)      /* Number of sectors to write */
{
    TRACE_PRINTF(">>> %s\n", __FUNCTION__);
    sd_card_t *p_sd = sd_get_by_num(pdrv);
    if (!p_sd)
        return DISK_PARERR;
    int rc = sd_write_blocks(p_sd, buff, sector, count);
    return sdrc2dresult(rc);
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

disk_result_t disk_ioctl(uint8_t pdrv,  /* Physical drive nmuber (0..) */
                   uint8_t cmd,   /* Control code */
                   void *buff) /* Buffer to send/receive control data */
{
    TRACE_PRINTF(">>> %s\n", __FUNCTION__);

    sd_card_t *p_sd = sd_get_by_num(pdrv);
    if (!p_sd)
        return DISK_PARERR;

    switch (cmd) {
    case GET_SECTOR_COUNT: {
        //
        // Retrieves number of available sectors, the
        // largest allowable LBA + 1, on the drive
        // into the fs_lba_t variable pointed by buff.
        // This command is used by f_mkfs and f_fdisk
        // function to determine the size of
        // volume/partition to be created. It is
        // required when FF_USE_MKFS == 1.
        //
        unsigned n = sd_sectors(p_sd);
        *(uint32_t *)buff = n;
        if (!n)
            return DISK_ERROR;
        return DISK_OK;
    }
    case GET_SECTOR_SIZE:
        //
        // Retrieves the sector size on this media (512, 1024, 2048 or 4096).
        // Always set 512 for most systems, generic memory card and harddisk,
        // but a larger value may be required for on-board flash memory and some
        // type of optical media. This command is used by f_mount() and f_mkfs()
        // functions when FF_MAX_SS is larger than FF_MIN_SS.
        //
        *(uint16_t *)buff = 512;
        return DISK_OK;

    case GET_BLOCK_SIZE:
        //
        // Retrieves erase block size of the flash
        // memory media in unit of sector into the uint32_t
        // variable pointed by buff. The allowable value
        // is 1 to 32768 in power of 2. Return 1 if the
        // erase block size is unknown or non flash
        // memory media. This command is used by only
        // f_mkfs function and it attempts to align data
        // area on the erase block boundary. It is
        // required when FF_USE_MKFS == 1.
        //
        *(uint32_t *)buff = 1;
        return DISK_OK;

    case CTRL_SYNC:
        return DISK_OK;

    default:
        return DISK_PARERR;
    }
}
