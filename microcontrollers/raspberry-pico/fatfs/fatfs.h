/*----------------------------------------------------------------------------/
/  FatFs - Generic FAT Filesystem module  R0.15                               /
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2022, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:

/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/----------------------------------------------------------------------------*/
#ifndef RPM_FATFS_H
#define RPM_FATFS_H

// Declarations of FatFs API.
#include "fs.h"

// FatFs configuration options.
#include "ffconf.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Type for logical block address.
// Disk sizes up to 2 Tbytes are supported (LBA-32).
//
typedef uint32_t fs_lba_t;

//
// Filesystem object structure.
//
struct _filesystem_t {
    uint8_t fs_type;   /* Filesystem type (0:not mounted) */
    uint8_t pdrv;      /* Volume hosting physical drive */
    uint8_t ldrv;      /* Logical drive number (used only when FF_FS_REENTRANT) */
    uint8_t n_fats;    /* Number of FATs (1 or 2) */
    uint8_t wflag;     /* win[] status (b0:dirty) */
    uint8_t fsi_flag;  /* FSINFO status (b7:disabled, b0:dirty) */
    uint16_t id;        /* Volume mount ID */
    uint16_t n_rootdir; /* Number of root directory entries (FAT12/16) */
    uint16_t csize;     /* Cluster size [sectors] */
#if FF_MAX_SS != FF_MIN_SS
    uint16_t ssize; /* Sector size (512, 1024, 2048 or 4096) */
#endif
    uint16_t *lfnbuf; /* LFN working buffer */
    uint8_t *dirbuf; /* Directory entry block scratchpad buffer for exFAT */
#if !FF_FS_READONLY
    uint32_t last_clst; /* Last allocated cluster */
    uint32_t free_clst; /* Number of free clusters */
#endif
#if FF_FS_RPATH
    uint32_t cdir; /* Current directory start cluster (0:root) */
    uint32_t cdc_scl;  /* Containing directory start cluster (invalid when cdir is 0) */
    uint32_t cdc_size; /* b31-b8:Size of containing directory, b7-b0: Chain status */
    uint32_t cdc_ofs;  /* Offset in the containing directory (invalid when cdir is 0) */
#endif
    uint32_t n_fatent; /* Number of FAT entries (number of clusters + 2) */
    uint32_t fsize;    /* Number of sectors per FAT */
    fs_lba_t volbase;  /* Volume base sector */
    fs_lba_t fatbase;  /* FAT base sector */
    fs_lba_t dirbase;  /* Root directory base sector (FAT12/16) or cluster (FAT32/exFAT) */
    fs_lba_t database; /* Data base sector */
    fs_lba_t bitbase; /* Allocation bitmap base sector */
    fs_lba_t winsect;       /* Current sector appearing in the win[] */
    uint8_t win[FF_MAX_SS]; /* Disk access window for Directory, FAT (and file data at tiny cfg) */
};

//
// Object ID and allocation information.
//
typedef struct {
    struct _filesystem_t *fs; /* Pointer to the hosting volume of this object */
    uint16_t id;   /* Hosting volume's mount ID */
    uint8_t attr; /* Object attribute */
    uint8_t stat; /* Object chain status (b1-0: =0:not contiguous, =2:contiguous, =3:fragmented in this
                  session, b2:sub-directory stretched) */
    uint32_t sclust;    /* Object data start cluster (0:no cluster or root directory) */
    fs_size_t objsize; /* Object size (valid when sclust != 0) */
    uint32_t n_cont; /* Size of first fragment - 1 (valid when stat == 3) */
    uint32_t n_frag; /* Size of last fragment needs to be written to FAT (valid when not zero) */
    uint32_t c_scl;  /* Containing directory start cluster (valid when sclust != 0) */
    uint32_t c_size; /* b31-b8:Size of containing directory, b7-b0: Chain status (valid when c_scl !=
                     0) */
    uint32_t c_ofs;  /* Offset in the containing directory (valid when file object and sclust != 0) */
#if FF_FS_LOCK
    unsigned lockid; /* File lock ID origin from 1 (index of file semaphore table Files[]) */
#endif
} obj_id_t;

//
// File object structure.
//
struct _file_t {
    obj_id_t obj;  /* Object identifier (must be the 1st member to detect invalid object pointer) */
    uint8_t flag;    /* File status flags */
    uint8_t err;     /* Abort flag (error code) */
    fs_size_t fptr; /* File read/write pointer (Zeroed on file open) */
    uint32_t clust;  /* Current cluster of fpter (invalid when fptr is 0) */
    fs_lba_t sect;   /* Sector number appearing in buf[] (0:invalid) */
#if !FF_FS_READONLY
    fs_lba_t dir_sect; /* Sector number containing the directory entry (not used at exFAT) */
    uint8_t *dir_ptr;  /* Pointer to the directory entry in the win[] (not used at exFAT) */
#endif
#if FF_USE_FASTSEEK
    uint32_t *cltbl; /* Pointer to the cluster link map table (nulled on open, set by application) */
#endif
#if !FF_FS_TINY
    uint8_t buf[FF_MAX_SS]; /* File private data read/write window */
#endif
};

//
// Directory object structure.
//
struct _directory_t {
    obj_id_t obj; /* Object identifier */
    uint32_t dptr;  /* Current read/write offset */
    uint32_t clust; /* Current cluster */
    fs_lba_t sect;  /* Current sector (0:Read operation has terminated) */
    uint8_t *dir;   /* Pointer to the directory item in the win[] */
    uint8_t fn[12]; /* SFN (in/out) {body[8],ext[3],status[1]} */
    uint32_t blk_ofs; /* Offset of current entry block being processed (0xFFFFFFFF:Invalid) */
#if FF_USE_FIND
    const char *pat; /* Pointer to the name matching pattern */
#endif
};

//
// Format parameter structure.
//
typedef struct {
    uint8_t n_fat;    // Number of FATs
    unsigned align;   // Data area alignment (sector)
    unsigned n_root;  // Number of root directory entries
    uint32_t au_size; // Cluster size (byte)
} mkfs_parm_t;

/*--------------------------------------------------------------*/
/* Additional Functions                                         */
/*--------------------------------------------------------------*/

/* LFN support functions (defined in ffunicode.c) */

uint16_t ff_oem2uni(uint16_t oem, uint16_t cp); /* OEM code to Unicode conversion */
uint16_t ff_uni2oem(uint32_t uni, uint16_t cp); /* Unicode to OEM code conversion */
uint32_t ff_wtoupper(uint32_t uni);         /* Unicode upper-case conversion */

/* O/S dependent functions (samples available in ffsystem.c) */

#if FF_USE_LFN == 3            /* Dynamic memory allocation */
void *ff_memalloc(unsigned msize); /* Allocate memory block */
void ff_memfree(void *mblock); /* Free memory block */
#endif
#if FF_FS_REENTRANT            /* Sync functions */
int ff_mutex_create(int vol);  /* Create a sync object */
void ff_mutex_delete(int vol); /* Delete a sync object */
int ff_mutex_take(int vol);    /* Lock sync object */
void ff_mutex_give(int vol);   /* Unlock sync object */
#endif

/* Definitions of volume management */

#if FF_STR_VOLUME_ID
#ifndef FF_VOLUME_STRS
extern const char *VolumeStr[FF_VOLUMES]; /* User defied volume ID */
#endif
#endif

/*--------------------------------------------------------------*/
/* Flags and Offset Address                                     */
/*--------------------------------------------------------------*/

/* Fast seek controls (2nd argument of f_lseek) */
#define CREATE_LINKMAP ((fs_size_t)0 - 1)

/* Filesystem type (filesystem_t.fs_type) */
#define FS_FAT12 1
#define FS_FAT16 2
#define FS_FAT32 3
#define FS_EXFAT 4

// Get date and time (local).
void get_datetime(int *year, int *month, int *day, int *dotw, int *hour, int *min, int *sec);

// Get day of the week.
int get_dotw(int year, int month, int day);

#ifdef __cplusplus
}
#endif

#endif // RPM_FATFS_H
